/** -*-tab-width:4-*-
 * @file  AppMain.cpp
 * @brief rotate puzzle mail routine 
 *
 *  Created by IKEDA KOHEI on 11/04/06.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 * @author h.godai godai@techarts.co.jp
 *
 */

#include <fstream>
#include "AppMain.h"
#include "GameLogic.hpp"
#include "libTS/appfw/gl/button.hpp"
#include "libTS/appfw/gl/rectangle.hpp"
#include "libTS/appfw/gl/show_image.hpp"
#include "libTS/appfw/gadgets/slide_selector.hpp"
#include "libTS/appfw/gadgets/swipe_button.hpp" 
#include "libTS/appfw/serialize.hpp"
#include "libTS/Util/mpl.hpp"
#include <boost/array.hpp>

using namespace std;

namespace app {
  using ts::appfw::sounds::music_player;
  using namespace ts::appfw;
  using namespace ts::appfw::gl;
  using namespace boost::gil;
  using namespace ts;
  using namespace std;

  typedef swipe_button::se<SoundTypes, sounds::sound_player> se;
  
  /// @brief アプリケーションメインクラス
  class AppMain : public drawframe_app {
	GameResource& res_;       // ゲームリソース
	GameLogic game_;          // ゲームロジッククラス
	uint32_t level_;          // 現在の難易度(グリッドの数)
    ImageFileType puzzlePic_; // 現在の画像
	map<uint32_t, in::tap_event::ptr> tapEvent_;
	map<int32_t, map<int32_t, float> > bestTime_;
	bool in_game_;
	boost::array<image32,NumOfPictures> picImages_;

  public:
	AppMain()
	  : res_(GameResource::Instance())
	  , game_(*this, res_)
	  , level_(3)
      , puzzlePic_(PuzzlePic1)
	  , in_game_(false)
    
	{}
	virtual ~AppMain() throw() 
	{}

	// 初期化処理
	virtual void initialize_impl() {
	  drawframe_app::initialize_impl();
	  res_.initialize(this->app_info_);
	}	

	// サウンドリソースファイルのリストを取得する
	virtual void get_sound_list(vector<string>& list) {
	  list = res_.soundFileList();
	}

	// 音楽リソースファイルのリストを取得する
	virtual void get_music_list(vector<string>& list) {
	  list = res_.musicFileList();
	}

	// ゲームの進行を制御するメインプログラム
	// 描画系とは別のスレッドから呼ばれる
	virtual void main() {
	  try {
		// レイヤーの設定（depth順にソートされる)
		ortho_layer::ptr depthlayer(new ortho_layer(DepthLayer));
		depthlayer->enableDepthTest_ = true;
		gr_.set_layer(depthlayer);

		// スプラッシュ画面の表示
		drawSplash();

		// besttimeの読み込み
		try {
		  deserialize(to_istream(res_.bestTimeFile())) >> bestTime_;  // こちらのほうが効率が良い
		  //to_istream(res_.bestTimeFile()) >> deserialize() >> bestTime_;
		}
		catch (const file_can_not_open& ex) {
		  TS_DEBUGLOG("no besttime file(%s)\n", ex.fn_.c_str());
		}
		catch (const buffer_over_run&) {
		  TS_WARNINGLOG("Besttime.datが異常なので削除します。\n");
		  bestTime_.clear();
		  unlink(res_.bestTimeFile().c_str());
		}
		

		// メインループ
		bool exitGame = false; // ゲーム終了フラグ
		while (!exitGame) {
		  switch (mainMenu()) {
		  case GameMenu:
			// ゲーム開始
			gameMain();
		  default:
			break;
		  }
		}
	  }
	  catch (const appfw_exception& ex) {
		// App終了
		TS_DEBUGLOG("Application Exception:%s", ex.what());
	  }
	  catch (const std::exception& ex) {
		// 異常発生
		TS_WARNINGLOG("STD Exception:%s", ex.what());
	  }
	  music_.stop(music_player::fade_out);
	}

	
	// スプラッシュ画面
	void drawSplash() {
	  try {
        wait_for_sec(0.1); // 1.5秒
		gr_.show_image("splash"
					   , res_[SplashPic]
					   , align_center // positioning(align_center, size_f(2.0f,2.0f))
					   , BackgroundLayer);
        wait_for_sec(1.5f); // 1.5秒
        gr_.hide_image("splash", fade_out(0.5f));
      }
	  catch (const glfw_exception& ex) {
		TS_WARNINGLOG("ファイルの読み込みに失敗しました。[%s]\n", ex.what());
	  }
	}
	

