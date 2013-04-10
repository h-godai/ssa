/* unified string class -*-tab-width:4-*-

*/

/*!
    @file
    @brief    libTS 統合文字列操作クラス
    @author   h.godai
*/

#ifndef TSL_USTRING_HPP
#define TSL_USTRING_HPP
#include <stdio.h>
#include <string.h>
#include <string>
#include <map>
#include <vector>
#include <iterator>
#include <stdarg.h>


#if defined(WIN32) 
#  if defined(_AFXDLL)
#    define USE_MFC
#    ifndef WINVER
#      define WINVER 0x0502
#    endif
#      define VC_EXTRALEAN        // Exclude rarely-used stuff from Windows headers
#      include <afxwin.h>
#      include <afxext.h>         // MFC extensions
#      define snprintf _snprintf
#  else
#    include <windows.h>
#    define snprintf _snprintf
#  endif
#pragma warning (disable : 4996)
#endif

#include "../Common/Common.h"
#include "mpl.hpp"
#include "unittest.hpp"
#include "strutil.hpp"

// マルチバイトのデフォルト文字コードの設定
#ifndef TS_MBCHARCODE
# ifdef TS_PLATFORM_WIN32
#  define TS_MBCHARCODE strutil::ShiftJis
# else
#  define TS_MBCHARCODE strutil::Utf8
# endif
#endif


namespace ts {
  
  /*!
	@brief 型によって適切なstd::stringクラスを提供するメタテンプレート
	@note  charTの型により、typeがstd::stringもしくはstd::wstringに定義されます。
  */
  template <strutil::CharactorType CharSet> struct StringType { 
	typedef std::string type; 
	typedef std::wstring negtype;
	typedef char charT;
	static const strutil::CharactorType DefaultCharaSet = CharSet;
	static type ToString(const negtype& str) {
	  type dst;
	  strutil::wstring2string(str, dst);
	  return dst;
	}
	static const char* CharName() { return CharactorTypeName<CharSet>::Name(); }
  };
  template <> struct StringType<strutil::Utf16>      {
	typedef std::wstring type; 
	typedef std::string negtype;
	typedef wchar_t charT;
	static const strutil::CharactorType DefaultCharaSet = strutil::Utf16;
	static type ToString(const negtype& str) {
	  type wstr;
	  strutil::char2wstring(str.c_str(), wstr);
	  return wstr;
	}
	static const char* CharName() { return CharactorTypeName<strutil::Utf16>::Name(); }
  };

  typedef StringType<strutil::ShiftJis> StringTypeSjis;
  typedef StringType<strutil::Utf8> StringTypeUtf8;
  typedef StringType<strutil::Utf16> StringTypeUtf16;
  typedef StringType<TS_MBCHARCODE> StringTypeChar; // デフォルトのマルチバイト文字列

  
  /*!
	@brief  STLのstring互換の文字列操作クラスです
	@note   STLのstring,wstringとほぼ同等の機能に加えて、文字コードの
	　　変換機能(マルチバイト-UNICODE)、printf互換のformat機能など
	　を追加した文字列クラスです。
	charTは、charもしくはwcharTのみ指定可で、通常はtypedef
	された、ustring(UNICODE string), mstring(Multibyte string)
	を私用します。
	例:<br>
	char* mbstr = mstring(L"UNICODE STRING").c_str(); // wchar_t -> char<br>
	wchar_t* mbstr = ustring("Multibyte STRING").c_str(); // char -> wchar_t<br>
	
  */
  template <typename StringT>
  class UnifiedString {
  public:
	typedef typename StringT::charT charT;
	
	//! @brief std::string::npos互換の変数
	static const size_t npos = static_cast<size_t>(-1);
	
	//! @brief ベースとなるstlのstringクラスの定義
	typedef typename StringT::type BaseString;
	//! @brief ベースとなるstlのstringクラスと逆の定義
	typedef typename StringT::negtype NegString;
	//! @brief iteratorの定義
	typedef typename StringT::type::iterator iterator;
	//! @brief const_iteratorの定義
	typedef typename StringT::type::const_iterator const_iterator;
	//! @brief default CharactorTypeの定義
	static const strutil::CharactorType DefCharType = StringT::DefaultCharaSet;

