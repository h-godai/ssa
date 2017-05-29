/* text.hpp -*-tab-width:4-*-
   libTS glfw  Graphics Language Frame Work

 */

#ifndef TS_APPFW_GL_TEXT_HPP
#define TS_APPFW_GL_TEXT_HPP

#include <string>
#include "../platform.hpp"
#include "effect.hpp"
#include "libTS/math/vec.hpp"
#include "libTS/Image/Color.hpp"


namespace ts {
namespace appfw {
namespace gl {
  using ts::gl::color_rgba;
  using ts::gl::f2b;
  

  using math::rect_f;
  using math::size_f;
  using math::pos2_f;
  using ts::appfw::gl::rc;

  struct textdata {
	std::string text_;
	rect_f rect_;
	float fontsize_; // 0 is unknown
	color_rgba color_;
	textdata(const char* txt
			 , pos2_f pos = pos2_f()
			 , const color_rgba& col = color_rgba()
			 , int fontsize=0)
	  : text_(txt)
	  , rect_(pos, rc::get_textsize(txt, fontsize))
	  , fontsize_(fontsize)
	  , color_(col)
	{}
	textdata(const std::string& txt
			 , pos2_f pos = pos2_f()
			 , const color_rgba& col = color_rgba()
			 , int fontsize=0)
	  : text_(txt)
	  , rect_(pos, rc::get_textsize(txt, fontsize))
	  , fontsize_(fontsize)
	{}
  };

  
  template <typename ImgTyp>
  struct text2d_t {
	typedef ImgTyp ImageType;
	static const bool event_process = false; // イベント処理なし
	ImgTyp text_;
	rect_f rect_;
	GLfloat depth_;
	pos2_f stpos_;
	float stdepth_;
	bool texture_repeat_;
	float scale_;
	float rscale_;
	color_rgba col_;
	rect_f scaled_rect_;
	float rotate_;
  protected:
	effect::ptr effect_;
  public:

	text2d_t()
	  : depth_(0)
	  , texture_repeat_(false)
	  , scale_(1.0f)
	  , rotate_(NoRotate)
	{}

	text2d_t(ImgTyp txt, const pos2_f& pos=pos2_f(), size_f size=size_f(), float dep=0.0f)
	  : text_(txt)
	  , rect_(pos, size)
	  , depth_(dep)
	  , texture_repeat_(false)
	  , scale_(1.0f)
	  , col_(text_.color_)
	  , rotate_(NoRotate)
	{
	  set_rect_size(size);
	  set_size(rect_.size_);
	}

	float depth() const { return depth_; }
	
	const rect_f& rect() const { return rect_; }
	const size_f& size() const { return rect_.size_; }

	//const size_f& img_size() const { return img_.rect_.size_; }

	void set_color(float r, float g, float b, float a=1.0f) {
	  col_ = color_rgba(r,g,b,a);
	}

	void set_pos(const pos2_f& pos) {
	  rect_.pos_ = pos;
	}

	void set_rect(const rect_f& rect) {
	  rect_.pos_ = rect.pos_;
	  set_rect_size(rect.size_);
	  set_size(rect_.size_);
	}

	void set_rect_size(size_i size) {
	  if (size.empty()) {
		size = size_i(32,32); // img_.rect_.size_;
		rect_.set_size(size);
	  }
	  else {
		rect_.set_size(size);
	  }
	}


	bool is_inside(const pos2_f& pos) const {
	  return scaled_rect_.is_inside(pos);
	}

	void set_size(const size_f& size) {
	}

	void set_image(ImgTyp& txt) { text_ = txt; }
	ImgTyp& get_image() { return text_; }

	void set_effect(effect::ptr ef) {
	  effect_ = ef;
	}
	effect::ptr get_effect() {
	  return effect_;
	}

	void add_effect(effect ef) {
	  if (!effect_) 
		effect_ = effect::ptr(new effect(ef));
	  else
		*effect_ |= ef;
	}
	void remove_effect(const effect& ef) {
	  if (!effect_) return;
	  *effect_ -= ef;
	}

