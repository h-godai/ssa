/* image.hpp -*-tab-width:4-*-
   libTS appfw  Graphics Language Frame Work
 */

#ifndef TS_APPFW_GL_IMAGE_PP
#define TS_APPFW_GL_IMAGE_PP

#define png_infopp_NULL (png_infopp)NULL
#define int_p_NULL (int*)NULL
#define BOOST_GIL_USE_CONCEPT_CHECK 1
#include <boost/gil/gil_concept.hpp>
#include <boost/gil/gil_all.hpp>
#include <boost/gil/extension/io/png_dynamic_io.hpp>
#include <boost/thread.hpp>

#include "common.hpp"
#include "libTS/Util/policy.hpp"
#include "libTS/math/geometric.hpp"


namespace ts {
namespace appfw {
namespace gl {
  using ts::math::pos2_f;
  using ts::math::rect_f;
  using ts::math::size_f;
  using boost::gil::rgba8_image_t;
  using boost::gil::rgb8_image_t;
  using boost::gil::view;


  struct image_exception : glfw_exception {
	image_exception(const char * const & msg) : glfw_exception(msg) {}
	virtual ~image_exception() throw() {}
  };

  // 使用済みのテクスチャを解放するためのリスト
  struct release_texture_handles : singleton_policy<release_texture_handles> {
	typedef uint32_t handle_t;
	boost::mutex guard_;
	std::vector<handle_t> handles_;
	size_t count() const { return handles_.size(); }
	void add(handle_t h) {
	  scoped_lock lock(guard_);
	  handles_.push_back(h);
	}
	void clear() { handles_.clear(); }
	void lock() { guard_.lock(); }
	void unlock() { guard_.unlock(); }

	std::vector<handle_t>& get_list() { return  handles_; }
  };




  template <typename PixelT>
  struct image {
	static const uint32_t invalid_handle = 0xffffffff;
	struct pixeldata {
	  PixelT pixel_;
	  uint32_t handle_;
	  boost::mutex guard_;
	  pixeldata() : handle_(invalid_handle) {}
	  template <typename T>
	  pixeldata(T& arg) : pixel_(arg), handle_(invalid_handle) {}
	  pixeldata(uint32_t w, uint32_t h) : pixel_(w,h), handle_(invalid_handle) {}
	  pixeldata(PixelT& pix, uint32_t ha=invalid_handle) : pixel_(pix), handle_(ha) {}
	  ~pixeldata() {
		TS_DEBUGLOG("image::pixeldata will release texture handle:%d\n", handle_);
		release_texture_handles::Instance().add(handle_);
	  }

	  uint32_t width() const { return pixel_.width(); }
	  uint32_t height() const { return pixel_.height(); }
	  const typename PixelT::point_t& dimensions() const { return pixel_.dimensions(); }
	  void lock() { guard_.lock(); }
	  void unlock() { guard_.unlock(); }
	};
	shared_ptr<pixeldata> data_;
	std::string name_;
	rect_f rect_; // 画像データに置ける使用する矩形
	float scale_;
	image() : scale_(1.0f) {}
	// copy constructor 
	image(const image& img) : data_(img.data_), name_(img.name_), rect_(img.rect_), scale_(img.scale_) {
	  if (rect_.size_.empty()) {
		rect_.size_ = img.original_size();
	  }
	}
	image(const image& img, const rect_f& r, float scale=1.0f) : data_(img.data_), name_(img.name_), rect_(r), scale_(scale) {
	  if (rect_.size_.empty()) {
		rect_.size_ = img.original_size();
	  }
	}
	image(const char* fn, const rect_f& r=rect_f(), float scale=1.0f) : rect_(r), scale_(scale) {
	  name_ = fn;
	  *this << name_;
	}
	image(const std::string& fn, float scale=1.0f) : scale_(scale){
	  name_ = fn;
	  *this << fn;
	}
	image(size_t w, size_t h) : data_(new pixeldata(w,h)), scale_(1.0f) {}

	void swap(image& img) throw() {
	  std::swap(data_, img.data_);
	  std::swap(name_, img.name_);
	  rect_.swap(img.rect_);
	  std::swap(scale_, img.scale_);
	}

	image& operator = (image img) throw() {
	  swap(img);
	  return *this;
	}

	void clear() throw() {
	  swap(image());
	}

	bool empty() const { return rect_.size_.empty(); }
	bool has_image() const { return data_; }
	operator bool () const { return data_; }

	void lock() { if (data_) data_->lock(); }
	void unlock() { if (data_) data_->unlock(); }

	const std::string& name() const { return name_; }
	template <typename T>
	void set_name(T name) { name_ = name; }


	uint32_t handle() throw() { return data_ ? data_->handle_ : invalid_handle; }
	void set_handle(uint32_t h) throw() { if (data_) data_->handle_ = h; }

	operator PixelT& () { return data_->pixel_; }
	PixelT& get() { return data_->pixel_; }

	const typename PixelT::point_t& dimensions() const { return data_->dimensions(); }
	uint32_t width() const { return rect_.size_.width_; }
	uint32_t height() const { return rect_.size_.height_; }
	ts::math::size_i original_size() const { return data_ ? ts::math::size_i(data_->width(),data_->height()) : ts::math::size_i(0,0); }
	const rect_f& rect() const { return rect_; }
	ts::math::size_f viewsize() const { return size_f(rect_.size_.width_*scale_, rect_.size_.height_*scale_); } // 表示用のサイズ


	// 表示する矩形のUV座標を得る
	rect_f uvrect() const {
	  if (!data_) return rect_f();
	  return rect_ * pos2_f(1.0f / data_->width(), 1.0f / data_->height());
	}
	void set_uvrect(const rect_f& uvrect) {
	  assert(data_);
	  rect_ = uvrect * pos2_f(data_->width(), data_->height());
	}

	// ファイルからイメージの生成
	image& operator << (const std::string& fn) {
	  // pngファイル読み込み
	  try {
		// 指定のフォーマットで読む
	    data_ = shared_ptr<pixeldata>(new pixeldata());
	    png_read_image(fn, data_->pixel_);
		//png_read_and_convert_image(fn, data_->pixel_);
	  }
	  catch (const std::ios_base::failure& faild) {
		try {
		  // 24bitでリトライ
#if 0
		  typedef boost::mpl::vector<rgba8_image_t, rgb8_image_t> myTypes;
		  boost::gil::any_image<myTypes> any32;
		  png_read_image(fn, any32);
		  data_ = shared_ptr<pixeldata> (new pixeldata(any32.dimensions()));
		  copy_and_convert_pixels(view(any32), view(data_->pixel_));
#else
		  rgb8_image_t s24;
		  png_read_image(fn, s24);
		  //png_read_and_convert_image(fn, s24);

		  data_ = shared_ptr<pixeldata> (new pixeldata(s24.dimensions()));
		  copy_and_convert_pixels(view(s24), view(data_->pixel_));
#endif
		  TS_DEBUGLOG("image is 24bit\n");
		}
		catch (const std::ios_base::failure& faild) {
		  data_ = shared_ptr<pixeldata>();
		  TS_WARNINGLOG("image parse error:%s\n", fn.c_str());
		  throw image_exception((std::string("image parse error:")+fn).c_str());
		}
	  }
	  if (rect_.size_.empty()) {
		rect_.size_ = size_f(data_->width(), data_->height());
	  }
	  return *this;
	}
  };

  typedef image<rgba8_image_t> tex_image_t;
  typedef image<rgba8_image_t> image32;


}}} // ts::appfw::gl
#endif

