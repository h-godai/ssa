/* rectangle.hpp -*-tab-width:4-*-
   libTS glfw  Graphics Language Frame Work

 */

#ifndef TS_APPFW_GL_RECTANGLE_HPP
#define TS_APPFW_GL_RECTANGLE_HPP

#include "image.hpp"
#include "effect.hpp"
#include "libTS/math/vec.hpp"

namespace ts {
namespace appfw {
namespace gl {

  using math::rect_f;
  using math::size_f;
  using math::pos2_f;

  const float NoRotate = 360.0f * 1000.0f;
  
  template <typename ImgTyp>
  struct rectangle2d_t {
	typedef ImgTyp ImageType;
	static const bool event_process = false; // イベント処理なし
	ImgTyp img_;
	rect_f rect_;
	GLfloat vertices_[12]; // [0,1,2][3,4,5][6,7,8][9,10,11]
	GLfloat squareTexCoords[8];
	GLfloat depth_;
	pos2_f stpos_;
	float stdepth_;
	bool texture_repeat_;
	float scale_;
	float rscale_;
	float r_, g_, b_, a_;
	rect_f scaled_rect_;
	float rotate_;
	int glStack_; // for debug
  protected:
	effect::ptr effect_;
  public:

	rectangle2d_t()
	  : depth_(0)
	  , texture_repeat_(false)
	  , scale_(1.0f)
	  , r_(1.0f)
	  , g_(1.0f)
	  , b_(1.0f)
	  , a_(1.0f)
	  , rotate_(NoRotate)
	  , glStack_(0) // for debug
	{}

	rectangle2d_t(ImgTyp img, const pos2_f& pos=pos2_f(), size_f size=size_f(), float dep=0.0f)
	  : img_(img)
	  , rect_(pos, size)
	  , depth_(dep)
	  , texture_repeat_(false)
	  , scale_(1.0f)
	  , r_(1.0f)
	  , g_(1.0f)
	  , b_(1.0f)
	  , a_(1.0f)
	  , rotate_(NoRotate)
	  , glStack_(0) // for debug
	{
	  set_rect_size(size);
	  vertices_[0] = vertices_[1] = vertices_[4] = vertices_[6] = 0.0f;
	  vertices_[2] = vertices_[5] = vertices_[8] = vertices_[11] = dep;
	  set_size(rect_.size_);
	}
	rectangle2d_t(ImgTyp img, const rect_f& rect, float dep=0.0f)
	  : img_(img)
	  , rect_(rect)
	  , depth_(dep)
	  , texture_repeat_(false)
	  , scale_(1.0f)
	  , r_(1.0f)
	  , g_(1.0f)
	  , b_(1.0f)
	  , a_(1.0f)
	  , rotate_(NoRotate)
	  , glStack_(0) // for debug
	{
	  set_rect_size(rect.size_);
	  vertices_[0] = vertices_[1] = vertices_[4] = vertices_[6] = 0.0f;
	  vertices_[2] = vertices_[5] = vertices_[8] = vertices_[11] = dep;
	  set_size(rect_.size_);
	}

	float depth() const { return depth_; }
	
	const rect_f& rect() const { return rect_; }
	const size_f& size() const { return rect_.size_; }

	const size_f& img_size() const { return img_.rect_.size_; }