	// メニュー処理
	MenuTypes mainMenu() {
	  try {
		music_.start(RotorsMusic, music_player::repeat);
        gr_.hide_image("background");
		gr_.show_image("background", res_[BackgroundPic]
					   , rect_f(0,0, -gr_.screen_size().width_, -gr_.screen_size().height_)
					   , Croppinglayer);
		gr_.show_image("logo", res_[LogoPic], fit_to_width, Croppinglayer);

		// パズルのスライドセレクタの位置を計算
		float scalebase = 0.5f;
		float ypos = 190.0f;
		if (gr_.context_.portlate_) {
		  scalebase = 0.33f;
		  ypos += 48.0f;
		}
		float btypos = ypos + 2.0f*640.0f*scalebase + 8.0f;
		float meshYpos = ypos + 1.5f*640.0f*scalebase - 64.0f+4.0f;

		
		// gadgetのイメージを読み込み
		if (!game_.gadget_) {
		  game_.gadget_ = image32(res_[GadgetPic]);
		}
		
		// Copyright表示
		gr_.show_image("copyright", image32(game_.gadget_, CopyrightRect, CharScale), align_center_bottom
					   ,MenuLayer, scale_to(0.1f, 1.0f, 1.0f)|fade_in(2.0f));


		// ボタンオブジェクト生成
		button2d buttons[] = {
		  button2d("easy",     image32(game_.gadget_, EasyRect), gr_.positioner_(align_center_h, gr_.screen_size().height_*0.8f), button2d::enable_dragevent),
		  button2d("medium",   image32(game_.gadget_, MediumRect), gr_.positioner_(align_center_h, gr_.screen_size().height_*0.8f), button2d::enable_dragevent),
		  button2d("hard",     image32(game_.gadget_, HardRect), gr_.positioner_(align_center_h, gr_.screen_size().height_*0.8f), button2d::enable_dragevent),
		  button2d("veryhard", image32(game_.gadget_, VeryHardRect), gr_.positioner_(align_center_h, gr_.screen_size().height_*0.8f), button2d::enable_dragevent)
		};

		swipe_button levelbuttons(buttons); // [] = {&easyBtn, &mediumBtn, &hardBtn, &veryHardBtn};
		pos2_f ctrlpos(-1.0f, gr_.screen_size().height_*0.8f);

		level_ = 3 + ((level_  - 3) % count_of(buttons));

		slide_selector selector(gr_, ypos, scalebase, DepthLayer);
		selector.current_ = puzzlePic_ - PuzzlePic1;
		selector.add(button2d("pic1", res_[MenuPuzzlePic1], pos(0,0)));
		selector.add(button2d("pic2", res_[MenuPuzzlePic2], pos(0,0)));
		selector.add(button2d("pic3", res_[MenuPuzzlePic3], pos(0,0)));
		selector.add(button2d("pic4", res_[MenuPuzzlePic4], pos(0,0)));
		selector.add(button2d("pic5", res_[MenuPuzzlePic5], pos(0,0)));
		selector.add(button2d("pic6", res_[MenuPuzzlePic6], pos(0,0)));
		selector.add(button2d("pic7", res_[MenuPuzzlePic7], pos(0,0)));
		

		image32 mesh(res_[MeshPic]);
		gr_.show_image("mesh", image32(mesh, MeshRects[level_-3])
					   , positioning(align_center_h, meshYpos)
					   , MenuLayer, scale_to(2.0f,640.0f*scalebase/128.0f,1.0f));
		

		// easyボタン表示
		levelbuttons.current_ = level_ - 3;

		gr_.show_object(levelbuttons.current_btn(), MenuLayer, fade_in(0.1f));
		//gr_.show_object(*buttons[level_-3], MenuLayer, fade_in(1.0f)|scale_to(1.0f,2.0f,1.0f));

		// BestTime表示
		gr_.show_image("besttime", image32(game_.gadget_, BestTimeRect, CharScale)
					   , pos2_f(gr_.screen_size().width_/2.0f - (BestTimeRect.width()*CharScale), btypos)
					   , MenuLayer
					   , fade_in(1.0f));

		showBestTime(btypos, selector.current_);
		for (;;){
		  boost::optional<const string&> eventname;
		  selector.show_all();
		  for (;;) {
			eventname =  wait_for_event(0.1f);
			if (eventname) break;
			if (selector.drag_check()) break;
		  }

		  
		  if (!eventname) {
			//sound_.play(TapSound1);
			showBestTime(btypos, selector.current_);
			continue; // may be dragged
		  }


		  if (levelbuttons.check(gr_, *eventname, ctrlpos, MenuLayer, se(this->sound_, TapSound2))) {
			if (levelbuttons.current_  != level_ - 3) {
			  // LEVEL選択が変わった
			  sound_.play(TapSound2);
			  gr_.hide_image("mesh");
			  wait_for_frame(4);
			  level_ = levelbuttons.current_ + 3;
			  assert(level_ >= 3 && level_ < 3 + count_of(MeshRects));
			  gr_.show_image("mesh", image32(mesh, MeshRects[level_-3])
							 , positioning(align_center_h, meshYpos)
							 , MenuLayer, scale_to(2.0f,640.0f*scalebase/128.0f,1.0f));
			  showBestTime(btypos, selector.current_);
			}
			continue;
		  }
		  else if (selector.hit_check(*eventname)) {
			if (selector.is_selected()) {
			  sound_.play(TapSound2);
			  gr_.hide_image("mesh");
			  selector.hide_all(scale_to(scalebase,4.0f,1.0f)|fade_out(1.0f), fade_out(0.5f));
			  gr_.hide_image("startbtn", move_to(pos(keep_position, gr_.screen_bottom()), 1.0f) | fade_out(1.0f));
			  wait_for_sec(0.5f);
			  puzzlePic_ = static_cast<ImageFileType>(selector.current_ + PuzzlePic1);
			  // メニューを消す
			  gr_.hide_image("logo", fade_out(1.0f));
			  gr_.hide_image(levelbuttons.current_btn(), fade_out(0.1f));
			  gr_.hide_image("copyright");
			  gr_.hide_image("besttime");
			  gr_.hide_image("btime");
			  return GameMenu;
			}
			sound_.play(TapSound1);
			showBestTime(btypos, selector.current_);
		  }
		  else {
			// なぞ
			break;
		  }
		}
		return GameMenu;
	  }
	  catch (const glfw_exception& ex) {
		TS_DEBUGLOG("ファイルの読み込みに失敗しました。[%s]\n", ex.what());
		return ExitMenu;
	  }
	}