	//! コンストラクタ
	UnifiedString()
	  : m_charCode(DefCharType)
	{}
	//! コピーコンストラクタ
	UnifiedString(const UnifiedString& ustr) 
	  : m_body(ustr.m_body)
	  , m_charCode(DefCharType)
	{}
	//! 部分コピーコンストラクタ
	UnifiedString(const UnifiedString& ustr, size_t pos, size_t n = npos) 
	  : m_body(ustr.m_body, pos, n)
	  , m_charCode(DefCharType)
	{}
	//! wstringからのコンストラクタ
	UnifiedString(const NegString&  str) : m_body(StringT::ToString(str)), m_charCode(DefCharType) {}
	//! stringからのコンストラクタ
	UnifiedString(const BaseString& str) : m_body(str), m_charCode(DefCharType) {}
	//! char*からのコンストラクタ
	UnifiedString(const char* str);
	//! char*からn文字のコンストラクタ
	UnifiedString(const char* str, size_t n);
	//! wchar_t*からのコンストラクタ
	UnifiedString(const wchar_t* wstr);
	//! char*からn文字のコンストラクタ
	UnifiedString(const wchar_t* wstr, size_t n);
	//! StringBufferからのコンストラクタ
	UnifiedString(const strutil::StringBuffer& buffer, strutil::CharactorType type = DefCharType)
	  : m_charCode(type) {
	  strutil::toString(buffer, m_body, type); 
	}
	
	//! 整数値からのコンストラクタ
	explicit UnifiedString(int num) {
	  char buf[64];
	  //sprintf_s(buf, 64, "%d", num);
	  snprintf(buf, 64, "%d", num);
	  UnifiedString b(buf);
	  swap(b);
	}
	//! 浮動小数点値からのコンストラクタ
	explicit UnifiedString(double num, const char* fmt = "%lf") {
	  char buf[64];
	  //sprintf_s(buf, 64, fmt, num);
	  snprintf(buf, 64, fmt, num);
	  UnifiedString tmp(buf);
	  swap(tmp);
	}
	//! booleanからのコンストラクタ
	explicit UnifiedString(bool value) {
	  UnifiedString tmp(value ? "true" : "false");
	  swap(tmp);
	}
	
#ifdef USE_MFC
	//! MCF CStringからのコンストラクタ
	UnifiedString(const CString& cstr) : m_body(static_cast<LPCTSTR>(cstr)), m_charCode(DefCharType) {}
#endif
	//~UnifiedString() {};
	
	void swap(UnifiedString& src) {
	  std::swap(m_body, src.m_body);
	  std::swap(m_strtmp, src.m_strtmp);
	  std::swap(m_charCode, src.m_charCode);
	}
	
	/// 型変換用オペレータ
	operator const std::wstring& () const { return m_body; }
	operator const std::string& () const { strutil::wstring2string(m_body, m_strtmp); return m_strtmp; } 
	
	/* =====================================================================*/
	/* Unified String 固有のstaticメソッド                                  */
	/* =====================================================================*/

		
	//! @brief     std::stringをstd::wstringに変換する
	//! @param[in] 変換前のstd::stringオブジェクト
	//! @return    変換後のstd::wstringオブジェクト
	static std::wstring toWstring(const std::string& str) {
	  return toWstring(str.c_str());
	}
	
	//! @brief     const char*をstd::wstringに変換する
	//! @param[in] 変換前のconst char*文字列
	//! @return    変換後のstd::wstringオブジェクト
	static std::wstring toWstring(const char* str) {
	  std::wstring wstr;
	  strutil::char2wstring(str, wstr);
	  return wstr;
	}
	
