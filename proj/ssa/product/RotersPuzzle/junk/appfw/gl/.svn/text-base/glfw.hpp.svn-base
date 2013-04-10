/* -*-tab-width:4-*-
   libTS glfw  Graphics Language Frame Work
 */

#ifndef TS_APPGW_GLFW_HPP
#define TS_APPGW_GLFW_HPP

#include "common.hpp"
#include "context.hpp"
#include "show_image.hpp"

namespace ts {
namespace appfw {
namespace gl {
  typedef boost::mutex::scoped_lock scoped_lock;
  
  enum stretch_type {
	dot_by_dot
	, fit_to_width
	, fit_to_height
	, stretch_screen
  };
  
  class glfw {
  public:
	context context_;
	std::deque<cmd_base::ptr> cmdQueue_;
	boost::mutex guard_;
	bool updateRequred_;

	glfw()
	  : updateRequred_(true)
	{}

	void update() {
	  updateRequred_ = true;
	}

	const size_i& screen_size() const { return context_.screen_size_; }
	void set_screen_size(uint32_t width, uint32_t height) { 
	  context_.set_screen_size(width, height);
	}

	  
	// 描画時にプラットフォームのスレッドから呼ばれる
	void drawframe() {
	  
	  scoped_lock lock(guard_);
	  if (!updateRequred_) return;
	  updateRequred_ = false;

	  glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	  glClear(GL_COLOR_BUFFER_BIT);

	  std::deque<cmd_base::ptr> nextQueue;
	  while (!cmdQueue_.empty()) {
		cmd_base::ptr cmd = cmdQueue_.front();
		cmdQueue_.pop_front();
		guard_.unlock();
		bool result = cmd->execute(context_);
		guard_.lock();
		if (result) {

		  nextQueue.push_back(cmd);
		}
	  }
	  swap(cmdQueue_, nextQueue);
	}

	void add_cmd(cmd_base::ptr cmd) {
	  scoped_lock lock(guard_);
	  updateRequred_ = true;
	  cmdQueue_.push_back(cmd);
	}

	show_image_cmd::ptr show_image(uint32_t id, tex_image_t img, const pos2d_f& pos, stretch_type st) {
	  size_f size;
	  switch (st) {
	  case fit_to_width:
		size = size_f(context_.screen_size_.width_,context_.screen_size_.width_);
		break;
	  case fit_to_height:
		size = size_f(context_.screen_size_.height_,context_.screen_size_.height_);
		break;
	  case stretch_screen:
		size = size_f(context_.screen_size_.width_,context_.screen_size_.height_);
		break;
	  default:
		size = size_f(0,0);
		break;
	  }
	  return show_image(id, img, pos, size);
	}
	
	show_image_cmd::ptr show_image(uint32_t id, tex_image_t img, const pos2d_f& pos, const size_f& sz=size_f(0,0)) {
	  show_image_cmd::ptr sm(new show_image_cmd(id, img, pos, sz));
	  add_cmd(sm);
	  return sm;
	}
	template <typename T>
	typename show_images_cmd<T>::ptr show_images(uint32_t id, const std::vector<T>& imgs) {
	  typename show_images_cmd<T>::ptr sm(new show_images_cmd<T>(id, imgs));
	  add_cmd(sm);
	  return sm;
	}
	  
  
	void hide_image(uint32_t id) {
	  scoped_lock lock(guard_);
	  BOOST_FOREACH(cmd_base::ptr& cmd, cmdQueue_) {
		if (cmd->type() == cmd_base::show_image_type) {
		  show_image_cmd::ptr si = boost::static_pointer_cast<show_image_cmd>(cmd);
		  assert(si);
		  if (si->id_ == id) {
			si->hide();
		  }
		}
	  }
	  updateRequred_ = true;
	}

	
  };


}}} // ts::appfw::gl

#endif
