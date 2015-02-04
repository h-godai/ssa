#include "stdafx.h"
#include <stdio.h>
#include <boost/foreach.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/format.hpp>

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
	  std::vector<int> kindnum;
	  int testnum = -1;
	  fprintf(stderr, "Performance Test v1.1\n");
	  for (int i = 1; i < ac; ++i) {
		if (string(av[i]) == "-name") {
			// allow test name
			string name(av[++i]);
		    int num = 1;
            for (auto& n : form_->getTestNames()) {
              if (n.substr(0, name.length()) == name) {
                kindnum.push_back(num);
              }
              ++num;
            }

		}
		else {
			if (av[i][0] != '-') {
			  if (kindnum.empty())
				kindnum.push_back(atoi(av[i]));
			  else
				testnum = atoi(av[i]);
			}
		}
	  }
	  if (kindnum.empty()) {
		show_testnames();
		return;
	  }
	  if (testnum == -1) {
		  for (auto& k : kindnum) {
			  testMain_.doAllTest(k - 1);
              testMain_.saveResult("result.txt");
          }
	  }
	  else {
		  for (auto& k : kindnum) {
			  testMain_.doTest(k - 1, testnum);
              testMain_.saveResult("result.txt");
          }
	  }
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
