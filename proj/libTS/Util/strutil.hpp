/* string utility class -*-tab-width:4-*-

*/

/*!    @file
    @brief    libTS 文字列ユーティリティクラス
    @author   ikeda
*/
#pragma once

#include <string>
#include <iterator>
#include <locale>
#include <algorithm>
#include <stdexcept>

#include "mpl.hpp"
#include "unittest.hpp"


namespace ts { 

  const char DefaultLocaleID[] = "japanese"; // デフォルトのローケールID

  // 文字列操作ユーティリティ
  struct strutil {
	
	
    /*!
	  @brief 文字列置換テンプレート
	  @note  string系の文字列オブジェクトのfromをtoに置換します。
	  @param[in,out] body  置換の対象となる文字列
	  @param[in]     from  変換元の文字列
	  @param[in]     to    変換後の文字列
	  @return        変換後のbody
    */
    template <typename T>
    static T& replace_string(T& body, const T& from, const T& to) {
	  if (from.empty()) return body;
        for (size_t pos = 0; (pos = body.find(from, pos)) != T::npos; ) {
            body.replace(pos, from.length(), to);
            pos += to.length();
        }
        return body;
    }

	
    template <typename charT>
    struct to_lower_char {
      std::locale loc1;
  	  to_lower_char(const char* localeID) {
		    try {
          loc1 = std::locale(localeID);
		    }
		    catch (const std::runtime_error&) {
			    loc1 = std::locale();
		    }
	    }
  	  template <typename T>
	    charT operator () (T a) {
		    return std::tolower(a, loc1);
      }
    };
    template <typename charT>
    struct to_upper_char {
      std::locale loc1;
  	  to_upper_char(const char* localeID) {
		try {
		  loc1 = std::locale(localeID);
		}
        catch (const std::runtime_error&) {
		  loc1 = std::locale();
		}
	  }
  	  template <typename T>
	  charT operator () (T a) {
		return std::toupper(a, loc1);
      }
    };

	

    // 小文字に変換
    template <typename StrT>
    static StrT to_lower(const StrT& src, const char* localeID = DefaultLocaleID) {
      StrT output;
      to_lower_char<typename StrT::value_type> lower(localeID);
      std::transform(src.begin(), src.end(), std::back_inserter(output), lower);
      return output;
    }
    // 大文字に変換
    template <typename StrT>
    static StrT to_upper(const StrT& src, const char* localeID = DefaultLocaleID) {
      StrT output;
      std::transform(src.begin(), src.end(), std::back_inserter(output)
					 , to_upper_char<typename StrT::value_type>(localeID));
      return output;
    }
	
    // 右端に適合すればStrT
    template <typename StrT>
    static bool right_match(const StrT& name, const StrT& match) {
      return name.substr(name.length() - match.length()) == match;
    }
    // 右端に適合すればStrT
    template <typename StrT>
    static bool right_match(const StrT& name, const typename StrT::value_type* match) {
      return name.substr(name.length() - StrT(match).length()) == match;
    }


	// フォーマットストリング
	template <typename StrT, typename CharT>
	static StrT format(const CharT * fmt, ...) {
	  va_list args;
	  va_start(args, fmt);
	  StrT str;
	  format(str, fmt, args);
	  return str;
	}

	template <typename StrT, typename CharT>
	static void formatstr(StrT& str, const CharT* fmt, va_list& args);
	

	

	

    /*!
	  @brief UNICODE <-> マルチバイトの変換ユーティリティ
    */
	
	/*! 文字コードの定義 */
	enum CharactorType {
	  UnknownType,
	  ShiftJis,
	  Utf8,    // UTF-8
	  Utf16,   // UTF-16 Little Endian
	  Utf16BE, // UTF-16 Big Endian
	};



	
	struct CharactorConvertFailed : public std::runtime_error {
	  CharactorConvertFailed(const char * const msg = "CharactorConvertFailed") : std::runtime_error(msg) {
	  }
	};
	struct InvalidCharactorType : public std::runtime_error {
	  InvalidCharactorType(const char * const msg = "InvalidCharactorType") : std::runtime_error(msg) {
	  }
	};
	
	// 文字列バッファの定義
	struct StringBuffer {
	  const uint8_t* buffer;
	  CharactorType type;
	  StringBuffer(const uint8_t* buf, CharactorType tp = UnknownType) : buffer(buf), type(tp) {}
	};
	
	// bufferの文字コードを判別する
	static bool checkCharCode(StringBuffer& buffer);
	
	// StringBufferからstring/wstringに変換する
	static void toString(const StringBuffer& buffer, std::wstring& body, CharactorType type = Utf16);
	static void toString(const StringBuffer& buffer, std::string& body, CharactorType type = ShiftJis);
	

	/*!
	  @brief            マルチバイト文字列をUNICODEに変換する
	  @param[in]    src    ソース文字列（char *）
	  @param[in]    dst    出力文字列（wstring）
	  @return            変換した文字数
	*/
	
	static size_t char2wstring(const char* src, std::wstring& dst);
	/*!
	  @brief            UNICODE文字列をマルチバイト文字列に変換する
	  @param[in]    src    ソース文字列（wstring）
	  @param[out]    dst    出力文字列（string）
	  @return            変換した文字数
	*/
	static size_t wstring2string(const std::wstring& src, std::string& dst);
	/*!
	  @brief            UTF-8文字列をUNICODE文字列に変換する
	  @param[in]    inStr    ソース文字列（char*）
	  @param[in]    len    文字列のサイズ
	  @param[out]    dst    出力文字列（string）
	  @return            変換した文字数
	*/
	static size_t utf8towstring(const unsigned char *inStr, size_t len, std::wstring& dst);
	/*!
	  @brief        UTF-8文字列をUNICODE文字列に変換する
	  @param[in]    inStr    ソース文字列（string）
	  @param[out]    dst    出力文字列（string）
	  @return            変換した文字数
	*/
	static size_t utf8towstring(const std::string& inStr, std::wstring& dst) {
	  return utf8towstring(reinterpret_cast<const unsigned char*>(inStr.c_str()), inStr.length(), dst);
	}
	/*!
	  @brief            UNICODE文字列をUTF-8文字列に変換する
	  @param[in]    src    ソース文字列（wstring）
	  @param[out]    dst    出力文字列（string）
	  @return            変換した文字数
	*/
	static size_t wstring2utf8(const std::wstring& src, std::string& dst);
	
  }; // strutil
  
  template <strutil::CharactorType CharT> struct CharactorTypeName {};
  template <>	struct CharactorTypeName<strutil::ShiftJis> { static const char* Name() { return "S-JIS"; }};
  template <>	struct CharactorTypeName<strutil::Utf8> { static const char* Name() { return "UTF-8"; }};
  template <>	struct CharactorTypeName<strutil::Utf16> { static const char* Name() { return "UTF-16"; }};
  template <>	struct CharactorTypeName<strutil::Utf16BE> { static const char* Name() { return "UTF-16BE"; }};
  
} // namespace
