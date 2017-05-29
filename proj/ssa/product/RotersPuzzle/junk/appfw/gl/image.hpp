/* image.hpp -*-tab-width:4-*-
   libTS appfw  Graphics Language Frame Work
 */

#ifndef TS_APPFW_GL_IMAGE_PP
#define TS_APPFW_GL_IMAGE_PP

#include "common.hpp"

namespace ts {
namespace appfw {
namespace gl {


  struct image_exception : glfw_exception {
	image_exception(const char * const & msg) : glfw_exception(msg) {}
	virtual ~image_exception() throw() {}
  };
  
  template <typename PixelT>
  struct image {
	shared_ptr<PixelT> data_;
	image() {}
	image(const image& img) : data_(img.data_) {}
	image(const std::string& fn) {
	  *this << fn;
	}
	image(size_t w, size_t h) : data_(new PixelT(w,h)) {}

	void swap(image& img) throw() {
	  data_->swap(*img.data_);
	}
	
	image& operator = (image img) throw() {
	  swap(img);
	  return *this;
	}
	
	void clear() throw() {
	  swap(image());
	}

	operator PixelT& () { return *data_; }
	PixelT& get() { return *data_; }

	const typename PixelT::point_t& dimensions() const { return data_->dimensions(); }
	uint32_t width() const { return data_->width(); }
	uint32_t height() const { return data_->height(); }


	// ファイルからイメージの生成
	image& operator << (const std::string& fn) {
	  // pngファイル読み込み
	  try {
		// 指定のフォーマットで読む
	    data_ = shared_ptr<PixelT>(new PixelT());
	    png_read_image(fn, *data_);
	  }
	  catch (const std::ios_base::failure& faild) {
		try {
		  // 24bitでリトライ
		  rgb8_image_t s24;
		  png_read_image(fn, s24);
		  data_ = shared_ptr<PixelT> (new PixelT(s24.dimensions()));
		  copy_and_convert_pixels(view(s24), view(*data_));
		}
		catch (const std::ios_base::failure& faild) {
		  data_ = shared_ptr<PixelT>();
		  throw image_exception("image parse error");
		}
	  }
	  return *this;
	}
  };

  typedef image<rgba8_image_t> tex_image_t;


}}} // ts::appfw::gl
#endif

