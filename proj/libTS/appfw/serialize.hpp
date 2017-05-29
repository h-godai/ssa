/* -*-tab-width:4-*-
   libTS appfw/serialize.hpp
 */
#ifndef TS_APPFW_SERIALIZE_HPP
#define TS_APPFW_SERIALIZE_HPP

#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
//#include "libTS/Util/bstream.hpp"
#include "libTS/appfw/exception.hpp"
#include "libTS/Util/util.hpp"

namespace ts {
namespace appfw {

  struct serialize {

	std::vector<uint8_t> buffer_;
	serialize() {}
	serialize(const serialize& s) : buffer_(s.buffer_) {}

	serialize(const std::string& str) { *this << str; }
	serialize(int32_t i)  { *this << i; }
	serialize(uint32_t i) { *this << i; }
	serialize(size_t i)  { *this << i; }
	serialize(float f)  { *this << f; }
	serialize(double d) { *this << d; }

	template <typename T1, typename T2>
	serialize(std::map<T1,T2>& map) {
	  this->operator << (map);
	}
	template <typename T1>
	serialize(std::vector<T1>& vec) {
	  this->operator << (vec);
	}

	//template <typename T>
	//serialize(const T& val) {
	//write(static_cast<const void*>(&val), sizeof(T));
	//}

	template <typename T1, typename T2>
	serialize& operator << (const std::map<T1,T2>& map) {
	  typename std::map<T1,T2>::const_iterator i = map.begin();
	  TS_DEBUGLOG("map:size=%d\n", map.size());
	  *this << map.size();	  
	  while (i != map.end()) {
		*this << (*i).first;
		*this << (*i).second;
		++i;
	  }
	  return *this;
	}
	
	template <typename T1>
	serialize& operator << (const std::vector<T1>& vec) {
	  typename  std::vector<T1>::const_iterator i = vec.begin();
	  *this << vec.size();
	  while (i != vec.end()) {
		*this << *i;
		++i;
	  }
	  return *this;
	}



	serialize& operator << (const std::string& str) {
	  *this << static_cast<uint32_t>(str.length());
	  if (str.length() != 0) {
		return write(static_cast<const void*>(str.c_str()), str.length()+1);
	  }
	  else {
		return *this;
	  }
	}
	serialize& operator << (int32_t val) { return write(&val, sizeof(int32_t)); }
	serialize& operator << (uint32_t val) { return write(&val, sizeof(uint32_t)); }
	serialize& operator << (size_t val) { return write(&val, sizeof(size_t)); }
	serialize& operator << (float val) { return write(&val, sizeof(float)); }
	serialize& operator << (double val) { return write(&val, sizeof(double)); }
	serialize& operator << (const serialize& obj) {
	  std::copy(obj.buffer_.begin(), obj.buffer_.end(), std::back_inserter(buffer_));
	  return *this;
	}
	

	serialize& write(const void* buffer, size_t length) {
	  size_t pos = buffer_.size();
	  buffer_.resize(pos + length );
	  memcpy(&buffer_[pos], buffer, length);
	  return *this;
	}

	// to ostream
	void operator >> (boost::shared_ptr<std::ostream> os) const { *this >>  *os; }
	void operator >> (std::ostream& os) const {
	  size_t sz = buffer_.size();
      os.write(reinterpret_cast<const char*>(&sz), sizeof(size_t));
	  if (sz != 0) {
		os.write(reinterpret_cast<const char*>(&buffer_[0]), static_cast<std::streamsize>(sz));
	  }
	}
  };

  // アーカイブからデータを取り出す
  struct deserialize {
	std::vector<uint8_t> buffer_;
	mutable size_t pos_;
	deserialize() : pos_(0) 
	{}

	deserialize(boost::shared_ptr<std::istream> is) 
	  : pos_(0)
	{ 
	  try {
		load_from_istream(*is); 
	  }
	  catch (const buffer_over_run&) {
		// 読み込みエラー
		pos_ = 0;
		buffer_.clear();
	  }
	}
	

