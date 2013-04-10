/* effect.hpp -*-tab-width:4-*-
   libTS glfw  Graphics Language Frame Work
 */

#ifndef TS_APPGW_GL_EFFECT_HPP
#define TS_APPGW_GL_EFFECT_HPP
#include <boost/intrusive_ptr.hpp>
//#include <boost/shared_ptr.hpp>
#include "libTS/math/pos.hpp"
#include "context.hpp"
#include "libTS/Util/pointer_policy.hpp"
 
namespace ts {


  // 櫛形コンテナのエレメントタイプ(CRTP)
  template <typename T>
  struct comb_list_element : public intrusive_ptr_policy<T> {
	typedef T self;
	typedef typename intrusive_ptr_policy<T>::ptr ptr;
	ptr next_;
	ptr after_;

  };

  // 櫛形コンテナ(CRTP)
  template <typename T>
  struct comb_list  {
	typedef comb_list_element<T> element_t;
	typedef typename element_t::ptr element_ptr;

	struct iterator {
	  mutable element_ptr at_;
	  mutable element_ptr back_;
	  iterator() {}
	  iterator(const iterator& it) : at_(it.at_), back_(it.back_) {}
	  iterator(element_ptr at, element_ptr back) : at_(at), back_(back) {}
	  element_ptr operator -> () { return at_; }
	  element_ptr operator * () { return at_; }
	  const element_ptr operator -> () const { return at_; }
	  const element_ptr operator * () const { return at_; }
	  iterator operator ++ () { back_ = at_; at_ = at_->next_; return *this; }
	  const iterator operator ++ () const { back_ = at_; at_ = at_->next_; return *this; }
	  bool operator == (const iterator& p) const { return at_.get() == p.at_.get(); }
	  bool operator != (const iterator& p) const { return at_.get() != p.at_.get(); }
	};
	typedef const iterator const_iterator;

	element_ptr element_;

	comb_list() {}
	comb_list(element_ptr ele) : element_(ele) {}
	// copy constructor
	comb_list(const comb_list& c) : element_(c.element_) {}
	void swap(comb_list& c) {
	  std::swap(element_, c.element_);
	}
	comb_list& operator = (comb_list c) {
	  swap(c);
	  return *this;
	}

	bool empty() const { return !element_; }
	operator bool () const { return element_; }

	operator element_ptr () {
	  return element_;
	}

	iterator begin() { return iterator(element_, element_ptr()); }
	iterator end()   { return iterator(); }
	const_iterator begin() const { return iterator(element_, element_ptr()); }
	const_iterator end() const   { return iterator(); }
	iterator last() { 
	  iterator last = begin();
	  while (last->next_) ++last;
	  return last;
	}
	
	// 次に実行するものに入れ替える
	iterator dispose(iterator it) {
	  if (!it->after_) {
		// 次は空っぽ
		if (element_ == *it) {
		  element_ = it->next_;
		  return iterator(element_, element_ptr());
		}
		else {
		  assert(it.back_);
		  it.back_->next_ = it->next_;
		  return iterator(it->next_, it.back_);
		}
	  }

	  if (element_ == *it) {
		// 最初の場合
		//it->  [at][at->after]        [at->after]
        //      [at->next]        it-> [next]
		element_ =  it->after_;
		element_->next_ = it->next_;
		return iterator(it->next_, element_);
	  }
	  else {
		// 途中の場合
		//      [back]                 [back]
		//it->  [at][at->after]        [at->after]
        //      [at->next]        it-> [next]
		assert(it.back_);
		it.back_->next_ = it->after_;
		it->after_->next_ = it->next_;
		return iterator(it->next_, it->after_);
	  }
	}	  
	
	// 次に実行するものを追加
	void add_after(element_ptr e) { 
	  if (element_) {
		element_ptr el = element_;
		while (el->after_) el = e->after_;
		el->after_ = e;
	  }
	  else {
		element_ = e;
	  }
	}

	void add_next(element_ptr e) {
	  if (element_) {
		last()->next_ = e;
	  }
	  else {
		element_ = e;
	  }
	}