	void make_effect(pos2_f& posofs) {
	  if (effect_->has(effect::scale_to)) {
		float progress = effect_->get<scale_to>().progress_;
		float scale1 = effect_->get<scale_to>().param1_;
		float scale2 = effect_->get<scale_to>().param2_;
		if (progress == 0.0f) {
		  scale_ = scale1;
		}
		else if (progress == 1.0f) {
		  scale_ = scale2;
		}
		else {
		  scale_ = scale1*(1.0f - progress) + (scale2 * progress);
		}
	  }
	  rscale_ = scale_;
	  if (effect_->has(effect::tapin_effect)) {
		float progress = effect_->get<tapin_effect>().progress_;
		float scale = 1.0f;
		if (progress == 0.0f) {
		  scale = 0.95f;
		}
		else if (progress == 1.0f) {
		  scale = 1.0f;
		}
		else {
		  scale = 0.95f*(1.0f - progress) +  progress;
		}
		rscale_ *= scale;
	  }
	  if (effect_->has(effect::tapout_effect)) {
		tapout_effect::impl_t& e = effect_->get<tapout_effect>();
		pos2_f ofs = e.param1_;
		float progress = e.progress_;
		if (progress < 0.6f) {
		  posofs = ofs * (1.0f - (2.0f*progress)); // max -0.2f
		}
		else {
		  progress -= 0.6f; // 0 to 0.3 -> -0.2 to 0.1
		  if (progress < 0.3f) {
			posofs = ofs * (progress - 0.2f);
		  }
		  else { // 0 to 0.1 -> 0.1 to 0
			posofs = ofs * -(progress - 0.3f);
		  }
		}
	  }
	  else if (effect_->has(effect::tapmove_effect)) {
		posofs = effect_->get<tapmove_effect>().param1_;
	  }
	  if (effect_->has(effect::rotate_to)) {
		float progress = effect_->get<rotate_to>().progress_;
		float th1 = effect_->get<rotate_to>().param1_;
		float th2 = effect_->get<rotate_to>().param2_;
		if (progress == 0.0f) {
		  rotate_ = th1;
		}
		else if (progress == 1.0f) {
		  rotate_ = th2;
		}
		else {
		  rotate_ = th1*(1.0f - progress) + (th2 * progress);
		}
	  }
	  if (effect_->has(effect::beat_effect)) {
		rscale_ *= effect_->get<beat_effect>().get_scale();
	  }
	}

	bool make_effect2() {
	  bool haseffect = false;
	  if (effect_->has(effect::xy_rotation)) {
		glRotatef(effect_->get<xy_rotation>().param_, 0.0f,0.0f,1.0f);
	  } else if (rotate_ != NoRotate) {
		glTranslatef(rect_.size_.width_/2.0f, rect_.size_.height_/2.0f,  0.0f);
		glRotatef(rotate_, 0.0f,0.0f,1.0f);
		glTranslatef(-rect_.size_.width_/2.0f, -rect_.size_.height_/2.0f,  0.0f);
	  }


	  if (effect_->has(effect::transparent)) {
		//glEnable(GL_BLEND);
		glColor4ub(col_.r, col_.g, col_.b, f2b(effect_->get<transparent>().param_));
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		haseffect = true;
	  }
	  if (effect_->has(effect::flicker)) {
		//glEnable(GL_BLEND);
		glColor4ub(col_.r, col_.g, col_.b, f2b(effect_->get<flicker>().flick_ ? 1.0f : 0.4f));
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		haseffect = true;
	  }
	  if (effect_->has(effect::fade_out)) {
		//glEnable(GL_BLEND);
		glColor4ub(col_.r, col_.g, col_.b,f2b(1.0f - effect_->get<fade_out>().progress_));
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		haseffect = true;
	  }
	  else if (effect_->has(effect::transparent_to)) {
		float progress = effect_->get<transparent_to>().progress_;
		float t1 = effect_->get<transparent_to>().param1_;
		float t2 = effect_->get<transparent_to>().param2_;
		float t =  t1*(1.0f - progress) + (t2 * progress);
		glColor4ub(col_.r, col_.g, col_.b, f2b(t));
		haseffect = true;
	  }
	  else if (effect_->has(effect::fade_in)) {
		glColor4ub(col_.r, col_.g, col_.b, f2b(effect_->get<fade_in>().progress_));
		haseffect = true;
	  }
	  else if (effect_->has(effect::highlights)) {
		//glEnable(GL_BLEND);
		//glDisable(GL_TEXTURE_2D);
		//glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glColor4ub(col_.r, col_.g, col_.b, col_.a);
		//glVertexPointer(3, GL_FLOAT, 0, vertices_);
		//glEnableClientState(GL_VERTEX_ARRAY);
		//glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		//glColor4f(1.0f,1.0f,1.0f,0.3f);
		//glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_COLOR);
		//glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
		//glBlendFunc(GL_ONE, GL_ONE);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		haseffect = true;
	  }
	  return haseffect;
	}

