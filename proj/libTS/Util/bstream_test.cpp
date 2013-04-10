// bstream_test.cpp
//
// bstreamの単体テストプログラム

#include "util.hpp"
#include "bstream.hpp"

#ifdef WIN32
#pragma warning (disable : 4996)
#endif

using namespace std;
namespace ts {

  bool bstream::unittest(Unittest* test) {
    if (test == 0) return false;

    uint32_t i32;
    float32_t f32;
    float64_t f64;
    test->message("*** start ts::bstream unittest ***\n");
    {
      bstream bs;
      // test 1 データの書き込み
      bs << static_cast<int32_t>(1) << 1.0f << 1.0;
      test->check(bs.size() == 16, "bstream: test-1 operator<< size check");

      // test 2 ファイル入出力
      FILE* f = tmpfile();
      if (f == 0) f = fopen("tmpfile", "wb");
      assert(f);
      bs.writeTo(f);
      fseek(f, 0, SEEK_SET);
      bstream bs2(f);
      bs2 >> i32 >> f32 >> f64;
      test->check(i32 == 1 && f32 == 1.0f && f64 == 1.0, "bstream: test-2 file read/write");
      fclose(f);
    }

    {
      float32_t f123, f234, f345, f456, f567;
      string hoge("hoge");
      ustring uhoge(L"ユニコード!!");
      mstring mhoge("シフトジス!!");
      bstream bs;
      // test 3 入れ子テスト
      bs << 1.23f;
      {
        bstream bs2;
        bs2 << 2.34f;
        bs2 << hoge;
        {
          bstream bs3;
          bs3 << uhoge;
          bs3 << 3.45f;
          bs3 << 4.56f;
          {
            bstream bs4;
            bs4 << mhoge;
            bs4 << 5.67f;
            bs3 << bs4;
          }
          bs2 << bs3;
        }
        bs << bs2;
      }
      bs.rewind(); // 書きこんだデータを読み出す場合はポインタを戻す
      hoge.clear();
      uhoge.clear();
      mhoge.clear();
      bs >> f123;
      {
        bstream bs2;
        bs >> bs2;
        bs2 >> f234;
        bs2 >> hoge;
          bstream bs3;
          bs2 >> bs3;
          bs3 >> uhoge >> f345 >> f456;
            bstream bs4;
            bs3 >> bs4;
            bs4 >> mhoge >> f567;
      }
      test->check(f123 == 1.23f && f234 == 2.34f && f345 == 3.45f && f456 == 4.56f && f567 == 5.67f
        , "bstream: test-3 ireko test");
      test->check(hoge == string("hoge") && uhoge == ustring(L"ユニコード!!") && mhoge == mstring("シフトジス!!")
        , "bstream: test-4 string test");
    }

    {
      // 配列の読み書きテスト
      vector<float64_t> srcdata;
      vector<float64_t> dstdata;
      for (size_t n = 0; n < 100; ++n) {
        srcdata.push_back(1.0 / (util::rand() + 1));
      }
      bstream bs;
      bs << srcdata;
      bs.rewind();
      bs >> dstdata;
      bool ok = true;
      for (size_t n = 0; n < srcdata.size(); ++n) {
        if (srcdata[n] != dstdata[n]) ok = false;
      }
      test->check(ok, "bstream: test-5 vector array read/write test");
    }






    return true;
  
  }


//POINTCUT_UNITTESET_REGIST(bstream);

//static struct unittestRegistbstream { unittestRegistbstream() { ts::Unittest::append(bstream::unittest); } } globalRegistbstream;

}

  // 登録


