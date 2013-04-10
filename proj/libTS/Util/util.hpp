/*!	@file	util.hpp     -*-tab-width:4-*-
	@brief	ユーティリティークラス・ヘッダー
	@author	平松邦仁 ikeda
	@note	util.hppはC++専用のユーティリティークラスです。
*/
#ifndef ts_UTIL_H
#define ts_UTIL_H

#include "libTS/Common/Common.h"
#include <boost/shared_ptr.hpp>
#include <iostream>
#include <string>
#include <cstdio>

#include "ustring.hpp"
#include "mpl.hpp"
#include "bstream.hpp"
#include "PointerPolicy.hpp"

namespace ts {

  // exceptions
  struct tsException : public std::runtime_error {
    explicit tsException(const ustring& msg) throw() : std::runtime_error(msg.toChar()), m_msg(msg) {}
	virtual ~tsException() throw() {}
    const ustring& getMessage() const { return m_msg; }
    ustring m_msg;
  };
    
  struct InvalidPathName : public tsException {
    explicit InvalidPathName(const ustring& msg) : tsException(msg) {}
  };
  struct FileNotFoundException : tsException {
    explicit FileNotFoundException(const ustring& msg) : tsException(msg) {}
  };
  struct FileCanNotOpenException : tsException {
    explicit FileCanNotOpenException(const ustring& msg) : tsException(msg) {}
  };


  /*!
	  @class	util
	  @brief	ユーティリティークラス
	  @note	ファイル名文字列の操作、よく使う関数を集めたクラス
			  すべてstaticなメソッドになっています。
  			
  */
  class util {
  public:

		/*!
			@brief フルパス名をdrive,directory,filename,extentionに分離する
			@param[in]  path      フルパス名
			@param[out] drive     ドライブレター
			@param[out] directory ディレクトリ部(末尾の\を含む)
			@param[out] filename  ファイル名
			@param[out] ext       拡張子(先頭の.を含む)
			@throw	    util::InvalidPathName  無効なパス名
		*/
		static void splitPath(const ustring& path, ustring& drive, ustring& directory, ustring& filename, ustring& ext);

		/*!
			@brief					ディレクトリ名を取り除いてファイル名だけを取り出す
			@param[in]	pathname	パス名
			@return					ディレクトリ名を取り除いたファイル名
		*/
    static ustring getFilename(const wchar_t* pathname) { return getFilename(ustring(pathname)); }
    static mstring getFilename(const char* pathname) { return getFilename(mstring(pathname)); }
		static ustring getFilename(const ustring& pathname);
		static mstring getFilename(const mstring& pathname);

		/*!
			@brief					パス名に含まれるディレクトリ名を取り出す
            @note                   末尾の'\'もしくは'/'は取り除かない
			@param[in]	pathname	パス名
			@return					ディレクトリ名
		*/
		static ustring getDirectoryName(const ustring& pathname);
		static mstring getDirectoryName(const mstring& pathname);

		/*!
			@brief					ファイル名の拡張子を取り除く
			@param[in]	filename	ファイル名
			@return					拡張子を取り除いたファイル名
		*/
		static ustring removeFilenameExt(const ustring& filename);

		/*!
			@brief		ファイル名の拡張子だけを取り出す
			@param[in]	filename	ファイル名
			@return		拡張子のみ
		*/
		static ustring getFilenameExt(const ustring& filename);

		/*!
			@brief	ANSI標準関数に備わっている乱数の初期値を設定する
			@param[in]	s	乱数の初期値
		*/
		static void srand(unsigned long s);

		/*!
			@brief	ANSI標準関数に備わっている乱数生成関数
					※VisualC++ の仕様を設定
		*/
		static int rand();

		/*!
			@brief 暗号化アルゴリズムの定義
		*/
		enum CryptionType {
			STDC_RAND, // Cのrand系列を使ったもの
			UNKNOWN
		};
		/*!
			@brief	プロファイルの暗号解除処理
			@param[in]	buf バッファのポインタ
			@param[in]	len バッファのサイズ(byte)
		*/
		static void decryption(unsigned char* buf, size_t len, CryptionType type=STDC_RAND);

		/*!
			@brief	プロファイルの暗号化処理
			@param[in]	buf バッファのポインタ
			@param[in]	len バッファのサイズ(byte)
		*/
		static void encryption(unsigned char* buf, size_t len, CryptionType type=STDC_RAND);


		/*!
			@brief	pathが正しいか調べる
			@param[in]	path path文字列
			@return		正しい場合はtrue
		*/
		static bool isValidPath(const ustring& path);

		/*!
			@brief	ファイル名が正しいか調べる
			@param[in]	path path文字列
			@return		正しい場合はtrue
		*/
		static bool isValidFileName(const ustring& path);

		/*!
			@brief	    ファイルが存在するか調べる
			@param[in]	path path文字列
			@return		存在する場合はtrue
		*/
		static bool isExistPath(const ustring& path);

        static int64_t getFileSize(const ustring& path);

        static ustring toString(int);
        static ustring toString(unsigned int);