	void remove(element_ptr target) {
	  if (!element_) return;
	  element_ptr before; // 
	  element_ptr first = element_;
	  while (first) {
		// search from after
		element_ptr prev;
		element_ptr e = first;
		while (e) {
		  if (e.get() == target.get()) {
			if (prev) {
			  prev->after_ = e->after_;
			}
			else {
			  // 最初の列が削除されるので、前後のリンクを再設定
			  if (!e->after_) {
				//後はないので行を削除
				if (before) {
				  before->next_ = first->next_; 
				}
				else { // first == element_ 最初の行
				  assert(first == element_);
				  element_ = first->next_; 
				}
				  
			  }
			  else {
				// 後はある
				if (before) {
				  before->next_ = e->after_;
				}
				else {
				  element_ = e->after_;
				}
				e->after_->next_ = first->next_;
			  }
			}
			return;
		  }
		  prev = e;
		  e = e->after_;
		}
		before = first;
		first = first->next_;
	  }
	}
  };


namespace appfw {
namespace gl {
  using ts::math::pos2_f;
  using ts::math::pos3_f;

  // 各種エフェクトを扱うクラス
  class effect : public intrusive_ptr_policy<effect> {
  public:
	static const uint32_t persistent_flag = 0x40000000;
	enum  effect_type {
	  none
	  , highlights
	  , fade_in
	  , move_to     // 2D move
	  , move_to3    // 3D move
	  , distance
	  , transparent // 半透明
	  , transparent_to // 半透明(animtion)
	  , mirror_lr   // 左右反転
	  , mirror_tb   // 上下反転
	  , select_image  // 複数の画像あある場合に表示する画像番号を指定する。-1の場合はすべて。
	  , scale_to
	  , xy_rotation // XY平面の静的な回転
	  , rotate_to   // XY平面の回転アニメーション
	  , tapin_effect
	  , tapmove_effect
	  , tapout_effect
	  , user_effect = 0x1000
	  , persistent_effect = persistent_flag
	  , fade_out = persistent_flag | 1
	  , flicker = persistent_flag | 2
	  , beat_effect = persistent_flag | 3
	};

	struct effect_impl : public comb_list_element<effect_impl> {
	  typedef ptr effect_impl_ptr;
	  int32_t effect_type_;
	  effect_impl(effect_type tp, bool persistent = false) 
		: effect_type_(tp)
	  {}
	  virtual ~effect_impl() {}
	  virtual effect_impl_ptr clone() const = 0; // 自分自身のコピーを返す
	  virtual bool is_finished() const = 0;
	  virtual float progress(float) = 0;
	  virtual void reset() = 0;
	  // 比較する
	  virtual bool operator == (const effect_impl& e) {
		return type() == e.type();
	  }

	  effect_type type() const { return static_cast<effect_type>(effect_type_); }
	  bool is_persistent() const { return (effect_type_ & persistent_flag) != 0; }
	};

  protected:
	typedef comb_list<effect_impl> body_t;
	body_t body_;
	typedef body_t::element_ptr impl_ptr;
	typedef body_t::iterator impl_iterator;

  public:
	typedef impl_ptr element_t;
	effect() {}
	effect(impl_ptr ef) : body_(ef) {}
	// copy constructor
	effect(const effect& e) : body_(e.body_) {}
	~effect() {}
	// 代入オペレータ
	effect& operator = (effect e) throw() { swap(e); return *this; }
	// swap
	void swap(effect& e) throw () {
	  body_.swap(e.body_);
	}
	// no effectのときはfalseを返す
	operator bool () const { return !body_.empty(); }
	// no effectのときはtrueを返す
	bool empty() const { return body_.empty(); }

	effect clone() {
	  if (body_.empty()) return effect();
	  return effect(body_.element_->clone());
	}

