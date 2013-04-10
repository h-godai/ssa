// binary stream class


#ifndef TSL_BSTREAM_HPP
#define TSL_BSTREAM_HPP

#ifndef STDAFX_H
#include <stdexcept>
#include <vector>
#include <string>
#include <iostream>
#include <boost/foreach.hpp>
#include <stdlib.h>
#endif
#include "libTS/Common/Common.h"
#include "unittest.hpp"
#include "ustring.hpp"

namespace ts {

  class bstream {
  public:
    //! @brief デフォルトコンストラクタ
    bstream() : m_buffer(0), m_size(0), m_offset(0), m_reference(false) {}

    //! @brief バッファとサイズから生成
    //! @param[in] buf 元となるバッファのポインタ
    //! @param[in] length バッファのサイズ
    //! @param[in] reference tureの場合は参照する。falseの場合はデータをコピーする
    bstream(uint8_t* buf, size_t length, bool reference=true) 
      : m_buffer(buf)
      , m_size(length)
      , m_offset(0)
      , m_reference(reference) 
      , m_ignoreOverRun(false)
    {
      if (!m_reference) {
        // バッファを自前で持つ場合
        alloc(m_size);
        memcpy(m_buffer, buf, m_size);
      }
    }

    //! @brief バッファを生成
    //! @param[in] length バッファのサイズ
    bstream(size_t length) 
      : m_size(length)
      , m_offset(0)
      , m_reference(false)
      , m_myBuffer(length)
      , m_ignoreOverRun(false)
    {
      m_buffer = &m_myBuffer[0];
    }

     //! @brief istreamからバッファを作成
     //! @note  istreamを終わりまで読みこんでバッファを作成する
     //! @note  serializeしたデータは先頭4バイトにlengthが入っているので注意
     bstream(std::istream& ins) 
       : m_size(0)
       , m_offset(0) {
       readFrom(ins);
     }

     //! @brief FILE*からバッファを作成
     //! @note  ファイルを最初から終わりまで読みこんでバッファを作成する
     //! @note  serializeしたデータは先頭4バイトにlengthが入っているので注意
     bstream(FILE* file) 
       : m_size(0)
       , m_offset(0) {
       readFrom(file);
     }



    //! @brief コピーコンストラクタ
    bstream(const bstream& src) {
      *this = src;
    }

    virtual ~bstream() {
      clear();
    }

    bstream& operator = (const bstream& src) {
      if (src.m_reference) {
        this->m_reference = true;
        this->m_buffer = src.m_buffer;
        this->m_size = src.m_size;
        this->m_offset = src.m_offset;
      }
      else {
        this->alloc(src.m_size);
        memcpy(this->m_buffer, src.m_buffer, this->m_size);
        this->m_offset = src.m_offset;
        this->m_reference = false;
      }
      return *this;
    }


    void clear() {
      if (!m_reference) {
        m_myBuffer.clear();
      }
      m_buffer = 0;
      m_reference = false;
      m_size = 0;
      m_offset = 0;
    }


    //! buf,lengthからデータを割り当てる（参照)
    void attach(uint8_t* buf, size_t length) {
      clear();
      m_size = length;
      m_buffer = buf;
      m_reference = true;
    }

    //! ファイルポインタを元に戻す
    void rewind() const {
      m_offset = 0;
    }

    //! 残りサイズを返す
    size_t getLeft() const {
      return m_size - m_offset;
    }
    bool empty() const { return m_size == 0; }
    bool is_eof() const { return getLeft() == 0; }

    //! 読み込みのオーバーランを無視する
    void ignoreOverRun(bool enable=true) const {
      m_ignoreOverRun = enable;
    }


    // vectorタイプのread
    template <typename DataT>
    const bstream& read(std::vector<DataT>& data) const {
      if (m_ignoreOverRun && getLeft() == 0) return *this;
      size_t sz;
      read(sz);
      data.resize(sz);
      for (size_t i = 0; i < sz; ++i) {
        *this >> data[i];
      }
      return *this;
    }



     template <typename ReadT>
    const bstream& read(ReadT& data) const {
      if (m_ignoreOverRun && getLeft() == 0) return *this;
      if (m_offset + sizeof(ReadT) > m_size) throw std::out_of_range("bstream::read");
      memcpy(static_cast<void*>(&data), m_buffer + m_offset, sizeof(ReadT));
      m_offset += sizeof(ReadT);
      return *this;
    }