	//! @brief     std::wstringをstd::stringに変換する
	//! @param[in] 変換前のstd::wstringオブジェクト
	//! @return    変換後のstd::stringオブジェクト
	static std::string toString(const std::wstring& wstr) {
	  std::string dst;
	  strutil::wstring2string(wstr, dst);
	  return dst;
	}
	/* =====================================================================*/
	/* Unified String 固有のメソッド                                        */
	/* =====================================================================*/
	
#ifdef USE_MFC
	//! MFC指定のデフォルト文字コードを返す
#ifdef UNICODE
	LPCTSTR getLPCTSTR() const { return toWChar(); }
#else
	LPCTSTR getLPCTSTR() const { return toChar(); }
#endif
#endif
	
	//! マルチバイトの文字列のポインタを返す
	const char* toChar() const;
	
	//! マルチバイトの文字列のポインタを返す
	const wchar_t* toWChar() const;
    
	//! 整数値に変換
	int toInt() const { return atoi(toChar()); }
	
	//! 浮動小数点値に変換
	double toDouble() const { return atof(toChar()); }
	float toFloat() const { return static_cast<float>(atof(toChar())); }
	
	//! 戻り値の型によって推論するget
	struct AnyType {
	  const UnifiedString& self_;
	  AnyType(const UnifiedString& self) : self_(self) {}
	  operator const char*() const { return self_.toChar(); }
	  operator const wchar_t*() const { return self_.toWChar(); }
	  operator int () const { return self_.toInt(); }
	  operator unsigned int () const { return static_cast<unsigned int>(self_.toInt()); }
	  operator float () const { return self_.toFloat(); }
	  operator double () const { return self_.toDouble(); }
	  operator bool () const { return self_ == UnifiedString("true"); }
#ifndef __APPLE__
//	  operator int32_t () const { return self_.toInt(); }
//	  operator uint32_t () const { return static_cast<unsigned int>(self_.toInt()); }
#endif
	};
	
	//! get()の戻り値はUniTypeになるので、定義されているオペレーターにより暗黙の型変換がおこなわれる
	//! ex.
	//! int i = str.get();   // これで、AnyType::operator intが呼ばれる。
	//! float f = str.get(); // これで、AnyType::operator floatが呼ばれる。
	AnyType get() const {
	  return AnyType(*this);
	}
	
    
	//! @brief すべて小文字に変換する
	//! @param[in] locaieID 変換用のロケール識別子(初期値は"japanese")
	//! @return    自分への参照
	UnifiedString& tolower(const char* localeID = "japanese") {
	  m_body = strutil::to_lower(m_body, localeID);
	  return *this;
	}

	//! @brief すべて大文字に変換する
	//! @param[in] locaieID 変換用のロケール識別子(初期値は"japanese")
	//! @return    自分への参照
	UnifiedString& toupper(const char* localeID = "japanese") {
	  m_body = strutil::to_upper(m_body, localeID);
	  return *this;
	}
	
	//! ベースとなるオブジェクト(std::string もしくは std::wstring)を返す
	BaseString& body() { return m_body; }
	
	//! ベースとなるオブジェクト(std::string もしくは std::wstring)を返す
	const BaseString& body() const { return m_body; }
	
	//! @brief デミリタで文字列を分割する
	//! @note boost::algorithm::split もどき
	//! @param[out] container  分割後の文字列のリスト
	//! @param[in]  separator  分割用文字
	//! @return     分割された文字列の数
	template <typename Container> 
	int split(Container& container, charT separator) const {
	  std::back_insert_iterator<Container> ite(container);
	  int count = 0;
	  for (size_t pos = 0;;) {
		size_t newpos = m_body.find(separator, pos);
		if (newpos == UnifiedString::npos) { // ,ない
		  if (pos >= m_body.length()) return count;
		  ite = m_body.substr(pos); // これで終わり
		  return ++count;
		}
		else { // ,あり
		  ite = m_body.substr(pos, newpos-pos);
		  ++count;
		}
		pos = newpos+1;
	  }
	}
	
