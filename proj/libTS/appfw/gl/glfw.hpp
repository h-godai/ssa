/* -*-tab-width:4-*-
   libTS glfw  Graphics Language Frame Work
 */

#ifndef TS_APPGW_GLFW_HPP
#define TS_APPGW_GLFW_HPP

#include <set>
#include <utility>
#include <boost/ref.hpp>
#include <boost/foreach.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/mpl/if.hpp>
#include <boost/static_assert.hpp>
#include "common.hpp"
#include "context.hpp"
#include "show_image.hpp"
#include "layer.hpp"
#include "positioning.hpp"

namespace ts {
namespace appfw {
namespace gl {
  using ts::math::pos2_f;
  using ts::math::rect_f;
  using ts::math::size_f;
  using ts::math::vec2_f;
  //using boost::lambda::_1;
  using boost::mpl::if_c;


  class glfw {
  public:
	context context_;
	typedef std::set<draw_object::ptr> CmdQueue_t;
	typedef std::map<uint32_t, layer_base::ptr> layer_map_t;
	typedef show_image_rect2d::ptr show_image_handle;
	CmdQueue_t cmdQueue_;
	layer_map_t layers_;
	boost::mutex guard_;
	bool updateRequred_;
	typedef std::vector<event::event_processor::ptr> event_processors_t;
	event_processors_t event_processors_;
	positioner positioner_;
	volatile bool initialized_;


	glfw()
	  : updateRequred_(true)
	  , positioner_(context_)
	  , initialized_(false)
	{
	}

	void initialize(uint32_t width, uint32_t height, float scale_factor=1.0f) {
	  set_device_screen_size(width, height, scale_factor);
	  TS_DEBUGLOG("screen_size(%d,%d)\n", width, height);
	  initialized_ = true;
	}

	// イベント処理
	event::event_processor::ptr operator () (event::event_t::ptr ev) {
	  sort(event_processors_.begin(), event_processors_.end());
	  event::event_processor::ptr processed;
	  BOOST_FOREACH(event::event_processor::ptr e, event_processors_) {
		if ((*e)(ev)) {
		  processed = e;
		  ev->processed_ = true;
		  return processed;
		}
	  }
	  return processed;
	}
	void add_event_processor(event::event_processor::ptr e) {
	  scoped_lock lock(guard_);
	  event_processors_.push_back(e);
	}
	struct add_event_processor_t {
	  add_event_processor_t(glfw* gl, event::event_processor::ptr e) {
		gl->add_event_processor(e);
	  }
	};
	struct no_add {
	  no_add(glfw*, event::event_processor::ptr) {}
	};

	void remove_event_processor(event::event_processor::ptr e) {
	  event_processors_t::iterator found = find(event_processors_.begin(),event_processors_.end(),e);
	  if (found != event_processors_.end()) {
		event_processors_.erase(found);
		TS_DEBUGLOG("event_processor removed\n");
		return;
	  }
	  //TS_DEBUGLOG("event_processor not found\n");
	}


	void update() {
	  updateRequred_ = true;
	}

	const size_i& screen_size() const { return context_.screen_size_; }

	void set_device_screen_size(uint32_t width, uint32_t height, float scf=1.0f) {
	  context_.set_device_screen_size(width, height, scf);
	}

	// 描画レイヤーの登録
	void set_layer(layer_base::ptr layer) {
	  layers_[layer->number_] = layer;
	}

	void lock() {
	  guard_.lock();
	}
	void unlock() {
	  guard_.unlock();
	}