	void set_color(float r, float g, float b, float a=1.0f) {
	  r_ = r; g_ = g; b_= b; a_ = a;
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
		//		size = img_.rect_.size_;
		size = img_.viewsize(); // rect_.size_;
		rect_.set_size(size);
	  }
	  else if (size.width_ < 0 && size.height_ < 0) {
		// サイズがマイナスの場合はミラーリング
		size *= -1;
		rect_.set_size(size);
		img_.set_uvrect(rect_f(0.0f, 0.0f, float(size.width_) / img_.original_size().width_
							             , float(size.height_) / img_.original_size().height_));
		texture_repeat_ = true;
	  }
	  else if (size.width_ <= 8.0f && size.height_ <= 8.0f) {
		// 8x8以下のサイズ指定は倍率
		size.width_ *= img_.rect_.size_.width_;
		size.height_ *= img_.rect_.size_.height_;
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
	  vertices_[3] = vertices_[9] = static_cast<float>(size.width_);
	  vertices_[7] = vertices_[10] = static_cast<float>(size.height_);
	  set_imguv();
	}
	void set_imguv(bool mirror_lr=false) {
	  rect_f r = img_.uvrect(); // 表示する矩形のUV座標を得る
	  if (mirror_lr) {
		squareTexCoords[0] = r.right(); //r.left();
		squareTexCoords[4] = r.right(); //r.left();
		squareTexCoords[2] = r.left(); //r.right();
		squareTexCoords[6] = r.left(); //r.right();
		squareTexCoords[1] = r.top();
		squareTexCoords[3] = r.top();
		squareTexCoords[5] = r.bottom();
		squareTexCoords[7] = r.bottom();
	  }
	  else {
		squareTexCoords[0] = r.left();
		squareTexCoords[1] = r.top();
		squareTexCoords[2] = r.right();
		squareTexCoords[3] = r.top();
		squareTexCoords[4] = r.left();
		squareTexCoords[5] = r.bottom();
		squareTexCoords[6] = r.right();
		squareTexCoords[7] = r.bottom();
	  }
	}
	void set_image(ImgTyp& img) { img_ = img; set_imguv(); }
	ImgTyp& get_image() { return img_; }
	const ImgTyp& get_image() const { return img_; }

	void set_effect(effect::ptr ef) {
	  //TS_DEBUGLOG("rectangle2d::set_effect\n");
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
		glEnable(GL_BLEND);
		glColor4f(r_, g_, b_, effect_->get<transparent>().param_);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		haseffect = true;
	  }
	  if (effect_->has(effect::flicker)) {
		glEnable(GL_BLEND);
		glColor4f(r_, g_, b_, effect_->get<flicker>().flick_ ? 1.0f : 0.4f);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		haseffect = true;
	  }
	  if (effect_->has(effect::fade_out)) {
		glEnable(GL_BLEND);
		glColor4f(1.0f,1.0f,1.0f,1.0f - effect_->get<fade_out>().progress_);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		haseffect = true;
	  }
	  else if (effect_->has(effect::transparent_to)) {
		float progress = effect_->get<transparent_to>().progress_;
		float t1 = effect_->get<transparent_to>().param1_;
		float t2 = effect_->get<transparent_to>().param2_;
		float t =  t1*(1.0f - progress) + (t2 * progress);
		glEnable(GL_BLEND);
		glColor4f(1.0f,1.0f,1.0f,t);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		haseffect = true;
	  }
	  else if (effect_->has(effect::fade_in)) {
		glEnable(GL_BLEND);
		glColor4f(1.0f,1.0f,1.0f, effect_->get<fade_in>().progress_);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		haseffect = true;
	  }
	  else if (effect_->has(effect::highlights)) {
		glEnable(GL_BLEND);
		glDisable(GL_TEXTURE_2D);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glColor4f(r_, g_, b_, a_);
		glVertexPointer(3, GL_FLOAT, 0, vertices_);
		glEnableClientState(GL_VERTEX_ARRAY);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glColor4f(1.0f,1.0f,1.0f,0.3f);
		//glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_COLOR);
		//glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
		//glBlendFunc(GL_ONE, GL_ONE);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		haseffect = true;
	  }
	  return haseffect;
	}