	//! @brief デミリタ(複数)で文字列を分割する
	//! @note boost::algorithm::split もどき
	//! @param[out] container  分割後の文字列のリスト
	//! @param[in]  separator  分割用文字列
	//! @return     分割された文字列の数
	template <typename Container> 
	int split(Container& container, const charT* separator) const {
	  std::back_insert_iterator<Container> ite(container);
	  int count = 0;
	  for (size_t pos = 0;;) {
		size_t newpos = m_body.find_first_of(separator, pos);
		if (newpos == UnifiedString::npos) { // ,ない
		  if (pos >= m_body.length()) return count;
		  ite = m_body.substr(pos); // これで終わり
		  return ++count;
		}
		else { // あり
		  if (newpos-pos > 0) {
			ite = m_body.substr(pos, newpos-pos);
			++count;
		  }
		}
		pos = newpos+1;
	  }
	}
	
	//! @brief trim
	//! @note 文字列の前後からstrで指定した文字を取り除く
	
	UnifiedString& trim(const charT* str) {
	  size_t spos = m_body.find_first_not_of(str);
	  size_t epos = m_body.find_last_not_of(str);
	  if (epos == UnifiedString::npos || spos == UnifiedString::npos) {
		m_body.clear();
		return *this;
	  }
	  if (epos < spos) {
		epos = UnifiedString::npos;
	  }
	  else {
		epos = epos + 1 - spos;
	  }
	  if (spos == epos)
	    m_body.clear();
	  else
	    m_body = m_body.substr(spos, epos);
	  if (!m_body.empty() && m_body[0] == 0) m_body.clear(); // \0だけ?
	  return *this;
	}
	
	//! @brief 文字列置換
	//! @param[in] from 置換前の文字列
	//! @param[in] to   置換後の文字列
	//! @return   thisへの参照
	UnifiedString& replace_string(const UnifiedString& from, const UnifiedString& to) {
	  ts::strutil::replace_string(m_body, from.m_body, to.m_body);
	  return *this;
	}
	
	//! printf互換の文字列整形
	UnifiedString& format(const charT* fmt, ...) {
	  va_list args;
	  va_start(args, fmt);
	  strutil::formatstr(m_body, fmt, args);
	  return *this;
	}
	
	//! 左端から一致すればtrue
	bool leftMatch(const UnifiedString& match) const {
	  return substr(0, match.length()) == match;
	}
	//! 右端から一致すればtrue
	bool rightMatch(const UnifiedString& match) const {
	  if (length() < match.length()) return false;
	  return substr(length() - match.length()) == match;
	}
	
	//! デミリタで区切られたn番目の単語を返す
	UnifiedString word(size_t index, const charT* demi) const {
	  std::vector<UnifiedString> lst;
	  this->split(lst, demi);
	  if (lst.size() <= index) return UnifiedString();
	  return lst[index];
	}
	
	
	//! 単体試験用メソッド
	POINTCUT_UNITTEST_METHOD;
	
	/* =====================================================================*/
	/* stl::string 互換メソッド                                             */
	/* =====================================================================*/
	
	void clear() { m_body.clear(); } //!< STL互換メソッド
	const charT* c_str() const { return m_body.c_str(); }  //!< STL互換メソッド
	const charT* data() const { return m_body.data(); }  //!< STL互換メソッド
	
	//! STL互換メソッド
	//! strとの比較を行う。Cの標準ライブラリ関数strcmp()と同じようなもの。
	int compare(const UnifiedString& str) const {
	  return m_body.compare(str.m_body);
	}
	//! STL互換メソッド
	//! pos1文字目からn1文字をstrと比較。
	int compare(size_t pos1, size_t n1, const UnifiedString& str) const {
	  return m_body.compare(pos1, n1, str.m_body);
	}
	
	//! STL互換メソッド
	//! pos1文字目からn1文字をstrのpos2文字目からn2文字と比較。
	int compare(size_t pos1, size_t n1, const UnifiedString& str, size_t pos2, size_t n2) const {
	  return m_body.compare(pos1, n1, str.m_body, pos2, n2);
	}
	
	
	//! STL互換メソッド
	UnifiedString& operator = (const UnifiedString& dst) { 
	  m_body = dst.m_body;
	  return *this;
	}
	