    const bstream& read(uint32_t& n) const {
      if (m_ignoreOverRun && getLeft() == 0) return *this;
      if (m_offset + 4 > m_size) throw std::out_of_range("bstream::read");
      uint8_t* ptr = m_buffer + m_offset;
      n = *ptr + (*(ptr+1)<<8) + (*(ptr+2)<<16) + (*(ptr+3)<<24);
      m_offset += 4;
      return *this;
    }

    const bstream& read(int32_t& n) const {
      if (m_ignoreOverRun && getLeft() == 0) return *this;
      return read(reinterpret_cast<uint32_t&>(n));
    }


    const bstream& read(uint16_t& n) const {
      if (m_ignoreOverRun && getLeft() == 0) return *this;
      if (m_offset + 2 > m_size) throw std::out_of_range("bstream::read");
      uint8_t* ptr = m_buffer + m_offset;
      n = *ptr + (*(ptr+1)<<8);
      m_offset += 2;
      return *this;
    }

    const bstream& read(int16_t& n) const {
      return read(reinterpret_cast<uint16_t&>(n));
    }


    const bstream& read(uint8_t& n) const {
      if (m_ignoreOverRun && getLeft() == 0) return *this;
      if (m_offset + 1 > m_size) throw std::out_of_range("bstream::read");
      n = *(m_buffer + m_offset);
      m_offset += 1;
      return *this;
    }

    const bstream& read(int8_t& n) const {
      return read(reinterpret_cast<uint8_t&>(n));
    }

    template <typename ReadT>
    const bstream& readArray(ReadT& ar) const {
      for (typename ReadT::iterator i = ar.begin(); i != ar.end(); ++i) {
        *this >> *i;
      }
      return *this;
    }

    // データとサイズでbstreamから読み込む
    template <typename ReadT>
    const bstream& read(ReadT& data, size_t size) const {
      if (m_ignoreOverRun && getLeft() == 0) return *this;
      if (m_offset + size > m_size) throw std::out_of_range("bstream::read");
      memcpy(&data, m_buffer + m_offset, size);
      m_offset += size;
      return *this;
    }

    // データとサイズでbstreamに書き込む
    template <typename DataT>
    bstream& write(const DataT& data, size_t size) {
      if (m_reference) throw std::out_of_range("bstream::write");
      if (m_offset + size > m_size) {
        alloc(m_offset + size);
      }
      if (size > 0) {
        memcpy(&m_myBuffer[m_offset], &data, size);
      }
      m_offset += size;
      return *this;
    }



    // stringの読み込み。
    const bstream& readString(std::string& str) const {
      if (m_ignoreOverRun && getLeft() == 0) return *this;
      uint32_t sz;
      read(sz);
      if (sz > 1024*1024) throw std::out_of_range("bstream::readString"); // 1M以上はNG
      str.resize(sz);
      read(str[0], sz);
      return *this;
    }

    // wstringの読み込み。
    const bstream& readString(std::wstring& str) const {
      if (m_ignoreOverRun && getLeft() == 0) return *this;
      uint32_t sz;
      read(sz);
      if (sz > 1024*1024) throw std::out_of_range("bstream::readString"); // 1M以上はNG
      str.resize(sz / 2);
      read(str[0], sz);
      return *this;
    }

    // vectorタイプのwrite
    template <typename DataT>
    bstream& write(const std::vector<DataT>& data) {
      write(static_cast<uint32_t>(data.size()));
      for (typename std::vector<DataT>::const_iterator i = data.begin(); i != data.end(); ++i) {
        *this << *i;
      }
      return *this;
    }


    template <typename DataT>
    bstream& write(const DataT& data) {
      return write(data, sizeof(data));
    }

    bstream& write(uint32_t n) {
#ifdef LITTLE_ENDIAN
      return write(n, sizeof(uint32_t));
#else
      uint8_t d[4];
      d[0] = static_cast<uint8_t>(n & 0xff);
      d[1] = static_cast<uint8_t>((n >> 8) & 0xff);
      d[2] = static_cast<uint8_t>((n >> 16) & 0xff);
      d[3] = static_cast<uint8_t>((n >> 24) & 0xff);
      return write(d);
#endif
    }

