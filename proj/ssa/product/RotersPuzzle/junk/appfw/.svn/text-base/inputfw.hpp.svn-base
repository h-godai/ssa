/* -*-tab-width:4-*- 
  input framework

*/

#ifndef TS_APPFW_INPUTFW_HPP
#define TS_APPFW_INPUTFW_HPP

#define DATE_TIME_INLINE
#include <iostream>
#include <queue>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>
#include <boost/thread.hpp>

namespace ts {
namespace appfw {
namespace in {
  using namespace std;
  using namespace boost;
  using namespace boost::posix_time;
  typedef boost::mutex::scoped_lock scoped_lock;

  enum input_device_type {
	button_device
	, mouse_device
	, tap_device
	, key_device
  };
  
  struct button_ctrl {
	static const input_device_type dev_tp = button_device;
	enum button_action {
	  no_action
	  , on
	  , off
	};
	button_action action_;
	button_ctrl() : action_(no_action) {}
  };

  struct pointer_ctrl {
	float x_;
	float y_;
	pointer_ctrl() : x_(0), y_(0) {}
	template<typename T>
	pointer_ctrl(T x, T y) : x_(static_cast<float>(x)), y_(static_cast<float>(y)) {}
  };

  struct mouse_ctrl {
	static const input_device_type dev_tp = mouse_device;
	button_ctrl left_btn_;
	button_ctrl right_btn_;
	button_ctrl center_btn_;
	pointer_ctrl pos_;
	mouse_ctrl() {}
  };

  struct tap_ctrl {
	static const input_device_type dev_tp = tap_device;
	enum tap_action {
	  no_action
	  , touch     // タッチした
	  , release   // 離した
	  , move      // 動かした
	};
	pointer_ctrl pos_;
	tap_action action_;
	tap_ctrl(tap_action act=no_action, float x=0, float y=0) : action_(act), pos_(x,y) {}
  };

  struct key_ctrl {
	static const input_device_type dev_tp = key_device;
	enum key_action {
	  no_action
	  , push
	  , release
	};
	key_action action_;
	uint32_t char_;
	uint32_t flags_;
	key_ctrl() : action_(no_action), char_(0), flags_(0) {}
  };

  struct event_t {
	typedef shared_ptr<event_t> ptr;
	ptime ptime_;
	uint32_t devnum_; // デバイス番号
	event_t(uint32_t devnum=0) : ptime_(microsec_clock::universal_time()), devnum_(devnum) {}
	virtual input_device_type device_type() const = 0;
  };
  
  template <typename Ctrl>
  struct event_ctrl : event_t {
	typedef shared_ptr<event_ctrl<Ctrl> > ptr;
	Ctrl ctrl_;
	event_ctrl(const Ctrl& ctrl, uint32_t devnum = 0) : event_t(devnum), ctrl_(ctrl) {}
	virtual input_device_type device_type() const { return Ctrl::dev_tp; }
  };

  typedef event_ctrl<tap_ctrl> tap_event;
  typedef event_ctrl<mouse_ctrl> mouse_event;
  typedef event_ctrl<key_ctrl> key_event;
  
  // 入力イベントのイベントハンドラ
  class event_handler {
	queue<event_t::ptr> eventQueue_;
	mutex guard_;
	boost::condition_variable_any     wakeup_;
  public:
	volatile bool quit_;
  public:
	event_handler() : quit_(false) {}

	template <typename Ctrl>
	void notify_event(const Ctrl& ctrl, uint32_t devnum=0) {
	  scoped_lock lock(guard_);
	  shared_ptr<event_ctrl<Ctrl> > p(new event_ctrl<Ctrl>(ctrl, devnum));
	  eventQueue_.push(p);
	  wakeup_.notify_one();
	}


	// イベントが来るまで待つ。(最大1秒)
	bool wait_for_event() {
	  scoped_lock lk(guard_);
	  // 現在の1秒後の時間を計算
	  boost::xtime xt;
	  boost::xtime_get(&xt, boost::TIME_UTC);
	  xt.sec += 1;
	  return wakeup_.timed_wait(lk, xt);
	}

	// イベントを処理する
	template <typename Pred>
	bool execute(Pred& cb) {
	  event_t::ptr e;
	  {
		scoped_lock lock(guard_);
		if (eventQueue_.empty()) return false;
		e = eventQueue_.front();
		eventQueue_.pop();
	  }
	  cb(e);
	  return true;
	}

	template <typename Pred>
	void listener(Pred& cb) {
	  quit_ = false;
	  while (!quit_) {
		if (wait_for_event()) {
		  // イベントが来てたら処理
		  while (execute(cb));
		}
	  };
	  return;
	}
	
	// listenerを中断する
	void stop_listener() {
	  quit_ = true;
	  wakeup_.notify_one();
	}
  };


}}} // namespace ts::appfw::in

#endif
