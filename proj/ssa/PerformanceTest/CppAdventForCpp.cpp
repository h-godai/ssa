// C++ Advent test -*-tab-width:4-*-
#include "stdafx.h"
#undef new
#undef delete
#undef malloc
#undef free

#include <vector>
#include <boost/range/algorithm/for_each.hpp>
#include "CppAdventForC.h"

using namespace boost;
using namespace std;

namespace app {

  void bitmap_color_transform_cpp(frame_buffer_t& buffer) {
	for_each(buffer, transcolor());
  }

}

