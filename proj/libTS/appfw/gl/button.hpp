/* button.hpp -*-tab-width:4-*-
   libTS glfw  Graphics Language Frame Work
 */

#ifndef TS_APPFW_GL_BUTTON_HPP
#define TS_APPFW_GL_BUTTON_HPP

#include "image.hpp"
#include "context.hpp"
#include "draw_object.hpp"
#include "rectangle.hpp"
#include "../event.hpp"
#include "positioning.hpp"
#include "../inputfw.hpp"

namespace ts {
namespace appfw {
namespace gl {
  using math::size_f;
  using math::vec2_f;

  template <typename GObj>
  class button_obj : public event::event_generator<true> {
  public:

	enum button_type {
	  normal_type
	  , pushed_type
	  , focusd_type
	  , disable_type
	};
	enum button_action {
	  no_action
	  , clicked  // ボタンがクリックされた
	  , pushing  // ボタンが押されっぱなしの状態
	  , dragged  // ボタンがドラッグされた
	};
	typedef std::map<button_type, GObj> buttons_t;

  protected:
	struct button_impl {
	  button_type type_;
	  buttons_t buttons_;
	  button_action action_;
	  object_name name_;
	  vec2_f drag_vec_;
	  button_impl(image32 img, positioner& pos)
		: type_(normal_type)
		, action_(no_action)
	  {
		rect_f rect = pos.rect(img.viewsize()); // rect_.size_);
		buttons_[normal_type] = GObj(img, rect.pos_, rect.size_);
		name_ = img.name_;
	  }
	  button_impl(image32 img, rect_f rect = rect_f())
		: type_(normal_type)
		, action_(no_action)
	  {
          
		if (rect.size().empty()) {
		  rect.size_ = img.rect().size_;
		}
		buttons_[normal_type] = GObj(img, rect.pos_, rect.size_);
		name_ = img.name_;
	  }
	  button_impl(object_name nm, image32 img, positioner& pos)
		: type_(normal_type)
		, action_(no_action)
		, name_(nm)
	  {
		rect_f rect = pos.rect(img.viewsize()); // rect_.size_);
		buttons_[normal_type] = GObj(img, rect.pos_, rect.size_);
	  }
	  button_impl(object_name nm, image32 img, const rect_f& rect)
		: type_(normal_type)
		, action_(no_action)
		, name_(nm)
	  {
		buttons_[normal_type] = GObj(img, rect.pos_, rect.size_);
	  }
	  button_impl(object_name nm, image32 img, const pos2_f& pos)
		: type_(normal_type)
		, action_(no_action)
		, name_(nm)
	  {
		buttons_[normal_type] = GObj(img, pos);
	  }
	  button_impl(object_name nm, const std::map <button_type, GObj>& buttons)
		: type_(normal_type)
		, buttons_(buttons)
		, action_(no_action)
		, name_(nm)
	  {}
	  GObj& current_button()  {
		assert(!buttons_.empty());
		typename buttons_t::iterator i = buttons_.find(type_);
		if (i == buttons_.end()) {
		  return buttons_.begin()->second;
		}
		else {
		  return i->second;
		}
	  }

	  const GObj& current_button() const {
		assert(!buttons_.empty());
		typename buttons_t::const_iterator i = buttons_.find(type_);
		if (i == buttons_.end()) {
		  return buttons_.begin()->second;
		}
		else {
		  return i->second;
		}
	  }
	  void set_rect(const rect_f& rect) {
		for (typename buttons_t::iterator i = buttons_.begin(); i != buttons_.end(); ++i) {
		  i->second.set_rect(rect);
		}
	  }
	  void set_position(positioner& pos) {
		for (typename buttons_t::iterator i = buttons_.begin(); i != buttons_.end(); ++i) {
		  i->second.set_rect(pos.rect(i->second.size()));
		}
	  }
	  void set_pos(const pos2_f& pos) {
		for (typename buttons_t::iterator i = buttons_.begin(); i != buttons_.end(); ++i) {
		  i->second.rect_.pos_ = pos;
		}
	  }
	};