	void showBestTime(float btypos, int32_t pic) {
	  if (bestTime_[level_][PuzzlePic1 + pic] == 0.0f) {
		gr_.hide_image("btime", fade_out(0.2f));
		wait_for_frame();
		gr_.show_image("btime", image32(game_.gadget_, NotYetRect, CharScale)
					   , pos2_f(gr_.screen_size().width_/2.0f, btypos)
					   , MenuLayer
					   , fade_in(1.0f));
	  }
	  else {
		game_.printTime("btime"
						, pos2_f(gr_.screen_size().width_/2.0f, btypos)
						, bestTime_[level_][PuzzlePic1 + pic]
						, scale_to(0.5f,0.6f,0.5f));
	  }
	}


	
	// ゲームメイン
	GameExitStatus gameMain() {	  
	  try {
		// 音楽停止
		music_.stop(music_player::fade_out);
		// パズル画像を表示
		image32& pic = picImages_[puzzlePic_ - PuzzlePic1];
		if (pic.empty()) {
		  pic = image32(res_[puzzlePic_]);
		}
		// メニュー画面を下に移動
		gr_.effect_image("background", move_to(pos(0, gr_.screen_size().width_), 0.3f));
		wait_for_frame();
		//
		gr_.show_image("puzzle", pic, fit_to_width, BackgroundLayer);
		// 1秒待ち
		wait_for_sec(1.0f);
		// ゲーム初期化(パズルが表示される)
		game_.initialize(level_, puzzlePic_);
		// パズル画像を消す
		gr_.hide_image("puzzle", fade_out(5.0f));
		// ゲーム開始
		in_game_ = true;
		if (game_.main()) {
		  gr_.show_image("puzzle", pic, fit_to_width, MenuLayer);
		  gameClear();
		  gr_.hide_image("puzzle", fade_out(2.0f));
		}
		in_game_ = false;
		// メニュー画面を上に移動
		//gr_.effect_image("menupic", move_to(pos(0,0), 0.5f));
		//gr_.effect_image("background", move_to(pos(0,0), 0.3f));
        //gr_.hide_image("background");

	  }
	  catch (const glfw_exception& ex) {
		TS_DEBUGLOG("ファイルの読み込みに失敗しました。%s\n", ex.what());
	  }
	  return ShowEnding;
	}

