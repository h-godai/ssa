  //#include "stdafx.h"
/*!	@file
	@brief	TSL ユーティリティー・クラス
	@author	平松邦仁 ikeda
*/
#include "../Common/Common.h"
#include <cstdlib>
#ifdef WIN32
#include <direct.h>
#define PLATFORM_WINDOWS
#else
#include <sys/types.h>
#include <sys/stat.h>
#endif
#include "util.hpp"
#include "unittest.hpp"
#include "bstream.hpp"
#include "mpl.hpp"
#include "enum.hpp"

using namespace std;
namespace ts {


/*!
	@brief デフォルトのロケール識別子
*/
//static const char* LOCALE_JP = "Japanese";

/*
	@brief フルパス名をdrive,directory,filename,extentionに分離する
*/
void util::splitPath(const ustring& path, ustring& drive, ustring& directory, ustring& filename, ustring& ext) {
	if (!isValidPath(path)) throw InvalidPathName(path.toChar());
	ustring pathtmp;
	// ドライブレターの分離
	if (path[1] == L':') {
		drive = path.substr(0,2);
		pathtmp = path.substr(2);
	}
	else 	{
		drive.clear();
		pathtmp = path;
	}
	// ディレクトリ名の分離
	size_t pos = pathtmp.find_last_of(ustring(L"\\/"));
	if (pos != ustring::npos) {
		// ディレクトリあり
		filename = pathtmp.substr(pos+1);
		directory = pathtmp.substr(0, pos+1);
	}
	else {
		// ディレクトリなし
		filename = pathtmp;
		directory.clear();
	}
	// 拡張子の分離
	pos = filename.find_last_of(L'.');
	if (pos != ustring::npos) {
		ext = filename.substr(pos);
		filename = filename.substr(0,pos);
	}
	else {
		ext.clear();
	}
}

/*
	@brief					ディレクトリ名を取り除いてファイル名だけを取り出す
	@param[in]	filename	パス名
	@return					ディレクトリ名を取り除いたファイル名
*/
ustring util::getFilename(const ustring& pathname)
{
	size_t pos = pathname.find_last_of(ustring(L"\\/"));
	if (pos != ustring::npos) {
		return pathname.substr(pos+1);
	}
	else {
		return pathname;
	}
}
mstring util::getFilename(const mstring& pathname)
{
	size_t pos = pathname.find_last_of(mstring("\\/"));
	if (pos != ustring::npos) {
		return pathname.substr(pos+1);
	}
	else {
		return pathname;
	}
}

/*
	@brief					パス名に含まれるディレクトリ名を取り出す
	@param[in]	pathname	パス名
	@return					ディレクトリ名
*/
ustring util::getDirectoryName(const ustring& pathname)
{
	size_t pos = pathname.find_last_of(ustring(L"\\/"));
	if(pos != ustring::npos) {
		return pathname.substr(0, pos+1);
	} else {
		return ""; // ディレクトリが無い場合は ""
	}
}
mstring util::getDirectoryName(const mstring& pathname)
{
	size_t pos = pathname.find_last_of(mstring("\\/"));
	if(pos != mstring::npos) {
		return pathname.substr(0, pos+1);
	} else {
		return ""; // ディレクトリが無い場合は ""
	}
}


/*
	@brief					拡張子を取り除く
	@param[in]	filename	ファイル名
	@return					拡張子を取り除いたファイル名
*/
ustring util::removeFilenameExt(const ustring& filename)
{
	if (getFilename(filename).find_last_of('.') == ustring::npos) {
		// ファイルネーム部に'.'が無い場合は拡張子なし
		return filename;
	}
	return filename.substr(0, filename.find_last_of('.'));
}


/*
	@brief		ファイル名の拡張子だけを取り出す
	@param[in]	filename	ファイル名
	@return		拡張子のみ
*/
ustring util::getFilenameExt(const ustring& filename)
{
	if (getFilename(filename).find_last_of('.') == ustring::npos) {
		// ファイルネーム部に'.'が無い場合は拡張子なし
		return L"";
	}
	return filename.substr(filename.find_last_of('.'));
}



//! Visual C++:	A=214013, C=2531011, F=0, S=1
const int RAND_A = 214013;
const int RAND_C = 2531011;
const int RAND_F = 0;
const int RAND_S = 1;

//! 自前の乱数生成、乱数値保持用
static long rand_value = RAND_S;

/*
	@brief	ANSI標準関数に備わっている乱数生成関数
			※VisualC++ の仕様を設定
*/
int util::rand()
{
	rand_value = rand_value * RAND_A + RAND_C;
	return (int) (rand_value >> 16) & 32767;
}

/*
	@brief	ANSI標準関数に備わっている乱数の初期値を設定する
	@param[in]	s	乱数の初期値
*/
void util::srand(unsigned long s)
{
	rand_value = s;
	if(RAND_F != 0) rand();
}

/*
	@brief	プロファイルの暗号解除処理
	@param[in]	buf バッファのポインタ
	@param[in]	len バッファのサイズ(byte)
*/
void util::decryption(unsigned char* buf, size_t len, CryptionType type) {
	switch(type) {
	case STDC_RAND:
	default:
		util::srand(8086);
		for(size_t i = 0; i < len; i++) {
			unsigned char rchar = (unsigned char )util::rand();
			buf[i] = buf[i] ^ rchar;
		}
	}
}

/*
	@brief	プロファイルの暗号化処理
	@param[in]	buf バッファのポインタ
	@param[in]	len バッファのサイズ(byte)
*/
void util::encryption(unsigned char* buf, size_t len, CryptionType ) {
	decryption(buf, len);
}


/*
	@brief	パス名のチェック
	@param[in]
	@param[in]
*/
bool util::isValidPath(const ustring& path) {
	const wchar_t* invalidChr = L":*?<>|&\"";

	// 不正な \\ // がないか調べる
	if (path.find(L"\\\\") != ustring::npos) return false;
	if (path.find(L"//") != ustring::npos) return false;

	ustring buf;
	size_t pos = 0;
	if (path[1] == static_cast<wchar_t>(':')) {
		// ドライブ名つき
		if ((path[0] < 'A' || path[0] > 'Z') &&
			(path[0] < 'a' || path[0] > 'z')) return false;
		pos = 2;
	}
	buf = path.substr(pos);
	if (buf.find_first_of(invalidChr) != ustring::npos) return false;
	return true;
}

/*
	@brief	ファイル名のチェック
	@param[in]
	@param[in]
*/
bool util::isValidFileName(const ustring& path) {
	const wchar_t* invalidChr = L"\\:*?<>|&\"/";
	if (path.find_first_of(invalidChr) != ustring::npos) return false;
	return true;
}

/*
	@brief	パスが存在するかチェック
*/
bool util::isExistPath(const ustring& path) {
#ifdef PLATFORM_WINDOWS
  struct _stat st;
  return _wstat(path.c_str(), &st) == 0;
#else
  struct stat st;
  return ::stat(path.toChar(), &st) == 0;
#endif
}

time_t util::getFileTime(const ustring& path) {
#ifdef PLATFORM_WINDOWS
  struct _stat st;
  if (_wstat(path.c_str(), &st) != 0) return 0;
  return st.st_mtime;
#else
  struct stat st;
  if (::stat(path.toChar(), &st) != 0) return 0;
  return st.st_mtime;
#endif
}


int64_t util::getFileSize(const ustring& path) {
#ifdef PLATFORM_WINDOWS
    struct _stat64 st;
    if (_wstat64(path.c_str(), &st) != 0) return 0;
    return st.st_size;
#else
    struct stat st;
    if (::stat(path.toChar(), &st) != 0) return 0;
    return st.st_size;
#endif
};

ustring util::toString(int n) {
  ustring str;
  str.format(L"%d", n);
  return str;
}
ustring util::toString(unsigned int n) {
  ustring str;
  str.format(L"%u", n);
  return str;
}


#ifndef WIN32
ustring util::selectDirectory(const ustring& initialDirectory) {
  return initialDirectory;
}
#endif

void util::makeDirectory(const ustring& dir) {
  vector<ustring> lst;
  ustring newdir(dir);
  if (newdir[newdir.length()-1] == '\\' ||newdir[newdir.length()-1] == '/') newdir = newdir.substr(0, newdir.length()-1);
  while (!isExistPath(newdir)) {
    lst.push_back(newdir);
    newdir = getDirectoryName(newdir);
    newdir = newdir.substr(0, newdir.length()-1);
    if (newdir.length() == 0) break;
  }
  if (newdir != dir) {
    for (vector<ustring>::reverse_iterator i = lst.rbegin(); i != lst.rend(); ++i) {
#ifdef PLATFORM_WINDOWS
      _mkdir(i->toChar());
#else
      mkdir(i->toChar(), 0x777);
#endif
    }
  }
}