	// 進捗度合いを設定
	float progress(float p) {
	  float result = 1.0f; // 最も遅れているものを採集する
	  for (impl_iterator it = body_.begin(); it != body_.end();) {
		if (it->is_finished()) {
		  if (!it->is_persistent()) {
			it = body_.dispose(it);
		  }
		  else {
			++it;
		  }
		}
		else {
		  float prg = it->progress(p);
		  //TS_DEBUGLOG("Progress:%d %f\n", it->type(), prg);
		  if (prg < result) result = prg;
		  ++it;
		}
	  }
	  return result;
	}
	// 状態をリセットする
	effect& reset() {
	  for (impl_iterator slot = body_.begin(); slot != body_.end(); ++slot) {
		slot->reset();
	  }
	  return *this;
	}

	// すべてが終わっていたらtrue
	bool is_finished() const {
	  for (const impl_iterator slot = body_.begin(); slot != body_.end(); ++slot) {
		if (!slot->is_finished()) return false;
	  }
	  return true;
	}

	// 多重要素の設定
	void bind(effect& e) {
	  if (body_.empty()) {
		body_ = e.body_;
		return;
	  }
	  body_.add_next(e.body_);
	}
	// 複数のエフェクトを結合する
	effect& operator |= (effect e) {
	  bind(e);
	  return *this;
	}

	effect& operator | (effect e) {
	  bind(e);
	  return *this;
	}
	// 指定のエフェクトを削除する
	effect& operator -= (const effect& e) {
	  if (body_.empty()) return *this; // nothing
	  body_.remove(*e.body_.begin());
	  return *this;
	}

	// エフェクト終了後の次のエフェクトを追加
	effect& operator + (effect e) {
	  body_.add_after(e.body_);
	  return *this;
	}

#if 0
	// 次のエフェクトにする 次があった場合はtrue
	bool effect_done() {
	  if (impl_ &&  impl_->after_) {
		impl_ptr next = impl_->next_;
		impl_ = impl_->after_;
		impl_->next_ = next;
		return true;
	  }
	  return false;
	}
#endif
	
  private:
	// effectがあるかしらべる
	bool operator == (effect_type typ) const {
	  return has(typ);
	}
  public:

	// effectがあるかしらべる
	bool has(effect_type typ) const {
	  for (const impl_iterator slot = body_.begin(); slot != body_.end(); ++slot) {
		if (slot->type() == typ) return true;
	  }
	  return false;
	}

	// effectを検索(内部用)
	impl_ptr find(effect_type typ) {
	  for (impl_iterator slot = body_.begin(); slot != body_.end(); ++slot) {
		if (slot->type() == typ) return *slot;
	  }
	  return impl_ptr();
	}

	// 特定の型にキャストして返す
	template <typename T>
	typename T::impl_t& get() {
	  return *boost::static_pointer_cast<typename T::impl_t>(find(T::type_));
	}

	// エフェクトを削除する
	void remove(effect_type type) {
	  impl_ptr found = find(type);
	  if (found) {
		body_.remove(found);
	  }
	}
	//template <typename T>
	//shared_ptr<T> get_impl() { return boost::dynamic_pointer_cast<T>(impl_); }

	static ptr no_effect_ptr() { return ptr(new effect); }
	static bool valid(const effect::ptr& ef) {
	  return ef && !ef->empty();
	}
	static bool valid(const effect& ef) {
	  return !ef.empty();
	}

  protected:
	// トップのタイプを調べる内部専用。外部からはoperator == かfindを使う
	effect_type type() const { return body_.empty() ? none : body_.begin()->type(); }

  };
  typedef effect no_effect;