    bstream& write(int32_t n) {
      return write(static_cast<uint32_t>(n));
    }

  
    // stringの書き出し
    bstream& writeString(const std::string& str) {
      uint32_t sz = static_cast<uint32_t>(str.size());
      write(sz);
      if (sz > 0) {
        write(str[0], sz);
      }
      return *this;
    }

    // wstringの書き出し
    bstream& writeString(const std::wstring& str) {
      uint32_t sz = static_cast<uint32_t>(str.length() * sizeof(wchar_t));
      write(sz);
      if (sz > 0) {
        write(str[0], sz);
      }
      return *this;
    }

    // const bstreamから読み込み
    // 最初の4バイトはlength
    size_t deserialize(const bstream& bs) {
      size_t len = deserialize(bs.m_buffer, bs.m_offset, bs.m_size);
      bs.skip(static_cast<int32_t>(len));
      return len;
    }

    // uint8_t*から読み込み
    // 最初の4バイトはlength
    size_t deserialize(uint8_t* buf, size_t offset, size_t bufsz, bool reference=true) {
      clear();
      uint8_t* ptr = buf + offset;
      size_t length;

      if (offset + 4 > bufsz) throw std::out_of_range("bstream::deserialize");
      length = *ptr + (*(ptr+1)<<8) + (*(ptr+2)<<16) + (*(ptr+3)<<24);
      if (offset + 4 + length > bufsz) throw std::out_of_range("bstream::deserialize");
      m_size = length;
      m_reference = reference;
      if (reference) {
        m_buffer = ptr + 4;
      }
      else {
        alloc(m_size);
        memcpy(m_buffer,  ptr + 4, length);
      }
      m_offset= 0;
      return length+4;
    }

    // ostreamから読み出す
    void deserialize(std::istream& is) {
      clear();
      is.read(reinterpret_cast<char*>(&m_size), sizeof(m_size));
      if (m_size > 0) {
        alloc(m_size);
        is.read(reinterpret_cast<char*>(m_buffer), static_cast<std::streamsize>(m_size));
      }
      m_offset = 0;
      m_reference = false;
    }

    // bstreamに書き出す
    // 最初の4バイトはlength
    void serialize(bstream& bs) const {
      bs.write(static_cast<uint32_t>(m_size));
      bs.write(m_buffer[0], static_cast<uint32_t>(m_size));
    }
    // ostreamに書き出す
    // 最初の4バイトはlength
    void serialize(std::ostream& os) const {
      os.write(reinterpret_cast<const char*>(&m_size), sizeof(m_size));
      os.write(reinterpret_cast<char*>(m_buffer), static_cast<std::streamsize>(m_size));
    }

    // Fileに書き出す
    const bstream& writeTo(FILE* file) {
      ::fwrite(m_buffer, m_size, 1, file);
      return *this;
    }
    // Fileから読み込む
    bstream& readFrom(FILE* file) {
      m_offset = 0;
      m_reference = false;
      ::fseek(file, 0, SEEK_END);
      long pos = ::ftell(file);
      alloc(static_cast<size_t>(pos));
      ::fseek(file, 0, SEEK_SET);
      if (m_size > 0) {
        if (::fread(m_buffer, m_size, 1, file) != m_size) {
	  throw std::out_of_range("bstream::read error");
	}
      }
      return *this;
    }
    //! @brief istreamからバッファを作成
    //! @note  32bit環境ではストリームサイズが4GBに制限されるので注意
    bstream& readFrom(std::istream& ins) {
      m_offset = 0;
      m_reference = false;
      // 現在のポジションからストリームの最後まで読み込む
      std::streamsize cur = ins.tellg();
      ins.seekg(0, std::istream::end);
      std::streamsize size = ins.tellg();
      size -= cur;
      ins.seekg(cur, std::istream::beg);
      alloc(size);
      if (size > 0) {
        ins.read(getBuffer<char*>(), size);
      }
      return *this;
    }

    template <typename RetT>
    const RetT getBuffer() const { return reinterpret_cast<RetT>(m_buffer + m_offset); }
    template <typename RetT>
    RetT getBuffer() { return reinterpret_cast<RetT>(m_buffer + m_offset); }
    size_t size() const { return m_size; }

    void skip(int n) const { m_offset += n; }

    size_t getPos() const { return m_offset; }

    //! 単体試験用メソッド
  public:

    POINTCUT_UNITTEST_METHOD;

