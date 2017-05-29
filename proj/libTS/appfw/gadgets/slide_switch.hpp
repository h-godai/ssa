/** @file slide_switch.hpp -*-tab-width:4-*-
// 
// @brief スライドスイッチ
//
**/

#ifndef TS_APPFW_GADGET_SLIDE_SWITCH_HPP
#define TS_APPFW_GADGET_SLIDE_SWITCH_HPP

#include <string>
#include "libTS/math/pos.hpp"
#include "libTS/math/rect.hpp"
#include "libTS/appfw/gl/glfw.hpp"
#include "libTS/appfw/gl/image.hpp"
#include "libTS/appfw/gl/effect.hpp"

namespace ts {
namespace appfw {
  using ts::math::pos2_f;
  using ts::math::rect_f;

  struct slide_switch {
    gl::glfw& gl_;
    std::string name_;
    pos2_f pos_;
    pos2_f onoffpos_;
    gl::image32 offimg_;
    gl::image32 onimg_;
    gl::image32 arrowimg_;
    float progress_;
    bool tap_in_;
    pos2_f tapin_pos_;
    int32_t layer_;
    gl::glfw::show_image_handle imgh_;
    gl::glfw::show_image_handle arrowh_;
    float scale_;

    slide_switch(gl::glfw& gl, const std::string& name)
      : gl_(gl)
      , name_(name)
      , progress_(0.0f)
      , tap_in_(false)
      , layer_(0)
      , scale_(1.0f)
    {}
    
	  slide_switch(gl::glfw& gl
				   , const std::string& name
				   , const pos2_f& pos
				   , gl::image32 offimg
				   , gl::image32 onimg
				   , gl::image32 arrowimg)
		: gl_(gl)
		, name_(name)
		, pos_(pos)
		, offimg_(offimg)
		, onimg_(onimg)
		, arrowimg_(arrowimg)
		, progress_(0.0f)
		, tap_in_(false)
		, layer_(0)
		, onoffpos_(pos + pos2_f(arrowimg.viewsize().width_, 0.0f))
		, scale_(1.0f)
	  {}

	  void set_scale(float sc) { scale_ = sc; }

	  void show(const pos2_f& pos
				, gl::image32 offimg
				, gl::image32 onimg
				, gl::image32 arrowimg
				, int32_t layer
				, gl::effect ef = gl::effect()) {
		pos_ = pos;
		offimg_ = offimg;
		onimg_ = onimg;
		arrowimg_ = arrowimg;
		layer_ = layer;
		onoffpos_ = pos_ + pos2_f(scale_*arrowimg.viewsize().width_, 0.0f);

		show(ef);
	  }
	  void show(gl::effect ef = gl::effect()) {
		gl_.show_image(name_.c_str()
					 , offimg_
				     , rect_f(onoffpos_, offimg_.viewsize()*scale_)
					 , layer_
					 , ef);
		arrowh_ = gl_.show_image((name_+"a").c_str()
							   , arrowimg_
				 			   , rect_f(pos_, arrowimg_.viewsize()*scale_)
							   , layer_+1
							   , ef);
		progress_ = 0.0f;
	  }

	  void hide() {
		gl_.hide_image(name_.c_str());
		gl_.hide_image((name_+"a").c_str());
	  }
	  
	  void tap_in(const pos2_f& p) {
		tap_in_ = true;
		tapin_pos_ = p;
		progress_ = 0.0f;
		imgh_ = gl_.show_image((name_+"2").c_str()
							   , onimg_
							   , rect_f(onoffpos_, onimg_.viewsize()*scale_)
							   , layer_);
		on_tap_move(p);
	  }


	  bool on_tap_in(const pos2_f& p) {
		if (!rect_f(pos_, onimg_.viewsize()*scale_).is_inside(p)) return false;
		tap_in(p);
		return true;
	  }

	  bool on_tap_out(const pos2_f& p) {
		if (tap_in_) {
		  if (progress_ != 1.0f) {
			progress_ = 0.0f;
			gl_.effect_image(*arrowh_, gl::move_to(pos_, 0.2f));
		  }
		  tap_in_ = false;
		  gl_.hide_image((name_+"2").c_str());
		  return true;
		}
		return false;
	  }

	  bool on_tap_move(const pos2_f& p) {
		if (!tap_in_) {
		  if (!rect_f(pos_, onimg_.viewsize()*scale_).is_inside(p)) return false;
		  tap_in(p);
		  return true;
		}
		progress_ = (p.x_ - tapin_pos_.x_) / (scale_*(offimg_.viewsize().width_ + arrowimg_.viewsize().width_));
		if (progress_ < 0.0f) return true;
		if (progress_ > 1.0f) progress_ = 1.0f;
		float x = (scale_*offimg_.viewsize().width_ + (scale_ * arrowimg_.viewsize().width_)) * progress_;
		gl_.effect_image(*arrowh_, gl::effect()); // clear old effect
		arrowh_->set_pos(pos2_f(pos_.x_ + x, pos_.y_));
		rect_f r = onimg_.rect();
		r.size_.width_ = r.size_.width_ * progress_;
		if (imgh_) {
		  imgh_->set_image(gl::image32(onimg_, r, onimg_.scale_));
		  r.size_ *= scale_;
		  imgh_->set_rect(rect_f(onoffpos_, r.size_ * onimg_.scale_));
		}
		return true;
	  }

	  bool is_finished() const { return !tap_in_ && progress_ == 1.0f; }
	};



}} // ts::appfw
#endif