  // effectを継承するクラスのテンプレート
  template <int Args, typename ImplT>
  struct effect_sub : effect {
	static const effect::effect_type type_ = ImplT::type_;
	typedef ImplT impl_t ;
	typedef typename impl_t::ptr impl_ptr;
	effect_sub() : effect(impl_ptr(new impl_t())) {}
	effect_sub(typename ImplT::arg1_t a1) : effect(impl_ptr(new impl_t(a1))) {}
	effect_sub(impl_ptr impl) : effect(impl) {}
  };
  template <typename ImplT>
  struct effect_sub<2, ImplT> : effect {
	static const effect::effect_type type_ = ImplT::type_;
	typedef ImplT impl_t ;
	typedef typename impl_t::ptr impl_ptr;
	effect_sub(typename ImplT::arg1_t a1, typename ImplT::arg2_t a2) 
	  : effect(impl_ptr(new impl_t(a1, a2))) {}
	effect_sub(impl_ptr impl) : effect(impl) {}
  };
  template <typename ImplT>
  struct effect_sub<3, ImplT> : effect {
	static const effect::effect_type type_ = ImplT::type_;
	typedef ImplT impl_t ;
	typedef typename impl_t::ptr impl_ptr;
	effect_sub(typename ImplT::arg1_t a1, typename ImplT::arg2_t a2, typename ImplT::arg3_t a3) 
	  : effect(impl_ptr(new impl_t(a1, a2, a3))) {}
	effect_sub(typename ImplT::arg2_t a2) 
	  : effect(impl_ptr(new impl_t(a2))) {}
	effect_sub(impl_ptr impl) : effect(impl) {}
  };

  // パラメータ1つ、一度だけ作用するエフェクト
  template <effect::effect_type ET, typename ParamT>
  struct oneshot_has_param1_effect_impl : effect::effect_impl {
	typedef typename effect::effect_impl::get_ptr<oneshot_has_param1_effect_impl>::type ptr;
	static const effect::effect_type type_ = ET;
	enum { num_of_args = 1 };
	typedef ParamT arg1_t;
	arg1_t param_;
	bool done_;
	oneshot_has_param1_effect_impl(arg1_t param) : effect::effect_impl(ET), param_(param), done_(false) {};
	virtual effect_impl_ptr clone() const { return effect_impl_ptr(new oneshot_has_param1_effect_impl(param_)); }
	virtual float progress(float passed) { return done_ ? 1.0f : 0.0f;	}
	virtual void reset() { done_ = false; }
	bool is_finished() const { return done_; }
  };


  // パラメータを持ち、永続的に作用するエフェクト
  template <effect::effect_type ET, typename ParamT>
  struct infinity_has_param1_effect_impl : effect::effect_impl {
	typedef typename effect::effect_impl::get_ptr<infinity_has_param1_effect_impl>::type ptr;
	static const effect::effect_type type_ = ET;
	enum { num_of_args = 1 };
	typedef ParamT arg1_t;
	arg1_t param_;
	infinity_has_param1_effect_impl(arg1_t param) : effect::effect_impl(ET), param_(param) {};
	virtual effect_impl_ptr clone() const { return effect_impl_ptr(new infinity_has_param1_effect_impl(param_)); }
	virtual ~infinity_has_param1_effect_impl() {}
	virtual float progress(float passed) { return 0.99f;	}
	bool is_finished() const { return false; }
	virtual void reset() {}
  };

  // パラメータを2つ持ち、永続的に作用するエフェクト
  template <effect::effect_type ET, typename Param1T, typename Param2T>
  struct infinity_has_param2_effect_impl : effect::effect_impl {
	typedef typename effect::effect_impl::get_ptr<infinity_has_param2_effect_impl>::type ptr;
	static const effect::effect_type type_ = ET;
	enum { num_of_args = 2 };
	typedef Param1T arg1_t;
	typedef Param2T arg2_t;
	arg1_t param1_;
	arg2_t param2_;
	infinity_has_param2_effect_impl(arg1_t param1, arg2_t param2) : effect::effect_impl(ET), param1_(param1), param2_(param2) {};
	virtual effect_impl_ptr clone() const { return effect_impl_ptr(new infinity_has_param2_effect_impl(param1_, param2_)); }
	virtual ~infinity_has_param2_effect_impl() {}
	virtual float progress(float passed) { return 0.99f;	}
	bool is_finished() const { return false; }
	virtual void reset() {}
  };
  
