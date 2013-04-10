// -*-tab-width:4-*-
#ifndef TS_UTIL_MLSTRING_HPP
#define TS_UTIL_MLSTRING_HPP
#include "ustring.hpp"

namespace ts {

    //! マルチライン文字列クラス!
    template <typename stringT>
    struct MultiLineString {
        /// デフォルトコンストラクタ!
        MultiLineString() {}

        /// stringオブジェクトからのコンストラクタ!
        MultiLineString(const stringT& src) {
            set(src);
        }

        /// 文字列をラインに分割してセットする!
        void set(const stringT& src) {
            src.split(line_, '\n');
        }

        size_t count() const { return line_.size(); }

        /// 空の場合trueを返します!
        bool empty() const { return line_.empty(); }

        /// iteratorの再定義!
        typedef typename std::vector<stringT>::iterator iterator;

        /// const_iteratorの再定義!
        typedef typename std::vector<stringT>::const_iterator const_iterator;

        /// begin()の再定義!
        iterator begin() { return line_.begin(); }

        /// begin()の再定義!
        const_iterator begin() const { return line_.begin(); }

        /// end()の再定義!
        iterator end() { return line_.end(); }

        /// end()の再定義!
        const_iterator end() const { return line_.end(); }

        /// ラインで分割されたstringオブジェクトのリスト!
        std::vector<stringT> line_;

        /// unittest用メソッド!
        POINTCUT_UNITTEST_METHOD
    };


  typedef MultiLineString<ustring>      mlustring; //!< multi line UNICODE string class
  typedef MultiLineString<mstring>      mlmstring; //!< multi line Mutibytes string class
  typedef MultiLineString<std::wstring>  mlwstring; //!< multi line wstring class
  typedef MultiLineString<std::string>   mlstring; //!< multi line string class
  // std::string, std::wstring利用時の特化
  template <> inline void MultiLineString<std::string>::set(const std::string& src) { UnifiedString<StringTypeChar>(src).split(line_, '\n'); }
  template <> inline void MultiLineString<std::wstring>::set(const std::wstring& src) { UnifiedString<StringTypeUtf16>(src).split(line_, L'\n'); }

}
#endif
