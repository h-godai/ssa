/* draw_object.hpp -*-tab-width:4-*-
   libTS glfw  Graphics Language Frame Work 
   draw_object class
 */

#ifndef TS_APPFW_GL_DRAWOBJECT_HPP
#define TS_APPFW_GL_DRAWOBJECT_HPP

#include "context.hpp"
#include "../event.hpp"
#include "effect.hpp"

namespace ts {
namespace appfw {
namespace gl {



  // 画面に表示するグラフィックスオブジェクトの基底クラス
  struct draw_object : public event::event_processor {
	typedef shared_ptr<draw_object> ptr;
	static const int32_t invalid_layer = 0x80000000;
	enum draw_result {
	  draw_continue,            // 描画は継続
	  draw_continue_next_frame, // 次のフレームも描画が必要
	  draw_finished             // このオブジェクトを破棄して描画しない
	};

	uint32_t id_;
	int32_t layer_;
	object_name name_;
	draw_object(int32_t layer=0) 
	  : id_(object_id_counter::get_count())
	  , layer_(layer) 
	{}
	draw_object(const object_name& name, int32_t layer=0) 
	  : id_(object_id_counter::get_count())
	  , layer_(layer) 
	  , name_(name)
	{}
	draw_object(const draw_object& o)
	  : id_(o.id_)
	  , layer_(o.layer_)
	  , name_(o.name_)
	{}

	virtual ~draw_object() {}
	int32_t layer() const { return layer_; }
	uint32_t id() const { return id_;}
	bool operator < (const draw_object& t) const { 
	  if (layer_ == t.layer_) {
		if (depth() == t.depth()) {
		  return id_ < t.id_;	
		}
		else {
		  return depth() > t.depth(); // 深いものから順
		}
	  }
	  else {
		return layer_ < t.layer_;
	  }
	}
	bool operator == (const draw_object& o) const { return id_ == o.id_; }
	//bool operator == (const object_name& nm) const { return name_ == nm; }
   	//bool operator == (const std::string& nm) const { return name_ == nm; }
   	//bool operator == (const char* nm) const { return name_.name_ == nm; }
	//bool operator == (uint32_t id) const { return id_ == id; }

	static bool progress_effect(effect::ptr ef, float timeprogress = 1.0f/60.0f) {
	  if (!ef) return false;
	  return progress_effect(*ef, timeprogress);
	}
	static bool progress_effect(effect& effect_, float timeprogress = 1.0f/60.0f) {
	  if (effect_) {
		if (effect_.is_finished()) {
		  effect_ = no_effect();
		  return false;
		}
		else {
		  effect_.progress(timeprogress);
		  return true;
		}
	  }
	  return false;
	}
	
	virtual float depth() const { return 0.0f; }
	virtual draw_result draw(context& ) = 0;
	virtual bool event(event::event_t::ptr) { return false; }
	virtual const object_name& name() const { return name_; }
	virtual void hide(const effect& ef) {}
	virtual void set_effect(effect::ptr ef)  = 0;
	virtual effect::ptr get_effect()  = 0;
	virtual bool operator () (event::event_t::ptr ev) { return false; }
	virtual int32_t priority() const { return layer_ * 1000 + static_cast<int32_t>(depth() * 1000.0f); }
	//void set_effect(const effect& ef) { set_effect(effect::ptr(new effect(ef))); }
  };
  inline bool operator < (const draw_object::ptr& a,const draw_object::ptr& b) {
	return *a < *b;
  }
  inline bool operator == (draw_object::ptr p, uint32_t id) { return p->name_ == id; }
  inline bool operator == (draw_object::ptr p, const std::string& s) { return p->name_ == s; }
  //inline bool operator == (draw_object::ptr p, const std::draw_object& o) { return *p == o; }



}}} // ts::appfw::gl
#endif
