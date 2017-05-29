/* -*-tab-width:4-*-
   libTS appfw.hpp
 */
#ifndef TS_APPFW_APPFW_HPP
#define TS_APPFW_APPFW_HPP

#include "libTS/Common/Common.h"
#include <stdint.h>
#include <iostream>
#include <string>
#include <queue>
#include <stdexcept>
//#include <boost/shared_ptr.hpp>
#include <boost/optional.hpp>

#include "libTS/Util/Logger.h"
#include "libTS/math/pos.hpp"
#include "exception.hpp"
#include "inputfw.hpp"
#include "gl/glfw.hpp"
#include "sound/soundfw.hpp"

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

  // アプリからシステムに対するコマンド
  struct system_command {
	string cmd_;
	string param_;
	system_command() {}
	system_command(const string& cmd, const string& param)
	  : cmd_(cmd), param_(param)
	{}
  };

  // アプリケーションフレームワークのベースクラス
  class app_framework {
  protected:
	
	app_info::ptr app_info_;
	event::event_handler input_eh_; // 入力デバイスのイベントハンドラ
	std::map<std::string, bool> eventProceeded_; // イベント処理したオブジェクト名のリスト
  public:
	sounds::audio_ctrl& audio_;
	sounds::music_player music_;
	sounds::sound_player sound_;
	float fps_;
	float rendertime_;
	float tap_device_scale_factor_; // タップデバイスの座標の倍率
	volatile double time_now_; // 現在時刻(time_tのdouble版)
	boost::mutex cmdmutex_;
	vector<system_command> systemcmd_;


  protected:
	app_framework() 
	  : audio_(sounds::audio_ctrl::instance()) 
	  , music_(audio_)
	  , sound_(audio_)
	  , fps_(0.0f)
	  , rendertime_(0.0f)
	  , tap_device_scale_factor_(1.0f)

	{}
	virtual ~app_framework() throw() {}

  public:
	// フレームワークがアクセスするアプリケーションインスタンス
	static app_framework& app_instance();

	// 初期化
	void initialize(app_info::ptr info) {
	  app_info_ = info;
	  initialize_impl();
	}

	void set_fps(float fps, float rendertime) { fps_ = fps; rendertime_ = rendertime; }

	// サウンドリソースファイルのリストを取得する
	virtual void get_sound_list(std::vector<std::string>&) {}
	virtual void get_music_list(std::vector<std::string>&) {}


	// system command control hook
	void system_command_hook(std::vector<system_command>& list) {
	  scoped_lock lock(cmdmutex_);
	  copy(systemcmd_.begin(), systemcmd_.end(), back_inserter(list));
	  systemcmd_.clear();
	}

	void send_system_command(const string& cmd, const string& param) {
	  scoped_lock lock(cmdmutex_);
	  systemcmd_.push_back(system_command(cmd,param));
	}


	// Audioコントロールのフック
	void audio_control_hook(std::vector<sounds::audio_param>& list) {
      sounds::audio_ctrl::instance().control_hook(list);
	}

	// 演奏中の音楽の再生時間を設定する
	// 実行環境のスレッドで呼ばれる
	void set_current_music_time(float tim) {
	  music_.current_music_time_ = tim;
	}

	// アップデート通知
	virtual void update() {}

	// 入力フレームワークへのアクセス
	in::event_handler& input_eh() {
	  return input_eh_;
	}
	
	// イベント通知
	// 環境依存のスレッドから呼ばれる
	template <typename Ctrl>
	void notify_event(const Ctrl& ctrl, uint32_t devnum=0) {
	  input_eh_.notify_event(ctrl, devnum);
	}

	// イベント処理
	//  appfwのスレッドで実行される
	void process_events() {
	  while (input_eh_.execute(*this));
	}
	// イベント処理ルーチン
	// appfwのスレッドで実行される
	// input_eh_executeから呼ばれる。
	void operator () (event::event_t::ptr ev) {
	  event::event_processor::ptr e = event_handling_impl(ev);
	  // イベント処理したオブジェクトがある場合、通知する
	  if (e) {
		on_event_proseeded(e, ev);
	  }
	}


	virtual void wait_for_sec(float second) = 0;

	// ボタンなどのイベントが来るまでまつ
	// 単位は秒
	boost::optional<const std::string&> wait_for_event(float timeout=0) {
	  double timeouttime = time_now_ + timeout;
	  eventProceeded_.clear();
	  while (eventProceeded_.empty()) {
		wait_for_sec(0);
		if (timeout != 0 && time_now_ > timeouttime) break;
	  }
	  if (!eventProceeded_.empty()) {
		return boost::optional<const std::string&>(eventProceeded_.begin()->first);
	  }
	  return boost::none;
	}	  

	// イベントが処理された事の通知
	// イベントがオブジェクトによって処理された時にコールバックされる
	//  appfwのスレッドで実行される
	virtual void on_event_proseeded(event::event_processor::ptr e, event::event_t::ptr) {
	  eventProceeded_[e->name()] = true;
	}


	// サービスメソッド
	const string& application_dir() const { return app_info_->app_dir_; }
	const string& documentation_dir() const { return app_info_->doc_dir_; }
	const string& resource_dir() const { return app_info_->res_dir_; }
	
  protected:
	// イベント処理ルーチン
	// appfwのスレッドで実行される
	virtual void initialize_impl() = 0;

	// イベント処理ルーチン
	//  appfwのスレッドで実行される
	virtual event::event_processor::ptr event_handling_impl(event::event_t::ptr ev) {
	  switch (ev->device_type()) {
	  case event::tap_device:
		on_tap_ctrl(boost::static_pointer_cast<in::tap_event>(ev));
		break;
	  case event::acc_device:
		on_acc_ctrl(boost::static_pointer_cast<in::acc_event>(ev));
		break;
	  case event::key_device:
		on_key_ctrl(boost::static_pointer_cast<in::key_event>(ev));
		break;
	  default:
		break;
	  }
	  return event::event_processor::ptr();
	}

  public:
	virtual void position_correct(math::pos2_f& pos) {}
	virtual void vector_correct(math::vec2_f& pos) {}

	virtual void on_key_ctrl(in::key_event::ptr evt) {
	  TS_DEBUGLOG("on_key_ctrl(0x%x)\n", evt->ctrl_.char_);
	}
	virtual void on_tap_ctrl(in::tap_event::ptr evt) {
	  TS_DEBUGLOG("on_tap_ctrl(#%d, act:%d, (%.2f,%.2f)\n"
				  , evt->devnum_
				  , evt->ctrl_.action_
				  , evt->ctrl_.pos_.x_
				  , evt->ctrl_.pos_.y_);
	}
	
	virtual void on_acc_ctrl(in::acc_event::ptr evt) {
	  TS_DEBUGLOG("on_acc_ctrl(#%d, (%.2f,%.2f,%.2f)\n"
				  , evt->devnum_
				  , evt->ctrl_.acc_.x_
				  , evt->ctrl_.acc_.y_
				  , evt->ctrl_.acc_.z_);

	}


	virtual void tap_device_position_correct(math::pos2_f& pos) {
	  pos *= tap_device_scale_factor_;
	  position_correct(pos);
	}

	
	
  };


} // namespace appfw
} // namespace ts

#endif
