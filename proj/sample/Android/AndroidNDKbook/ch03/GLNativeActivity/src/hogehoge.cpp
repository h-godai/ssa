/*
 * hogehoge.cpp
 *
 *  Created on: 2013/02/05
 *      Author: godai
 */



#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <boost/shared_ptr.hpp>
//#include <iostream>
//using namespace std;

struct hogehoge {
//	std::vector<int> intvec_;
public:
	int hogehoge_;
	int hoho:8;
	int get() { return hogehoge_; }
	void dump() {
	  printf("%d\n", hogehoge_);
	}
	static boost::shared_ptr<hogehoge> create() {
		return boost::shared_ptr<hogehoge>(new hogehoge());
	}
};