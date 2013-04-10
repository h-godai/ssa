// IBitmapImage.hpp




#ifndef TSL_PLATFORMDEPENDENT_IBITMAPIMAGE_HPP
#define TSL_PLATFORMDEPENDENT_IBITMAPIMAGE_HPP

#ifndef STDAFX_H
#include <vector>
#include <boost/shared_ptr.hpp>
#endif

#include "Math/Rect.h"
#include "Graphics/GLCommon.h"
#include "Util/bstream.hpp"

namespace ts {
  using std::vector;
  using boost::shared_ptr;

  class ImageBuffer {

  public:
    typedef shared_ptr<ImageBuffer> ptr;
    enum PixelFormat {
      GRAY8,
      GRAY16,
      GRAY32,
      R8G8B8,
      A8R8G8B8,
      OTHER
    };
    ImageBuffer() 
      : width_(0)
      , height_(0)
      , padding_(1)
    {
    }
    virtual ~ImageBuffer() {}

    // イメージバッファオブジェクトの生成
    static ptr CreateImageBuffer(PixelFormat fmt, uint32_t width=0, uint32_t height=0);

    // getter/setter
    int32_t getWidth() const { return width_; }
    int32_t getHeight() const { return height_; }
    int32_t getPadding() const { return padding_; }
    virtual void setWidth(int width) { width_ = width; }
    virtual void setHeight(int height) { height_ = height; }
    virtual void setPadding(int padding) { padding_ = padding; }

    virtual void clear(const ColorRGBA& col = ColorRGBA(0,0,0,0)) = 0;

    // 1ピクセルあたりのバイト数を返す
    virtual int getPixelBytes() const = 0;
    // ピクセルフォーマットを返す
    virtual PixelFormat getPixelFormat() const = 0;
    // バッファを作成する
    virtual bool createBuffer(int width, int height) = 0;

    virtual uint32_t getPixelF(int x, int y)  const = 0;
    virtual uint32_t getPixel(int x, int y)  const = 0;
    virtual void setPixelF(int x, int y, uint32_t) = 0;
    virtual void setPixel(int x, int y, uint32_t) = 0;

    virtual void resize(int width, int height) = 0;
    // src:(srcX,srcY,srcWidth,srcHeight)を(dstX,dstY)にコピーする
    void bitblt(int32_t dstX, int32_t dstY, ImageBuffer* src, int32_t srcX, int32_t srcY, int32_t srcWidth, int32_t srcHeight) {
      bitblt(dstX, dstY, src, RectS32(srcX, srcY, srcWidth, srcHeight));
    }
    void bitblt(int32_t x, int32_t y, ImageBuffer* src) {
      bitblt(x, y, src, 0, 0, src->width_, src->height_);
    }
    virtual void bitblt(int32_t dstX, int32_t dstY, ImageBuffer* src, const RectS32& srcRect) {
      if (dstX + srcRect.width > getWidth() || dstY + srcRect.height > getHeight()
        || src->getWidth() < srcRect.right() 
        || src->getHeight() < srcRect.bottom()
        || srcRect.left < 0 || srcRect.top < 0 || dstX < 0 || dstY < 0
      )
      {
        throw std::out_of_range("ImageBuffer::bitblt");
      }
      for (int32_t y = 0; y < srcRect.height; ++y) {
        for (int32_t x = 0; x < srcRect.width; ++x) {
          setPixelF(dstX + x, dstY + y, src->getPixelF(x + srcRect.left, y + srcRect.top));
        }
      }
    }


    // バッファのアドレスを返す
    template <typename BufT>
    BufT getBuffer(int x = 0, int y = 0) {
      return static_cast<BufT>(getBufferAddress(x, y));
    }

    // すべてのピクセルに対する操作
    template <typename Pred>
    void enumPixel(Pred pred) {
      for (int32_t y = 0; y < height_; ++y) {
        for (int32_t x = 0; x < width_; ++x) {
          pred(getBuffer<uint8_t*>(x,y));
        }
      }
    }




