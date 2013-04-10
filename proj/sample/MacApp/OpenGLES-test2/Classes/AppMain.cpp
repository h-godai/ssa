/* -*-tab-width:4-*-
 *  AppMain.cpp
 *  OpenGLES-test2
 *
 *  Created by IKEDA KOHEI on 11/04/06.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "AppMain.h"
#include "GameLogic.hpp"

using namespace std;

namespace app {
  using ts::appfw::sounds::music_player;
  using namespace ts::appfw;
  using namespace boost::gil;

  
  class AppMain : public drawframe_app {
	music_player music_;
	enum MusigTypes {
		OpeningMusic
	  , GameMusic1
	  , GameMusic2
	  , GameMusic3
	  , EndingMusic
	};
	enum GameExitStatus {
		NoGameStatus
	  , ShowEnding
	};
	enum MenuTypes {
	    SettingMenu
	  , GameMenu
	  , ExitMenu
	};
	GameLogic game_;
  public:
	AppMain()
	{}
	virtual ~AppMain() throw() 
	{}
	

	// ゲームの進行を制御するメインプログラム
	// 描画系とは別のスレッドから呼ばれる
	virtual void main() {
	  try {
		// スプラッシュ画面の表示
		drawSplash();

		// メインループ
		bool exitGame = false; // ゲーム終了フラグ
		while (!exitGame) {
		  switch (mainMenu()) {
		  case SettingMenu:
			// 設定画面
			setting();
			break;
		  case GameMenu:
			// ゲーム開始
			if (gameMain() == ShowEnding) {
			  ending(); // エンディング
			}
			break;
		  case ExitMenu:
			// ゲーム終了
			exitGame = true;
			break;
		  }
		}
	  }
	  catch (const appfw_exception& ex) {
		// App終了
		TS_DEBUGLOG("Application Exception:%s", ex.what());
	  }
	  music_.stop(music_player::FadeOut);
	}

	// 設定画面
	void setting() {
	}
	
	// スプラッシュ画面
	void drawSplash() {
	  std::string fn(resource_dir() + "/godai.png");
	  try {
		music_.start(OpeningMusic, music_player::Repeat);
		gr_.show_image(0, fn, gl::pos2d_f(0,0), gl::fit_to_width); // 画面表示
		gr_.show_image(0, fn, gl::pos2d_f(0,0), gl::fit_to_width); // 画面表示
	  }
	  catch (const gl::glfw_exception& ex) {
		TS_DEBUGLOG("ファイルの読み込みに失敗しました。[%s] %s\n", ex.what(), fn.c_str());
	  }
	  wait_for_frame(60); // 60フレーム待機
	}
	
	// メニュー処理
	MenuTypes mainMenu() {
	  if (music_.is_playing()) {
		music_.stop(music_player::FadeOut);
		music_.start(OpeningMusic, music_player::Repeat);
	  }
	  return GameMenu;
	}
	
	// ゲームメイン
	GameExitStatus gameMain() {
	  std::string fn2(resource_dir() + "/RotersPuzzle.png");
	  music_.start(GameMusic1, music_player::Repeat);
	  game_.initialize(*this, gr_);
	  gr_.hide_image(0);  // 非表示
	  gr_.show_image(2, fn2, gl::pos2d_f(0.0f, float(gr_.screen_size().width_)), gl::fit_to_width); // 画面表示
	  game_.start(*this);
	  return ShowEnding;
	}
	
	// エンディング
	void ending() {
	  music_.start(EndingMusic, music_player::NoRepeat);
	}

  private:
	// イベント処理
	virtual void on_tap_ctrl(in::tap_event::ptr evt) {
	  if (evt->devnum_ >= 2) return; // 0,1以外は無視
	  switch (evt->ctrl_.action_) {
	  case in::tap_ctrl::touch: // タッチイン
		tapEvent_[evt->devnum_] = evt;
		game_.onTapIn(evt);
		break;
	  case in::tap_ctrl::move: // 移動
		game_.onTapMove(tapEvent_[evt->devnum_], evt);
		break;

	  case in::tap_ctrl::release: // タッチアウト
		game_.onTapOut(evt);
		tapEvent_[evt->devnum_] = in::tap_event::ptr();
		break;
	  }
	}

	in::tap_event::ptr tapEvent_[2];


	// シングルトン実装
  public:
	static AppMain& Instance() {
	  static AppMain instance;
	  return instance;
	}
	
  };

  drawframe_app& appInstance() {
	return static_cast<drawframe_app&>(AppMain::Instance());
  }
  
} // namespace
