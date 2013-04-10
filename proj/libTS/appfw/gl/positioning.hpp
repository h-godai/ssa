/* positioning.hpp -*-tab-width:4-*-
   libTS glfw  Graphics Language Frame Work
 */

#ifndef TS_APPGW_GL_POSITIONING_HPP
#define TS_APPGW_GL_POSITIONING_HPP

#include "context.hpp"
#include "libTS/math/pos.hpp"

namespace ts {
namespace appfw {
namespace gl {
  using ts::math::pos;
  using ts::math::pos2;
  using ts::math::pos2_f;



  enum positioning_type {
	position_immidiately // 位置は直接指定
	, fit_to_width
	, fit_to_height
	, stretch_screen
	, align_center
	, align_center_h   // 水平方向に中央、上下の位置は指定
	, align_center_v   // 垂直方向に中央、左右の位置は指定
	, align_left
	, align_top
	, align_right
	, align_bottom
	, fit_to_width_center_v
	, fit_to_height_center_h
	, align_left_top
	, align_right_top
	, align_left_bottom
	, align_center_top
	, align_center_bottom
	, align_right_bottom
	, tiled_to_screen  // タイリングでスクリーンサイズ
  };

  struct h_percent {
	float percent_;
  };


// イメージの位置を決めるためのクラス
  struct positioning {
    rect_f rect_;
    positioning_type postype_;
    float subpos_;
    float margin_;
	size_f fit_;
    positioning() : postype_(position_immidiately) {}
    positioning(const rect_f& rect) : rect_(rect), postype_(position_immidiately) {}
    template <typename T>
    positioning(const pos2<T>& pos) : postype_(position_immidiately){ rect_.pos_ = pos; }
    positioning(const pos2_f& pos, const size_f& size) : rect_(pos, size), postype_(position_immidiately) {}
    positioning(positioning_type st, float subpos=0.0f, float margin=0.0f) : postype_(st), subpos_(subpos), margin_(margin) {}
    positioning(positioning_type st, const size_f& fitSize, float subpos=0.0f, float margin=0.0f)
	  : postype_(st), subpos_(subpos), margin_(margin), fit_(fitSize) {}