  private:
    virtual void* getBufferAddress(int x = 0, int y = 0) = 0;


  protected:
    int32_t width_;
    int32_t height_;
    uint32_t padding_;
  };

  // R8 G8 B8 のビットマップ
  class ImageBufferRGB : public ImageBuffer {
  public:
    typedef boost::shared_ptr<ImageBufferRGB> ptr;

    ImageBufferRGB(int width = 0, int height = 0) {
      if (width != 0 && height != 0) {
        createBuffer(width, height);
      }
      padding_ = 1;
    }
    virtual ~ImageBufferRGB() {}

    // 1ピクセルあたりのバイト数を返す
    virtual int getPixelBytes() const { return 3; }
    // ピクセルフォーマットを返す
    virtual PixelFormat getPixelFormat() const { return R8G8B8; }
    // バッファを作成する
    bool createBuffer(int width, int height) {
      width_ = width;
      height_ = height;
      try {
        buffer_.resize(width*height*getPixelBytes());
      }
      catch (const std::exception&) {
        return false;
      }
      return true;
    }

    virtual void clear(const ColorRGBA& col = ColorRGBA(0,0,0,0)) {
      for (size_t n = 0; n < buffer_.size(); n += 3) {
        buffer_[n] = col.r;
        buffer_[n+1] = col.g;
        buffer_[n+2] = col.b;
      }
    }


    void setPixelF(int x, int y, uint32_t rgb) {
      uint8_t* ptr = &buffer_[(x + (y * width_)) * 3];
      *ptr = static_cast<uint8_t>((rgb >> 16) & 0xff);
      *++ptr = static_cast<uint8_t>((rgb >> 8) & 0xff);
      *++ptr = static_cast<uint8_t>(rgb & 0xff);
    }
    virtual void setPixelF(int x, int y, uint8_t r, uint8_t g, uint8_t b) {
      uint8_t* ptr = &buffer_[(x + (y * width_)) * 3];
      *ptr = r;
      *++ptr = g;
      *++ptr = b;
    }
    virtual uint32_t getPixelF(int x, int y)  const {
      const uint8_t* ptr = &buffer_[(x + (y * width_)) * 3];
      return (*ptr << 16) + (*(ptr+1) << 8) + *(ptr+2);
    }
    void setPixel(int x, int y, uint32_t rgb) {
      uint8_t* ptr = &buffer_[(x + (y * width_)) * 3];
      *ptr = static_cast<uint8_t>((rgb >> 16) & 0xff);
      *++ptr = static_cast<uint8_t>((rgb >> 8) & 0xff);
      *++ptr = static_cast<uint8_t>(rgb & 0xff);
    }
    virtual void setPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b) {
      uint8_t* ptr = &buffer_[(x + (y * width_)) * 3];
      *ptr = r;
      *++ptr = g;
      *++ptr = b;
    }
    virtual uint32_t getPixel(int x, int y)  const {
      const uint8_t* ptr = &buffer_[(x + (y * width_)) * 3];
      return (*ptr << 16) + (*(ptr+1) << 8) + *(ptr+2);
    }

    // イメージをnewWidth x newHeightサイズに拡大・縮小する
    virtual void resize(int newWidth, int newHeight) {
      vector<uint8_t> newBuffer(newWidth * newHeight * 3);
      resize(newWidth, newHeight, newBuffer, *this);
      buffer_.swap(newBuffer);
      setWidth(newWidth);
      setHeight(newHeight);
    }

    // イメージをnewWidth x newHeightサイズに拡大・縮小してnewImageに格納する
    virtual void copyFrom(const ImageBufferRGB& from) {
      resize(width_, height_, buffer_, from);
    }