	void render(context& gc) {
	  //assert(img_.data_);
	  //if (!img_.data_) {
	  //TS_DEBUGLOG("rectangle2d: img is null\n");
	  //return;
	  //}

	  // mirror effect
	  if (effect_ && effect_->has(effect::mirror_lr)) {
		set_imguv(effect_->get<mirror_lr>().param_);
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
	  if (img_.has_image()) {
		if (img_.handle() == image32::invalid_handle) {
		  img_.lock(); // unlockまでnothrowを確認すること
		  if (img_.handle() == image32::invalid_handle) {
			uint8_t* buffer = &view(img_.data_->pixel_)[0][0];
			GLuint th = 0;
			glGenTextures(1, &th); 
			img_.set_handle(th);
			img_.unlock();
			glBindTexture(GL_TEXTURE_2D, th); 
			if (texture_repeat_) {
			  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
			  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
			}
			else {
			  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
			  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); 
			}
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_.original_size().width_, img_.original_size().height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer); 
			//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D, textureid_, 0); 
			
		  }
		  else {
			img_.unlock();
			glBindTexture(GL_TEXTURE_2D, img_.handle()); 
		  }
		}
		else {
		  //img_.unlock();
		  if (gc.lastTexture_ != img_.handle()) {
			glBindTexture(GL_TEXTURE_2D, img_.handle()); 
			gc.lastTexture_ = img_.handle();
		  }
		}
	  }

	  // disable multi texture
	  glClientActiveTexture(GL_TEXTURE1);
	  glActiveTexture(GL_TEXTURE1);
	  glDisable(GL_TEXTURE_2D);
	  glClientActiveTexture(GL_TEXTURE0);
	  glActiveTexture(GL_TEXTURE0);

	  //glEnable(GL_DEPTH_TEST);
	  //depth_ = -0.5f;

	  // glDisable(GL_ALPHA_TEST);
	  

	  //glLoadIdentity();
	  assert(++glStack_ < 10);
	  glPushMatrix();
	  

	  //	  TS_DEBUGLOG("rectangle depth:%f\n", depth_);
	  glTranslatef(rect_.pos_.x_ + posofs.x_, rect_.pos_.y_ + posofs.y_, -0.1f-depth_);
	  if (rscale_ != 1.0f) {
		glScalef(rscale_,rscale_,rscale_);
	  }

	  bool haseffect = false;
	  if (effect_) haseffect = make_effect2();
	  
	  if (!haseffect) {
		glEnable(GL_BLEND);
		glColor4f(r_, g_, b_, a_);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//glDisable(GL_BLEND);
	  }