        // fileを書きだす
        static void outputFile(const ustring& fname, const std::string& buffer, const ustring& outputDir = ustring() ) {
          ustring fileName(outputDir.empty() ? fname : outputDir + L"\\" + fname);
          FILE* f = 0;
#ifdef WIN32
          fopen_s(&f, fileName.toChar(), "wb");
#else
		  f = fopen( fileName.toChar(), "wb");
#endif
		  
          if (f == 0) throw FileCanNotOpenException("fine can not write open");
          fwrite(&(buffer[0]), static_cast<size_t>(buffer.length()), 1, f);
          fclose(f);
        }


        // filenameを読みこんで stringのポインタを返す
        static boost::shared_ptr<std::string> readFromFile(const ustring& filename) {
          int64_t filesize = getFileSize(filename);
          if (filesize == 0) throw FileNotFoundException("fine not found or zero size");
          boost::shared_ptr<std::string> buffer(new std::string);
          buffer->resize(static_cast<size_t>(filesize));
          FILE* f = 0;
#ifdef WIN32
          fopen_s(&f, filename.toChar(), "rb");
#else
		  f = fopen(filename.toChar(), "rb");
#endif
          if (f == 0) throw FileCanNotOpenException(ustring("file can not read open:") + filename);
          if (::fread(& (*buffer)[0], static_cast<size_t>(filesize), 1, f) == 0) {
			throw FileCanNotOpenException(ustring("file read error:") + filename);
		  }
          fclose(f);
          return buffer;
        }


        // @brief 文字列をセパレータで分割する
        static inline
        void separateString(const ustring& src, ustring& main, ustring& sub, const wchar_t* sep) {
          std::vector<ustring> typew;
          src.split(typew, sep[0]);
          if (typew.size() > 0 && typew[0].length() > 0) main = typew[0];
          if (typew.size() > 1 && typew[1].length() > 0) sub = typew[1].trim(sep);
        }


        static
        void sjisToUtf8File(const ustring& sjisFile, const ustring& utf8File) {
          boost::shared_ptr<std::string> buffer(util::readFromFile(sjisFile));
          std::string utf8;
          ustring sjis(*buffer);
          sjis.replace_string("\r\n", "\n"); // 改行コードを\nに変換
          strutil::wstring2utf8(sjis, utf8);
          util::outputFile(utf8File, utf8);
        }

        static
        void utf8ToSjisFile(const ustring& utf8File, const ustring& sjisFile) {
          boost::shared_ptr<std::string> buffer(util::readFromFile(utf8File));
          std::wstring wstr;
          strutil::utf8towstring(*buffer, wstr);
          util::outputFile(sjisFile, ustring(wstr).toChar());
        }

        // ファイルのタイムスタンプを調べる
        static time_t getFileTime(const ustring& path);



        // @brief ディレクトリ選択
        static ustring selectDirectory(const ustring& initialDirectory);

        // @brief 階層ディレクトリを作成する
        static void makeDirectory(const ustring& dir);

		/*!
			@brief	単体試験用のメソッド
		*/
		POINTCUT_UNITTEST_METHOD;

    }; // class util



  // 配列の大きさを返す
  template <typename T, uint32_t N>
  inline uint32_t CountOf(T(&)[N]) {
    return N;
  }
  template <typename T>
  inline uint32_t CountOf(std::vector<T>& v) {
    return v.size();
  }
  template <typename K, typename T>
  inline uint32_t CountOf(std::map<K, T>& v) {
    return v.size();
  }


  namespace logic {

    // 3値をもつboolみたいなもの
    // boost::logic::triboolはいまひとつ使いにくい(boolを返す比較演算子に乏しい)
    // こちらはすべての比較演算子はboolを返す。
    // triboolをboolで評価するとビルドエラーになる。必ず比較演算子で評価すること。
    struct tribool {
      enum TriType { false_value, true_value, indeterminate_value } value;
      tribool(TriType t) : value(t) {}
      tribool(bool b) : value(b ? true_value : false_value) {}
      tribool() : value(indeterminate_value) {}

      // !演算子ではtrueとfalseのみ反転する!?
      tribool operator ! () {
        if (value == true_value) return false_value;
        if (value == false_value) return true_value;
        return indeterminate_value;
      }

      /*!
			  @brief	単体試験用のメソッド
		  */
		  POINTCUT_UNITTEST_METHOD;

      // boolで評価するのは禁止！
    private:
      operator bool () {
        abort(); // ビルドできちゃった時のためのabort()
        // ビルドエラーにするためにわざとreturn文が無い(gccで要確認)
      }
    };
    const tribool::TriType indeterminate = tribool::indeterminate_value; // indeterminate(不確定)の定義
    inline bool operator == (tribool a, tribool b) { return a.value == b.value; }
    inline bool operator == (tribool a, tribool::TriType b) { return a.value == b; }
    inline bool operator == (bool a,    tribool b) { return tribool(a) == b; }
    inline bool operator == (tribool a, bool b)    { return a == tribool(b); }
    inline bool operator != (tribool a, tribool b) { return a.value != b.value; }
    inline bool operator != (tribool a, tribool::TriType b) { return a.value != b; }
    inline bool operator != (bool a,    tribool b) { return tribool(a) != b; }
    inline bool operator != (tribool a, bool b)    { return a != tribool(b); }
  } // namespace logic



