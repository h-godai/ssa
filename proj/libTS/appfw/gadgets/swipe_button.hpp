// swipe_button.hpp -*-tab-width:4-*-
// 
// ドラッグすると左右に動いて消えるボタン
//
//

#ifndef TS_APPFW_GADGET_SWIPE_BUTTON_HPP
#define TS_APPFW_GADGET_SWIPE_BUTTON_HPP

#include "libTS/appfw/gl/glfw.hpp"
#include "libTS/appfw/gl/button.hpp"
#include "libTS/appfw/gl/effect.hpp"
#include "libTS/math/math.hpp"

namespace ts {
namespace appfw {
  using namespace std;
  using namespace boost;
  using ts::math::pos2_f;
  
  struct swipe_button {
	gl::button2d* buttons_;
    const size_t btncount_;
    uint32_t current_;
	// constant
	static const int32_t SwipeFactor = 1000;


	// wipe時のsound effect
	template <typename SoundT, typename SoundPlayer>
	struct se {
	  SoundPlayer& sound_;
	  SoundT s1_;
	  SoundT s2_;
	  se(SoundPlayer& sp, SoundT s1) : sound_(sp), s1_(s1), s2_(s1) {}
	  se(SoundPlayer& sp, SoundT s1, SoundT s2) : sound_(sp), s1_(s1), s2_(s2) {}

	  void operator() (int dir) {
		if (dir < 0) 
		  sound_.play(s1_);
		else if (dir > 0) 
		  sound_.play(s2_);
	  }
	};

    
    // 左右にWipeする動作
    struct default_wipe_effect {
	  gl::glfw& gr_;
	  pos2_f pos_;
	  uint32_t layer_;
	  default_wipe_effect(gl::glfw& gr, const pos2_f& pos, uint32_t layer) : gr_(gr), pos_(pos), layer_(layer)
	  {}
	  void operator () (bool is_wipein, gl::button2d& btn, int32_t direction, uint32_t number) const {
		if (is_wipein) {
		  pos2_f pos(pos_);
		  if (pos_.x_ == -1.0f) {
			pos.x_ = (gr_.screen_size().width_ - btn.viewsize().width_) / 2.0f;
		  }
		  btn.set_pos(pos);
		  gr_.show_object(btn, layer_, wipein(direction, number));
		}
		else {
		  gr_.hide_image(btn, wipeout(direction, number));
		}
	  }

	  gl::effect wipeout(int32_t direction, uint32_t /*outnumber*/) const {
		if (direction == 0) return gl::effect(gl::fade_out(1.0f));
		float xpos = gr_.screen_size().width_;
		if (direction < 0) xpos = -xpos + pos_.x_;
		return gl::effect(gl::move_to(pos2_f(xpos, pos_.y_), 0.2f)|gl::fade_out(1.0f));
	  }
	  gl::effect wipein(int32_t /*direction*/, uint32_t /*innumber*/) const {
		return gl::effect(gl::fade_in(0.1f));
	  }
	};
	
	template <size_t N>
	swipe_button(gl::button2d (&btns)[N])
	: buttons_(&btns[0])
	, btncount_(N)
	, current_(0)
	{}
	
	template <typename SoundEffect>
	bool check(gl::glfw& gr, const string& result, const pos2_f& pos, uint32_t layer, SoundEffect sef) {
	  return check(result, default_wipe_effect(gr, pos, layer), sef);
	}
	template <typename WipeEffect, typename SoundEffect>
	bool check(const string& result, WipeEffect Wef, SoundEffect Sef) {
	  assert(current_ < btncount_);
	  gl::button2d& hitbtn = buttons_[current_];
	  if (hitbtn.name() != result) return false;
	  int32_t direction = 0;
	  uint32_t newcur = current_ + 1;
	  if (hitbtn.action() == gl::button2d::dragged) {
		hitbtn.clear_action();
		float vecx = hitbtn.get_drag_vec().x_;
		if (fabs(vecx) > static_cast<float>(SwipeFactor)) {
		  if (vecx > 0) {
			newcur = current_ + btncount_ - 1;
			direction = -1;
		  }
		  else {
			direction = 1;
		  }
		}
		else {
		  return true;
		}
	  }
	  Sef(direction);
	  Wef(false, hitbtn, direction, current_);
	  current_ = newcur % btncount_;
	  Wef(true, buttons_[current_], direction, current_);
	  return  true;
	}
	
	gl::button2d& current_btn() { return buttons_[current_]; }
  };
  


}}
#endif
