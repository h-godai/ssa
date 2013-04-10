/* -*-tab-width:4-*-
   libTS drawframe_app.hpp

   DrawFrame毎に呼び出されるタイプのアプリケーション
   OpenGLでの描画時に使用する

 */
#ifndef TS_APPFW_DRAWFRAMEAPP_HPP
#define TS_APPFW_DRAWFRAMEAPP_HPP

#include "appfw.hpp"

namespace ts {
namespace appfw {

  // DrawFrame毎に呼び出されるタイプのアプリケーション
  class drawframe_app : public app_framework {
	uint32_t numOfBuffers_;
  protected:
	virtual ~drawframe_app() throw() {}

  public:
	gl::glfw gr_;


	enum thread_status {
	  unintialized,
	  running,
	  stopped
	};
	volatile thread_status        threadStatus_;
	boost::mutex                      ef_guard_;
	boost::condition_variable_any     wakeup_;
	boost::thread mainthread_;
	volatile uint32_t frame_counter_;


  protected:
	drawframe_app()
	  : numOfBuffers_(2)
	  , threadStatus_(unintialized)
	  , mainthread_(boost::ref(*this))
		//, mainthread_(&drawframe_app::mainthread, this) // boost::ref(*this))
	  , frame_counter_(0)
	{}

	virtual event::event_processor::ptr event_handling_impl(event::event_t::ptr ev) {
	  event::event_processor::ptr e = gr_(ev);
	  if (e) {
		ev->processed_ = true;
		app_framework::event_handling_impl(ev);
		return e;
	  }
	  return app_framework::event_handling_impl(ev);
	}

  protected:
	virtual void initialize_impl() {
	  // スレッド起動
	  threadStatus_ = running;
	}
	// メインルーチン
	virtual void main() = 0;

  public:

	// 描画処理
	void swap_buffers() {
	}

	// アップデート通知
	virtual void update() {
	  gr_.update();
	}



	// 描画フレーム開始のフック
	// 実行環境のフレームワークから毎フレーム呼び出される。
	void draw_frame_hook(double timestamp) {
	  time_now_ = timestamp; // need thread safe operation

	  gr_.drawframe();
	  {
		scoped_lock lk(ef_guard_);
		wakeup_.notify_one();
	  }
	  ++frame_counter_;
	}

	uint32_t frame_counter() const { return frame_counter_; }


	bool is_running() const {
	  return threadStatus_ == running;
	}
	bool is_stopped() const {
	  return threadStatus_ == stopped;
	}

	virtual void wait_for_sec(float second) {
	  uint32_t frames = static_cast<uint32_t>(second * 60.0f);
	  if (frames == 0) frames = 1;
	  wait_for_frame(frames);
	}

	void wait_for_frame(uint32_t frames=1) {
	  while (frames--) {
		{
		  scoped_lock lk(ef_guard_);
		  // 現在の60秒後の時間を計算
		  boost::xtime xt;
		  boost::xtime_get(&xt, boost::TIME_UTC);
		  xt.sec += 60;
		  if (!wakeup_.timed_wait(lk, xt)) {
			// タイムアウトした！
			on_drawframe_timeout();
		  }
		}
		// イベント処理
		process_events();
	  }
	  return;
	}

	// スレッドのメインルーチン
  public:
	void operator()() {
//    void mainthread() {
	  // 基本的に無限ループしておく
	  TS_DEBUGLOG("appfw::Enter mainthread\n");
	  for(;;) {
		switch (threadStatus_) {
		case unintialized:
		  wait_for_frame();
		  break;
		case running:
          TS_DEBUGLOG("appfw::Enter app main routine\n");
		  main();
          TS_DEBUGLOG("appfw::Exit app main routine\n");
		  threadStatus_ = stopped;
		  break;
		case stopped:
		default:
          TS_DEBUGLOG("appfw::mainthread stopped\n");
		  return;
		};
	  }
	}

	// イベント

	// 描画が行われない
	virtual void on_drawframe_timeout() {
	  TS_DEBUGLOG("drawframe wait timeout");
	  throw drawframe_exception("drawframe wait timeout");
	}

	// デバイスが回転した
	virtual void on_device_rotate(float rot) {
	  gr_.on_device_rotate(rot);
	  TS_DEBUGLOG("rotate %f\n", rot);
	}

	// デバイスの回転による座標の補正
	virtual void position_correct(math::pos2_f& pos) {
	  if (gr_.context_.rotate() < 45.0f) {
		return;
	  }
	  if (gr_.context_.rotate() < 90.0f+45.0f) { // 90
		pos.y_ = gr_.context_.device_screen_size().height_ - pos.y_ - 1;
		swap(pos.x_, pos.y_);
		return;
	  }
	  if (gr_.context_.rotate() < 180.0f+45.0f) {
		pos.x_ = gr_.context_.device_screen_size().width_ - pos.x_ - 1;
		pos.y_ = gr_.context_.device_screen_size().height_ - pos.y_ - 1;
		return;
	  }
	  else { // 270
		pos.x_ = gr_.context_.device_screen_size().width_ - pos.x_ - 1;
		swap(pos.x_, pos.y_);
		return;
	  }
	}
	// デバイスの回転によるベクトルの補正
	virtual void vector_correct(math::vec2_f& pos) {
	  if (gr_.context_.rotate() < 45.0f) {
		return;
	  }
	  if (gr_.context_.rotate() < 90.0f+45.0f) { // 90
		swap(pos.x_, pos.y_);
		pos.y_ = -pos.y_;
		return;
	  }
	  if (gr_.context_.rotate() < 180.0f+45.0f) {
		pos.x_ =  - pos.x_;
		pos.y_ =  - pos.y_;
		return;
	  }
	  else { // 270
		swap(pos.x_, pos.y_);
		pos.x_ = - pos.x_;
		return;
	  }
	}

	// フレームワークがアクセスするアプリケーションインスタンス
	static drawframe_app& app_instance();
  };


} // namespace appfw
} // namespace ts

#endif