  // durationを持つエフェクトの基底クラス
  struct has_duration_effect_impl : effect::effect_impl {
	typedef effect::effect_impl::get_ptr<has_duration_effect_impl>::type ptr;
	enum { num_of_args = 1 };
	typedef float arg1_t;
	float duration_; // 期間 (秒)
	float progress_; // 進捗 0.0-1.0
	has_duration_effect_impl(effect::effect_type et, float dur=1.0f) : effect::effect_impl(et), duration_(dur), progress_(0.0f) {}
	virtual float progress(float passed) {
	  progress_ += passed / duration_;
	  if (progress_ > 1.0f) progress_ = 1.0f;
	  return progress_;
	}
	bool is_finished() const { return progress_ == 1.0f; }
	virtual void reset() { progress_ = 0.0f; }
  };
  
  // durationのみのエフェクト
  template <effect::effect_type ET>
  struct has_duration_effect_impl_t : has_duration_effect_impl {
	typedef typename effect::effect_impl::get_ptr<has_duration_effect_impl_t<ET> >::type ptr;
	static const effect::effect_type type_ = ET;
	has_duration_effect_impl_t(float dur=1.0f) : has_duration_effect_impl(ET, dur) {}
	virtual effect_impl::effect_impl_ptr clone() const { return effect_impl::effect_impl_ptr(new has_duration_effect_impl_t(duration_)); }
  };

  // duration + param1を持つエフェクト
  template <effect::effect_type ET, typename ParamT>
  struct has_duration_param1_effect_impl : has_duration_effect_impl_t<ET> {
	typedef typename effect::effect_impl::get_ptr<has_duration_param1_effect_impl<ET, ParamT> >::type ptr;
	static const effect::effect_type type_ = ET;
	enum { num_of_args = 2 };
	typedef ParamT arg1_t;
	typedef float arg2_t;
	ParamT param1_;
	has_duration_param1_effect_impl(ParamT param1, float dur) 
	  : has_duration_effect_impl_t<ET>(dur), param1_(param1) {}
	virtual effect::effect_impl::effect_impl_ptr clone() const { return effect::effect_impl::effect_impl_ptr(new has_duration_param1_effect_impl(param1_, this->duration_)); }
  };
  template <effect::effect_type ET, typename Param1T, typename Param2T>
  struct has_duration_param2_effect_impl : has_duration_param1_effect_impl<ET,Param1T> {
	typedef typename effect::effect_impl::get_ptr<has_duration_param2_effect_impl<ET, Param1T, Param2T> >::type ptr;
	static const effect::effect_type type_ = ET;
	enum { num_of_args = 3 };
	typedef Param2T arg2_t;
	typedef float arg3_t;
	Param2T param2_;
	has_duration_param2_effect_impl(Param1T param1, Param2T param2, float dur) 
	  : has_duration_param1_effect_impl<ET,Param1T>(param1, dur)
	  , param2_(param2)
	{}
	has_duration_param2_effect_impl(Param2T param2) 
	  : has_duration_param1_effect_impl<ET,Param1T>(param2, 1.0f)
	  , param2_(param2)
	{}
	// copy constructor
	has_duration_param2_effect_impl(const has_duration_param2_effect_impl& impl) 
	  : has_duration_param1_effect_impl<ET,Param1T>(impl.param1_, impl.duration_)
	  , param2_(impl.param2_)
	{}
	virtual effect::effect_impl::effect_impl_ptr clone() const { return effect::effect_impl::effect_impl_ptr(new has_duration_param2_effect_impl(*this)); }
  };


  // duration + positionを持つエフェクト
  const float keep_position = 999999.9f; // ポジションをキープする識別子 (move_toで使用)
  // keep_position入りのポジションを返す
  inline pos2_f get_pos(const pos2_f& original, const pos2_f& pos_) {
	return pos2_f(pos_.x_ == keep_position ? original.x_ : pos_.x_
				  , pos_.y_ == keep_position ? original.y_ : pos_.y_);
  }

  // effect_subとimplを結合するアダプタ
  template <typename Impl>
  struct effect_adapt {
	typedef effect_sub<Impl::num_of_args, Impl> type;
  };


  // effectの定義