	boost::shared_ptr<button_impl> impl_;
	int32_t tapin_device_;
	pos2_f tapin_pos_;
	pos2_f drag_pos_;
	boost::posix_time::ptime otime_;

	//bool dragevt_;
	uint32_t flags_; // dragevt_, tapeffect_

  public:
	static const uint32_t enable_dragevent =  0x00000001;
	static const uint32_t disable_tapeffect = 0x00000002;

	button_obj(image32 img, positioner pos, uint32_t flags = 0)
	  : impl_(new button_impl(img, pos))
	  , tapin_device_(-1)
	  , flags_(flags)
	{}

	button_obj(object_name nm, image32 img, positioner pos, uint32_t flags = 0)
	  : impl_(new button_impl(nm, img, pos))
	  , tapin_device_(-1)
	  , flags_(flags)
	{}
	button_obj(object_name nm, image32 img, const rect_f& rect, uint32_t flags = 0)
	  : impl_(new button_impl(nm, img, rect))
	  , tapin_device_(-1)
	  , flags_(flags)
	{}
	button_obj(object_name nm, image32 img, const pos2_f& rect, uint32_t flags = 0)
	  : impl_(new button_impl(nm, img, rect))
	  , tapin_device_(-1)
	  , flags_(flags)
	{}

	button_obj(object_name nm, const std::map <button_type, GObj>& buttons, uint32_t flags = 0)
	  : impl_(new button_impl(nm, buttons))
	  , tapin_device_(-1)
	  , flags_(flags)
	{}
	// copy constructor
	button_obj(const button_obj& b)
	  : impl_(b.impl_)
	  , tapin_device_(-1)
	  , flags_(b.flags_)
	{}

	button_obj& operator = (button_obj b) throw() {
	  swap(b);
	  return *this;
	}
	void swap(button_obj& b) throw() {
	  std::swap(impl_, b.impl_);
	  std::swap(tapin_device_, b.tapin_device_);
	  std::swap(flags_, b.flags_);
	  ts::math::swap(tapin_pos_, b.tapin_pos_);
	}

	const object_name& name() const { return impl_->name_; }

	virtual int32_t status() const { return static_cast<int32_t>(impl_->action_); }
	button_action action() const { return impl_->action_; }
	void clear_action() { impl_->action_ = no_action; }

	void set_button(button_type tp, const GObj& obj) {
	  impl_->buttons_[tp] = obj;
	}

	void render(context& gc) {
	  current_button().render(gc);
	}
	void post_render(context& gc) {
	  current_button().post_render(gc);
	}
    
	float depth() const { return current_button().depth(); }

	GObj& current_button()  { return impl_->current_button(); }
	const GObj& current_button() const { return impl_->current_button(); }

	typedef typename GObj::ImageType ImageType;
	ImageType& image() { return current_button().get_image(); }
	//size_i size() { return current_button().get_image().rect().size(); }
	size_f viewsize() const { return current_button().get_image().viewsize(); }
	const rect_f& rect() const { return current_button().get_image().rect(); }


	void set_enable_dragevent(bool en = true) { if (en) flags_ |= enable_dragevent; else flags_ &= ~enable_dragevent; }
	void set_disable_tapeffect(bool en = true) { if (en) flags_ |= disable_tapeffect; else flags_ &= ~disable_tapeffect; }

	const vec2_f& get_drag_vec() const { return impl_->drag_vec_; }
	void clear_drag_vec() { impl_->drag_vec_ = vec2_f(); }

	void remove_effect() {
	  if (is_enable_tapeffect()) {
		if (drag_pos_ != pos2_f()) {
		  current_button().get_effect()->remove(tapmove_effect::type_);
		  *current_button().get_effect() |= tapout_effect(drag_pos_ - tapin_pos_, 0.2f);
		}
	  }
	  drag_pos_ = pos2_f();
	}

