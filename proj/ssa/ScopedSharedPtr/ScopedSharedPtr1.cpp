// スコープ内で有効なshared_ptrが欲しい#1
#include <iostream>
#include <memory>
#include <vector>
#include <assert.h>
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
  shared_ptr<T> value_;
  template <typename ...Param>
  scoped_shared_ptr(Param&&... params) : value_(new T(params...))
  {}
  ~scoped_shared_ptr() noexcept {
	assert(value_.use_count() == 1);
  }
  template<typename TT>
  operator shared_ptr<TT> () { return value_; }
  T& get() { return value_.get(); }
};

int main() {
  
  // デバッグ出力用のクラス
  // このインスタンスはmain()関数から出たら無効になるので注意が必要
  ClassTracer ct(cout);
  
  try {
	// 'MyTask hoge(ct);'の代用として、scoped_shared_ptr<MyTask>を作成した
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



