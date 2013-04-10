// -*-tab-width:4-*-

#include <assert.h>
#include <locale>
#include <algorithm>
#include <string.h>
#include "ustring.hpp"

#ifdef WIN32
#include "unittest.hpp"
#include "Logger.h"
#pragma warning (disable : 4996)
#else
#include <stdarg.h>
#define DEBUGLOG
#endif

#ifndef _AFXDLL
#define _T(n) static_cast<const char*>(n)
#endif

using namespace std;

namespace ts {

  //! デフォルトのロケール識別子

  static const char* LOCALE_JP = "Japanese";



  // 文字コードの自動認識
      // bufferの文字コードを判別する
  bool strutil::checkCharCode(StringBuffer& buffer) {
    const uint8_t* u8buf = static_cast<const uint8_t*>(buffer.buffer);
    size_t length = strlen(reinterpret_cast<const char*>(buffer.buffer));
    if (length < 2) return UnknownType;

    if (u8buf[0] == 0xff && u8buf[1] == 0xfe) {buffer.type = Utf16; return true; }
    if (u8buf[0] == 0xfe && u8buf[1] == 0xff) {buffer.type = Utf16BE; return true; }
    if (length >= 3 && u8buf[0] == 0xef && u8buf[1] == 0xbb && u8buf[2] == 0xbf) { buffer.type = Utf8; return true; }


    const char* charBuf = reinterpret_cast<const char*>(buffer.buffer);

    if (strstr(charBuf, "charset=\"utf-8\"") != 0 
      || strstr(charBuf, "charset=utf-8") != 0
      || strstr(charBuf, "encoding=\"UTF-8\"") != 0) {
        buffer.type = Utf8;
        return true; 
    }
    if (strstr(charBuf, "charset=\"Shift_JIS\"") != 0
      || strstr(charBuf, "charset=Shift_JIS") != 0
      || strstr(charBuf, "encoding=\"Shift_JIS\"") != 0) {
        buffer.type = ShiftJis;
        return true; 
    }

    return false;
  }

