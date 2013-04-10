/** @file   GameLogic.hpp                    -*-tab-width:4-*-
 *
    @brief  RotatePuzzleゲームロジック部
    @author h.godai godai@techarts.co.jp

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
#include "libTS/appfw/gadgets/slide_switch.hpp"
#include "ChipObj.hpp"
#include "GameResource.hpp"

namespace app {
  using namespace ts::appfw;
  using namespace ts::appfw::gl;
  using namespace std;
  using namespace boost::gil;
  using namespace boost;
  using ::size_t;

  // ゲームロジッククラス
  class GameLogic {
	gl::show_images_cmd<ChipObj>::ptr obj_;
	boost::optional<ChipObj*> selected_;
  public:
	drawframe_app& app_;
	glfw& gr_;
	GameResource& res_;
	uint32_t rows_;
	uint32_t cols_;
	float texSize_;   // テクスチャのサイズ
	bool redraw_;
	gl::vec2_f drag_;
	gl::vec2_f rotate_;

	vector<uint32_t> slot_;
	gl::size_i scrSize_;
	gl::size_f padding_;
	bool canChipMove_;
	gl::rect_f chipRect_;
	float moveSpeed_;
	uint32_t lastMoveTime_;
	bool rotateMode_;
	pos2_f lastPos_;
	vec2_f moved_; // 移動量
	image32 gadget_;
	slide_switch giveupSw_;
	float pastTime_;


	GameLogic(drawframe_app& app, GameResource& res, uint32_t level=3)
	  : app_(app)
	  , gr_(app.gr_)
	  , res_(res)
	  , rows_(level)
	  , cols_(level)
	  , redraw_(false)
	  , selected_(boost::none)
	  , padding_(1.0f, 1.0f)
	  , canChipMove_(false)
	  , moveSpeed_(8.0f)
	  , lastMoveTime_(0)
	  , rotateMode_(false)
	  , giveupSw_(app.gr_, "giveupsw")
	{}

	pos2_f getPos(const pos2_ui& p) { return getPos(p.x_, p.y_); }
	pos2_f getPos(uint32_t col, uint32_t row) {
	  return pos2_f(col * texSize_, row * texSize_); // + padding_;
	}

	pos2_f correctPos(pos2_f p) {
	  if (p.x_ < -texSize_/2.0f) p.x_ += cols_ * texSize_;
	  if (p.y_ < -texSize_/2.0f) p.y_ += rows_ * texSize_;
	  if (p.x_ > (cols_-0.5f)*texSize_) p.x_ -= cols_ * texSize_;
	  if (p.y_ > (rows_-0.5f)*texSize_) p.y_ -= rows_ * texSize_;
	  return p;
	}

	void initialize(uint32_t level, ImageFileType puzzlePic_) {
	  vector<ChipObj> chips;
	  gl::tex_image_t src(res_[puzzlePic_]);
	  rows_ = level;
	  cols_ = level;
	  // cols_ x rows_に分割する
	  uint32_t w = src.width() / cols_;
	  uint32_t h = src.height() / rows_;
	  scrSize_ = app_.gr_.screen_size();
	  texSize_ = min(1.0f*scrSize_.width_/cols_, 1.0f*scrSize_.height_/rows_);
	  chipRect_.pos_.x_ = 0.0f;
	  chipRect_.pos_.y_ = 0.0f;
	  chipRect_.size_.width_ = float(texSize_*cols_);
	  chipRect_.size_.height_ = float(texSize_*rows_);

	  if (obj_) {
		app_.gr_.remove_object(1);
	  }
	  gl::size_f tsz = gl::size_f(texSize_, texSize_);// - (padding_ * 2.0f);
	  for (uint32_t row = 0; row < rows_; ++row) {
		for (uint32_t col= 0; col < cols_; ++col) {
		  // 画像を切り出してimgに入れる
		  //gl::tex_image_t img(w,h);
		  // transform_pixels(subimage_view(view(src.get()),col*w,row*h, w, h), view(img.get()), g );

		  // 元画像の一部の矩形をアタッチする
		  rect_f clop(col * src.width() / cols_, row * src.height() / rows_, w, h);
		  gl::tex_image_t img(src, clop);

		  // 切り出したチップデータを保存
		  chips.push_back(ChipObj(col
								, row
								, img
								, getPos(col, row)
								, tsz
							    , padding_));
		}
	  }
	  obj_ = app_.gr_.show_images("Chips", chips);

	  // 乱数で位置を設定
	  slot_.resize(rows_*cols_);
	  for (size_t i = 0; i < rows_*cols_ ; ++i) slot_[i]=i;
	  mt19937 gen( static_cast<unsigned long>(time(0)) );
	  variate_generator< mt19937&, uniform_smallint<> > rand_r( gen, uniform_smallint<>(0, 2*rows_-1));
	  variate_generator< mt19937&, uniform_smallint<> > rand_c( gen, uniform_smallint<>(0, 2*cols_-1));
	  for (int n = 0; n < rows_*cols_; ++n) {
		uint32_t r =rand_r();
		uint32_t c =rand_c();
		rotate_col(r >> 1, (r&1) != 0);
		rotate_row(c >> 1, (c&1) != 0);
	  }
	  // 配置
	  for (size_t i = 0; i < rows_*cols_ ; ++i) {
		uint32_t col = i % cols_;
		uint32_t row = i / cols_;
		ChipObj& co = obj_->at(slot_[i]);
		co.target_ = getPos(col, row);
		co.slt_.x_ = col;
		co.slt_.y_ = row;
	  }
	}




	// ゲーム開始
	bool main() {
	  canChipMove_ = true;
	  pastTime_ = -1.0f;
	  float hsz = (gr_.screen_size().height_ - gr_.screen_size().width_);
	  selected_ = boost::none;
	  pos2_f timepos((gr_.screen_size().width_ - (64.0f*3.0f+64.0f*2.0f*1.4f))/2.0f
					 , gr_.screen_size().width_  + hsz * 0.2f);
	  float giveupscale = 1.5f;
	  giveupSw_.set_scale(giveupscale);
	  pos2_f giveuppos((gr_.screen_size().width_ - (64.0f*4.0f*giveupscale))/2.0f
					   , gr_.screen_size().height_ - 64.0f*giveupscale-12.0f);

	  giveupSw_.show(giveuppos
					 ,image32(gadget_, GiveUp1Rect, CharScale)
					 ,image32(gadget_, GiveUp2Rect, CharScale)
					 ,image32(gadget_, GiveArrowRect, CharScale)
					 , MenuLayer
					 );

	  bool giveup = false;

	  // BGM開始
	  app_.music_.start(GameMusic1, sounds::music_player::repeat);

	  while(app_.is_running()) {
		pastTime_ += 1.0f/60.0f;
		printTime("time", timepos, pastTime_, scale_to(1.4f,1.4f,1.4f));
		if (giveupSw_.is_finished()) {
		  giveup = true;
		  break;
		}


		redraw_ = false;
		app_.wait_for_frame(); // 画面書き換えを待つ
		if (rotateMode_) {
		  rotateAnimation();
		}
		if (!moveChips()) { // チップ移動アニメーション
		  // 静止状態
		  if (checkComplete()) {
			canChipMove_ = false;
			break;
		  }
		}
		if (redraw_) app_.update();
	  }
	  // ゲーム終了処理
	  giveupSw_.hide();
	  app_.music_.stop();

	  if (giveup) {
		hide_all();
		return false;
	  }
	  else {
		// クリア
		BOOST_FOREACH(ChipObj& chip, obj_->objs()) {
		  chip.effect(ChipObj::TapIn);
		}
		app_.music_.start(CompletedSound, sounds::music_player::no_repeat);
		app_.update();
		app_.wait_for_sec(1.0f);
		BOOST_FOREACH(ChipObj& chip, obj_->objs()) {
		  chip.effect(ChipObj::NoEffect);
		}
		return true; // clear
	  }
	}

	void hide_all() {
	  app_.gr_.hide_image("Chips");
	  gr_.hide_image("time");
	}

	////////////////////////////////////////
	// イベントハンドラ

	// 加速度
	void onAccEvent(in::acc_event::ptr evt) {
	  if (!obj_) return;
	  static bool first = true;
	  static vec3_f o;
	  o = evt->ctrl_.acc_ - o;
	  if (o.length() > 0.8f && !first) {
		BOOST_FOREACH(ChipObj& chip, obj_->objs()) {
		  pos2_f pos = chip.pos();
		  pos.x_ += 50.0f * evt->ctrl_.acc_.x_ + ((rand() & 0xf)-8);
		  pos.y_ -= 50.0f * evt->ctrl_.acc_.y_ + ((rand() & 0xf)-8);
		  chip.setPos(pos, chipRect_);
		}
		moveSpeed_ = 2.0f;
	  }
	  o = evt->ctrl_.acc_;
	  first = false;
	}


	// タップイン
	void onTapIn(in::tap_event::ptr evt) {
	  if (giveupSw_.on_tap_in(evt->ctrl_.pos_)) return;

	  if (rotateMode_) fixRotation();
	  moveSpeed_ = 8.0f;
	  drag_.clear();
	  rotateMode_ = false;
	  if (!canChipMove_) return;
	  if (boost::optional<ChipObj&> chip = getChip(evt->ctrl_.pos_)) {
		if (selected_) selected_.get()->effect(ChipObj::NoEffect);
		chip->effect(ChipObj::TapIn);
		selected_ = &chip.get();
		redraw_ = true;
		app_.sound_.play(TapSound1);
		return;
	  }
	  printf("not in chips\n");
	}

	// ドラッグ
	void onTapMove(in::tap_event::ptr old, in::tap_event::ptr now) {
	  if (!old || !now) return;
	  onTapMove(old->ctrl_.pos_, now->ctrl_.pos_);
	}
	void onTapMove(const pos2_f& stpos, const pos2_f& now) {
	  if (giveupSw_.on_tap_move(now)) return;

	  if (!selected_) return;
	  if (!canChipMove_) return;
	  if (!selected_) return;
	  bool movex = false;

	  moved_ = vec2_f(lastPos_, now);
	  lastPos_ = now;
	  TS_DEBUGLOG("move(%.2f,%.2f)\n", moved_.x_, moved_.y_);

	  if (drag_.length() > texSize_/2.0f) {
		// いっぱい動いた場合
		lastMoveTime_ = app_.frame_counter(); // 最後に移動したタイミングを記録
		movex = fabs(drag_.x_) > fabs(drag_.y_);
		drag_ = vec2_f(stpos, now);
	  }
	  else {
		if (drag_) {
		  // ちょびっと動いていた場合元に戻す
		  BOOST_FOREACH(ChipObj& chip, obj_->objs()) {
			chip.target_ = getPos(chip.slt_);
		  }
		}
		drag_ = vec2_f(stpos, now);
		movex = fabs(drag_.x_) > fabs(drag_.y_);
	  }
	  if (movex) {
		// 横方向に移動
		uint32_t row = selected_.get()->slt_.y_;
		moveRow(row, drag_.x_);
	  }
	  else {
		// 縦方向に移動
		uint32_t col = selected_.get()->slt_.x_;
		moveCol(col, drag_.y_);
	  }
	  redraw_ = true;
	}
	// 横方向に移動
	void moveRow(uint32_t row, float move) {
	  drag_.y_ = 0;
	  drag_.x_ = move;
	  for (uint32_t x = 0; x < cols_; ++x) {
		ChipObj& c = obj_->at(slot(x, row));
		c.target_ = getPos(x, row) + drag_;
		c.setPos(c.target_, chipRect_);
	  }
	}
	// 縦方向に移動
	void moveCol(uint32_t col, float move) {
	  drag_.x_ = 0;
	  drag_.y_ = move;
	  for (uint32_t y = 0; y < rows_; ++y) {
		ChipObj& c = obj_->at(slot(col, y));
		c.target_ = getPos(col, y) + drag_;
		c.setPos(c.target_, chipRect_);
	  }
	}

	// ぐるぐる回るアニメーション
	void setRotateAnimation() {
	  //TS_DEBUGLOG("アニメーション開始(%.2f,%.2f)\n", drag_.x_, drag_.y_);
	  rotate_ = moved_;
	  bool d = fabs(drag_.x_) > fabs(drag_.y_);
	  swapper2 sw(d);
	  sw(rotate_).x_ /= 2.0f;
	  sw(rotate_).y_ = 0;
	  for (uint32_t y = 0; y < rows_; ++y) {
		for (uint32_t x = 0; x < cols_; ++x) {
		  if (sw(y, x) == sw(selected_.get()->slt_).y_) continue;
		  ChipObj& chip = obj_->at(slot(x, y));
		  chip.target_ = getPos(chip.slt_);
		}
	  }
	  rotateMode_ = true;
	}

	void rotateAnimation() {
	  redraw_ = true;
	  //TS_DEBUGLOG("アニメーション処理(%.2f,%.2f)\n", drag_.x_, drag_.y_);
#if 1
      if (!selected_) return;
	  swapper2 sw(rotate_.y_ == 0.0f);
	  uint32_t ypos = sw(selected_.get()->slt_).y_;
	  for (uint32_t xy = 0; xy < sw(cols_, rows_); ++xy) {
		ChipObj& c = obj_->at(slot(sw(xy, ypos), sw(ypos, xy)));
		c.target_ += rotate_;
		if (sw) {
		  if (c.target_.x_ > texSize_* rows_) c.target_.x_ -= texSize_ * rows_;
		  if (c.target_.x_ <= texSize_) c.target_.x_ += texSize_ * rows_;
		}
		else {
		  if (c.target_.y_ > texSize_* cols_) c.target_.y_ -= texSize_ * cols_;
		  if (c.target_.y_ <= texSize_) c.target_.y_ += texSize_ * cols_;
		}
		c.setPos(c.target_, chipRect_);
	  }
#else
	  if (rotate_.y_ == 0.0f) {
		// 横方向に移動
		uint32_t row = selected_.get()->slt_.y_;
		for (uint32_t x = 0; x < cols_; ++x) {
		  ChipObj& c = obj_->at(slot(x, row));
		  c.target_ += rotate_;
		  if (c.target_.x_ > texSize_* rows_) c.target_.x_ -= texSize_ * rows_;
		  if (c.target_.x_ <= texSize_) c.target_.x_ += texSize_ * rows_;
		  c.setPos(c.target_, chipRect_);
		}
	  }
	  else {
		// 縦方向に移動
		uint32_t col = selected_.get()->slt_.x_;
		for (uint32_t y = 0; y < rows_; ++y) {
		  ChipObj& c = obj_->at(slot(col, y));
		  c.target_ += rotate_;
		  if (c.target_.y_ > texSize_* cols_) c.target_.y_ -= texSize_ * cols_;
		  if (c.target_.y_ <= texSize_) c.target_.y_ += texSize_ * cols_;
		  c.setPos(c.target_, chipRect_);
		}
	  }
#endif
	  drag_ += rotate_;
	  rotate_ *= 0.99f;
	  if (rotate_.length() < 1.0f) {
		fixRotation();
		rotateMode_ = false;
	  }
	}

	// タップ終了
	void onTapOut(in::tap_event::ptr evt) {
	  event::event_processor::ptr de(new event::dummy_event_processor("tapout"));
	  app_.on_event_proseeded(de, evt);

	  if (giveupSw_.on_tap_out(evt->ctrl_.pos_)) return;
	  if (!canChipMove_) return;
	  if (selected_) {
		selected_.get()->effect(ChipObj::NoEffect);
		// 勢いよく離した場合は慣性をつける (最後に移動してから5フレーム以内に離した場合)
		if (app_.frame_counter() - lastMoveTime_ < 2 && moved_.length() > 24.0f) {
		  setRotateAnimation();
		  return;
		}
		fixRotation();
	  }
	}


	// 回転を止めて場所を確定する
	void fixRotation() {
	  if (!selected_) return;
	  float ax = fmodf(fabs(drag_.x_), texSize_ * cols_);
	  float ay = fmodf(fabs(drag_.y_), texSize_ * rows_);
	  if (ax > ay) {
		uint32_t row = selected_.get()->slt_.y_;
		if (ax > texSize_/2.0f) {
		  for (uint32_t n = 0; n < uint32_t(ax/texSize_+0.5f); ++n)
			rotate_col(row, drag_.x_ > 0);
		}
		for (uint32_t x = 0; x < cols_; ++x) {
		  ChipObj& c = obj_->at(slot(x, row));
		  c.setPos(correctPos(c.pos()), chipRect_);
		  c.target_ = getPos(x, row);
		  c.slt_.x_ = x;
		  c.slt_.y_ = row;
		}
	  }
	  else {
		uint32_t col = selected_.get()->slt_.x_;
		if (ay > texSize_/2.0f) {
		  for (uint32_t n = 0; n < uint32_t(ay/texSize_+0.5f); ++n)
			rotate_row(col, drag_.y_ > 0);
		}
		for (uint32_t y = 0; y < rows_; ++y) {
		  ChipObj& c = obj_->at(slot(col, y));
		  c.setPos(correctPos(c.pos()), chipRect_);
		  c.target_ = getPos(col, y);
		  c.slt_.x_ = col;
		  c.slt_.y_ = y;
		}
	  }
	  app_.sound_.play(TapSound2);
	  selected_ = boost::none;
	  redraw_ = true;
	}

	// 内部処理
  private:

	// チップの移動処理
	bool moveChips() {
	  BOOST_FOREACH(ChipObj& chip, obj_->objs()) {
		gl::vec2_f v(chip.pos(), chip.target_);
		if (v) { // 移動が必要だったら
		  v /= 2.0f;
		  if (v.length() > moveSpeed_) v.set_length(moveSpeed_);
		  if (v.length() < 0.1f) {
			chip.setPos(chip.target_, chipRect_); // 目的地を設定して移動終了
		  }
		  else{
			pos2_f pos = chip.pos() + v;
			chip.setPos(pos, chipRect_);
		  }
		  redraw_ = true;
		}
	  }
	  return redraw_;
	}

	// 終了判定
	bool checkComplete() {
	  for (size_t i = 0; i < slot_.size(); ++i) {
		if (i != slot_[i]) return false;
	  }
	  return true;
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
		if (chip.rect().is_inside(pos)) {
		  return chip;
		}
	  }
	  return boost::none;
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



  public:
	void printTime(const char* name, pos2_f pos, float fsec, effect ef = effect()) {
	  uint32_t sec = static_cast<uint32_t>(fsec);
	  char demi = ':';
	  if (sec < 60) {
		sec = static_cast<uint32_t>(fsec*60);
		if (fsec < 0.0f) sec = 0;
		demi = ';';
	  }
	  uint32_t s0= sec % 10;
	  uint32_t s1= (sec % 60) / 10;
	  uint32_t m0= (sec / 60) % 10;
	  uint32_t m1= (sec / 600) % 10;
	  gr_.remove_object(name);
	  printNumeric(name, pos, '0' + m1, ef);
	  printNumeric(name, pos, '0' + m0, ef, 0.7f);
	  printNumeric(name, pos, demi, ef, 0.7f);
	  printNumeric(name, pos, '0' + s1, ef);
	  printNumeric(name, pos, '0' + s0, ef);
  }

	void printNumeric(const char* name, pos2_f& pos, uint32_t ch, effect ef, float xscale=1.0f) {
	  if (ch == ' ') ch = ';';
	  assert(ch >= '0' && ch < ('0'+16));
	  ch -= '0';
	  float x = (ch % 8) * CharSize;
	  float y = (ch / 8) * CharSize;
	  float sz = CharSize;
	  gr_.show_image(name, image32(gadget_, rect_f(x, y, sz, sz), CharScale), rect_f(pos,size_f(64.0f,64.0f)), MenuLayer, ef);
	  float scale = 1.0f;
	  if (ef.has(effect::scale_to)) {
		scale= ef.get<scale_to>().param2_;
	  }
	  pos.x_ += 64.0f * scale * xscale;
	}


  };


}


#endif


