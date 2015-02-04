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
	ClassTracer(ostream&& os) : os_(os) {}
	ClassTracer(ostream& os) : os_(os) {}
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

int main() {
  
  // デバッグ出力用のクラス
  ClassTracer ct(cout);
  
  // 本当は'MyTask hoge(ct);' としてスタック上に作りたいけど、
  // TaskManagerがshared_ptrを要求しているので無理
  auto hoge = make_shared<MyTask>(ct);
  evilTaskManager_.setTask(hoge);

  // メインループの処理
  int life = 3;
  while (life--) {
	evilTaskManager_.do_update();
  }
  ct.message("end of main proc.");
}
// ここでhogeが解放されるが、ClassTracerは解放済みなので、デストラクタで死ぬ
// ClassTracerのインスタンスが解放される前に、EvilTaskMangerが握っているタスク
// を解放しなければならない。



