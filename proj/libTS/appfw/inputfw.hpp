/* -*-tab-width:4-*- 
  input framework

*/

#ifndef TS_APPFW_INPUTFW_HPP
#define TS_APPFW_INPUTFW_HPP

#include "libTS/math/vec.hpp"
#include "libTS/math/pos.hpp"
#include "event.hpp"


namespace ts {
namespace appfw {
namespace in {
  using namespace std;
  using namespace boost;
  using namespace boost::posix_time;
  typedef boost::mutex::scoped_lock scoped_lock;
  using namespace event;
  using math::pos2_f;
  
  struct button_ctrl {
	static const event::device_types dev_tp = button_device;
	enum button_action {
	  no_action
	  , on
	  , off
	};
	button_action action_;
	button_ctrl() : action_(no_action) {}
  };

  struct pointer_ctrl : pos2_f {
  };

  struct mouse_ctrl {
	static const event::device_types dev_tp = mouse_device;
	button_ctrl left_btn_;
	button_ctrl right_btn_;
	button_ctrl center_btn_;
	pointer_ctrl pos_;
	mouse_ctrl() {}
  };

  struct tap_ctrl {
	static const event::device_types dev_tp = tap_device;
	enum tap_action {
	  no_action
	  , touch     // タッチした
	  , release   // 離した
	  , move      // 動かした
	};
	tap_action action_;
	pos2_f pos_; // pointer_ctrl pos_;
	tap_ctrl(tap_action act, const pos2_f& pos) : action_(act), pos_(pos) {}
  };

  struct key_ctrl {
	static const event::device_types dev_tp = key_device;
	enum key_action {
	  no_action
	  , pushed
	  , released
	};
	key_action action_;
	uint32_t char_;
	uint32_t flags_;
	key_ctrl() : action_(no_action), char_(0), flags_(0) {}
	key_ctrl(key_action act, uint32_t ch, uint32_t flags=0) 
	  : action_(act), char_(ch), flags_(flags) {}
  };

  struct acc_ctrl {
	static const event::device_types dev_tp = acc_device;
	enum acc_action {
	  no_action
	  , sensed
	};
	acc_action action_;
	math::vec3_f acc_;
	acc_ctrl() : action_(no_action) {}
	acc_ctrl(float x, float y, float z) : action_(sensed), acc_(x,y,z)  {}
  };

  typedef event::event_ctrl<tap_ctrl> tap_event;
  typedef event::event_ctrl<mouse_ctrl> mouse_event;
  typedef event::event_ctrl<key_ctrl> key_event;
  typedef event::event_ctrl<acc_ctrl> acc_event;

}}} // namespace ts::appfw::in

#endif