	// クリアした！
	void gameClear() {
	  float btypos = gr_.screen_size().height_ - 64.0f;
	  if (bestTime_[level_][puzzlePic_] == 0.0f ||
		  bestTime_[level_][puzzlePic_] > game_.pastTime_) {
		// 記録更新
		bestTime_[level_][puzzlePic_] = game_.pastTime_;

		// besttimeとtimeを点滅
		gr_.show_image("besttime", image32(game_.gadget_, BestTimeUpdatedRect, CharScale)
					   , positioning(fit_to_width, btypos - 64.0f)
					   , MenuLayer
					   , flicker(0.2f));
		gr_.effect_image("time", flicker(1.0f));
		try {
		  TS_DEBUGLOG("bestTime_:%s\n", to_string(bestTime_).c_str());
		  serialize(bestTime_) >> to_ostream(res_.bestTimeFile());
		}
		catch (const file_can_not_open& ex) {
		  TS_WARNINGLOG("ファイルが書き込みオープンできません。%s\n", ex.fn_.c_str());
		}
	  }
	  else {
		// besttimeの表示
		pos2_f btpos(gr_.screen_size().width_/2.0f - (BestTimeRect.width()*CharScale), btypos);
		gr_.show_image("besttime"
					   , image32(game_.gadget_, BestTimeRect, CharScale)
					   , btpos
					   , MenuLayer
					   , fade_in(0.2f));
		showBestTime(btypos, puzzlePic_ - PuzzlePic1);
	  }
	  
	  update();
	  wait_for_event(30.0f);

	  music_.stop();
	  gr_.hide_image("besttime");
	  gr_.hide_image("btime");
	  game_.hide_all();

	  // 面をすすめる
      puzzlePic_ = static_cast<app::ImageFileType>(puzzlePic_ + 1);
	  if (puzzlePic_ == static_cast<int>(PuzzlePic1) + NumOfPictures) {
		++level_;
		puzzlePic_ = PuzzlePic1;
	  }
	}


	
	// エンディング
	void ending() {
	  //music_.start(EndingMusic, music_player::no_repeat);
	}

  private:
	// イベント処理
	virtual void on_tap_ctrl(in::tap_event::ptr evt) {
	  //if (evt->devnum_ >= 2) return; // 0,1以外は無視

	  switch (evt->ctrl_.action_) {
	  case in::tap_ctrl::touch: // タッチイン
		tapEvent_[evt->devnum_] = evt;
		if (in_game_) game_.onTapIn(evt);
		break;
	  case in::tap_ctrl::move: // 移動
		if (in_game_) game_.onTapMove(tapEvent_[evt->devnum_], evt);
		break;

	  case in::tap_ctrl::release: // タッチアウト
		if (in_game_) game_.onTapOut(evt);
		tapEvent_[evt->devnum_] = in::tap_event::ptr();
		break;
	  default:
		break;
	  }
	}

	// 加速度センサ
	virtual void on_acc_ctrl(in::acc_event::ptr evt) {
	  game_.onAccEvent(evt);
	}

	// 描画処理が行われない場合のエラー処理
	virtual void on_drawframe_timeout() {
	  TS_DEBUGLOG("drawframe wait timeout");
	}

	// デバイスが回転した
	virtual void on_device_rotate(float rot) {
	  if (rot == 0.0f || rot == 180.0f) {
		drawframe_app::on_device_rotate(rot);
	  }
	}


	// シングルトン実装
  public:
	static AppMain& Instance() {
	  static AppMain instance;
	  return instance;
	}
	
  };
}

namespace ts {
  namespace appfw {
	app_framework& app_framework::app_instance() {
	  return static_cast<app_framework&>(app::AppMain::Instance());
	}
	drawframe_app& drawframe_app::app_instance() {
	  return static_cast<drawframe_app&>(app::AppMain::Instance());
	}
  }
} // namespace
