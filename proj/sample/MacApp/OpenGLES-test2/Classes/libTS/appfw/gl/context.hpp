/* context.hpp -*-tab-width:4-*-
   libTS glfw  Graphics Language Frame Work
 */

#ifndef TS_APPGW_CONTEXT_HPP
#define TS_APPGW_CONTEXT_HPP

#include "common.hpp"


namespace ts {
namespace appfw {
namespace gl {
  using boost::gil::rgba8_image_t;
  using boost::gil::rgb8_image_t;
  using boost::gil::view;
  using boost::shared_ptr;

  class context {
	
  public:
	size_i screen_size_;
  public:
	context(uint32_t w = 320, uint32_t h = 480) : screen_size_(w,h)
	{}
	
	// スクリーンサイズをセット
	void set_screen_size(uint32_t width, uint32_t height) {
	  screen_size_.width_ = width;
	  screen_size_.height_ = height;
	}
	

	
  };


  
}}} // namespace ts::appfw::gl



#endif
