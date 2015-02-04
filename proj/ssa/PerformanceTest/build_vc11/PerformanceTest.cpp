#include "stdafx.h"
#include <stdio.h>
#include <boost/foreach.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/format.hpp>
#include <boost/heap/fibonacci_heap.hpp>

#include "TestMain.h"
#include "PerformanceTestForm.h"

namespace app {
  using namespace boost;


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
		if (string(av[i]) == "-name") {
			// allow test name
			string name(av[++i]);
		    int num = 1;
			boost::for_each(form_->getTestNames(), [&](const string& i){
				if (i.substr(0, name.length()) == name) {
					kindnum = num;
					return;
				}
				++num;
			});

		}
		else {
			if (av[i][0] != '-') {
			  if (kindnum == -1)
				kindnum = atoi(av[i]);
			  else
				testnum = atoi(av[i]);
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
	  boost::for_each(form_->getTestNames(), [&](const string& i){
		  string msg((boost::format("%d %s\r\n") % num % i).str());
		fprintf(stderr, "%s", msg.c_str());
		OutputDebugStringA(msg.c_str());

		++num;
  	  });
	}


	shared_ptr<PerformanceTestForm> form_;
	TestMain testMain_;
	
  };


}



int main(int ac, char* av[]) {
  app::MainApp main;
  main.run(ac, av);

}
