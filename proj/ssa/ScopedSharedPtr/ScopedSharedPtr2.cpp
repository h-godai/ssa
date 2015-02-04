// スコープ内で有効なshared_ptrが欲しい#1
#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
#include <assert.h>
#include <boost/any.hpp>
#include <typeinfo>

using namespace std;

namespace {
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
	vector<shared_ptr<Task>> tasks_;
	void addTask(shared_ptr<Task> task) {
	  tasks_.push_back(task);
	}
	void do_update() {
	  for (auto& task : tasks_) task->update();
	}
	void clear() {
	  tasks_.clear();
	}
  } evilTaskManager_; // シングルトン実装です
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

// std::shared_ptrをラップしたscoped_shared_ptr
// スコープから出る時に参照が残っていたら、assertする
template <typename T>
struct scoped_shared_ptr {
  using element_type = T;
  // std::shared_ptrがクラスオブジェクトを破棄するときに呼ばれる
  // ここでは何もしない
  struct deleter {
	void operator() (element_type*) {
	  std::cout << "deleter called" << std::endl;
	}
  };
  // 共有するインンスタンスの実体。スコープ内で宣言されたときは、スタック上に生成される。
  T body_;
  // std:shared_ptrの実体。newしたクラスオブジェクトでなく、
  // スタック上に作られたクラスオブジェクトのアドレスを入れる
  std::shared_ptr<T> value_;
  template <typename ...Param>
  scoped_shared_ptr(Param&&... params)
	: body_(params...)
	, value_(&body_, deleter())
  {}
  ~scoped_shared_ptr() noexcept {
	assert(value_.use_count() == 1);
  }

  // shared_ptrへのアクセス用
  template<typename TT>
  operator shared_ptr<TT> () { return value_; }

};

int main() {
  
  // デバッグ出力用のクラス
  // このインスタンスはmain()関数から出たら無効になるので注意が必要
  ClassTracer ct(cout);
  
  try {
	// このscoped_shared_ptrは、MyTaskをnewしないので、幾分高速になる
	scoped_shared_ptr<MyTask> hoge(ct);
	evilTaskManager_.addTask(hoge); // shared_ptrを要求するAPIにも楽々対応!
  
	// メインループの処理
	int life = 3;
	while (life--) {
	  evilTaskManager_.do_update();
	}
	evilTaskManager_.clear(); // これを忘れると、tryブロックから出たときにassertする
  }
  catch (...) {
	// おっと、例外が呼ばれるとevilTaskManagerがクリアされないのでassertする
  }
  ct.message("end of main proc.");
}



