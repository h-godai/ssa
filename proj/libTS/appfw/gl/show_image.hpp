/* -*-tab-width:4-*-
   libTS glfw  Graphics Language Frame Work
 */

#ifndef TS_APPFW_GL_SHOWIMAGE_PP
#define TS_APPFW_GL_SHOWIMAGE_PP

#include <boost/lambda/lambda.hpp>
#include <boost/mpl/if.hpp>
#include <boost/foreach.hpp>
#include "image.hpp"
#include "context.hpp"
#include "draw_object.hpp"
#include "rectangle.hpp"
#include "text.hpp"
#include "effect.hpp"

namespace ts {
namespace appfw {
namespace gl {
  using math::size_f;
  //using boost::lambda::_1;
  using boost::mpl::if_c;


  template <typename GObj>
  class show_images_cmd : public draw_object {
  public:
	typedef shared_ptr<show_images_cmd> ptr;
	typedef GObj content_type;
	typedef typename GObj::ImageType ImageType;
	static const bool event_process = GObj::event_process; // イベント処理の有無
  private:
	bool show_;
	std::vector<GObj> objects_;
	effect::ptr effect_;
	int32_t showIndex_;
	bool hide_request_;
	boost::mutex lock_;
	typedef	std::vector<draw_object::ptr> ChildlenList;
	ChildlenList childlen_;
  public:

	show_images_cmd(object_name name, const ImageType& img, const rect_f& rect, int32_t layer = 0, effect ef = no_effect())
	  : draw_object(name, layer)
	  , show_(true)
	  , effect_(new effect(ef))
	  , showIndex_(-1)
	  , hide_request_(false)
	{
	  objects_.push_back(GObj(img, rect));
	  if (effect::valid(effect_)) set_effect();
	}

	show_images_cmd(const object_name& name, GObj& obj, int32_t layer=0, effect ef = no_effect())
	  : draw_object(name, layer)
	  , show_(true)
	  , effect_(new effect(ef))
	  , showIndex_(-1)
	  , hide_request_(false)
	{
	  objects_.push_back(obj);
	  if (effect_) set_effect();
	}
	show_images_cmd(const object_name& name, GObj& obj, const rect_f& rect, int32_t layer=0, effect ef = no_effect())
	  : draw_object(name, layer)
	  , show_(true)
	  , effect_(new effect(ef))
	  , showIndex_(-1)
	  , hide_request_(false)
	{
	  obj.set_rect(rect);
	  objects_.push_back(obj);
	  if (effect_) set_effect();
	}
	show_images_cmd(const object_name& name, const std::vector<GObj>& rects, int32_t layer=0, effect ef = no_effect())
	  : draw_object(name, layer)
	  , show_(true)
	  , objects_(rects)
	  , effect_(new effect(ef))
	  , showIndex_(-1)
	  , hide_request_(false)
	{
	  if (effect_) set_effect();
	}
	show_images_cmd(const object_name& name, const std::vector<GObj>& rects, const rect_f& rect, int32_t layer=0, effect ef = no_effect())
	  : draw_object(name, layer)
	  , show_(true)
	  , objects_(rects)
	  , effect_(new effect(ef))
	  , showIndex_(-1)
	  , hide_request_(false)
	{
	  if (effect_) set_effect();
	  set_rect(rect);
	}

	void add_child(ChildlenList::value_type ch) {
	  scoped_lock lock(lock_);
	  childlen_.push_back(ch);
	}
	void remove_child(object_name nm) {
	  scoped_lock lock(lock_);
	  for (ChildlenList::iterator i = childlen_.begin(); i != childlen_.end(); ++i) {
		if ((*i)->name_ == nm) {
		  childlen_.erase(i);
		  return;
		}
	  }
	}

	void operator += (const GObj& rec) {
	  objects_.push_back(rec);
	}
	GObj& operator [] (size_t idx) {
	  assert(idx < objects_.size());
	  return objects_[idx];
	}
	GObj& at(size_t idx) {
	  assert(idx < objects_.size());
	  return objects_[idx];
	}
	const GObj& at(size_t idx) const {
	  assert(idx < objects_.size());
	  return objects_[idx];
	}
	std::vector<GObj>& objs() { return objects_; }

	const rect_f& rect(size_t idx=0) const {
	  return at(idx).rect();
	}