  struct ClassA { static void print(Unittest* test) { test->check(true, "ClassA"); } };
  struct ClassB { static void print(Unittest* test) { test->check(true, "ClassB"); } };
  struct ClassC { static void print(Unittest* test) { test->check(true, "ClassC"); } };
  struct ClassD { static void print(Unittest* test) { test->check(true, "ClassD"); } };
  struct ClassE { static void print(Unittest* test) { test->check(true, "ClassE"); } };
  struct ClassF { static void print(Unittest* test) { test->check(true, "ClassF"); } };
  struct ClassG { static void print(Unittest* test) { test->check(true, "ClassG"); } };
  struct ClassH { static void print(Unittest* test) { test->check(true, "ClassH"); } };
  typedef ts::mpl::list<ClassA,ClassB,ClassC,ClassD>::type ClassList;
  typedef ts::mpl::begin<ClassList>::type BeginList;
  typedef ts::mpl::end<ClassList>::type EndList;

  typedef ts::mpl::list<ClassA,ClassB,ClassC,ClassD,ClassE,ClassF,ClassG,ClassH>::type ClassList2;
  typedef ts::mpl::begin<ClassList2>::type BeginList2;
  typedef ts::mpl::end<ClassList2>::type EndList2;


  template <typename Begin, typename End>
  struct ListLoopTest {
    typedef typename ts::mpl::deref<Begin>::type Type;
    static void execute(Unittest* test) {
      Type::print(test);
      ListLoopTest<typename mpl::next<Begin>::type, End>::execute(test);
    }
  };
  template <typename End>
  struct ListLoopTest<End, End> {
    static void execute(Unittest* test) {
      test->check(true, "End");
    }
  };


/*
	@brief	単体試験ルーチン
*/
bool util::unittest(Unittest* test) {
  if (test == 0) return false;

	test->message("*** start ts::util unittest ***\n");

  test->message("*** ts::mpl::list unittest ***\n");
  ListLoopTest<BeginList, EndList>::execute(test);
  ListLoopTest<BeginList2, EndList2>::execute(test);



	ustring drive,dir,file,ext;
	util::splitPath("x:\\ho\\hoge\\hoho.hoge", drive, dir, file, ext);
	test->check(drive==L"x:" && dir==L"\\ho\\hoge\\" && file == L"hoho" && ext ==L".hoge", "util::splitPath check");

	test->check(util::getFilename(L"c:\\ho\\hoho.h") == L"hoho.h",        "util::getFileName check");
	test->check(util::getDirectoryName(ustring(L"c:\\ho\\hoho.h")) == L"c:\\ho\\", "util::getDirectoryName check");
	test->check(util::removeFilenameExt(L"hoho.h") == L"hoho",            "util::removeFilenameExt check1");
	test->check(util::removeFilenameExt(L"\\h.h\\hoho") == L"\\h.h\\hoho","util::removeFilenameExt check2");
	test->check(util::getFilenameExt(L"hoho.h") == L".h",                 "util::getFilenameExt check1");
	test->check(util::getFilenameExt(L"\\h.h\\hoho") == L"",              "util::getFilenameExt check2");

	test->check(util::isValidPath("c:\\hoge\\hogehoge"),  "util::isValidPath check1");
	test->check(util::isValidPath("c:\\hoge\\hoge\\"),    "util::isValidPath check2");
	test->check(!util::isValidPath("c:\\hoge\\\\hoge\\"), "util::isValidPath check3");
	test->check(!util::isValidPath("c:\\hoge*hoge\\"),    "util::isValidPath check4");
	test->check(!util::isValidPath("c::\\hogehoge\\"),    "util::isValidPath check5");
	test->check(!util::isValidPath("\\hogehoge\\\""),     "util::isValidPath check6");
	test->check(!util::isValidPath("%:\\hoge"),           "util::isValidPath check7");

	test->check(util::isValidFileName("hogehoge"), 		  "util::isValidFileName check1");
	test->check(!util::isValidFileName("hogehoge\\"),     "util::isValidFileName check2");
	test->check(!util::isValidFileName("hoge\\hoge"),     "util::isValidFileName check3");
	test->check(!util::isValidFileName("/hoge"),          "util::isValidFileName check4");
	test->check(!util::isValidFileName("c::\\hogehoge"),  "util::isValidFileName check5");
	test->check(util::isValidFileName("hogehoge{{}}"),    "util::isValidFileName check6");
	test->check(!util::isValidFileName("<hoge>"),         "util::isValidFileName check7");

	test->check(util::isExistPath("."), 				  "util::isExistPath check1");
	test->check(!util::isExistPath("not exist path"),     "util::isExistPath check2");

  return true;
}

namespace logic {
bool tribool::unittest(Unittest* test) {
  if (test == 0) return false;
	test->message("*** start ts::tribool unittest ***\n");

  tribool hoge, hogehoge = false;
  // if (hoge) {}  // これはビルドエラー！
  test->check(hoge == indeterminate,     "util::tribool check-1");
  test->check(hoge != true,              "util::tribool check-2");
  test->check(hoge != false,             "util::tribool check-3");
  test->check(hogehoge == false,         "util::tribool check-4");
  test->check(hogehoge != true,          "util::tribool check-5");
  test->check(hogehoge != indeterminate, "util::tribool check-6");
  test->check(hoge != hogehoge,          "util::tribool check-7");
  test->check(!hoge == indeterminate,    "util::tribool check-8");
  test->check(!hogehoge == true,         "util::tribool check-9");
  return true;
}
// 単体試験エントリの登録
POINTCUT_UNITTEST_REGIST(tribool);
}

namespace enum_test {
using namespace ts;

#ifdef DEKITARA_IINA
  enum Hoge {
    Hoge1,
    Hoge2,
    Hoge3,
    END_ENUM
  };