	//! STL互換メソッド
	UnifiedString& operator += (const UnifiedString& dst) {
	  m_body += dst.m_body;
	  return *this;
	}
	
	//! STL互換メソッド
	UnifiedString& operator += (charT dst) {
	  m_body += dst;
	  return *this;
	}
	
	//! STL互換メソッド
	UnifiedString operator + (const UnifiedString& dst) {
	  return UnifiedString(m_body + dst.m_body);
	}
	
	// 検索関係
	
	//! STL互換メソッド
	size_t find(const UnifiedString& dst, size_t pos = 0) const {
	  return m_body.find(dst.m_body, pos);
	};
	//! STL互換メソッド
	size_t rfind(const UnifiedString& dst, size_t pos = npos) const {
	  return m_body.rfind(dst.m_body, pos);
	};
	//! STL互換メソッド
	size_t find(charT c, size_t pos = 0) const {
	  return m_body.find(c, pos);
	}
	//! STL互換メソッド
	size_t rfind(charT c, size_t pos = npos) const {
	  return m_body.rfind(c, pos);
	}
	//! STL互換メソッド
	size_t find_first_of(const UnifiedString& str, size_t pos = 0) const {
	  return m_body.find_first_of(str.m_body, pos);
	}
	//! STL互換メソッド
	size_t find_last_of(const UnifiedString& str, size_t pos = npos) const {
	  return m_body.find_last_of(str.m_body, pos);
	}
	//! STL互換メソッド
	size_t find_first_of(charT c, size_t pos = 0) const {
	  return m_body.find_first_of(c, pos);
	}
	//! STL互換メソッド
	size_t find_last_of(charT c, size_t pos = npos) const {
	  return m_body.find_last_of(c, pos);
	}
	//! STL互換メソッド
	size_t find_first_not_of(const UnifiedString& str, size_t pos = 0) const {
	  return m_body.find_first_not_of(str, pos);
	}
	//! STL互換メソッド
	size_t find_last_not_of(const UnifiedString& str, size_t pos = npos) const {
	  return m_body.find_last_not_of(str, pos);
	}
	//! STL互換メソッド
	size_t find_first_not_of(charT c, size_t pos = 0) const {
	  return m_body.find_first_not_of(c, pos);
	}
	//! STL互換メソッド
	size_t find_last_not_of(charT c, size_t pos = npos) const {
	  return m_body.find_last_not_of(c, pos);
	}
    
	/// replace関係
	
	//! STL互換メソッド
	//! pos1文字目からn1文字をstrのpos2からnpos文字で置き換える
	UnifiedString& replace(size_t pos1, size_t n1, const UnifiedString& str, size_t pos2 = 0, size_t n2 = npos) {
	  m_body.replace(pos1, n1, str.m_body, pos2, npos);
	  return *this;
	}
	//! STL互換メソッド
	//! pos文字目からn1文字をn2文字の文字cで置き換える
	UnifiedString& replace(size_t pos, size_t n1, size_t n2, charT c) {
	  m_body.replace(pos, n1, n2, c);
	  return *this;
	}
	//! STL互換メソッド
	//! i1〜i2をstrで置き換える
	UnifiedString& replace(iterator i1, iterator i2, const UnifiedString& str) {
	  m_body.replace(i1, i2, str.m_body);
	  return *this;
	}
	//! STL互換メソッド
	//! i1〜i2を文字列定数sでn文字置き換える
	//! i1〜i2をn文字のcで置き換える
	UnifiedString& replace(iterator i1, iterator i2, size_t n, charT c) {
	  m_body.replace(i1, i2, n, c);
	  return *this;
	}
	//! STL互換メソッド
	//! i1〜i2をj1〜j2で置き換える
	template<typename InputIterator>
	UnifiedString& replace(iterator i1, iterator i2, InputIterator j1, InputIterator j2) {
	  m_body.replace(i1, i2, j1, j2);
	  return *this;
	}
	