	// イベント処理
	bool operator () (event::event_t::ptr ev) {
	  if (ev->device_type() != in::tap_device) return false;
	  in::tap_event::ptr tap(boost::static_pointer_cast<in::tap_event>(ev));
	  if (!is_inside(tap->ctrl_.pos_)) {
		remove_effect();
		if (tap->ctrl_.action_ == in::tap_ctrl::release) {
		  tapin_device_ = -1;
		}
		return false;
	  }
	  switch (tap->ctrl_.action_) {

		// tap in
	  case in::tap_ctrl::touch:
		tapin_device_ = tap->devnum_;
		tapin_pos_ = tap->ctrl_.pos_;
		drag_pos_ = tapin_pos_;
		if (is_enable_tapeffect()) {
		  *current_button().get_effect() |= tapin_effect(0.1f);
		}
		//set_effect(tapin_effect());
		break;

		// dragging
	  case in::tap_ctrl::move:
		{ // 経過時間を調べる
		  boost::posix_time::time_duration diff = tap->ptime_ - otime_;
		  if (tapin_device_ != static_cast<int>(tap->devnum_)) {
			//tapin_device_ = tap->devnum_;
			//impl_->drag_vec_ = vec2_f();
		  }
		  else {
			if (diff > boost::posix_time::microseconds(1000*100)) {
			  TS_DEBUGLOG("move time diff over\n");
			  impl_->drag_vec_ = vec2_f();
			  otime_ = tap->ptime_;
			  drag_pos_ = tap->ctrl_.pos_;
			}
			else if (drag_pos_ != pos2_f()) {
			  impl_->drag_vec_ = vec2_f(tap->ctrl_.pos_, drag_pos_);
			  float speed = 1000000.0f * impl_->drag_vec_.length() / diff.total_microseconds();
			  TS_DEBUGLOG("move dragging now sp:%f %d\n", speed, diff.total_microseconds());
			  impl_->drag_vec_.set_length(speed);
			  otime_ = tap->ptime_;
			  drag_pos_ = tap->ctrl_.pos_;

			  if (is_enable_tapeffect()) {
				if (current_button().get_effect()->has(effect::tapmove_effect)) {
				  effect::ptr p = current_button().get_effect();
				  tapmove_effect::impl_t& ef = p->get<tapmove_effect>();
				  ef.param1_ = tap->ctrl_.pos_ - tapin_pos_;
				  ef.progress_ = 0.0f;
				}
				else {
				  *current_button().get_effect() |= tapmove_effect(pos2_f(), 1.0f);
				}
			  }

			  if (is_enable_dragevent()) {
				impl_->action_ = dragged;
				return true;
			  }
			}
		  }
		}
		break;

		// tap out
	  case in::tap_ctrl::release:
		remove_effect();

		if (tapin_device_ == static_cast<int>(tap->devnum_) && vec2_f(tapin_pos_, tap->ctrl_.pos_).length() < 32.0f) {
		  impl_->action_ = clicked;
		  return true;
		}
		tapin_device_ = -1;



		break;
        default:
		break;
	  }
	  return false;
	}



	bool is_inside(const pos2_f& pos) const {
	  if (impl_->type_ == disable_type) return false;
	  return current_button().is_inside(pos);
	}

	bool is_enable_tapeffect() const {
	  return (flags_ & disable_tapeffect) == 0;
	}
	bool is_enable_dragevent() const {
	  return (flags_ & enable_dragevent) != 0;
	}

	void set_effect(effect::ptr ef) {
	  current_button().set_effect(ef);
	}
	void set_rect(const rect_f& rect) {
	  impl_->set_rect(rect);
	}

	void set_pos(const pos2_f& pos) {
	  impl_->set_pos(pos);
	}


  };

  typedef button_obj<rectangle2d> button2d;


}}} // ts::appfw::gl
#endif
