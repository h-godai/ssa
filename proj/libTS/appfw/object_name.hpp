/* object_name.hpp -*-tab-width:4-*-


 */

#ifndef TS_APPFW_OBJECTNAME_HPP
#define TS_APPFW_OBJECTNAME_HPP

#include <string>

namespace ts {
namespace appfw {


  struct object_id_counter {
	uint32_t count_;
	object_id_counter() : count_(0) {}
	static uint32_t get_count() {
	  static object_id_counter c;
	  return c.count_++;
	}
  };

  struct object_name {
	uint32_t id_;
	std::string name_;
	static const uint32_t invalid_id = 0xffffffff;
	object_name() : id_(invalid_id) {}
	object_name(uint32_t id) : id_(id) {
	  TS_DEBUGLOG("object_name set id to %d\n", id);
	}
	object_name(const std::string& name) : id_(0), name_(name) {}
	object_name(const char* name) : id_(0), name_(name) {}
	object_name(const object_name& o) : id_(o.id_), name_(o.name_) {}
	bool is_valid() const { return id_ != invalid_id; }
	bool operator == (const object_name& o) const {
	  //if (!is_valid()) return false;
	  if (name_.empty()) return id_ == o.id_;
	  return name_ == o.name_;
	}
	bool operator == (uint32_t id) const {
	  return id_ == id;
	}
	bool operator == (const std::string& name) const {
	  if (name_.empty()) return false;
	  return name_ == name;
	}
	bool operator != (const std::string& name) const {
	  if (name_.empty()) return true;
	  return name_ != name;
	}
	bool operator != (const char* name) const {
	  return !(*this == name);
	}
	bool operator == (const char* name) const {
	  if (name_.empty()) return false;
	  return name_ == name;
	}
	//operator std::string () { return name_; }
	operator const std::string& () const { return name_; }
	//operator uint32_t () const { return id_; }
	const char* c_str() const { return name_.c_str(); }
  };
}} // ts::appfw
#endif