	//! STL互換メソッド
	//! 現文字列にstrをposからn文字追加
	UnifiedString& append(const UnifiedString& str, size_t pos = 0, size_t n = npos) {
	  m_body.append(str.m_body, pos, n);
	  return *this;
	}
	//! STL互換メソッド
	//! 文字cをn文字追加
	UnifiedString& append(size_t n, charT c) {
	  m_body.append(n, c);
	  return *this;
	}
	//! STL互換メソッド
	//! InputIteratorから追加
	template<class InputIterator> UnifiedString& append(InputIterator first, InputIterator last) {
	  m_body.append(first, last);
	  return *this;
	}
	
    
	const_iterator begin() const { return m_body.begin(); } //!> STL互換メソッド
	const_iterator end() const { return m_body.end(); } //!> STL互換メソッド
	iterator begin()  { return m_body.begin(); } //!> STL互換メソッド
	iterator end()  { return m_body.end(); } //!> STL互換メソッド
	
	//! STL互換メソッド
	const charT& operator [] (size_t idx) const { return m_body[idx]; }
	//! STL互換メソッド
	charT& operator [] (size_t idx) { return m_body[idx]; }
	
	//! STL互換メソッド
	UnifiedString substr(size_t pos=0, size_t n = std::wstring::npos ) const {
	  return UnifiedString(m_body.substr(pos,n));
	}
	
	//! STL互換メソッド
	size_t length() const { return m_body.length(); }
	
	//! STL互換メソッド
	bool empty() const { return m_body.empty(); }
	
	
	BaseString m_body;              //<! ベースとなるstringオブジェクト
	mutable NegString m_strtmp; // toChar/toWString用の一時オブジェクト
	strutil::CharactorType m_charCode; // 文字コード
	
  };
  
  // std互換のgetline
  template <typename StringT>
  inline std::istream& getline(std::istream& ins, UnifiedString<StringT>& str) {
	return std::getline(ins, str.body());
  }
  template <>
  inline std::istream& getline(std::istream& ins, UnifiedString<StringTypeUtf16>& str) {
	std::string mstr;
	std::getline(ins, mstr);
	str = mstr;
	return ins;
  }



  // charT別の実装

  // charT=wchar_tの場合
  //template <> inline UnifiedString<StringTypeUtf16>::UnifiedString(const std::wstring& wstr) : m_body(wstr), m_charCode(DefCharType) {}
  //template <> inline UnifiedString<StringTypeUtf16>::UnifiedString(const std::string& str) : m_body(toWstring(str)), m_charCode(DefCharType) {}
  template <> inline UnifiedString<StringTypeUtf16>::UnifiedString(const char* str) : m_body(toWstring(str)), m_charCode(DefCharType) {}
  template <> inline UnifiedString<StringTypeUtf16>::UnifiedString(const char* str, size_t n) : m_body(toWstring(str), 0, n), m_charCode(DefCharType) {}
  template <> inline UnifiedString<StringTypeUtf16>::UnifiedString(const wchar_t* wstr) : m_body(wstr), m_charCode(DefCharType) {}
  template <> inline UnifiedString<StringTypeUtf16>::UnifiedString(const wchar_t* wstr, size_t n) : m_body(wstr, n), m_charCode(DefCharType) {}
  template <> inline const char* UnifiedString<StringTypeUtf16>::toChar() const {    strutil::wstring2string(m_body, m_strtmp);    return m_strtmp.c_str(); }
  template <> inline const wchar_t* UnifiedString<StringTypeUtf16>::toWChar() const {    return m_body.c_str(); }


