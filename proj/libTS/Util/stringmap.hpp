// -*-tab-width:4-*-
#ifndef TS_UTIL_STRINGMAP_HPP
#define TS_UTIL_STRINGMAP_HPP
#include "ustring.hpp"

namespace ts {

//! 文字列マップクラス
  template <typename StringT>
  struct StringMapT {
	typedef UnifiedString<StringT> str_t;
	StringMapT() {}
	StringMapT(std::istream& ins, char demi = '=') {
	  load(ins, demi);
	}
	
	// istreamから読み込む
	void load(std::istream& ins, char demi = '=') {
	  while (!ins.eof()) {
		mstring demistr("=");
		demistr[0] = demi;
		str_t ustr;
		getline(ins, ustr);
		size_t pos = ustr.find(str_t(demistr.c_str())[0], 0);
		if (pos != str_t::npos) {
		  stringMap[ustr.substr(0, pos)] = ustr.substr(pos+1);
		}
	  }
	}
	
	// ostreamへ書き込む
	void save(std::ostream& outs, char demi = '=') {
	  for (typename std::map<str_t, str_t>::iterator i = stringMap.begin(); i != stringMap.end(); ++i) {
		outs << i->first.toChar() << demi << i->second.toChar() << std::endl;
	  }
	}

	
	const str_t& operator [] (const str_t& key) {
	  return stringMap[key];
	}
	
	bool isExist(const str_t& key) const {
	  return stringMap.find(key) != stringMap.end();
	}
	
	int32_t getInt(const str_t& key) {
	  return stringMap[key].toInt();
	}
	
	uint32_t getUint(const str_t& key) {
	  return static_cast<uint32_t>(stringMap[key].toInt());
	}
	
	double getDouble(const str_t& key) {
	  return stringMap[key].toDouble();
	}
	
	
	template <typename valT>
	void set(const str_t& key, valT val) {
	  stringMap[key] = str_t(val);
	}
	void set(const str_t& key, const str_t& val) {
	  stringMap[key] = val;
	}
	
	
	std::map<str_t, str_t> stringMap;
	
	/// unittest用メソッド
	POINTCUT_UNITTEST_METHOD
  };
  
  typedef StringMapT<StringTypeChar>  mstring_map;
  typedef StringMapT<StringTypeUtf16> ustring_map;

}
#endif
