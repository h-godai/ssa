/* -*-tab-width:4-*-
   libTS appfw exception.hpp
 */
#ifndef TS_APPFW_EXCEPTION_HPP
#define TS_APPFW_EXCEPTION_HPP

#include <string>

namespace ts{
namespace appfw {

  struct appfw_exception : std::exception {
	appfw_exception(const char* const & msg) : std::exception()
											 , msg_(msg)
	{}
	virtual ~appfw_exception() throw() {}
	virtual const char* what() const throw() { return msg_.c_str(); }
	std::string msg_;
  };
  
  struct app_exit_exception : appfw_exception {
	app_exit_exception() : appfw_exception("exit_exception") {}
	virtual ~app_exit_exception() throw() {}
  };

  struct drawframe_exception : appfw_exception {
	drawframe_exception(const char* const & msg) : appfw_exception(msg) {}
	virtual ~drawframe_exception() throw() {}
  };

  struct buffer_over_run : std::exception {
	buffer_over_run() {}
	virtual ~buffer_over_run() throw() {}
	virtual const char* what() const throw() { return "buffer over run"; }
  };

  struct file_can_not_open : std::exception {
	file_can_not_open(const std::string& fn) : fn_(fn) {}
	virtual ~file_can_not_open() throw() {}
	virtual const char* what() const throw() { return fn_.c_str(); }
	std::string fn_;
  };


  
  

}} // namespace
#endif
