//
// スコープ内で有効なmapとunordered_map
#include <iostream>
#include <memory>
#include <assert.h>
#include <boost/any.hpp>
#include <typeinfo>
#include <map>
#include <unordered_map>
#include <vector>

#include "static_string.hpp"
#include "static_map.hpp"
#include "static_unordered_map.hpp"
#include "static_vector.hpp"

using namespace ts;

int main() {
  using namespace std;

  // static_string test
  try {
	cout << "static_stringr start" << endl;
	static_string<10> str;
	str = "123456789";
	static_string<10> str2("hogehoge");
	static_string<100> str3{"hoge fuga piyo poyo boo"};
	static_wstring<10> str4{L"hogehoge!"};
  }
  catch (const std::exception& ex) {
	cerr << "static_string exception caught:" << ex.what() << endl;
  }
  
  // static_vector test
  try {
	cout << "static_vector start" << endl;
	static_vector<int,3> vec;
	vec.push_back(1);
	vec.push_back(2);
	vec.push_back(3);
	// vec.push_back(4); // これを実行するとout_of_rangeがthrowされる
	static_vector<char,4>{1,2,3,4};
  }
  catch (const std::exception& ex) {
	cerr << "static_vector exception caught:" << ex.what() << endl;
  }

  //scoped_map_sample
  try {
	cout << "scoped_map start" << endl;
	static_map<int, int, 3> hoge = {{1,1}};
	hoge.insert(make_pair(1,1.0));
	
	hoge[1] = 2;
	hoge[0] = 1;
	hoge[2] = 3;
  }
  catch (const std::exception& ex) {
	cerr << "scoped_map exception caught:" << ex.what() << endl;
  }

  // scoped_unordered_map_sample
  try {
	cout << "scoped_unorderd_map<int,double> start" << endl;
	static_unordered_map<int, double, 1000> fuga{{1,1.1}};
	fuga[2] = 2.1;
	fuga[0] = 1.1;
	fuga[3] = 3.1;
	fuga[1] = 0.1;

	cout << "scoped_unorderd_map<static_string,int> start" << endl;
	using string10 = static_string<10>;
	static_unordered_map<string10, int, 1000, string10::hash> strmap = {
	  {"hoge",5}, {"fuga",4}, {"piyo",3}, {"apoon",2}
	};
	cout << "fuga is " << strmap["fuga"] << endl;
	cout << "piyo is " << strmap["piyo"] << endl;
	cout << "foo  is " << strmap["foo"] << endl;
  }
  catch (const std::exception& ex) {
	cerr << "scoped_unordered_map exception caught:" << ex.what() << endl;
  }
  
}



// static_map<int, const char*, 256> static_string{{1,"hoge"}, {2,"fuga"}, {3,"piyo"}};