	void render(context& gc) {
	  // mirror effect
	  if (effect_ && effect_->has(effect::mirror_lr)) {
		//set_imguv(effect_->get<mirror_lr>().param_);
		effect_->get<mirror_lr>().done_ = true;
	  }

	  // 移動アニメーション
	  if (effect_) set_move_animation();

	  // 前段階でのエフェクト
	  pos2_f posofs(0,0);
	  scaled_rect_ = rect_;
	  rscale_ = scale_;
	  if (effect_) make_effect(posofs);
	  if (rscale_ != 1.0f) {
		posofs += (rect_.size_ * (1.0f-rscale_)) / 2.0f;
		scaled_rect_.pos_ = rect_.pos_ + posofs;
		scaled_rect_.size_ = rect_.size_ * rscale_;
	  }



	  // テクスチャ処理
	  glDisable(GL_TEXTURE_2D);

	  //glEnable(GL_DEPTH_TEST);
	  //depth_ = -0.5f;

	  // glDisable(GL_ALPHA_TEST);
	  

	  //glLoadIdentity();
	  glPushMatrix();

	  glTranslatef(rect_.pos_.x_ + posofs.x_, rect_.pos_.y_ + posofs.y_, -0.1f-depth_);
	  if (rscale_ != 1.0f) {
		glScalef(rscale_,rscale_,rscale_);
	  }

	  bool haseffect = false;
	  if (effect_) haseffect = make_effect2();
	  
	  if (!haseffect) {
		glEnable(GL_BLEND);
		glColor4ub(col_.r, col_.g, col_.b, col_.a);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	  }

	  rc::render_string(text_.text_);
	}
	void post_render(context&) {
	  glPopMatrix();
	}

	void set_move_animation() {
	  if (!effect::valid(effect_)) return;
	  if (effect_->has(effect::move_to)) {
		float progress = effect_->get<move_to>().progress_;
		if (progress == 0.0f) {
		  stpos_ = rect_.pos_;
		}
		else if (progress == 1.0f) {
		  rect_.pos_ = get_pos(rect_.pos_, effect_->get<move_to>().param1_);
		}
		else {
		  math::vec2_f v(stpos_, get_pos(rect_.pos_, effect_->get<move_to>().param1_));
		  rect_.pos_ = stpos_ + (v * progress);
		}
	  }
	  else if (effect_->has(effect::move_to3)) {
		float progress = effect_->get<move_to3>().progress_;
		pos3_f pos3 = effect_->get<move_to3>().param1_;
		if (progress == 0.0f) {
		  stpos_ = rect_.pos_;
		  stdepth_ = depth_;
		}
		else if (progress == 1.0f) {
		  rect_.pos_ = pos2_f(pos3);
		  depth_ = pos3.z_;
		}
		else {
		  math::vec2_f v(stpos_, pos2_f(pos3));
		  rect_.pos_ = stpos_ + (v * progress);
		  depth_ = stdepth_ * (1.0f-progress) + (pos3.z_ * progress);
		}
	  }
	}

  };


  typedef text2d_t<textdata> text2d;
  typedef text2d_t<textdata&> text2d_ref;


}}} // ts::appfw::gl
#endif
