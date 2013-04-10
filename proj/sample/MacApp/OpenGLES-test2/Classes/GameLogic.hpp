/* -*-tab-width:4-*-
 *  GameLogic.hpp

 */

#ifndef APP_GAME_MAIN_HPP
#define APP_GAME_MAIN_HPP

#include <math.h>
#include <vector>
#include <boost/random.hpp>
#include <boost/optional.hpp>
#include <boost/array.hpp>
#include <algorithm>
#include "libTS/appfw/appfw.hpp"

namespace app {
  using namespace ts::appfw;
  using namespace std;
  using namespace boost::gil;
  using namespace boost;

  // ピクセル変換
  rgba8_pixel_t g(rgba8_pixel_t p) {
	//uint8_t c = std::min(uint32_t(255), static_cast<uint32_t>((at_c<0>(p)+at_c<1>(p)+at_c<2>(p))/3));
	//uint8_t c = std::max(at_c<0>(p), uint8_t(std::max(at_c<1>(p),at_c<2>(p))/3));
	//return rgba8_pixel_t(c,c,c,at_c<3>(p));
	return p;
  }

  class GameLogic {
  public:
	struct ChipObj : gl::rectangle2d {
	  enum EffectType {
		NoEffect
		, TapIn
		, TapOut
		, Moving
	  };

	  uint32_t col_;
	  uint32_t row_;
	  uint32_t tx_;
	  uint32_t ty_;
	  gl::pos2d_f target_;
	  bool update_;
	  ChipObj(uint32_t x, uint32_t y, gl::tex_image_t img, gl::pos2d_f pos, gl::size_f sz)
		: gl::rectangle2d(img, pos, sz)
		, col_(x)
		, row_(y)
		, tx_(x)
		, ty_(y)
		, target_(pos)
		, update_(true)
	  {}

	  const gl::pos2d_f& pos() const { return this->rect_.pos_; }
	  void setPos(const gl::pos2d_f& pos) { this->rect_.pos_ = pos; }
	  void effect(EffectType ef) {
		switch(ef) {
		case TapIn:
		  this->set_effect(gl::ef_highlights);
		  break;
		case TapOut:
		case NoEffect:
		default:
		  this->set_effect(gl::ef_none);
		  break;
		}
		update_ = true;
	  }

	};

	uint32_t rows_;
	uint32_t cols_;
	uint32_t chipW_;
	uint32_t chipH_;
	float texSize_;   // テクスチャのサイズ
	bool redraw_;
	gl::vec2_f drag_;

	gl::show_images_cmd<ChipObj>::ptr obj_;
	boost::optional<ChipObj*> selected_;
	vector<uint32_t> slot_;
	gl::size_i scrSize_;
	gl::size_f padding_;

	GameLogic()
	  : rows_(4)
	  , cols_(4)
	  , redraw_(false)
	  , selected_(boost::none)
	  , padding_(2.0f, 2.0f)
	{}

	gl::pos2d_f getPos(uint32_t col, uint32_t row) {
	  return gl::pos2d_f(col * texSize_, row * texSize_) + padding_;
	}
	gl::pos2d_f correctPos(gl::pos2d_f p) {
	  if (p.x_ < -texSize_/2.0f) p.x_ += cols_ * texSize_;
	  if (p.y_ < -texSize_/2.0f) p.y_ += rows_ * texSize_;
	  if (p.x_ > (cols_-0.5f)*texSize_) p.x_ -= cols_ * texSize_;
	  if (p.y_ > (rows_-0.5f)*texSize_) p.y_ -= rows_ * texSize_;
	  return p;
	}

