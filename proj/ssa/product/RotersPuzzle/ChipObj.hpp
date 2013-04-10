/* -*-tab-width:4-*-
 *  ChipObj.hpp

 */

#ifndef APP_CHIPOBJ_HPP
#define APP_CHIPOBJ_HPP

#include <math.h>
#include <vector>
#include <boost/random.hpp>
#include <boost/optional.hpp>
#include <boost/array.hpp>
#include <algorithm>
#include "libTS/appfw/appfw.hpp"

namespace app {
  using namespace ts::appfw;
  using namespace ts::math;

  struct ChipObj {
	static const bool event_process = false; // イベント処理の有無
	enum EffectType {
	  NoEffect
	  , TapIn
	  , TapOut
	  , Moving
	};
	gl::rectangle2d chip_;
	gl::rectangle2d chip2_; // スクロール途中用
    typedef gl::rectangle2d ImageType;

	uint32_t col_;
	uint32_t row_;
	pos2_i slt_;
	pos2_f target_;
	bool update_;
	gl::size_f padding_;
	ChipObj(uint32_t x, uint32_t y
			, gl::tex_image_t img
			, pos2_f pos
			, const gl::size_f& sz
			, const gl::size_f& padding)
	  : chip_(img, pos + padding, sz - (padding*2))
	  , chip2_(img, pos2_f(-999.0f,0.0f), sz - (padding*2))
	  , col_(x)
	  , row_(y)
	  , slt_(x,y)
	  , target_(pos)
	  , padding_(padding)
	  , update_(true)
	{}
	
	pos2_f pos() const { return chip_.rect_.pos_ - padding_; }
	rect_f rect() const { return chip_.rect_ + (padding_*2); }
	size_f viewsize() const { return rect().size_; }
	void setPos(const pos2_f& pos, const rect_f& rect) {
	  chip_.rect_.pos_ = pos + padding_; 
	  if (pos.x_ < rect.left()) setPos2(pos.x_ + rect.width(), pos.y_);
	  else if (pos.y_ > rect.bottom()-chip_.rect_.height()) setPos2(pos.x_ , pos.y_ - rect.height());
	  else if (pos.y_ < rect.top()) setPos2(pos.x_, pos.y_ + rect.height());
	  else if (pos.x_ > rect.right()-chip_.rect_.width()) setPos2(pos.x_ - rect.width(), pos.y_);
	  else { setPos2(-999.0f,0.0f); }
	}
	void setPos2(float x, float y) {
	  pos2_f p(x,y);
	  p += padding_;
	  chip2_.rect_.pos_ = p;

	}
    
    float depth() const { return 0.0f; }

	void set_effect(gl::effect::ptr ef) {}

	
	void effect(EffectType ef) {
	  switch(ef) {
	  case TapIn:
          chip_.set_effect(gl::effect::ptr(new gl::highlights()));
          chip2_.set_effect(gl::effect::ptr(new gl::highlights()));
		break;
	  case TapOut:
	  case NoEffect:
	  default:
          chip_.set_effect(gl::effect::ptr(new gl::no_effect()));
          chip2_.set_effect(gl::effect::ptr(new gl::no_effect()));
		break;
	  }
	  update_ = true;
	}
	void render(gl::context& gc) {
	  chip_.render(gc);
      chip_.post_render(gc);
	  chip2_.render(gc);
      chip2_.post_render(gc);
	}
    void post_render(gl::context& gc) {
    }
	
  };
} // namespace
#endif


