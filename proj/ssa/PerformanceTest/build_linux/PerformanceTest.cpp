#include <iostream>
#include <string>
#include <boost/range/algorithm.hpp>
#include <boost/lexical_cast.hpp>

#include "TestMain.h"
#include "PerformanceTestForm.h"

namespace app {
  using namespace boost;
  using namespace std;
  using boost::for_each;

  struct MainApp {

	MainApp()
	  : form_(new PerformanceTestForm())
	  , testMain_(form_)
	{}
	
	int run(int ac, char* av[]) {
	  const int undefined = -1;
	  int kindnum = undefined;
	  int testnum = undefined;
	  for (int i = 1; i < ac; ++i) {
		try {
		  if (av[i][0] != '-') {
			if (kindnum == undefined)
			  kindnum = lexical_cast<int>(av[i]);
			else
			  testnum = lexical_cast<int>(av[i]);
		  }
		  else {
			if (string(av[i]) == "-name") {
			  ++i;
			  int k = 1;
			  string arg(av[i]);
			  for_each(form_->getTestNames(), [&](const string& name) {
				  if (name.substr(0, arg.length()) == arg) { // forward match
					kindnum = k;
					return;
				  }
				  ++k;
				});
			}
		  }
		}
		catch (...) {
		  cerr << "parse error: " << av[i] << endl;
		  return -1;
		}
	  }
	  if (kindnum == undefined) {
		show_testnames();
		return -1;
	  }
	  if (testnum == undefined) 
		testMain_.doAllTest(kindnum-1);
	  else
		testMain_.doTest(kindnum-1, testnum);

	  testMain_.saveResult("result.txt");
	  return 0;
	}


	void show_testnames() {
	  int num = 1;
	  for_each(form_->getTestNames(), [&](const string& name) {
		  cerr << "[" <<  num << "] " << name << endl;
		  ++num;
		});
	}

	shared_ptr<PerformanceTestForm> form_;
	TestMain testMain_;
	
  }; // struct MainApp
} // namespace app



int main(int ac, char* av[]) {
  return app::MainApp().run(ac, av);
}