    void resize(int newWidth, int newHeight, vector<uint8_t>& newBuffer, const ImageBufferRGB& from) {
      for (int y = 0; y < newHeight; ++y) {
        for (int x = 0; x < newWidth; ++x) {
          uint8_t* ptr = &newBuffer[(x+(y*newWidth))*3];
          uint32_t rgb = from.getPixelF(x * from.width_  / newWidth , y * from.height_  / newHeight );
          *ptr = static_cast<uint8_t>((rgb >> 16) & 0xff);
          *++ptr = static_cast<uint8_t>((rgb >> 8) & 0xff);
          *++ptr = static_cast<uint8_t>(rgb & 0xff);
        }
      }
    }


  private:
    virtual void* getBufferAddress(int x = 0, int y = 0) {
      if (buffer_.empty()) return 0;
      return static_cast<void*>(&buffer_[(y * width_ + x)*3]);
    }
    vector<uint8_t> buffer_;
  };

  // 8bit GrayScaleのビットマップ
  class ImageBufferG8 : public ImageBuffer {
  public:
    typedef boost::shared_ptr<ImageBufferG8> ptr;
    ImageBufferG8(int width = 0, int height = 0) {
      if (width != 0 && height != 0) {
        createBuffer(width, height);
      }
      padding_ = 1;
    }
    virtual ~ImageBufferG8() {}
    // 1ピクセルあたりのバイト数を返す
    virtual int getPixelBytes() const { return 1; }
    // ピクセルフォーマットを返す
    virtual PixelFormat getPixelFormat() const { return GRAY8; }

    virtual uint32_t getPixelF(int x, int y)  const {
      return buffer_[x + (y * width_)];
    }
    virtual uint32_t getPixel(int x, int y)  const {
      return buffer_.at(x + (y * width_));
    }
    virtual void setPixelF(int x, int y, uint32_t c) {
      buffer_[x + y * width_] = static_cast<uint8_t>(c);
    }
    virtual void setPixel(int x, int y, uint32_t c) {
      buffer_.at(x + y * width_) = static_cast<uint8_t>(c);
    }

    virtual void clear(const ColorRGBA& col = ColorRGBA(0,0,0,0)) {
      for (size_t n = 0; n < buffer_.size(); ++n) {
        buffer_[n] = static_cast<uint8_t>(col.r | col.b | col.g);
      }
    }


    // イメージをnewWidth x newHeightサイズに拡大・縮小する
    virtual void resize(int newWidth, int newHeight) {
      vector<uint8_t> newBuffer(newWidth * newHeight);
      resize(newWidth, newHeight, newBuffer, *this);
      buffer_.swap(newBuffer);
      setWidth(newWidth);
      setHeight(newHeight);
    }

    // イメージをnewWidth x newHeightサイズに拡大・縮小してnewImageに格納する
    virtual void copyFrom(const ImageBufferG8& from) {
      resize(width_, height_, buffer_, from);
    }


    void resize(int newWidth, int newHeight, vector<uint8_t>& newBuffer, const ImageBufferG8& from) {
      for (int y = 0; y < newHeight; ++y) {
        for (int x = 0; x < newWidth; ++x) {
          newBuffer[x+(y*newWidth)] = static_cast<uint8_t>(from.getPixelF(x * from.width_  / newWidth , y * from.height_  / newHeight ));
        }
      }
    }

  protected:
      // バッファを作成する
     virtual bool createBuffer(int width, int height) {
      width_ = width;
      height_ = height;
      try {
        buffer_.resize(width*height*getPixelBytes());
      }
      catch (const std::exception&) {
        return false;
      }
      return true;
    }
  private:
    virtual void* getBufferAddress(int x = 0, int y = 0) {
      if (buffer_.empty()) return 0;
      return static_cast<void*>(&buffer_[(y * width_ + x)*3]);
    }
    vector<uint8_t> buffer_;

  };



 // A8 R8 G8 B8 のビットマップ
  class ImageBufferARGB : public ImageBuffer {
  public:
    typedef boost::shared_ptr<ImageBufferARGB> ptr;

    ImageBufferARGB(int width = 0, int height = 0) {
      if (width != 0 && height != 0) {
        createBuffer(width, height);
      }
      padding_ = 1;
    }
    virtual ~ImageBufferARGB() {}

    // 1ピクセルあたりのバイト数を返す
    virtual int getPixelBytes() const { return 4; }
    // ピクセルフォーマットを返す
    virtual PixelFormat getPixelFormat() const { return A8R8G8B8; }
    // バッファを作成する
    bool createBuffer(int width, int height) {
      width_ = width;
      height_ = height;
      try {
        buffer_.resize(width*height);
      }
      catch (const std::exception&) {
        return false;
      }
      return true;
    }

    virtual void clear(const ColorRGBA& col = ColorRGBA(0,0,0,0)) {
      for (size_t n = 0; n < buffer_.size(); ++n) {
        buffer_[n] = col.rgba;
      }
    }

    void setPixelF(int x, int y, uint32_t rgba) {
      buffer_[x + (y * width_)] = rgba;
    }
    virtual void setPixelF(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
      buffer_[x + (y * width_)] = (a << 24) + (r << 16) + (g << 8) + (b);
    }
    virtual uint32_t getPixelF(int x, int y)  const {
      return buffer_[x + (y * width_)];
    }
    void setPixel(int x, int y, uint32_t rgba) {
      buffer_.at(x + (y * width_)) = rgba;
    }
    virtual void setPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
      buffer_.at(x + (y * width_)) = (a << 24) + (r << 16) + (g << 8) + (b);
    }
    virtual uint32_t getPixel(int x, int y)  const {
      return buffer_.at(x + (y * width_));
    }
    static uint8_t getA(uint32_t argb) { return static_cast<uint8_t>((argb >> 24) & 0xff); }
    static uint8_t getR(uint32_t argb) { return static_cast<uint8_t>((argb >> 16) & 0xff); }
    static uint8_t getG(uint32_t argb) { return static_cast<uint8_t>((argb >> 8) & 0xff); }
    static uint8_t getB(uint32_t argb) { return static_cast<uint8_t>(argb & 0xff); }

    // uint8_t*からARGBを得る。ENDIAN依存
    static uint8_t& getA(uint8_t* argb) { return *(argb + 3); }
    static uint8_t& getR(uint8_t* argb) { return *(argb + 0); }
    static uint8_t& getG(uint8_t* argb) { return *(argb + 1); }
    static uint8_t& getB(uint8_t* argb) { return *(argb + 2); }

    // イメージをnewWidth x newHeightサイズに拡大・縮小する
    virtual void resize(int newWidth, int newHeight) {
      vector<uint32_t> newBuffer(newWidth * newHeight);
      resize(newWidth, newHeight, newBuffer, *this);
      buffer_.swap(newBuffer);
      setWidth(newWidth);
      setHeight(newHeight);
    }

    // イメージをnewWidth x newHeightサイズに拡大・縮小してnewImageに格納する
    virtual void copyFrom(const ImageBufferARGB& from) {
      resize(width_, height_, buffer_, from);
    }


    void resize(int newWidth, int newHeight, vector<uint32_t>& newBuffer, const ImageBufferARGB& from) {
        if (from.width_ == 0 || from.height_ == 0) { throw std::invalid_argument("ImageBuffer::resize: width or height is zero"); }
      for (int y = 0; y < newHeight; ++y) {
        for (int x = 0; x < newWidth; ++x) {
          newBuffer[x + (y * newWidth)] = from.getPixelF(x * from.width_  / newWidth , y * from.height_  / newHeight );
        }
      }
    }


  private:
    virtual void* getBufferAddress(int x = 0, int y = 0) {
      if (buffer_.empty()) return 0;
      return static_cast<void*>(&buffer_[y * width_ + x]);
    }
    vector<uint32_t> buffer_;
  };
} // namespace

#endif











