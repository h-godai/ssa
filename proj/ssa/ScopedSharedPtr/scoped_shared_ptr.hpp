#pragma once
#include <memory>
#include "static_allocator.hpp"

namespace ts {

  // ローカル変数のバッファを利用するクラスインスタンスを、std::shared_ptr
  // として利用可能にするためのクラス
  // インスタンスが破棄されるときに、参照が残っていたらエラーにする
  template <typename T>
  struct scoped_shared_ptr {
	using element_type = T;
	// インスタンスを解放するためのデリーター。何もしない
	struct deleter {
	  void operator () (element_type*) {}
	};
	element_type                  body_;       // クラスのインスタンス
	static_allocator<64>          allocator_;  // アロケーターの
	std::shared_ptr<element_type> shared_ptr_; // shared_ptrのインスタンス

	// コンストラクタ
	template <typename ...Param>
	scoped_shared_ptr(Param&&... params)
	  : body_(params...)
	  , shared_ptr_(&body_, deleter(), allocator_.get<T>())
	{}
	// デストラクタでは、shared_ptrの参照カウントが１（自分自身のみ）
	// でないとエラーにする。
	~scoped_shared_ptr() {
	  assert(shared_ptr_.use_count() == 1);
	}

	// shared_ptr<T>に変換するオペレーター
	// 派生クラスへの変換を可能にするためテンプレート実装にする
	template <typename TT>
	operator std::shared_ptr<TT> () { return shared_ptr_; }
	
	// アクセッサ
	element_type& get() const { return body_; }
	element_type* operator -> () const { return &body_; }
	element_type& operator * () const { return body_; }
  };
  

}