	// 描画時にプラットフォームのスレッドから呼ばれる
	void drawframe() {

	  // テクスチャの解放
	  release_texture_handles& rth = release_texture_handles::Instance();
	  rth.lock();
	  if (rth.count() != 0) {
		TS_DEBUGLOG("release %d textures\n", rth.count());
		glDeleteTextures(rth.count()
						 , static_cast<const GLuint*>(&(rth.get_list()[0])));
		rth.clear();
	  }
	  rth.unlock();


	  scoped_lock lock(guard_);
	 // if (!updateRequred_) return;
	  updateRequred_ = false;
	  context_.lastTexture_ = image32::invalid_handle;

	  glClearColor(0.01f, 0.01f, 0.01f, 1.0f);
	  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	  //glClear(GL_DEPTH_BUFFER_BIT);

	  CmdQueue_t nextQueue;
	  int32_t layer = draw_object::invalid_layer;
	  //BOOST_FOREACH(CmdQueue_t::value_type& cmd, cmdQueue_) {
      for (CmdQueue_t::iterator i = cmdQueue_.begin(); i != cmdQueue_.end(); ++i) {
//        CmdQueue_t::value_type& cmd = *i;
        const draw_object::ptr& cmd = *i;

		//guard_.unlock();
		if (layer != cmd->layer()) {
		  // レイヤーが変わった
		  if (layer != draw_object::invalid_layer) {
			layers_[layer]->end_frame(context_);
		  }
		  layer = cmd->layer();
		  if (!layers_[layer]) {
			TS_DEBUGLOG("layer#%dは登録されていません。ortho_layerを作成します。\n", layer);
			set_layer(ortho_layer::ptr(new ortho_layer(layer)));
		  }
		  layers_[layer]->begin_frame(context_);
		}
		draw_object::draw_result result = cmd->draw(context_);
//		draw_object::draw_result result = draw_object::draw_continue_next_frame;
		//guard_.lock();
		if (result == draw_object::draw_finished) {
		  // イベントハンドラリストから削除
		  remove_event_processor(cmd);
		}
		else {
		  // 次のフレームでも実行するもの
		  nextQueue.insert(cmd);
		  if (result == draw_object::draw_continue_next_frame) {
			updateRequred_ = true;
		  }
		}
	  }
	  if (layer != draw_object::invalid_layer) {
		layers_[layer]->end_frame(context_);
	  }

	  swap(cmdQueue_, nextQueue);
	}

	void add_object(draw_object::ptr cmd) {
	  scoped_lock lock(guard_);
	  updateRequred_ = true;
	  cmdQueue_.insert(cmd);
	}

	show_image_text2d::ptr show_text(object_name name, textdata txt, positioning pos, int32_t layer=0, effect ef = no_effect()) {
	  show_image_text2d::ptr st(new show_image_text2d(name, txt, pos.rect(context_, txt.rect_.size_), layer, ef));
	  add_object(st);
	  return st;
	}
	show_image_text2d::ptr show_text(textdata txt, positioning pos, int32_t layer=0, effect ef = no_effect()) {
	  show_image_text2d::ptr st(new show_image_text2d(txt.text_, txt, pos.rect(context_, txt.rect_.size_), layer, ef));
	  add_object(st);
	  return st;
	}

	show_image_rect2d::ptr show_image(object_name name, image32 img, positioning pos, int32_t layer=0, effect ef = no_effect()) {
	  show_image_rect2d::ptr sm(new show_image_rect2d(name, img, pos.rect(context_, img.viewsize()), layer, ef));
	  add_object(sm);
	  return sm;
	}

	// vector<image32>からのshow_image
	show_image_rect2d::ptr show_image(object_name name, const std::vector<image32>& imgs, positioning pos, int32_t layer, effect ef = no_effect()) {
	  std::vector<rectangle2d> rects;
	  std::copy(imgs.begin(), imgs.end(), std::back_inserter(rects));
	  return show_images(name, rects, pos, layer, ef);
	}
	show_image_rect2d::ptr show_image(image32& img, positioning pos, int32_t layer, effect ef = no_effect()) {
	  show_image_rect2d::ptr sm(new show_image_rect2d(img.name_, img, pos.rect(context_, img.viewsize()), layer, ef));
	  add_object(sm);
	  return sm;
	}
	
