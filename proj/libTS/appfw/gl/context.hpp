/* context.hpp -*-tab-width:4-*-
   libTS glfw  Graphics Language Frame Work
 */

#ifndef TS_APPGW_CONTEXT_HPP
#define TS_APPGW_CONTEXT_HPP

#include "common.hpp"


namespace ts {
namespace appfw {
namespace gl {
  using boost::shared_ptr;
  using math::size_i;
  using math::rect_f;

  class context {
  private:
	size_i device_screen_size_; // デバイスの画面サイズ
	volatile float rotate_; // デバイスの回転角度

  public:
    size_i screen_size_;    // アプリから見た画面のサイズ(回転角度によって変化する)
	float scale_factor_;
	rect_f viewport_;
	uint32_t lastTexture_;
	bool portlate_;
  public:
	context(uint32_t w = 320, uint32_t h = 480, float scf=1.0f)
	  :
	    rotate_(0.0f)
	  , screen_size_(w,h)
	  , scale_factor_(scf)
	  , viewport_(0.f,0.f,float(w),float(h))
	  , lastTexture_(0xffffffff)
	  , portlate_(true)
	{
	}

	// スクリーンサイズをセット
	void set_device_screen_size(int32_t width, int32_t height, float scf=1.0f) {
	  device_screen_size_.width_ = width;
	  device_screen_size_.height_ = height;
	  scale_factor_ = scf;
	  viewport_ = rect_f(0.f, 0.f, float(width), float(height));
	  set_rotate(rotate_);
	}
	const size_i screen_size() const { return screen_size_; }
	const size_i device_screen_size() const { return device_screen_size_; }


	float screen_bottom() const { return float(screen_size_.height_); }
	float screen_right() const { return float(screen_size_.width_); }

	void set_rotate(float rotate) {
	  rotate_ = rotate;
	  if (rotate_ == 90 || rotate_ == 270) {
		screen_size_.width_ = device_screen_size_.height_;
		screen_size_.height_ = device_screen_size_.width_;
		portlate_ = false;
	  }
	  else {
		screen_size_ = device_screen_size_;
		portlate_ = true;
	  }
	}
	float rotate() const { return rotate_; }



  };



}}} // namespace ts::appfw::gl



#endif
