// スコープ内で有効なshared_ptrが欲しい#1
#include <iostream>
#include <memory>
#include <assert.h>
#include <boost/any.hpp>
#include <typeinfo>


namespace {
  using namespace std;
  
  // クラスの生成と破棄を記録するデバッグ用クラス
  struct ClassTracer {
	ostream& os_;
	ClassTracer(ostream& os) : os_(os) {}
	ClassTracer(ostream&& os) : os_(os) {}
	~ClassTracer() {
	  os_ << "ClassTracer destructed" << endl;
	}
	template <typename T>
	void construct(T*) {
	  os_ << "construct: " << typeid(T).name() << endl;
	}
	template <typename T>
	void destruct(T*) {
	  os_ << "destruct: " << typeid(T).name() << endl;
	}
	void message(const string& msg) {
	  os_ << msg << endl;
	}
	
  };

  // イケてないタスクマネージャー
  struct EvilTaskManager {
	struct Task {
	  virtual void update() = 0;
	};
	shared_ptr<Task> task_;
	void setTask(shared_ptr<Task> task) {
	  task_ = task;
	}
	void do_update() {
	  if (task_) task_->update();
	}
	void clear() {
	  *this = EvilTaskManager();
	}

	// シングルトン実装です
	static EvilTaskManager& instance() {
	  static EvilTaskManager me;
	  return me;
	}
  };
}

 
struct MyTask final : EvilTaskManager::Task {
  ClassTracer* ct_ = nullptr;
  MyTask(ClassTracer& ct) : ct_(&ct) {
	ct_->construct(this);
  }
  void update() override {
	if (ct_) ct_->message("MyTask update");
  }
  ~MyTask() {
	if (ct_) ct_->destruct(this);
  }
};

using Hoge = void*(size_t);

template<typename tag, size_t BufferSize>
void* Buffer(size_t) {
  static char buffer[BufferSize];
  cout << "Buffer called" << endl;
  return buffer;
}

template <class T, typename Alloc>
struct custom_allocator {
  typedef T value_type;
  custom_allocator() noexcept {}
  template <class U, typename A> custom_allocator (const custom_allocator<U,A>&) noexcept {}
  T* allocate (std::size_t n) {
	std::cout << "malloc:" << typeid(T).name() << " " << n*sizeof(T) << "bytes" << endl;
	return reinterpret_cast<T*>(Alloc::alc(n*sizeof(T))); // ::malloc(n*sizeof(T)));
  }
  void deallocate (T* p, std::size_t n) {
	std::cout << "deallocate:" << n*sizeof(T) << endl;
	//::free(p);
  }

  template<class Other>
  struct rebind {	// convert this type to allocator<_Other>
	typedef custom_allocator<Other, Alloc> other;
  };

};
template <class T>
struct stack_allocator {
  void* buffer_ = nullptr;
  size_t size_left_ = 0;
  typedef T value_type;
  stack_allocator(void* buffer, size_t buffersize) noexcept : buffer_(buffer), size_left_(buffersize)  {}
  template <class U> stack_allocator (const stack_allocator<U>& other) noexcept
	: buffer_(other.buffer_), size_left_(other.size_left_) {}
  T* allocate (std::size_t n) {
	std::cout << "malloc:" << typeid(T).name() << " " << n*sizeof(T) << "bytes" << endl;
	assert(buffer_);
	return static_cast<T*>(buffer_);
  }
  void deallocate (T* p, std::size_t n) {
	std::cout << "deallocate:" << n*sizeof(T) << endl;
	//::free(p);
  }

  template<class Other>
  struct rebind {	// convert this type to allocator<_Other>
	typedef stack_allocator<Other> other;
  };

};


template <typename T>
struct scoped_shared_ptr {
  using element_type = T;
  char buffer_[128];
  struct deleter {
	void operator () (element_type*) {}
  };
  element_type body_;
  std::shared_ptr<element_type> shared_ptr_;
  template <typename ...Param>
  scoped_shared_ptr(Param&&... params)
	: body_(params...)
	, shared_ptr_(&body_, deleter(), stack_allocator<T>(buffer_,128))
  {}
  ~scoped_shared_ptr() {
	assert(shared_ptr_.use_count() == 1);
  }
  template <typename TT>
  operator std::shared_ptr<TT> () { return shared_ptr_; }

  // アクセッサ
  element_type& get() const { return body_; }
  element_type* operator -> () const { return &body_; }
  element_type& operator * () const { return body_; }
};

int main() {
  
  // デバッグ出力用のクラス
  ClassTracer ct(cout);
  //ClassTracer ct(ofstream("debug.txt"));
	
  // 本当は'MyTask hoge(ct);' としてスタック上に作りたいけど、
  // TaskManagerがshared_ptrを要求しているので無理
  scoped_shared_ptr<MyTask> hoge(ct);
  EvilTaskManager::instance().setTask(hoge);
  
  // メインループの処理
  int life = 3;
  while (life--) {
	EvilTaskManager::instance().do_update();
  }
  EvilTaskManager::instance().clear();
  
  ct.message("end of main proc.");
}
// ここでhogeが解放されるが、ClassTracerは解放済みなので、デストラクタで死ぬ
// ClassTracerのインスタンスが解放される前に、EvilTaskMangerが握っているタスク
// を解放しなければならない。



