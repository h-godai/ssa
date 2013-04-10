// -*-tab-width:4-*-
#define _CRT_SECURE_NO_WARNINGS

#include <algorithm>
#include <stdexcept>
#include "unittest.hpp"
#include "ustring.hpp"
#ifdef USE_TS_LOGGER
#include "Logger.h"
#endif

#ifdef WIN32
#pragma warning (disable :4512)
#pragma warning (disable :4996)
#endif

using namespace std;

namespace ts {

  struct FatalErrorException : public exception {
	FatalErrorException() {}
	virtual ~FatalErrorException() throw() {}
	virtual const char* what() const throw() { return "test aborted"; }

  };
  
  struct DoTest {
	Unittest* my;
	DoTest(Unittest* l) : my(l) {}
	void operator() (Unittest::TestProc proc) {
	  if (!proc(my)) throw FatalErrorException(); 
	}
  };

  

  Unittest& Unittest::Instance() {
	static Unittest mySelf;
	return mySelf;
  }
  
  void Unittest::append(Unittest::TestProc proc) {
	Instance() += proc;
  }
  
  bool  Unittest::run(const std::string& testname) {
	m_testname = testname;
    bool success = false;
	m_count = 0;
	m_success = 0;
	m_log.clear();
	m_errlog.clear();
    m_log << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>> unittest [" << m_testname << "]\n";
    m_outmsg(m_log.str());
	
	DoTest doTest(this);
	try {
	  for_each(m_testList.begin(), m_testList.end(), doTest);
      success = m_count == m_success;
	}
	catch (const exception& ex) {
      string msg;
	  msg += "\n!!!!! exception cought in unittest [" + m_testname + "] !!!!! '";
	  msg += ex.what();
	  msg += "'\n";
	  m_log << msg;
	  m_outmsg(msg);
	}
	
	mstring log2;
	log2 += "===== unittest finish =====\n";
	log2 += mstring(m_count) + " tests\n";
	log2 += mstring(m_success) + " succeeded\n";
	log2 += mstring(m_count-m_success) + " failure\n";
	log2 += "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< unittest [" + m_testname +"]\n";
	m_outmsg(log2);
	m_log << log2.body();
	
	return success;
	
  }

  void Unittest::message(const string& msg) {
    m_outmsg(msg);
    m_log << msg;
  }
  void Unittest::startMessage(const string& msg) {
	std::string str("*** start ");
	str += msg + " unittest ***\n";
    m_outmsg(str);
    m_log << str;
  }

  bool Unittest::check(bool chk, const std::string& prefix, const char* comment) {
	std::string msg = prefix;
	++m_count;
	if (chk) {
      if (!m_silent) {
		msg += ", OK!";
		if (comment != 0) msg += std::string(", \"") + comment + "\"";
		msg += "\n";
		m_log << msg;
		m_outmsg(msg);
      }
	  ++m_success;
	  return true;
	}
	else {
	  msg += ", ### Failure! ###";
	  if (comment != 0) msg += std::string(", \"") + comment + "\"";
	  msg += "\n";
	  m_log << msg;
	  m_errlog << msg;
      m_outerrmsg(msg);
	  return false;
	}
  }
  
  void Unittest::DefaultOutMsg(const std::string& msg) {
#ifdef USE_TS_LOGGER
	TS_TESTLOG("%s", msg.c_str());
#else
	printf("%s", msg.c_str());
#endif
  }
  void Unittest::DefaultOutErrMsg(const std::string& msg) {
#ifdef USE_TS_LOGGER
	TS_ERRORLOG("%s", msg.c_str());
#else
	fprintf(stderr, "%s", msg.c_str());
#endif
  }
	  
}