  typedef effect_adapt<infinity_has_param1_effect_impl<effect::transparent,float> >::type transparent;
  typedef effect_adapt<infinity_has_param1_effect_impl<effect::xy_rotation,float> >::type xy_rotation;
  typedef effect_adapt<oneshot_has_param1_effect_impl<effect::mirror_lr,bool> >::type mirror_lr;
  typedef effect_adapt<oneshot_has_param1_effect_impl<effect::mirror_tb,bool> >::type mirror_tb;
  typedef effect_adapt<oneshot_has_param1_effect_impl<effect::select_image,int> >::type select_image;
  typedef effect_adapt<has_duration_effect_impl_t<effect::highlights> >::type highlights;
  typedef effect_adapt<has_duration_effect_impl_t<effect::fade_in> >::type fade_in;
  typedef effect_adapt<has_duration_effect_impl_t<effect::fade_out> >::type fade_out;
  typedef effect_adapt<has_duration_effect_impl_t<effect::distance> >::type delay;
  typedef effect_adapt<has_duration_effect_impl_t<effect::tapin_effect> >::type tapin_effect;
  typedef effect_adapt<has_duration_param2_effect_impl<effect::scale_to,float,float> >::type scale_to;
  typedef effect_adapt<has_duration_param2_effect_impl<effect::rotate_to,float,float> >::type rotate_to;
  typedef effect_adapt<has_duration_param2_effect_impl<effect::transparent_to,float,float> >::type transparent_to;
  typedef effect_adapt<has_duration_param1_effect_impl<effect::move_to,pos2_f> >::type move_to;
  typedef effect_adapt<has_duration_param1_effect_impl<effect::tapmove_effect,pos2_f> >::type tapmove_effect;
  typedef effect_adapt<has_duration_param1_effect_impl<effect::tapout_effect,pos2_f> >::type tapout_effect;
  typedef effect_adapt<has_duration_param1_effect_impl<effect::move_to3,pos3_f> >::type move_to3;

  // typedef effect_adapt<oneshot_has_param1_effect_impl<effect::tapin_effect,bool> >::type tapin_effect;

  // フリッカーのエフェクト
  struct flicker_impl : infinity_has_param1_effect_impl<effect::flicker,float> {
	float prog_;
	bool flick_;
	flicker_impl(float a1)
	  :	infinity_has_param1_effect_impl<effect::flicker,float>(a1)
	  , prog_(0)
	  , flick_(false)
	{}
	virtual effect::effect_impl::effect_impl_ptr clone() const { return effect::effect_impl::effect_impl_ptr(new flicker_impl(this->param_)); }

	virtual float progress(float passed) {
	  prog_ += passed;
	  if (prog_ > this->param_) {
		flick_ = !flick_;
		prog_ = 0.0f;
	  }
	  return 0.0f;
	}
  };
  typedef effect_adapt<flicker_impl>::type flicker;

  // 鼓動エフェクト
  struct beat_effect_impl : infinity_has_param2_effect_impl<effect::beat_effect,float,float> {
	float prog_;
	bool dir_;
	beat_effect_impl(float scale, float freq)
	  :	infinity_has_param2_effect_impl<effect::beat_effect,float,float>(scale, freq)
	  , prog_(0.0f)
	  , dir_(true)
	{}
	virtual effect::effect_impl::effect_impl_ptr clone() const { return effect::effect_impl::effect_impl_ptr(new beat_effect_impl(this->param1_, this->param2_)); }

	virtual float progress(float passed) {
	  if (dir_) {
		prog_ += passed;
		if (prog_ > this->param1_) {
		  prog_ = this->param1_;
		  dir_ = false;
		}
	  }
	  else {
		prog_ -= passed;
		if (prog_ < 0.0f) {
		  prog_ = 0.0f;
		  dir_ = true;
		}
	  }
	  return 0.0f;
	}
	float get_scale() const {
	  return this->param2_ +  ((1.0f - this->param2_) * prog_ / this->param1_); 
	}
  };
  typedef effect_adapt<beat_effect_impl>::type beat_effect;



}}} // ts:appfw::gl
#endif
