#include <stdio.h>
#include <string>

#include "TestMain.h"
#include "PerformanceTestForm.h"

namespace app {
  using namespace boost;
  using std::string;

  struct MainApp {

	MainApp()
	  : form_(new PerformanceTestForm())
	  , testMain_(form_)
	{}
	
	void run(int ac, char* av[]) {
	  int kindnum = -1;
	  int testnum = -1;
	  fprintf(stderr, "Performance Test v1.0\n");
	  for (int i = 1; i < ac; ++i) {
		if (av[i][0] != '-') {
		  if (kindnum == -1)
			kindnum = atoi(av[i]);
		  else
			testnum = atoi(av[i]);
		}
		else {
		  if (string(av[i]) == "-name") {
			++i;
			int k = 1;
			for (auto n = form_->getTestNames().begin();
				      n != form_->getTestNames().end(); ++n, ++k) {
			  if ((*n).substr(0, strlen(av[i])) == av[i]) {
				kindnum = k;
				break;
			  }
			}
		  }
		}
	  }
	  if (kindnum == -1) {
		show_testnames();
		return;
	  }
	  if (testnum == -1) 
		testMain_.doAllTest(kindnum-1);
	  else
		testMain_.doTest(kindnum-1, testnum);

	  testMain_.saveResult("result.txt");
	}


	void show_testnames() {
	  int num = 1;
	  for (PerformanceTestForm::nameListIterator i = form_->getTestNames().begin();
		   i != form_->getTestNames().end(); ++i) {
	    fprintf(stderr, "%d %s\n", num, i->c_str());
		++num;
	  }
	}


	shared_ptr<PerformanceTestForm> form_;
	TestMain testMain_;
	
  };


}



int main(int ac, char* av[]) {
  app::MainApp main;
  main.run(ac, av);

}