	  if (img_.has_image()) {
		glEnable(GL_TEXTURE_2D);
	  }
	  else {
		glDisable(GL_TEXTURE_2D);
	  }
	  glVertexPointer(3, GL_FLOAT, 0, vertices_);
	  glEnableClientState(GL_VERTEX_ARRAY);
	  glTexCoordPointer(2, GL_FLOAT, 0, squareTexCoords);
	  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
	void post_render(context&) {
	  glPopMatrix();
	  assert(--glStack_ >= 0);
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

  const float overscan1_ = 0.7f/1024.0f;
  const float overscan2_ = 0.7f/1024.0f;


  // 複数の四角形
  // テクスチャは一つのみ
  template <typename ImgTyp>
  struct rectangles2d_t : rectangle2d_t<ImgTyp> {
	typedef rectangle2d_t<ImgTyp>  Super;
	typedef ImgTyp ImageType;
	struct vertex {
	  float x,y,z;
	  float u,v;
	  float u2,v2;
	};

    using rectangle2d_t<ImgTyp>::glStack_;

	std::vector<vertex> vertices_; // n x [12]; // [0,1,2][3,4,5][6,7,8][9,10,11][12,13,14][15,16,17]
	//std::vector<GLfloat> squareTexCoords_; // n x [8];
	//std::vector<GLfloat> squareTexCoords2_; // n x [8];
	std::vector<rect_f> rects_;  // 個々の四角形のサイズ
	//static const size_t vst = 3*6 + 2*6;
	//static const size_t tst = 2*6;
	static const size_t VerticesOfRect = 6;
	bool hasSecondTexture_;

	rectangles2d_t()
	{}

	rectangles2d_t(ImgTyp img, const pos2_f& pos=pos2_f(), size_f size=size_f(), float dep=0.0f)
	  : rectangle2d_t<ImgTyp>(img, pos, size)
	  , hasSecondTexture_(false)
	{
	}
	
	
	void set_rect(size_t idx
				  , const rect_f& uvrect // (*charData_)[c].rect_
				  , const pos2_f& pos //, pos2_f(viewport_.pos_.x_ + x * pscale_.x_, viewport_.pos_.y_ + y * pscale_.y_)
				  , const size_f& csize
				  , const rect_f& uvrect2 = rect_f() // multi texture
				  ) {

	  rects_[idx] = rect_f(pos, csize);
	  size_t i = idx * VerticesOfRect;

	  vertices_[i].x = pos.x_;
	  vertices_[i].y = pos.y_;
	  vertices_[i].z = 0.0f;
	  ++i;
	  vertices_[i].x = pos.x_ + csize.width_;
	  vertices_[i].y = pos.y_;
	  vertices_[i].z = 0.0f;
	  ++i;
	  vertices_[i].x = pos.x_;
	  vertices_[i].y = pos.y_ + csize.height_ + 0.1f;
	  vertices_[i].z = 0.0f;
	  ++i;
	  vertices_[i].x = pos.x_ + csize.width_;
	  vertices_[i].y = pos.y_;
	  vertices_[i].z = 0.0f;
	  ++i;
	  vertices_[i].x = pos.x_;
	  vertices_[i].y = pos.y_ + csize.height_ + 0.1f;
	  vertices_[i].z = 0.0f;
	  ++i;
	  vertices_[i].x = pos.x_ + csize.width_;
	  vertices_[i].y = pos.y_ + csize.height_ + 0.1f;

	  set_imguv(idx, uvrect);
	  if (!uvrect2.size_.empty() && hasSecondTexture_) {
		set_imguv2(idx, uvrect2);
	  }
	}

	void enable_second_texture(bool en=true) {
	  hasSecondTexture_ = en;
	}

	void resize(size_t size) {
	  vertices_.resize(size*VerticesOfRect); //*vst);
	  //squareTexCoords_.resize(size*tst);
	  //squareTexCoords2_.resize(size*tst);
	  rects_.resize(size);
	  //for (size_t idx = 0; idx < size; ++idx) {
	  //vertices_[idx*vst+14] =vertices_[idx*vst+17] =
	  //vertices_[idx*vst+2] = vertices_[idx*vst+5] = vertices_[idx*vst+8] = vertices_[idx*vst+11] = this->depth_;
	  //}
	}


	void set_imguv(size_t idx, rect_f r) {
	  // 表示する矩形のUV座標を得る
	  r.pos_.x_ += overscan1_;
	  r.pos_.y_ += overscan1_;
	  r.size_.width_  -= overscan1_+overscan2_;
	  r.size_.height_ -= overscan1_+overscan2_;
	  size_t i = idx * VerticesOfRect;
	  vertices_[i].u = r.left();
	  vertices_[i].v = r.top();
	  vertices_[++i].u = r.right();
	  vertices_[i].v = r.top();
	  vertices_[++i].u = r.left();
	  vertices_[i].v = r.bottom();

	  vertices_[++i].u = r.right();
	  vertices_[i].v = r.top();
	  vertices_[++i].u = r.left();;
	  vertices_[i].v = r.bottom();
	  vertices_[++i].u = r.right();
	  vertices_[i].v = r.bottom();
#if 0
	  squareTexCoords_[idx*tst+0] = r.left();
	  squareTexCoords_[idx*tst+1] = r.top();
	  squareTexCoords_[idx*tst+2] = r.right();
	  squareTexCoords_[idx*tst+3] = r.top();
	  squareTexCoords_[idx*tst+4] = r.left();
	  squareTexCoords_[idx*tst+5] = r.bottom();

	  squareTexCoords_[idx*tst+6] = r.right();
	  squareTexCoords_[idx*tst+7] = r.top();
	  squareTexCoords_[idx*tst+8] = r.left();;
	  squareTexCoords_[idx*tst+9] = r.bottom();
	  squareTexCoords_[idx*tst+10] = r.right();
	  squareTexCoords_[idx*tst+11] = r.bottom();
#endif
	}

	void set_imguv2(size_t idx, rect_f r) {
	  r.pos_.x_ += overscan1_;
	  r.pos_.y_ += overscan1_;
	  r.size_.width_  -= overscan1_+overscan2_;
	  r.size_.height_ -= overscan1_+overscan2_;

	  size_t i = idx * VerticesOfRect;
	  vertices_[i].u2 = r.left();
	  vertices_[i].v2 = r.top();
	  vertices_[++i].u2 = r.right();
	  vertices_[i].v2 = r.top();
	  vertices_[++i].u2 = r.left();
	  vertices_[i].v2 = r.bottom();

	  vertices_[++i].u2 = r.right();
	  vertices_[i].v2 = r.top();
	  vertices_[++i].u2 = r.left();;
	  vertices_[i].v2 = r.bottom();
	  vertices_[++i].u2 = r.right();
	  vertices_[i].v2 = r.bottom();
#if 0
	  squareTexCoords2_[idx*tst+0] = r2.left();
	  squareTexCoords2_[idx*tst+1] = r2.top();
	  squareTexCoords2_[idx*tst+2] = r2.right();
	  squareTexCoords2_[idx*tst+3] = r2.top();
	  squareTexCoords2_[idx*tst+4] = r2.left();
	  squareTexCoords2_[idx*tst+5] = r2.bottom();

	  squareTexCoords2_[idx*tst+6] = r2.right();
	  squareTexCoords2_[idx*tst+7] = r2.top();
	  squareTexCoords2_[idx*tst+8] = r2.left();;
	  squareTexCoords2_[idx*tst+9] = r2.bottom();
	  squareTexCoords2_[idx*tst+10] = r2.right();
	  squareTexCoords2_[idx*tst+11] = r2.bottom();
#endif
	}

	void render(context& gc) {
	  assert(this->img_.data_);
	  if (!this->img_.data_) {
		TS_DEBUGLOG("rectangle2d: img is null\n");
	  return;
	  }

	  //hasSecondTexture_ = false;
	  bool use_mipmap_ = true;

	  // 移動アニメーション
	  this->set_move_animation();

	  if (this->img_.handle() == image32::invalid_handle) {
		this->img_.lock(); // unlockまでnothrowを確認すること
		if (this->img_.handle() == image32::invalid_handle) { // 二重ロックチェックイデオム
          assert(this->img_.data_);
		  uint8_t* buffer = &view(this->img_.data_->pixel_)[0][0];
		  GLuint th = 0;
		  glGenTextures(1, &th); 
		  this->img_.set_handle(th);
		  this->img_.unlock();
		  //glClientActiveTexture(GL_TEXTURE0);
		  glActiveTexture(GL_TEXTURE0);
		  glBindTexture(GL_TEXTURE_2D, th); 
		  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
		  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); 
		  if (use_mipmap_) {
			glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); //GL_NEAREST); 
		  }
		  else {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
		  }
		  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
		  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->img_.original_size().width_, this->img_.original_size().height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer); 

		  if (hasSecondTexture_) {
			//glClientActiveTexture(GL_TEXTURE1);
			glActiveTexture(GL_TEXTURE1);
			
			glBindTexture(GL_TEXTURE_2D, th); 
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); 
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
			if (use_mipmap_) {
			  glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
			  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); //GL_NEAREST); 
			}
			else {
			  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
			}
			
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
			glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
			glTexEnvi(GL_TEXTURE_ENV, GL_SRC0_RGB, GL_PREVIOUS);
			glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
			glTexEnvi(GL_TEXTURE_ENV, GL_SRC1_RGB, GL_TEXTURE);
			glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);
			
