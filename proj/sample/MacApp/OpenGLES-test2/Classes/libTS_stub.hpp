/* -*-tab-width:4-*-
   libTS Xcode stub
 */
#define png_infopp_NULL (png_infopp)NULL
#define int_p_NULL (int*)NULL

#include <stdint.h>
#include <iostream>
#include <string>
#include <queue>
#include <stdexcept>
#include <boost/shared_ptr.hpp>
#include <boost/gil/gil_all.hpp>
#include <boost/gil/extension/io/png_io.hpp>
#include <boost/ref.hpp>
#include <boost/thread.hpp>

#include <OpenGLES/ES1/gl.h>
#include <OpenGLES/ES1/glext.h>

#include "libTS/Util/Logger.h"

namespace ts {
namespace appfw {
  namespace sounds {
	class music_player {
	public:
	  enum PlayStyle {
		NoRepeat
		, Repeat
	  };
	  enum PlayStopStyle {
		StopNow
		, FadeOut
	  };
	  void start(uint32_t musicId, PlayStyle style) {
	  }
	  void stop(PlayStopStyle style, uint32_t param = 0) {
	  }
	  bool is_playing() const {
		return false;
	  }
	};
  }
}
} // namespace ts



#include "libTS/appfw/appfw.hpp"