    // スクリーンサイズとイメージサイズから位置を決定する
	const rect_f& rect(context& gc, const size_i& imgsz) {
	  return rect(gc.screen_size_, imgsz);
	}
    const rect_f& rect(const size_f& csz, const size_f& imgsz) {
	  if (rect_.size_.empty()) {
		rect_.size_ = imgsz;
	  }
      switch (postype_) {
      case fit_to_width:
		rect_.size_ = size_f(csz.width_, imgsz.height_ * csz.width_/imgsz.width_);
		rect_.pos_.x_ = 0.0f;
		rect_.pos_.y_ = subpos_;
		break;
	  case fit_to_width_center_v:
		rect_.size_ = size_f(csz.width_, imgsz.height_ * csz.width_/imgsz.width_);
		rect_.pos_.x_ = 0.0f;
		rect_.pos_.y_ = margin_ + (csz.height_ - rect_.size_.height_) / 2.0f;
		//TS_DEBUGLOG("fit_to_width_center_r:rect(%f,%f,%f,%f)\n", rect_.pos_.x_,rect_.pos_.y_,rect_.size_.width_,rect_.size_.height_);
		break;
      case fit_to_height:
		rect_.size_ = size_f(imgsz.width_ * csz.height_/imgsz.height_, csz.height_);
		rect_.pos_.x_ = subpos_;
		rect_.pos_.y_ = 0.0f;
		break;
	  case fit_to_height_center_h:
		rect_.size_ = size_f(imgsz.width_ * csz.height_/imgsz.height_, csz.height_);
		rect_.pos_.x_ = margin_ + (csz.width_ - rect_.size_.width_) / 2.0f;
		rect_.pos_.y_ = 0.0f;
		break;
      case stretch_screen:
		rect_.size_ = size_f(csz.width_, csz.height_);
		rect_.pos_.x_ = 0.0f;
		rect_.pos_.y_ = 0.0f;
		break;
	  case tiled_to_screen:
		rect_.size_ = size_f(-csz.width_, -csz.height_);
		rect_.pos_.x_ = 0.0f;
		rect_.pos_.y_ = 0.0f;
		break;

		// 画面中央表示。第2第3パラメータは画像の倍率
      case align_center:
		if (subpos_ != 0.0f) {
		  if (margin_ == 0.0f) margin_ = subpos_;
		  rect_.size_ = size_f(subpos_ * imgsz.width_, margin_ * imgsz.height_);
		}
		else {
          if (fit_.empty()) {
            rect_.size_ = imgsz;
          }
          else {
            if (fit_.width_ < 8.0f && fit_.height_ < 8.0f) {
              rect_.size_ = imgsz * fit_;
            }
            else {
              rect_.size_ = fit_;
            }
          }
		}
		rect_.pos_.x_ = (csz.width_ - rect_.size_.width_) / 2.0f;
		rect_.pos_.y_ = (csz.height_ - rect_.size_.height_) / 2.0f;
		break;
      case align_center_h:
		if (fit_.empty()) fit_ = imgsz;
		rect_.pos_.x_ = (csz.width_ - fit_.width_) / 2.0f;
		rect_.pos_.y_ = subpos_;
		rect_.size_ = fit_;
		break;
      case align_left:
		rect_.pos_.x_ = margin_;
		rect_.pos_.y_ = subpos_;
		break;
      case align_right:
		rect_.pos_.x_ = csz.width_ - imgsz.width_ - margin_;
		rect_.pos_.y_ = subpos_;
		break;
      case align_center_v:
		rect_.pos_.y_ = (csz.height_ - imgsz.height_) / 2.0f;
		rect_.pos_.x_ = subpos_;
		break;
      case align_top:
		rect_.pos_.x_ = subpos_;
		rect_.pos_.y_ = margin_;
		break;
      case align_bottom:
		rect_.pos_.x_ = subpos_;
		rect_.pos_.y_ = csz.height_ - imgsz.height_ - margin_;
		break;
	  case align_left_top:
		rect_.pos_.x_ = margin_;
		rect_.pos_.y_ = margin_;
		break;
	  case align_right_top:
		rect_.pos_.x_ = csz.width_ - imgsz.width_ - margin_;
		rect_.pos_.y_ = margin_;
		break;
	  case align_left_bottom:
		rect_.pos_.x_ = margin_;
		rect_.pos_.y_ = csz.height_ - imgsz.height_ - margin_;
		break;
	  case align_right_bottom:
		rect_.pos_.x_ = csz.width_ - imgsz.width_ - margin_;
		rect_.pos_.y_ = csz.height_ - imgsz.height_ - margin_;
		break;
      case align_center_bottom:
		rect_.pos_.x_ = (csz.width_ - imgsz.width_) / 2.0f;
		rect_.pos_.y_ = csz.height_ - imgsz.height_ - margin_;
		break;
      case align_center_top:
		rect_.pos_.x_ = (csz.width_ - imgsz.width_) / 2.0f;
		rect_.pos_.y_ = 0.0f + margin_;
		break;


	  case position_immidiately:
      default:
		if (rect_.size_.empty()) {
		  rect_.size_ = imgsz;
		}
		break;
      }
      return rect_;
    }
  };

  // positioningのサポートクラス
  struct positioner {
    const size_i& csz_;
    positioning pos_;
	size_i imgsz_;
    positioner(context& gc) : csz_(gc.screen_size_) {}
	positioner(const size_i& csz) : csz_(csz) {}
    positioner(const positioner& p) : csz_(p.csz_), pos_(p.pos_) {}
    positioner(context& gc, const positioning& pos) : csz_(gc.screen_size_), pos_(pos) {}
	positioner(const size_i& csz, const positioning& pos) : csz_(csz), pos_(pos) {}
    rect_f rect(const size_i& imgsz) {
      return pos_.rect(csz_, imgsz);
    }
	pos2_f pos(const size_i& imgsz) {
	  return pos_.rect(csz_, imgsz_).pos_;
	}
    positioner operator () (const rect_f& rect) { return positioner(csz_, positioning(rect)); }
    positioner operator () (const pos2_f& pos) { return positioner(csz_, positioning(pos)); }
    positioner operator () (const pos2_f& pos, const size_f& size) { return positioner(csz_, positioning(pos, size)); }
    positioner operator () (positioning_type st, float subpos=0.0f, float margin=0.0f)
    { return positioner(csz_, positioning(st, subpos, margin)); }
    rect_f operator () (image32& img, positioning_type st, float subpos=0.0f, float margin=0.0f)
    { return positioning(st, subpos, margin).rect(csz_, img.rect().size()); }
    rect_f operator () (const size_f& sz, positioning_type st, float subpos=0.0f, float margin=0.0f)
    { return positioning(st, subpos, margin).rect(csz_, sz); }
    rect_f operator () (const rect_f& rc, positioning_type st, float subpos=0.0f, float margin=0.0f)
    { return positioning(st, subpos, margin).rect(csz_, rc.size()); }

  };




}}} // ts::appfw::gl
#endif