	void initialize(app_framework& app, gl::glfw& gr) {
	  vector<ChipObj> chips_;
	  gl::tex_image_t src(app.resource_dir() + "/godai.png");
	  // 4x4の16分割にする
	  uint32_t w = src.width() / cols_;
	  uint32_t h = src.height() / rows_;
	  scrSize_ = gr.screen_size();
	  texSize_ = min(1.0f*scrSize_.width_/cols_, 1.0f*scrSize_.height_/rows_);
	  chipH_ = chipW_ = min(scrSize_.width_/cols_, scrSize_.height_/rows_);
	  gl::size_f tsz = gl::size_f(texSize_, texSize_) - (padding_ * 2.0f);


	  for (uint32_t row = 0; row < rows_; ++row) {
		for (uint32_t col= 0; col < cols_; ++col) {
		  // 画像を切り出してimgに入れる
		  gl::tex_image_t img(w,h);
		  transform_pixels(subimage_view(view(src.get()),col*w,row*h, w, h), view(img.get()), g );
		  // 切り出したチップデータを保存
		  chips_.push_back(ChipObj(col
								 , row
								 , img
								 , getPos(col, row)
								 , tsz));
		}
	  }
	  obj_ = gr.show_images(1, chips_);

	  // 乱数で位置を設定
	  slot_.resize(rows_*cols_);
	  for (size_t i = 0; i < rows_*cols_ ; ++i) slot_[i]=i;
	  mt19937 gen( static_cast<unsigned long>(time(0)) );
	  variate_generator< mt19937&, uniform_smallint<> > rand( gen, uniform_smallint<>(0, rows_*cols_-1));
	  for (int n = 0; n < rows_*cols_; ++n) {
		std::swap(slot_[n], slot_[rand()]);
	  }
	  // 配置
	  for (size_t i = 0; i < rows_*cols_ ; ++i) {
		uint32_t col = i % cols_;
		uint32_t row = i / cols_;
		ChipObj& co = obj_->at(slot_[i]);
		co.target_ = getPos(col, row);
		co.tx_ = col;
		co.ty_ = row;
	  }
	}

	void start(ts::appfw::drawframe_app& app) {
	  while(app.is_running()) {
		redraw_ = false;
		app.process_events(); // イベント処理
		for (size_t i = 0; i < obj_->objs().size() ; ++i) {
		  //gl::pos2d_f pos = (obj_->at(i).pos() + obj_->at(i).target_) / 2.0f;
		  gl::vec2_f v(obj_->at(i).pos(), obj_->at(i).target_);
		  if (v) {
			v /= 2.0f;
			if (v.length() > 8.0f) v.set_length(8.0f);
			if (v.length() < 0.1f) {
			  obj_->at(i).setPos(obj_->at(i).target_);
			}
			else{
			  gl::pos2d_f pos = obj_->at(i).pos() + v;
			  obj_->at(i).setPos(pos);
			}
			redraw_ = true;
		  }
		}
		if (redraw_) app.update();
		app.wait_for_frame(); // 画面書き換えを待つ
	  }
	}
	
	uint32_t& slot(uint32_t col, uint32_t row) {
	  assert(row*cols_+col < slot_.size());
	  return slot_[row*cols_+col];
	}

	template <typename PosT>
	boost::optional<ChipObj&> getChip(const PosT& pos) {
	  assert(obj_);
	  BOOST_FOREACH(ChipObj& chip, obj_->objs()) {
		//printf("is_inside?(%f,%f)\n", chip.rect_.left(), chip.rect_.top());
		if (chip.rect_.is_inside(pos)) {
		  return chip;
		}
	  }
	  return boost::none;
	}

