#include <iostream>
#include <memory>
#include <vector>
#include <assert.h>
#include <boost/any.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

using namespace std;

vector<boost::any> holder;

template <typename T>
void foo(boost::shared_ptr<T> hoge) {
  holder.push_back(hoge);
}

struct Hoge {
  const char* name_;
  Hoge(const char* nm = "noname") : name_(nm) {}
  ~Hoge() {
	cout << name_ << ": Destructor of Hoge called" << endl;
  }
};

template <typename T>
struct ScopedSharedPtr {
  struct Holder : public T, public boost::enable_shared_from_this<Holder> {
	Holder(const char* n) : T(n) {}
  };
  boost::shared_ptr<Holder> value_;
  ScopedSharedPtr(const char* nm) : value_(new Holder(nm)) {}
  ~ScopedSharedPtr() {
	assert(value_.use_count() == 1);
  }

  boost::shared_ptr<Holder> getShared() {
  	return value_->shared_from_this();
  }
  
};

int main() {
  try {
	cout << "Test#1 using shared_ptr" << endl;
	{
	  boost::shared_ptr<Hoge> hoge(new Hoge("Test#1"));
	  foo(hoge);
	}
	cout << "end of main #1" << endl;
	
	cout << "Test#2 using ScopedSharedPtr" << endl;
	{
	  ScopedSharedPtr<Hoge> hoge("Test#2");
	  foo(hoge.getShared());
	}
	cout << "end of main #2" << endl;
  }
  catch (const exception& ex) {
	cout << ex.what() << endl;
  }
}