  // 2次元配列
  template<class ValT>
  struct array_2d {
	typedef typename std::vector<ValT>::iterator iterator;
	typedef typename std::vector<ValT>::const_iterator const_iterator;
	typedef typename std::vector<ValT>::value_type value_type;
	std::vector<ValT> d_;
    int32_t sizeX_;
    int32_t sizeY_;

    array_2d(int32_t xsz=0, int32_t ysz=0)
    : sizeX_(xsz)
    , sizeY_(ysz)
    {
      if (xsz * ysz) {
        d_.resize(xsz*ysz);
        memset(&d_[0], 0, sizeof(ValT) * sizeX_ * sizeY_);
      }
    }
    ~array_2d() {}

    int32_t count() const {
      return sizeX_ * sizeY_;
    }
    size_t byte_size() const {
      return sizeX_ * sizeY_ * sizeof(ValT);
    }
	size_t size() const { return sizeX_ * sizeY_; }
	int32_t width() const { return sizeX_; }
	int32_t height() const { return sizeY_; }
	
	iterator begin() { return d_.begin(); }
	const_iterator begin() const { return d_.begin(); }
	iterator end() { return d_.end(); }
	const_iterator end() const { return d_.end(); }
	

    ValT& operator()(int32_t x, int32_t y) {
      if(x > sizeX_ || x < 0 || y > sizeY_ || y < 0) {
        throw std::string("array2 out of range"); // std::out_of_range("array2 out of range");
      }
      return d_[y * sizeX_ + x];
    }
    const ValT& operator[](size_t n) const { return d_[n]; }
    ValT& operator[](size_t n) { return d_[n]; }

    size_t index(int32_t x, int32_t y) const {
      return y * sizeX_ + x;
    }

    void resize(int32_t x, int32_t y, bool zeroflag=false) {
      d_.resize(x*y);
      sizeX_ = x;
      sizeY_ = y;
      if(zeroflag) {
        memset(&d_[0], 0, sizeof(ValT) * sizeX_ * sizeY_);
      }
    }

    void write(bstream& bs) {
      bs << sizeX_;
      bs << sizeY_;
      bs.write(d_[0], byte_size());
    }
    void read(const bstream& bs) {
      bs >> sizeX_;
      bs >> sizeY_;
      resize(sizeX_, sizeY_);
      bs.read(d_[0], byte_size());
    }

    // write
    bstream& operator >> (bstream& bs) {
      write(bs);
      return bs;
    }

    // read
    const bstream& operator << (const bstream& bs) {
      read(bs);
      return bs;
    }
  }; // array2

  template <typename T>
  bstream& operator >> (bstream& bs, array_2d<T>& ar) {
    ar.read(bs);
    return bs;
  }
  template <typename T>
  bstream& operator << (bstream& bs, array_2d<T>& ar) {
    ar.write(bs);
    return bs;
  }

  
  // 文字列を32bitの値にする
  inline uint32_t CStr2Int32(const char* str) {
	uint32_t d = 0;
	assert(str != 0);
	int shift =  32 / strlen(str);
	while (*str) {
	  d <<= shift;
	  d += *str++;
	};
	return d;
  }

  // 何でも文字列にする
#define TS_TO_STRING(T) inline std::string to_string(T v) {	std::stringstream ss; ss << v; return ss.str(); }
  TS_TO_STRING(int32_t);
  TS_TO_STRING(uint32_t);
  TS_TO_STRING(int16_t);
  TS_TO_STRING(uint16_t);
  TS_TO_STRING(int8_t);
  TS_TO_STRING(uint8_t);
  TS_TO_STRING(int64_t);
  TS_TO_STRING(uint64_t);
  TS_TO_STRING(float);
  TS_TO_STRING(double);
  TS_TO_STRING(bool);

  inline std::string to_string(const char* v) {	return std::string(v); }
  inline std::string to_string(const std::string& v) {	return std::string(v); }
  template <typename T>
  inline std::string to_string(const std::vector<T>& v) {
	std::string s("[");
	for (typename std::vector<T>::const_iterator i = v.begin(); i != v.end(); ++i) {
	  if (i != v.begin()) s += ",";
	  s += to_string(*i);
	}
	s += "]";
	return s;
  }
  template <typename T1, typename T2>
  inline std::string to_string(const std::map<T1, T2>& v) {
	std::string s("{");
	for (typename std::map<T1,T2>::const_iterator i = v.begin(); i != v.end(); ++i) {
	  if (i != v.begin()) s += ",";
	  s += to_string((*i).first);
	  s += ":";
	  s += to_string((*i).second);
	}
	s += "}";
	return s;
  }
	




} // namespace ts

#endif // UTILITY_H

