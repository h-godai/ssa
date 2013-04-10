// -*-tab-width:4-*-
#include <typeinfo>
#include <iostream>
#include <sstream>
#include "ustring.hpp"
#include "unittest.hpp"
#include "mlstring.hpp"
#include "stringmap.hpp"

#ifndef TRACE
#define TRACE __noop
#endif

#ifdef WIN32
#pragma warning (disable : 4996)
#endif

using namespace std;

namespace ts {
  
  ///////////////////////////////////////////////////////////////////////////////////////////
  // 単体試験
  ///////////////////////////////////////////////////////////////////////////////////////////

  template <typename StringT>
  bool UnifiedString<StringT>::unittest(Unittest* test) {
    if (test == 0) return false;
	
    std::string typeName(CharactorTypeName<StringT::DefaultCharaSet>::Name());
	
	test->startMessage("ts::UnifiedString<"+typeName+">");
  	
	test->check(UnifiedString::npos == wstring::npos, typeName + "::npos check");
	
	UnifiedString<StringT> ustr1("abcd",3), ustr2(L"ABCDE",3);
	test->check(ustr1 == L"abc" && ustr2 == "ABC", typeName + "::constructor check");

	ustr2.tolower();
	test->check(ustr2 == "abc",  typeName + "::tolower check");

	ustr2 = "abc";
	ustr2.toupper();
	test->check(ustr2 == "ABC",  typeName + "::toupper check");

	ustr2 = "abc";
	test->check(ustring::toWstring("abc") == L"abc",  typeName + "::toWstring check");
	
	UnifiedString<StringT> ustr3(ustr1), ustr4(string("hoge")), ustr5(L"hoge");
	ustr3.append(ustr4);
	ustr3 += ustr5;
	test->check(strcmp(ustr3.toChar(), "abchogehoge") == 0,  typeName + "::toChar check");
	
	ustr3.replace_string(UnifiedString<StringT>("hoge"), UnifiedString<StringT>("hogehoge"));
	test->check(strcmp(ustr3.toChar(), "abchogehogehogehoge") == 0,  typeName + "::replace_string check1");
	ustr3.replace_string(UnifiedString<StringT>("hoge"), UnifiedString<StringT>(""));
	test->check(strcmp(ustr3.toChar(), "abc") == 0,  typeName + "::replace_string check2");
	
	UnifiedString<StringT> ustr6(123456789);
	test->check(ustr6 == "123456789",  typeName + "::int constructor check");

    ustr1 = "%.1f %d %s";
    ustr2 = "hoge";
    ustr6.format(ustr1.c_str(), 1.2f, 123, ustr2.toChar());
	test->message("format:");
	test->message(ustr6 + "\n");
    test->check(ustr6 == "1.2 123 hoge", typeName + "::format check");

    return true;
  }


  POINTCUT_UNITTEST_REGIST(ustring);
  POINTCUT_UNITTEST_REGIST(mstring);


#if 1
  template <typename S> struct MLStringName {};
  template <> struct MLStringName<ustring> { static const char* Name() { return "ustring"; }};
  template <> struct MLStringName<mstring> { static const char* Name() { return "mstring"; }};
  template <> struct MLStringName<std::string> { static const char* Name() { return "std::string"; }};
  template <> struct MLStringName<std::wstring> { static const char* Name() { return "std::wstring"; }};

  
  template <typename stringT>
  bool MultiLineString<stringT>::unittest(Unittest* test) {
    if (test == 0) return false;
	std::string typeName(MLStringName<stringT>::Name());

	  test->startMessage("ts::MultiLineString<"+typeName+">");

	  stringT src("000\n\n2\n3\n");
	  MultiLineString<stringT> mlu(src);
	  test->check(mlu.count() == 4 && mlu.line_[2] == stringT("2"), typeName+" check1");
	  return true;
  }
  template <>
  bool MultiLineString<std::wstring>::unittest(Unittest* test) {
    if (test == 0) return false;
	  test->startMessage("ts::MultiLineString<mlwstring>");

	  wstring src(L"000\n\n2\n3\n");
	  MultiLineString<wstring> mlu(src);
	  test->check(mlu.count() == 4 && mlu.line_[2] == wstring(L"2"), "mlwstring check1");
	  return true;
  }

  POINTCUT_UNITTEST_REGIST(mlustring);
  POINTCUT_UNITTEST_REGIST(mlmstring);
  POINTCUT_UNITTEST_REGIST(mlwstring);
//  POINTCUT_UNITTEST_REGIST(mlstring);


  template <typename charT>
  bool StringMapT<charT>::unittest(Unittest* test) {
    if (test == 0) return false;
    std::string typeName(charT::CharName());
	test->startMessage("ts::StringMap<UnifiedString<"+typeName+">>");

    std::string data = "hoge1hoge=1\n"
                       "hoge2=HOGE2\n"
                       "z___=~~~\n";
    std::stringstream strm;
    strm.str(data);

    StringMapT<charT> strmap(strm);
    test->check(strmap["hoge1hoge"] == str_t("1"), "access test");
    std::stringstream strmout;
    strmap.save(strmout);
    test->check(strmout.str() == data, "save test");


    return true;
  }

  POINTCUT_UNITTEST_REGIST(mstring_map);
  POINTCUT_UNITTEST_REGIST(ustring_map);
#endif

} // namespace ts


// テンプレートが生成されない問題を回避するための定義
namespace {
  struct hoge {
    ts::ustring us;
    ts::mstring ms;
    hoge() {
      us = L"hoge";
      ms = "hoge";
      us.unittest(0);
      ms.unittest(0);
    }
  } hogehoge;

}

