/* event.hpp -*-tab-width:4-*-


 */

#ifndef TS_APPFW_EVENT_HPP
#define TS_APPFW_EVENT_HPP


#define DATE_TIME_INLINE
#include <iostream>
#include <queue>
#include <string>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>
#include <boost/thread.hpp>
#include "object_name.hpp"

namespace ts {
namespace appfw {
namespace event {
  using boost::shared_ptr;
  using boost::posix_time::ptime;
  using boost::posix_time::microsec_clock;
  typedef boost::mutex::scoped_lock scoped_lock;

  
  enum device_types {
	button_device
	, mouse_device
	, tap_device
	, key_device
	, acc_device   // 加速度センサ
	, audio_device
	, video_device
  };

  struct event_t {
	typedef shared_ptr<event_t> ptr;
	ptime ptime_;
	uint32_t devnum_; // デバイス番号
	bool processed_; // 処理済みのフラグ trueのときは他のオブジェクトが処理を行った
	event_t(uint32_t devnum=0) : ptime_(microsec_clock::universal_time()), devnum_(devnum), processed_(false) {}
	virtual device_types device_type() const = 0;
	bool valid() const { return processed_ == false; }
  };

  template <typename Ctrl>
  struct event_ctrl : event_t {
	typedef shared_ptr<event_ctrl<Ctrl> > ptr;
	Ctrl ctrl_;
	event_ctrl(const Ctrl& ctrl, uint32_t devnum = 0) : event_t(devnum), ctrl_(ctrl) {}
	virtual device_types device_type() const { return Ctrl::dev_tp; }
  };

  // 入力イベントのイベントハンドラ
  class event_handler {
	std::queue<event_t::ptr> eventQueue_;
	boost::mutex guard_;
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
  
  // イベントを処理する基底クラス
  struct event_processor {
	typedef boost::shared_ptr<event_processor> ptr;
	virtual ~event_processor() {}
	virtual bool operator () (event_t::ptr ev)  = 0;
	virtual const object_name& name() const = 0;
	virtual int32_t priority() const = 0; // 優先度。少ない方が高い
  };
  inline bool operator < (event_processor::ptr& a, event_processor::ptr& b) {
	if (!a) return false;
	if (!b) return true;
	return a->priority() < b->priority();
  }

  // イベントを発生させるオブジェクトの基底クラス
  template <bool Value>
  struct event_generator {
  	static const bool event_process = Value; // ture:イベント処理あり
  };

  struct dummy_event_processor : event_processor {
	object_name name_;
	dummy_event_processor(const char* name) : name_(name) {}
	virtual bool operator () (event_t::ptr ev) { return event_t::ptr(); }
	virtual const object_name& name() const { return name_; }
	virtual int32_t priority() const { return 0; }
  };




}}} // ts::appfw::event
#endif