	// positioningバージョン
	template <typename T>
	typename show_images_cmd<T>::ptr show_object_with_name(object_name name, T& img, positioning posi, int32_t layer, effect ef=no_effect()) {
	  return show_object_t(name, img, posi.rect(context_, img.viewsize()), layer, ef);
	}
	// positioningバージョン
	template <typename T>
	typename show_images_cmd<T>::ptr show_object(T& img, positioning posi, int32_t layer, effect ef=no_effect()) {
	  return show_object_t(img.name(), img, posi.rect(context_, img.viewsize()), layer, ef);
	}
	// rectありバージョン
	template <typename T>
	typename show_images_cmd<T>::ptr show_object_with_name(object_name name, T& img, const rect_f& rect, int32_t layer, effect ef=no_effect()) {
	  return show_object_t(name, img, rect, layer, ef);
	}
	// rectなしバージョン
	template <typename T>
	typename show_images_cmd<T>::ptr show_object_with_name(object_name name, T& img, int32_t layer, effect ef=no_effect()) {
	  return show_object_t(name, img, layer, ef);
	}

	// noname rectありバージョン
	template <typename T>
	typename show_images_cmd<T>::ptr show_object(T& img, const rect_f& rect, int32_t layer, effect ef=no_effect()) {
	  return show_object_t(img.name(), img, rect, layer, ef);
	}
	// rectなしバージョン
	template <typename T>
	typename show_images_cmd<T>::ptr show_object(T& img, int32_t layer, effect ef=no_effect()) {
	  return show_object_t(img.name(), img, layer, ef);
	}


	// show_objectの本体 with rect
	template <typename GObj>
	typename show_images_cmd<GObj>::ptr show_object_t(object_name name, GObj& gobj, const rect_f& rect, int32_t layer, effect ef=no_effect()) {
	  BOOST_STATIC_ASSERT(GObj::event_process != 999); // T:event_processが定義されていない
	  typename show_images_cmd<GObj>::ptr sm(new show_images_cmd<GObj>(name, gobj, rect, layer, ef));
	  add_object(sm);
	  typename if_c<show_images_cmd<GObj>::event_process, add_event_processor_t, no_add>::type(this, sm);
	  return sm;
	}
	// show_objectの本体 without rect
	template <typename GObj>
	typename show_images_cmd<GObj>::ptr show_object_t(object_name name, GObj& gobj, int32_t layer, effect ef=no_effect()) {
	  BOOST_STATIC_ASSERT(GObj::event_process != 999); // T:event_processが定義されていない
	  typename show_images_cmd<GObj>::ptr sm(new show_images_cmd<GObj>(name, gobj, layer, ef));
	  add_object(sm);
	  typename if_c<show_images_cmd<GObj>::event_process, add_event_processor_t, no_add>::type(this, sm);
	  return sm;
	}




	template <typename T>
	typename show_images_cmd<T>::ptr show_images(object_name name
											   , const std::vector<T>& imgs
											   , int32_t layer = 0
											   , effect ef = no_effect()) {
	  BOOST_STATIC_ASSERT(sizeof(typename T::ImageType) > 0);
	  typename show_images_cmd<T>::ptr sm(new show_images_cmd<T>(name
															   , imgs
															   , layer
															   , ef));
	  add_object(sm);
	  typename if_c<show_images_cmd<T>::event_process, add_event_processor_t, no_add>::type(this, sm);
	  return sm;
	}

	template <typename T>
	typename show_images_cmd<T>::ptr show_images(object_name name
											   , const std::vector<T>& imgs
											   , positioning pos
											   , int32_t layer = 0
											   , effect ef = no_effect()) {
	  BOOST_STATIC_ASSERT(sizeof(typename T::ImageType) > 0);
	  typename show_images_cmd<T>::ptr sm(new show_images_cmd<T>(name
															   , imgs
																 , pos.rect(context_, imgs[0].rect().size_)
															   , layer
															   , ef));
	  add_object(sm);
	  typename if_c<show_images_cmd<T>::event_process, add_event_processor_t, no_add>::type(this, sm);
	  return sm;
	}

