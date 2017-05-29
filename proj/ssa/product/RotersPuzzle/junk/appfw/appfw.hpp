/* -*-tab-width:4-*-
   libTS appfw.hpp
 */
#ifndef TS_APPFW_APPFW_HPP
#define TS_APPFW_APPFW_HPP


#include <stdint.h>
#include <iostream>
#include <string>
#include <queue>
#include <stdexcept>
#include <boost/shared_ptr.hpp>

#include "libTS/Util/Logger.h"
#include "exception.hpp"
#include "inputfw.hpp"
#include "gl/glfw.hpp"

namespace ts {
namespace appfw {
  // application framework
  using namespace std;

  typedef boost::mutex::scoped_lock scoped_lock;

  struct app_info {
	typedef boost::shared_ptr<app_info> ptr;
	vector<string> args_;
	string app_name_;
	string app_dir_; // アプリケーションのディレクトリ
	string res_dir_; // リソースのディレクトリ
	string doc_dir_; // アプリケーションドキュメントのディレクトリ
	string platformName_;
  };

  // アプリケーションフレームワークのベースクラス
  class app_framework {
  protected:
	app_framework() {}
	virtual ~app_framework() throw() {}
	
	app_info::ptr app_info_;
	in::event_handler input_eh_; // 入力デバイスのイベントハンドラ
	  
  public:
	// 初期化
	void initialize(app_info::ptr info) {
	  app_info_ = info;
	  initialize_impl();
	}

	// アップデート通知
	virtual void update() {
	}

	// 入力フレームワークへのアクセス
	in::event_handler& input_eh() {
	  return input_eh_;
	}
	
	// イベント通知
	template <typename Ctrl>
	void notify_event(const Ctrl& ctrl, uint32_t devnum=0) {
	  input_eh_.notify_event(ctrl, devnum);
	}

	// イベント処理
	void process_events() {
	  while (input_eh_.execute(*this));
	}
	// イベント処理ルーチン
	void operator () (in::event_t::ptr ev) {
	  switch (ev->device_type()) {
	  case in::tap_device:
		on_tap_ctrl(boost::static_pointer_cast<in::tap_event>(ev));
		break;
	  default:
		break;
	  }
	}

	virtual void on_tap_ctrl(in::tap_event::ptr evt) {
	  TS_DEBUGLOG("on_tap_ctrl(#%d, act:%d, (%.2f,%.2f)\n"
				  , evt->devnum_
				  , evt->ctrl_.action_
				  , evt->ctrl_.pos_.x_
				  , evt->ctrl_.pos_.y_);
	}
	

	
	
	// サービスメソッド
	const string& application_dir() const { return app_info_->app_dir_; }
	const string& documentation_dir() const { return app_info_->doc_dir_; }
	const string& resource_dir() const { return app_info_->res_dir_; }
	
  protected:
	virtual void initialize_impl() = 0;
  };



	


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
	
	
  protected:
	drawframe_app()
	  : numOfBuffers_(2)
	  , threadStatus_(unintialized)
	  , mainthread_(boost::ref(*this))
	{}


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



	// フレーム開始のコールバック
	void begin_frame_callback() {
	  gr_.drawframe();
	  {
		scoped_lock lk(ef_guard_);
		wakeup_.notify_one();
	  }
	}
	bool is_running() const {
	  return threadStatus_ == running;
	}
	bool is_stopped() const {
	  return threadStatus_ == stopped;
	}
	
	
	void wait_for_frame(uint32_t frames=1) {
	  while (frames--) {
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
	  return;
	}
	
	// スレッドのメインルーチン
  public:
	void operator()() {
	  // 基本的に無限ループしておく
	  for(;;) {
		switch (threadStatus_) {
		case unintialized:
		  wait_for_frame();
		  break;
		case running:
		  main();
		  threadStatus_ = stopped;
		  break;
		case stopped:
		default:
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

	  
  };
  


} // namespace appfw
} // namespace ts

#endif