	void show_index(int32_t idx=-1) {
	  scoped_lock lock(lock_);
	  showIndex_ = idx;
	}
	int32_t get_show_index() const {
	  return showIndex_;
	}

	virtual float depth() const {
	  if (!objects_.empty()) return objects_[0].depth();
	  return 0.0f;
	}

	virtual draw_result draw(context& gc) {
	  draw_result result = draw_continue;
	  if (!show_) return draw_finished;

	  scoped_lock lock(lock_);

	  if (effect::valid(effect_)) {
		if (effect_->is_finished()) {
		  if (effect_->has(effect::fade_out)) {
			show_ = false;
			hide_request_ = false;
			*effect_ =  no_effect();
			//TS_DEBUGLOG("show_image(%s) finished\n", name_.name_.c_str());
			return draw_finished;
		  }
		}
	  }

	  if (showIndex_ == -1) {
		bool first = true;
		BOOST_FOREACH(GObj& r, objects_) { 
		  r.render(gc);
		  if (first) {
			BOOST_FOREACH(ChildlenList::value_type& co, childlen_) {co->draw(gc);}
		  }
		  r.post_render(gc);
		  first = false;
		}
	  }
	  else {
		assert(showIndex_ < static_cast<int>(objects_.size()));
		objects_[showIndex_].render(gc);
		BOOST_FOREACH(ChildlenList::value_type& co, childlen_) {co->draw(gc);}
		objects_[showIndex_].post_render(gc);
	  }


	  if (effect::valid(effect_)) {
		if (effect_->is_finished()) {
		  *effect_ = no_effect();
		}
		else {
		  effect_->progress(1.0f/60.0f);
		  result = draw_continue_next_frame;
		}
	  }

	  return result;
	}

	virtual void hide(const effect& ef) {
	  if (!ef) {
		show_ = false;
	  }
	  else {
		set_effect(ef);
	  }
	  hide_request_ = true;
	}

	void set_image(image32 img, size_t idx = 0) {
	  assert(idx < objects_.size());
	  scoped_lock lock(lock_);
	  objects_[idx].set_image(img);
	}

	void set_images(const std::vector<image32>& imgs) {
	  rect_f rect = objects_[0].rect();
	  scoped_lock lock(lock_);
	  objects_.resize(imgs.size());
	  for (size_t n = 0; n < imgs.size(); ++n) {
		objects_[n] = GObj(imgs[n], rect.pos_, rect.size_);
	  }
	}

	virtual void set_effect(effect::ptr ef) {
	  if (hide_request_) return;
	  {
		scoped_lock lock(lock_);
		effect_ = ef;
	  }
	  set_effect();
	}

	void set_effect(const effect& ef) {
	  if (hide_request_) return;
	  {
		scoped_lock lock(lock_);
		effect_ = effect::ptr(new effect(ef));
	  }
	  set_effect();
	}

	effect::ptr get_effect() { return effect_; }

	void set_effect() {
	  if (effect::valid(effect_)) {
		if (effect_->has(effect::select_image)) {
		  show_index(effect_->get<select_image>().param_);
		  effect_->get<select_image>().done_ = true;
		}

		scoped_lock lock(lock_);
		BOOST_FOREACH(GObj& rec, objects_) {
		  rec.set_effect(effect_);
		}
	  }
	}

	void set_pos(const pos2_f& pos) {
	  scoped_lock lock(lock_);
	  BOOST_FOREACH(GObj& rec, objects_) {
		rec.set_pos(pos);
	  }
	}
	void set_rect(const rect_f& rect) {
	  scoped_lock lock(lock_);
	  BOOST_FOREACH(GObj& rec, objects_) {
		rec.set_rect(rect);
	  }
	}

	// イベント処理
	struct has_event_proc {
	  bool proc(show_images_cmd* ins, event::event_t::ptr ev) {
		BOOST_FOREACH(GObj& o, ins->objects_) { if (o(ev)) return true; }
		return false;
	  }
	};
	struct no_event_proc {
	  bool proc(show_images_cmd*, event::event_t::ptr) { return false; }
	};

	virtual bool operator () (event::event_t::ptr ev) {
	  return typename if_c<event_process, has_event_proc, no_event_proc>::type().proc(this, ev);
	}
  };

  typedef show_images_cmd<rectangle2d> show_image_rect2d;
  typedef show_images_cmd<text2d> show_image_text2d;
  


}}} // ts::appfw::gl
#endif