			//glClientActiveTexture(GL_TEXTURE0);
			glActiveTexture(GL_TEXTURE0);
		  }
		  //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D, textureid_, 0); 
		}
		else {
		  this->img_.unlock();
		  glBindTexture(GL_TEXTURE_2D, this->img_.handle()); 
		  if (hasSecondTexture_) {
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, this->img_.handle()); 
			glActiveTexture(GL_TEXTURE0);
		  }
		}
	  }
	  else {
		if (gc.lastTexture_ != this->img_.handle()) {
		  glBindTexture(GL_TEXTURE_2D, this->img_.handle()); 
		  gc.lastTexture_ = this->img_.handle();
		  if (hasSecondTexture_) {
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, this->img_.handle()); 
			glActiveTexture(GL_TEXTURE0);
		  }
		}
	  }

	  glDisable(GL_ALPHA_TEST); // ALPHA_TESTを禁止するとほんの少し速くなるようだ
	  assert(++glStack_ < 10);
	  glPushMatrix();
	  pos2_f posofs(0,0);
	  glTranslatef(this->rect_.pos_.x_ + posofs.x_, this->rect_.pos_.y_ + posofs.y_, this->depth_);
	  //if (scale_ != 1.0f) glScalef(scale_,scale_,scale_);

	  //if (effect_->has(effect::xy_rotation)) {
	  //glRotatef(effect_->get<xy_rotation>().param_, 0.0f,0.0f,1.0f);
	  //}

	  
	  
	  if (this->effect_ && this->effect_->has(effect::fade_out)) {
		// フェードアウト処理
		effect& ef = *this->effect_;
		glEnable(GL_BLEND);
		glColor4f(1.0f,1.0f,1.0f,1.0f - ef.get<fade_out>().progress_);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//TS_DEBUGLOG("Rectangles:fade_out(%f)\n", ef.get<fade_out>().progress_);
	  }
	  else if (this->effect_ && this->effect_->has(effect::fade_in)) {
		// フェードイン処理
		effect& ef = *this->effect_;
		glEnable(GL_BLEND);
		glColor4f(1.0f,1.0f,1.0f, ef.get<fade_in>().progress_);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	  }
	  else if (this->effect_ && this->effect_->has(effect::highlights)) {
	  }
	  else {
		glEnable(GL_BLEND);
		glColor4f(1.0f,1.0f,1.0f,1.0f);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//glDisable(GL_BLEND);
	  }


	  glEnable(GL_TEXTURE_2D);
	  glVertexPointer(3, GL_FLOAT, sizeof(vertex), &vertices_[0].x); 
	  glEnableClientState(GL_VERTEX_ARRAY);

	  glTexCoordPointer(2, GL_FLOAT, sizeof(vertex), &vertices_[0].u); 
	  glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	  if (hasSecondTexture_) {
		glClientActiveTexture(GL_TEXTURE1);
		glActiveTexture(GL_TEXTURE1);
		glEnable(GL_TEXTURE_2D);
		glTexCoordPointer(2, GL_FLOAT, sizeof(vertex), &vertices_[0].u2); 
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	  }
	  glDrawArrays(GL_TRIANGLES, 0, rects_.size()*VerticesOfRect);


	  if (hasSecondTexture_) {
		glDisable(GL_TEXTURE_2D);
		glClientActiveTexture(GL_TEXTURE0);
		glActiveTexture(GL_TEXTURE0);
	  }
	}
	void post_render(context&) {
	  glPopMatrix();
	  assert(--glStack_ >= 0);
	}
  };






  typedef rectangle2d_t<image32> rectangle2d;
  typedef rectangle2d_t<image32&> rectangle2d_ref;

  typedef rectangles2d_t<image32> rectangles2d;


}}} // ts::appfw::gl
#endif
