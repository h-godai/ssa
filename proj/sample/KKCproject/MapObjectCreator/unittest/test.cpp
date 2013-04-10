// test.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include <string>
#include "Util/unittest.hpp"
#include "Util/ustring.hpp"
#include "../MapObjectCreator.hpp"

using namespace std;
using namespace ts;

int _tmain(int /*argc*/, _TCHAR* /*argv*/[])
{
  // ライブラリのunittestは明示的に呼ばないとリンクされない
  ustring::unittest();
  bstream::unittest();
  util::unittest();
  tribool::unittest();
  ustring_map::unittest();
  mstring_map::unittest();

  Unittest::getInstance().setSilentMode();
  if (!Unittest::getInstance().run()) {
    abort();
  }

	return 0;
}