	void onTapIn(in::tap_event::ptr evt) {
	  printf("onTapIn(%f,%f)\n", evt->ctrl_.pos_.x_, evt->ctrl_.pos_.y_);
	  drag_.clear();
	  if (boost::optional<ChipObj&> chip = getChip(evt->ctrl_.pos_)) {
		chip->effect(ChipObj::TapIn);
		selected_ = &chip.get();
		redraw_ = true;
		return;
	  }
	  printf("not in chips\n");
	}
	void onTapMove(in::tap_event::ptr old, in::tap_event::ptr now) {
	  if (!selected_) return;
	  bool movex = false;
	  if (drag_) {
		movex = fabs(drag_.x_) > fabs(drag_.y_);
		drag_ = gl::vec2_f(gl::pos2d_f(old->ctrl_.pos_), gl::pos2d_f(now->ctrl_.pos_));
	  }
	  else {
		drag_ = gl::vec2_f(gl::pos2d_f(old->ctrl_.pos_), gl::pos2d_f(now->ctrl_.pos_));
		movex = fabs(drag_.x_) > fabs(drag_.y_);
	  }
	  if (movex) {
		// 横方向に移動
		uint32_t row = selected_.get()->ty_;
		drag_.y_ = 0;
		for (uint32_t x = 0; x < cols_; ++x) {
		  ChipObj& c = obj_->at(slot(x, row));
		  c.target_ = correctPos(getPos(x, row) + drag_);
		  c.setPos(c.target_);
			  
		}
	  }
	  else {
		// 縦方向に移動
		uint32_t col = selected_.get()->tx_;
		drag_.x_ = 0;
		for (uint32_t y = 0; y < rows_; ++y) {
		  ChipObj& c = obj_->at(slot(col, y));
		  c.target_ = correctPos(getPos(col, y) + drag_);
		  c.setPos(c.target_);

		}
	  }
	  redraw_ = true;
	}
	void rotate_col(uint32_t row, bool right) {
	  if (right) {
		uint32_t z = slot(cols_-1,row);
		for (uint32_t x = cols_ - 1; x > 0; --x) {
		  slot(x, row) = slot(x - 1, row);
		}
		slot(0, row) = z;
	  }
	  else {
		uint32_t z = slot(0, row);
		for (uint32_t x = 0; x < cols_-1; ++x) {
		  slot(x, row) = slot(x + 1, row);
		}
		slot(cols_ -1, row) = z;
	  }
	}
	void rotate_row(uint32_t col, bool bottom) {
	  if (bottom) {
		uint32_t z = slot(col, rows_-1);
		for (uint32_t y = rows_ - 1; y > 0; --y) {
		  slot(col, y) = slot(col, y - 1);
		}
		slot(col, 0) = z;
	  }
	  else {
		uint32_t z = slot(col, 0);
		for (uint32_t y = 0; y < rows_-1; ++y) {
		  slot(col, y) = slot(col, y + 1);
		}
		slot(col, rows_ -1) = z;
	  }
	}
	
	// タップ終了
	void onTapOut(in::tap_event::ptr evt) {
	  if (selected_) {
		// 移動中だったらFIXさせる
		selected_.get()->effect(ChipObj::NoEffect);
		float ax = fabs(drag_.x_);
		float ay = fabs(drag_.y_);
		if (ax > ay) {
		  uint32_t row = selected_.get()->ty_;
		  if (ax > texSize_/2.0f) {
			for (uint32_t n = 0; n < uint32_t(ax/texSize_+0.5f); ++n) 
			  rotate_col(row, drag_.x_ > 0);
		  }
		  for (uint32_t x = 0; x < cols_; ++x) {
			ChipObj& c = obj_->at(slot(x, row));
			c.target_ = getPos(x, row);
			c.tx_ = x;
			c.ty_ = row;
		  }
		}
		else {
		  uint32_t col = selected_.get()->tx_;
		  if (ay > texSize_/2.0f) {
			for (uint32_t n = 0; n < uint32_t(ay/texSize_+0.5f); ++n) 
			rotate_row(col, drag_.y_ > 0);
		  }
		  for (uint32_t y = 0; y < rows_; ++y) {
			ChipObj& c = obj_->at(slot(col, y));
			c.target_ = getPos(col, y);
			c.tx_ = col;
			c.ty_ = y;
		  }
		}
		selected_ = boost::none;
		redraw_ = true;
	  }
	}

  };









}


#endif