  // StringBufferからstring/wstringに変換する
  void strutil::toString(const StringBuffer& buffer, std::wstring& body, CharactorType type) {
    if (type != Utf16) throw InvalidCharactorType(); // wstringにUTF-16以外を入れようとしたらエラー
    switch(buffer.type) {
    case Utf16:
      body = reinterpret_cast<const wchar_t*>(buffer.buffer); 
      return;
    case Utf8: // utf8 -> utf16
      utf8towstring(reinterpret_cast<const unsigned char*>(buffer.buffer), strlen(reinterpret_cast<const char*>(buffer.buffer)), body);
      return;
    case ShiftJis: // sjis -> utf16
      char2wstring(reinterpret_cast<const char*>(buffer.buffer), body);
      return;
    default:
      throw InvalidCharactorType(); // wstringにUTF-16以外を入れようとしたらエラー
    }

  }
  void strutil::toString(const StringBuffer& buffer, std::string& body, CharactorType type) {
    switch(buffer.type) {
    case Utf16:
      switch(type) {
      case Utf8: // utf16 -> utf8
        wstring2utf8(wstring(reinterpret_cast<const wchar_t*>(buffer.buffer)), body);
        return;
      case ShiftJis: // utf16 -> sjis
        wstring2string(wstring(reinterpret_cast<const wchar_t*>(buffer.buffer)), body);
        return;
      default:
        throw InvalidCharactorType(); // wstringにUTF-16以外を入れようとしたらエラー
      }
    case Utf8: 
      switch(type) {
      case Utf8: // utf8 -> utf8
        body = reinterpret_cast<const char*>(buffer.buffer);
        return;
        return;
      case ShiftJis: // utf8 -> sjis
        {
          wstring ws;
          utf8towstring(reinterpret_cast<const unsigned char*>(buffer.buffer), strlen(reinterpret_cast<const char*>(buffer.buffer)), ws);
          wstring2string(ws, body);
        }
      default:
        throw InvalidCharactorType(); // wstringにUTF-16以外を入れようとしたらエラー
      }
    case ShiftJis:
      switch(type) {
      case Utf8: // sjis -> utf8
        {
          wstring ws;
          char2wstring(reinterpret_cast<const char*>(buffer.buffer), ws);
          wstring2utf8(ws, body);
          return;
          return;
        }
      case ShiftJis: // sjis -> sjis
        body = reinterpret_cast<const char*>(buffer.buffer);
        return;
      default:
        throw InvalidCharactorType(); // wstringにUTF-16以外を入れようとしたらエラー
      }
    default:
      throw InvalidCharactorType(); //
    }
  }


/*
	@brief			マルチバイト文字列をUNICODEに変換する
	@param[in]	src	ソース文字列（char *）
	@param[in]	dst	出力文字列（wstring）
	@return			変換した文字数
*/
size_t strutil::char2wstring(const char* src, std::wstring& dst)
{
#ifdef WIN32
	dst.clear();
	size_t	len = strlen(src);
	if(len == 0) return 0;
	wchar_t* unic = new wchar_t[len + 1];
	setlocale(LC_ALL, LOCALE_JP);
	size_t	num;
	mbstowcs_s(&num, unic, len + 1, src, len + 1);
	dst = unic;
	delete[] unic;
	return num;
#else
	dst.clear();
	size_t	len = strlen(src);
	if(len == 0) return 0;
	wchar_t* unic = new wchar_t[len + 1];
	setlocale(LC_ALL, LOCALE_JP);

	size_t num = mbstowcs(unic, src, len+1);
	if (num == (size_t)-1) {
	  dst.clear();
	}
	else {
	  dst = unic;
	}
	delete[] unic;
	return num;

#endif
}


/*
	@brief			UNICODE文字列をマルチバイト文字列に変換する
	@param[in]	src	ソース文字列（wstring）
	@param[in]	dst	出力文字列（string）
    @throw          throw CharactorConvertFailed
	@return			変換した文字数
*/
size_t strutil::wstring2string(const std::wstring& src, std::string& dst)
{
	size_t	num;
	dst.clear();
    if(src.length() == 0) {
      dst = "";
      return 0;
    }
	size_t	dstMaxSize = src.length() * 2 + 1;
	char*	tmp = new char[dstMaxSize];
	setlocale(LC_ALL, LOCALE_JP);
#ifdef WIN32
    if (wcstombs_s(&num, tmp, dstMaxSize, src.c_str(), _TRUNCATE) != 0) {
      // 変換失敗の場合は１行ずつ
      ustring s(src);
      vector<ustring> line;
      s.split(line, L'\n');
      for (size_t n = 0; n < line.size(); ++n) {
        if (wcstombs_s(&num, tmp, dstMaxSize, line[n].c_str(), _TRUNCATE) != 0) {
          // error
          dst += "// ERROR! Can't conversion Unicode to ShiftJIS code. \n";
        }
        else {
          dst += tmp;
          dst += '\n';
        }
      }
//      throw CharactorConvertFailed("wcstombs failed.");
    }
    else {
      dst = tmp;
    }
	delete[]	tmp;
	return num;
#else
	num = wcstombs(tmp, src.c_str(), dstMaxSize);
	if (num == (size_t)-1) {
	  dst.clear();
	}
	else {
	  dst = tmp;
	}
	delete [] tmp;
	return num;
#endif
}

size_t strutil::utf8towstring(const unsigned char *inStr,size_t len, std::wstring& dst) {
#ifdef WIN32
  // Utf8 -> Unicodeに必要な文字数を数える
  int wLen = MultiByteToWideChar(CP_UTF8,0,(const char *)inStr,(int)len,NULL,0);
  if(!wLen) {
    dst = L"";
    return 0;
  }
  wchar_t *wBuf = new wchar_t[wLen+1];
	
  // Utf8 -> Unicodeに変換
  MultiByteToWideChar(CP_UTF8,0,(const char *)inStr, (int)len,wBuf,wLen);
  wBuf[wLen] = L'\0';

  dst = wBuf;
  delete [] wBuf;
  return wLen;
#else
  assert(0);
  return 0;
#endif
}
/*
  int WideCharToMultiByte(
    UINT CodePage,            // code page
    DWORD dwFlags,            // performance and mapping flags
    LPCWSTR lpWideCharStr,    // wide-character string
    int cchWideChar,          // number of chars in string.
    LPSTR lpMultiByteStr,     // buffer for new string
    int cbMultiByte,          // size of buffer
    LPCSTR lpDefaultChar,     // default for unmappable chars
    LPBOOL lpUsedDefaultChar  // set when default char used
  );
*/
size_t strutil::wstring2utf8(const std::wstring& inStr, std::string& dst) {
#ifdef WIN32
  // Unicode -> Utf8 に必要な文字数を数える
  int utf8Len = WideCharToMultiByte(CP_UTF8,0, inStr.c_str(), (int)inStr.length(), NULL, 0, NULL ,NULL );
  if(!utf8Len) {
    dst = "";
    return 0;
  }
  char *utf8Buf = new char[utf8Len+1];
	
  // Utf8 -> Unicodeに変換
  WideCharToMultiByte(CP_UTF8, 0, inStr.c_str(), (int)inStr.length(), utf8Buf, utf8Len, NULL, NULL);
  utf8Buf[utf8Len] = '\0';

  dst = utf8Buf;
  delete [] utf8Buf;
  return utf8Len;
#else
  assert(0);
  return 0;
#endif
}



  template <typename StrT, typename charT>
  void strutil::formatstr(StrT& str, const charT* fmt, va_list& args) {
	
#ifdef WIN32
	size_t len = _vscprintf(fmt, args) + 1; // _vscprintf doesn't count terminating '\0'
	StrT::value_type* buffer = new StrT::value_type[len * sizeof(StrT::value_type)];
	vsprintf_s( buffer, len, fmt, args );
#else 
	size_t len = 65536;
	charT* buffer = new charT[len * sizeof(charT)];
	vsnprintf(buffer, len, fmt, args );
	va_end(args);
#endif
	str = buffer;
	delete [] buffer;
  }

template <>
void strutil::formatstr(std::wstring& str, const wchar_t* fmt, va_list& args) {
  typedef wchar_t charT;
#ifdef WIN32
  size_t len = _vscwprintf(fmt, args) + 1; // _vscprintf doesn't count terminating '\0'
  wchar_t* buffer = new wchar_t[len * sizeof(wchar_t)];
  vswprintf_s( buffer, len, fmt, args );
#else 
  size_t len = 65536;
  wchar_t* buffer = new wchar_t[len * sizeof(wchar_t)];
  vswprintf(buffer, len, fmt, args );
  va_end(args);
#endif
  str = buffer;
  delete [] buffer;
}



} // namespace ts;


// テンプレートが生成されない問題を回避するための定義
namespace {
  struct hoge {
    ts::ustring us;
    ts::mstring ms;
    hoge() {
      us = L"hoge";
      ms = "hoge";
      us.tolower();
      ms.tolower();
      us.toupper();
      ms.toupper();
      us.format(L"");
      ms.format("");

#ifdef UNITTEST
      us.unittest(0);
      ms.unittest(0);
#endif

    }
  } hogehoge;
}