  private:
    //! lengthサイズのバッファを確保する
    //  boost::poolを使うべき?
    void alloc(size_t length) {
      m_size = length;
      if (length > 0) {
        if (length < 512) {
          m_myBuffer.reserve(512); // 最低でも512バイト割り当てる
        }
        m_myBuffer.resize(length);
        m_buffer = &m_myBuffer[0];
      }
      else {
        m_myBuffer.clear();
        m_buffer = 0;
      }
    }

  private:
    uint8_t* m_buffer;
    size_t m_size;
    mutable size_t m_offset;
    bool m_reference;
    std::vector<uint8_t> m_myBuffer;
    mutable bool m_ignoreOverRun;
  };

  // bstream から対象のオブジェクトに書き出す
  inline    const bstream& operator >> (const bstream& b, float64_t& n) { return b.read(n); }
  inline    const bstream& operator >> (const bstream& b, float32_t& n) { return b.read(n); }
  inline    const bstream& operator >> (const bstream& b, uint64_t& n) { return b.read(n); }
  inline    const bstream& operator >> (const bstream& b, int64_t& n) { return b.read(n); }
  inline    const bstream& operator >> (const bstream& b, uint32_t& n) { return b.read(n); }
  inline    const bstream& operator >> (const bstream& b, int32_t& n) { return b.read(n); }
  inline    const bstream& operator >> (const bstream& b, uint8_t& n) { return b.read(n); }
  inline    const bstream& operator >> (const bstream& b, int8_t& n) { return b.read(n); }
  inline    const bstream& operator >> (const bstream& b, std::string& s) { return b.readString(s); }
  inline    const bstream& operator >> (const bstream& b, std::wstring& s) { return b.readString(s); }
  inline    const bstream& operator >> (const bstream& b, ustring& s) { return b.readString(s.body()); }
  inline    const bstream& operator >> (const bstream& b, mstring& s) { return b.readString(s.body()); }
  inline    const bstream& operator >> (const bstream& b, std::ostream& os) { b.serialize(os); return b; }
  inline    const bstream& operator >> (const bstream& b, bstream& n) { n.deserialize(b); return b; }
  template <typename T> inline const bstream& operator >> (const bstream& b, T& n) { return b.read(n); }

 
  // bstream へ書き込む
  inline bstream& operator << (bstream& b, float64_t n) { return b.write(n); }
  inline bstream& operator << (bstream& b, float32_t n) { return b.write(n); }
  inline bstream& operator << (bstream& b, uint64_t n) { return b.write(n); }
  inline bstream& operator << (bstream& b, int64_t n) { return b.write(n); }
  inline bstream& operator << (bstream& b, uint32_t n) { return b.write(n); }
  inline bstream& operator << (bstream& b, int32_t n) { return b.write(n); }
  inline bstream& operator << (bstream& b, uint8_t n) { return b.write(n); }
  inline bstream& operator << (bstream& b, int8_t n) { return b.write(n); }
  inline    bstream& operator << (bstream& b, std::string& n) { return b.writeString(n); }
  inline    bstream& operator << (bstream& b, const std::string& n) { return b.writeString(n); }
  inline    bstream& operator << (bstream& b, std::wstring& n) { return b.writeString(n); }
  inline    bstream& operator << (bstream& b, const std::wstring& n) { return b.writeString(n); }
  inline    bstream& operator << (bstream& b, ustring& n) { return b.writeString(n.body()); }
  inline    bstream& operator << (bstream& b, const ustring& n) { return b.writeString(n.body()); }
  inline    bstream& operator << (bstream& b, mstring& n) { return b.writeString(n.body()); }
  inline    bstream& operator << (bstream& b, const mstring& n) { return b.writeString(n.body()); }
  inline    bstream& operator << (bstream& b, std::istream& n) { b.deserialize(n); return b; }
  inline    bstream& operator << (bstream& b, const bstream& n) { n.serialize(b); return b; }
  inline    bstream& operator << (bstream& b, bstream& n) { n.serialize(b); return b; }
  template <typename T> bstream& operator << (bstream& b, T& n) { return b.write(n); }



  inline
  std::ostream& operator << (std::ostream& os, const bstream& bs) {
    bs.serialize(os);
    return os;
  }

  inline
  std::istream& operator >> (std::istream& is, bstream& bs) {
    bs.deserialize(is);
    return is;
  }

} // namespace tsl

#endif

