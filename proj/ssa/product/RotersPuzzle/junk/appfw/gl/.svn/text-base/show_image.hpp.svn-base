/* -*-tab-width:4-*-
   libTS glfw  Graphics Language Frame Work
 */

#ifndef TS_APPFW_GL_SHOWIMAGE_PP
#define TS_APPFW_GL_SHOWIMAGE_PP

#include "image.hpp"
#include "context.hpp"

namespace ts {
namespace appfw {
namespace gl {
  
  enum effect_type {
	ef_none
	, ef_highlights
  };

  struct rectangle2d {
	tex_image_t img_;
	rect_f rect_;
	GLuint textureid_;
	GLfloat vertices_[12]; // [0,1,2][3,4,5][6,7,8][9,10,11]
	effect_type effect_;
	GLfloat depth_;
	rectangle2d(tex_image_t img, const pos2d_f& pos, size_f size=size_f(0,0), float dep=0.0f)
	  : img_(img)
	  , rect_(pos, size)
	  , textureid_(-1)
	  , effect_(ef_none)
	  , depth_(dep)
	{
	  if (size == size_f(0,0)) {
		size.width_ = img_.width();
		size.height_ = img_.height();
		rect_.set_size(size);
	  }
	  vertices_[0] = vertices_[1] = vertices_[4] = vertices_[6] = 0.0f;
	  vertices_[2] = vertices_[5] = vertices_[8] = vertices_[11] = dep;
	  set_size(size);
	}

	void set_size(const size_f& size) {
	  vertices_[3] = vertices_[9] = static_cast<float>(size.width_);
	  vertices_[7] = vertices_[10] = static_cast<float>(size.height_);
	}

	void set_effect(effect_type ef) {
	  effect_ = ef;
	}

	void render(context& gc) {
	  static const GLfloat squareTexCoords[] = {
		0.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f
	  };
	  assert(img_.data_);

	  uint8_t* buffer = &view(*img_.data_)[0][0];
	  if (textureid_ == -1) {
		glGenTextures(1, &textureid_); 
		glBindTexture(GL_TEXTURE_2D, textureid_); 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); 
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_.width(), img_.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer); 
		//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D, textureid_, 0); 
	  }
	  else {
		glBindTexture(GL_TEXTURE_2D, textureid_); 
	  }
	  glLoadIdentity();
	  glTranslatef(rect_.pos_.x_, rect_.pos_.y_, depth_);
	  glEnable(GL_TEXTURE_2D);
	  switch (effect_) {
	  case ef_highlights:
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBlendFunc(GL_ONE, GL_ONE);
		glEnable(GL_BLEND);
		glColor4f(1.0f,1.0f,1.0f,0.5f);
		break;
	  default:
		glDisable(GL_BLEND);
		break;
	  }
	  glVertexPointer(3, GL_FLOAT, 0, vertices_);
	  glEnableClientState(GL_VERTEX_ARRAY);
	  glTexCoordPointer(2, GL_FLOAT, 0, squareTexCoords);
	  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
  };

  template <typename GObj>
  struct show_images_cmd : cmd_base {
	typedef shared_ptr<show_images_cmd> ptr;
	uint32_t id_;
	bool show_;
	//vector<GLfloat> vertices_;
	//vector<GLfloat> texcoords_;
	std::vector<GObj> rects_;

	show_images_cmd(uint32_t id) 
	  : id_(id)
	  , show_(true)
	{}

	show_images_cmd(uint32_t id, tex_image_t img, const pos2d_f& pos, const size_f& size=size_f(0,0))
	  : id_(id)
	  , show_(true)
	{
	  rects_.push_back(GObj(img, pos, size));
	}
	show_images_cmd(uint32_t id, const std::vector<GObj>& rects)
	  : id_(id)
	  , show_(true)
	  , rects_(rects)
	{
	}
	
	void operator += (const GObj& rec) {
	  rects_.push_back(rec);
	}
	GObj& operator [] (size_t idx) {
	  assert(idx < rects_.size());
	  return rects_[idx];
	}
	GObj& at(size_t idx) {
	  assert(idx < rects_.size());
	  return rects_[idx];
	}
	std::vector<GObj>& objs() { return rects_; }


	virtual cmd_type type() const { return show_image_type; }
	virtual bool execute(context& gc) {
	  if (!show_) return false;

	  glEnable(GL_DEPTH_TEST);
	  glMatrixMode(GL_PROJECTION);
	  glLoadIdentity();
	  glOrthof(0.0f, (float)gc.screen_size_.width_, (float)gc.screen_size_.height_, 0.0f, -1.0f, 1.0f);
	  glMatrixMode(GL_MODELVIEW);
	  BOOST_FOREACH(GObj& rec, rects_) {
		rec.depth_ = static_cast<float>(1.0f - (id_/10.0f));
		rec.render(gc);
	  }
	  return show_;
	}

	void hide() {
	  show_ = false;
	}
  };
  
  typedef show_images_cmd<rectangle2d> show_image_cmd;


}}} // ts::appfw::gl
#endif
