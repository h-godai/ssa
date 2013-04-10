#ifndef UTIL_CONFIGURATION_H
#define UTIL_CONFIGURATION_H

#ifndef STDAFX_H
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <fstream>
#endif

#include "Common/Common.h"
#include "ustring.hpp"

namespace ts {

  using std::vector;
  using std::string;
  using std::wstring;
  using std::ifstream;
  using std::ofstream;

  class Configuration {
  public:
    Configuration() {}

    struct Xget {
      mstring key_;
      Xget(const mstring& key) : key_(key) {}

      operator const char* () const    { return Configuration::GetInstance().m_data[key_].c_str(); }
      operator const wchar_t* () const { return Configuration::GetInstance().m_data[key_].toWChar(); }
      operator const mstring& () const { return Configuration::GetInstance().m_data[key_]; }
      operator const string& () const { return Configuration::GetInstance().m_data[key_].body(); }
      operator int () const        { return Configuration::GetInstance().m_data[key_].get(); }
      operator int32_t () const        { return Configuration::GetInstance().m_data[key_].get(); }
      operator uint32_t () const       { return Configuration::GetInstance().m_data[key_].get(); }
      operator float ()  const         { return Configuration::GetInstance().m_data[key_].get(); }
      operator double () const         { return Configuration::GetInstance().m_data[key_].get(); }
      operator bool () const           { return Configuration::GetInstance().m_data[key_].get(); }
    };

    // 文字列もしくは整数で取る
    static Xget Get(const mstring& key) {
      return Xget(key);
    }

    static const mstring& GetStr(const mstring& key) {
      return GetInstance().m_data[key];
    }

    // 文字列で値をセットする
    static void Set(const mstring& key, const mstring& value) {
        GetInstance().m_data.set(key, value);
    }
    // 文字列で値をセットする
    template <typename T>
    static void Set(const mstring& key, T value) {
        GetInstance().m_data.set(key, value);
    }

    static void Save() {
        Configuration& my = GetInstance();
        if (my.m_filename.empty()) return;
        ofstream ofs(my.m_filename.c_str());
        if (!ofs.is_open()) return;
        my.m_data.save(ofs);
    }

    static void Load(const std::wstring& fn) {
        Configuration& my = GetInstance();
        my.m_filename = fn;
        ifstream ifs(fn.c_str());
        if (!ifs.is_open()) return;
        my.m_data.load(ifs);
    }



    private:
      // シングルトンアクセス用
      static Configuration& GetInstance() {
          static Configuration myInstance;
          return myInstance;
      }

  private:
    mstring_map m_data;
    mstring m_filename;

  };
} // namespace

#endif