	template <typename T>
	uint32_t remove_object(const T& nm) {
	  uint32_t result = 0;
	  scoped_lock lock(guard_);
	  for (CmdQueue_t::iterator i = cmdQueue_.begin(); i != cmdQueue_.end();) {
		if (*i == nm) {
		  TS_DEBUGLOG("remove object:%s\n", (*i)->name_.name_.c_str());
		  remove_event_processor(*i);
		  cmdQueue_.erase(i++);
		  ++result;
		}
		else {
		  ++i;
		}
	  }
	  if (result == 0) {
		//TS_DEBUGLOG("glfw::remove_objectに失敗。(%s)\n", nm->name().name_.c_str());
	  }
	  return result;
	}


  	template <typename T>
	bool is_match(const CmdQueue_t::value_type& c, const T& nm) const {
	  return c->name() == nm.name();
	}
	bool is_match(const CmdQueue_t::value_type& c, draw_object::ptr nm) const {
	  return c == nm;
	}
	bool is_match(const CmdQueue_t::value_type& c, object_name nm) const {
	  return c->name() == nm;
	}
	bool is_match(const CmdQueue_t::value_type& c, int32_t  nm) const {
	  return c->name() == nm;
	}
	bool is_match(const CmdQueue_t::value_type& c, const char* nm) const {
	  return c->name() == nm;
	}

	void hide_object(draw_object::ptr obj, effect ef = no_effect()) {
	  scoped_lock lock(guard_);
	  BOOST_FOREACH(CmdQueue_t::value_type c, cmdQueue_) {
		if (c == obj) {
		  c->hide(ef);
		  remove_event_processor(c);
		}
	  }
	  updateRequred_ = true;
	}
  	template <typename T>
	void hide_image(const T& nm, effect ef = no_effect()) {
	  scoped_lock lock(guard_);
	  BOOST_FOREACH(CmdQueue_t::value_type c, cmdQueue_) {
		if (is_match(c, nm)) {
		  c->hide(ef);
		  remove_event_processor(c);
		}
	  }
	  updateRequred_ = true;
	}
  	template <typename T>
	void effect_image(const T& nm, effect ef) {
	  effect::ptr efp(new effect(ef));
	  scoped_lock lock(guard_);
	  BOOST_FOREACH(CmdQueue_t::value_type c, cmdQueue_) {
		if (is_match(c, nm)) {
		  c->set_effect(efp);
		}
	  }
	  updateRequred_ = true;
	}

  	template <typename T>
	effect::ptr get_effect(const T& nm) {
	  scoped_lock lock(guard_);
	  BOOST_FOREACH(CmdQueue_t::value_type c, cmdQueue_) {
		if (is_match(c, nm)) {
		  return c->get_effect();
		}
	  }
	  TS_DEBUGLOG("get_effect not found object\n");
	  return effect::no_effect_ptr();
	}

  	template <typename T>
	show_image_handle get_image(const T& nm) {
	  scoped_lock lock(guard_);
	  BOOST_FOREACH(CmdQueue_t::value_type c, cmdQueue_) {
		if (is_match(c, nm)) {
		  return boost::dynamic_pointer_cast<show_image_rect2d>(c);
		}
	  }
	  TS_DEBUGLOG(" - get_image not found\n");
	  return show_image_handle();
	}

  	template <typename T>
	bool is_exist_image(const T& nm) {
	  scoped_lock lock(guard_);
	  for (CmdQueue_t::iterator i = cmdQueue_.begin(); i != cmdQueue_.end();++i) {
		if (*i == nm) {
		  return true;
		}
	  }
	  return false;
	}



	float screen_bottom() const { return context_.screen_bottom(); }
	float screen_right() const { return context_.screen_right(); }

	virtual void on_device_rotate(float rot) {
	  context_.set_rotate(rot);
	}

  };



}}} // ts::appfw::gl

#endif