	const deserialize& operator >> (int32_t& val) const { return read(&val, sizeof(int32_t)); }
	const deserialize& operator >> (uint32_t& val) const { return read(&val, sizeof(uint32_t)); }
	const deserialize& operator >> (size_t& val) const { return read(&val, sizeof(size_t)); }
	const deserialize& operator >> (float& val) const { return read(&val, sizeof(float)); }
	const deserialize& operator >> (double& val) const { return read(&val, sizeof(double)); }


	const deserialize& operator >> (std::string& str) const {
	  uint32_t length;
	  *this >> length;
	  if (length != 0) {
		char* buf = new char[length+1];
		str.resize(length+1);
		read(buf, length+1);
		str = buf;
		delete [] buf;
	  }
	  return *this;
	}


	// from istream 
	void operator << (boost::shared_ptr<std::istream> os) { load_from_istream(*os); }
	void operator << (std::istream& ins) { load_from_istream(ins);}

	// istreamからデータを読み込む
	void load_from_istream(std::istream& ins)  throw (buffer_over_run) {
	  // 最初の4バイトはデータの長さ
	  size_t size = 0;
	  ins.read(reinterpret_cast<char*>(&size), sizeof(size));
	  TS_DEBUGLOG("%dbytes data stream\n", size);
      if (size > 0) {
		buffer_.resize(size);
        ins.read(reinterpret_cast<char*>(&buffer_[0]), size);
		if (ins.bad()) {
		  // 読み込みエラー
		  throw buffer_over_run();
		}
      }
	  else {
		buffer_.clear();
	  }
	}

	template <typename T1>
	const deserialize& operator >> (std::vector<T1>& vec) const {
	  typename  std::vector<T1>::iterator i = vec.begin();
	  size_t count;
	  *this >> count;
	  vec.resize(count);
	  for (size_t n = 0; n < count; ++n) {
		T1 val;
		*this >> val;
		vec[n] = val;
	  }
	  return *this;
	}

    
	// mapデータへの読み込み
	template <typename T1, typename T2>
	const deserialize& operator >> (std::map<T1,T2>& map) const {
	  size_t count;
	  *this >> count;
	  TS_DEBUGLOG("map:count=%d\n", count);
	  while (count-- > 0) {
		T1 key;
		T2 value;
		*this >> key;
		*this >> value;
		map[key] = value;
		//std::stringstream ss;
		//TS_DEBUGLOG("map[%s]=%s\n", ts::to_string(key).c_str(), ts::to_string(value).c_str());
	  }
	  return *this;
	}

  private:
	const deserialize& read(void* buffer, size_t length) const throw(buffer_over_run) {
	  if (buffer_.size() < pos_ + length) {
		throw buffer_over_run();
	  }
	  memcpy(buffer, &buffer_[pos_], length);
	  pos_ += length;
	  return *this;
	}
  };



  // 文字列をファイル名としてostreamのポインタを生成する
  inline boost::shared_ptr<std::ostream> to_ostream(const std::string& str) {
	std::ofstream* ofs = new std::ofstream(str.c_str(), ios::binary);
	if (!ofs || !ofs->is_open()) {
	  if (ofs) delete ofs;
	  throw file_can_not_open(str);
	}
	return boost::shared_ptr<ostream>(ofs);
  }
  // 文字列をファイル名として、istreamのポインタを生成する
  inline boost::shared_ptr<istream> to_istream(const std::string& str) {
	std::ifstream* ifs = new std::ifstream(str.c_str(), ios::binary);
	if (!ifs || !ifs->is_open()) {
	  if (ifs) delete ifs;
	  throw file_can_not_open(str);
	}
	return boost::shared_ptr<istream>(ifs);
  }

  // istream >> deserialize のオペレータ
  inline deserialize operator >> (boost::shared_ptr<istream> ist, deserialize sel) {
	sel << ist;
	return sel;
  }
  // ostream << serialize のオペレータ
  inline serialize operator << (boost::shared_ptr<ostream> ist, serialize sel) {
	sel >> ist;
	return sel;
  }






}} // ts::appfw
#endif