  // charT=charの場合
  //template <> inline UnifiedString<StringTypeChar>::UnifiedString(const std::wstring& wstr) : m_body(toString(wstr)), m_charCode(DefCharType) {}
  //template <> inline UnifiedString<StringTypeChar>::UnifiedString(const std::string& str) : m_body(str), m_charCode(DefCharType) {}
  template <> inline UnifiedString<StringTypeChar>::UnifiedString(const char* str) : m_body(str), m_charCode(DefCharType) {}
  template <> inline UnifiedString<StringTypeChar>::UnifiedString(const char* str, size_t n) : m_body(str, n), m_charCode(DefCharType) {}
  template <> inline UnifiedString<StringTypeChar>::UnifiedString(const wchar_t* wstr) : m_body(toString(wstr)), m_charCode(DefCharType) {}
  template <> inline UnifiedString<StringTypeChar>::UnifiedString(const wchar_t* wstr, size_t n) : m_body(toString(wstr), 0, n), m_charCode(DefCharType) {}
  template <> inline const char* UnifiedString<StringTypeChar>::toChar() const { return m_body.c_str(); }
  template <> inline const wchar_t* UnifiedString<StringTypeChar>::toWChar() const { strutil::char2wstring(m_body.c_str(), m_strtmp); return m_strtmp.c_str(); }
  template <> inline UnifiedString<StringTypeChar>::operator const std::wstring& () const  { strutil::char2wstring(m_body.c_str(), m_strtmp); return m_strtmp; }
  template <> inline UnifiedString<StringTypeChar>::operator const std::string& () const { return m_body; }



// なぜかこれだとリンクエラーになる
//    template <typename charT> bool operator == (const UnifiedString<charT>& a, const UnifiedString<charT>& b) { return a.m_body == b.m_body;    }
//    template <typename charT> bool operator != (const UnifiedString<charT>& a, const UnifiedString<charT>& b) { return a.m_body != b.m_body;    }
//    template <typename charT> bool operator < (const UnifiedString<charT>& a, const UnifiedString<charT>& b) { return a.m_body < b.m_body;    }
//    template <typename charT> bool operator > (const UnifiedString<charT>& a, const UnifiedString<charT>& b) { return a.m_body > b.m_body;    }

  inline bool operator == (const UnifiedString<StringTypeUtf16>& a, const UnifiedString<StringTypeUtf16>& b) { return a.m_body == b.m_body;    }
  inline bool operator != (const UnifiedString<StringTypeUtf16>& a, const UnifiedString<StringTypeUtf16>& b) { return a.m_body != b.m_body;    }
  inline bool operator < (const UnifiedString<StringTypeUtf16>& a, const UnifiedString<StringTypeUtf16>& b) { return a.m_body < b.m_body;    }
  inline bool operator > (const UnifiedString<StringTypeUtf16>& a, const UnifiedString<StringTypeUtf16>& b) { return a.m_body > b.m_body;    }
  inline UnifiedString<StringTypeUtf16> operator + (const UnifiedString<StringTypeUtf16>& a, const UnifiedString<StringTypeUtf16>& b) { return a.m_body + b.m_body;    }
  inline bool operator == (const UnifiedString<StringTypeChar>& a, const UnifiedString<StringTypeChar>& b) { return a.m_body == b.m_body;    }
  inline bool operator != (const UnifiedString<StringTypeChar>& a, const UnifiedString<StringTypeChar>& b) { return a.m_body != b.m_body;    }
  inline bool operator < (const UnifiedString<StringTypeChar>& a, const UnifiedString<StringTypeChar>& b) { return a.m_body < b.m_body;    }
  inline bool operator > (const UnifiedString<StringTypeChar>& a, const UnifiedString<StringTypeChar>& b) { return a.m_body > b.m_body;    }
  inline UnifiedString<StringTypeChar> operator + (const UnifiedString<StringTypeChar>& a, const UnifiedString<StringTypeChar>& b) { return a.m_body + b.m_body;    }




  // iostreamへのオペレーター
  template <typename CharT>
  inline std::ostream& operator << (std::ostream& os, const UnifiedString<CharT>& str) {
	return os << str.toChar();
  }
  
    // 型の再定義
  typedef UnifiedString<StringTypeUtf16> ustring;   //!< UNICODE string class
  typedef UnifiedString<StringTypeUtf8>  u8string;   //!< Multibyte string class
  typedef UnifiedString<StringTypeSjis>  sjstring;   //!< Multibyte string class
  typedef UnifiedString<StringTypeChar>  mstring;   //!< Multibyte string class
  



} // namespace ts

#endif