  enum Fuga : Hoge {
    Fuga1,
    Fuga2
  };
#endif
  // enumeration for base 
  struct Hoge {
    enum type {
      Hoge1,
      Hoge2,
      Hoge3,
      END_ENUM
    };
  };
  typedef enum8<Hoge>::type EnumHoge;

  // enumration for subclass
  template <typename Super>
  struct Fuga : Super {
    enum type {
      Fuga1 = Super::END_ENUM,
      Fuga2,
      Fuga3,
      END_ENUM
    };
  };
  // enumration for subclass
  template <typename Super>
  struct Mago : Super {
    enum type {
      Mago1 = Super::END_ENUM,
      Mago2,
      Mago3,
      END_ENUM
    };
  };
 template <typename Super>
  struct Himago : Super {
    enum type {
      Himago1 = Super::END_ENUM,
      Himago2,
      Himago3,
      END_ENUM
    };
  };

  typedef EnumHoge::append<Fuga>::type EnumFuga;
  typedef EnumFuga::append<Mago>::type EnumMago;
  typedef EnumMago::append<Himago>::type EnumHimago;

  struct enum_unittest {
    static bool unittest(Unittest* test) {
      if (test == 0) return false;
	    test->message("*** start emum unittest ***\n");
        test->check(test1(test),  "util::enum constructor check");
        test->check(test2(test),  "util::enum operator =  check");
        return true;
    }
    // constructor test
    static bool test1(Unittest* /*test*/) {
      EnumHoge hoge1;
      EnumHoge hoge2(EnumHoge::Hoge1);
      EnumHoge hoge3 = EnumHoge::Hoge2;
      EnumFuga fuga1;
      EnumFuga fuga2(EnumFuga::Hoge1);
      EnumFuga fuga3(EnumFuga::Fuga1);
      EnumFuga fuga4 = EnumFuga::Hoge1;
      EnumFuga fuga5 = EnumFuga::Fuga1;
      EnumMago mago1;
      EnumMago mago2(EnumMago::Hoge1);
      EnumMago mago3(EnumMago::Fuga1);
      EnumMago mago4(EnumMago::Mago1);
      EnumMago mago5 = EnumMago::Hoge1;
      EnumMago mago6 = EnumMago::Fuga1;
      EnumMago mago7 = EnumMago::Mago1;
      return true;
    }
    static bool test2(Unittest* /*test*/) {

      EnumFuga hofu;
      switch (hofu.get()) {
        case EnumFuga::Hoge1:
        case EnumFuga::Hoge2:
        case EnumFuga::Hoge3:
        case EnumFuga::Fuga1:
        default:
          break;
      }
      if (hofu == EnumFuga::Hoge1) printf("hofu is hoge1\n");
      if (hofu == EnumFuga::Fuga1) printf("hofu is fuga1\n");
      if (hofu != EnumFuga::Hoge1) printf("hofu is not hoge1\n");
      if (hofu != EnumFuga::Fuga1) printf("hofu is not fuga1\n");
      hofu = EnumFuga::Hoge1;
      hofu = EnumFuga::Fuga1;
      // hofu = 1; // must be error
      // hofu == 1; // must be error
      EnumFuga hofu2(EnumFuga::Hoge1);
      EnumFuga hofu3(EnumFuga::Fuga1);
      EnumFuga hofu4 = EnumFuga::Hoge1;
      EnumFuga hofu5 = EnumFuga::Fuga1;

      EnumMago mago( EnumMago::Mago1 );
      EnumMago mago2( EnumMago::Hoge1 );
      EnumMago mago3( EnumMago::Fuga1 );
      EnumMago mago4 = EnumMago::Mago1 ;
      EnumMago mago5 = EnumMago::Hoge1 ;
      EnumMago mago6 = EnumMago::Fuga1 ;
      mago = EnumMago::Mago1;
      mago = EnumMago::Fuga2;
      mago = EnumMago::Hoge1;
      if (mago == EnumMago::Mago1) printf("mago is mago1\n");
      if (mago == EnumMago::Hoge1) printf("mago is hoge1\n");
      if (mago == EnumMago::Fuga1) printf("mago is fuga1\n");
      if (mago != EnumMago::Mago1) printf("mago is mago1\n");
      if (mago != EnumMago::Hoge1) printf("mago is hoge1\n");
      if (mago != EnumMago::Fuga1) printf("mago is fuga1\n");
      
      // mago = 1; // must be error
      // mago == 1; // must be error

      //ENUM_FOR_EACH(EnumHogeFuga::EnumType e, EnumHogeFuga) {
      //  printf("enum is %d\n", e);
      //}
      return true;
    }
  };
// 単体試験エントリの登録
POINTCUT_UNITTEST_REGIST(enum_unittest);
} // namespace





// lib内にtemplateのインスタンスを生成するために必要
#ifndef RELEASE
// 単体試験エントリの登録
POINTCUT_UNITTEST_REGIST(util);
#endif

} // namespace tsl
/* ----- End Of File "Utility.cpp" ----- */
