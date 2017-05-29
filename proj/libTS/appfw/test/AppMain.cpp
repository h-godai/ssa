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
#include "libTS/appfw/drawframe_app.hpp"
#include "libTS/appfw/gl/button.hpp"
#include "libTS/appfw/gl/rectangle.hpp"
#include "libTS/appfw/gl/show_image.hpp"
#include "libTS/appfw/gadgets/slide_selector.hpp"
#include "libTS/appfw/serialize.hpp"
#include "libTS/appfw/sound/soundfw.hpp"
#include "libTS/Util/mpl.hpp"

using namespace std;

namespace app {
  using ts::appfw::sounds::music_player;
  using namespace ts::appfw;
  using namespace ts::appfw::gl;
  using namespace boost::gil;
  using namespace ts;
  using namespace std;


  /// @brief アプリケーションメインクラス
  class AppMain : public drawframe_app {
	map<uint32_t, in::tap_event::ptr> tapEvent_;

  public:
	AppMain()
	{}
	virtual ~AppMain() throw()
	{}

	// 初期化処理
	virtual void initialize_impl() {
	  drawframe_app::initialize_impl();
	  //res_.initialize(this->app_info_);
	}

	// サウンドリソースファイルのリストを取得する
	virtual void get_sound_list(vector<string>& list) {
	  //list = res_.soundFileList();
	}

	// 音楽リソースファイルのリストを取得する
	virtual void get_music_list(vector<string>& list) {
	  //list = res_.musicFileList();
	}

	// ゲームの進行を制御するメインプログラム
	// 描画系とは別のスレッドから呼ばれる
	virtual void main() {
	  try {
        gameMain();
        #if 0
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
		#endif
	  }
	  catch (const appfw_exception& ex) {
		// App終了
		TS_DEBUGLOG("Application Exception:%s", ex.what());
	  }
	  catch (const std::exception& ex) {
		// 異常発生
		TS_WARNINGLOG("STD Exception:%s", ex.what());
	  }
		//	  music_.stop(music_player::fade_out);

#if 0
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
#endif
	}

  private:
	// イベント処理
	virtual void on_tap_ctrl(in::tap_event::ptr evt) {
	  switch (evt->ctrl_.action_) {
	  case in::tap_ctrl::touch: // タッチイン
		tapEvent_[evt->devnum_] = evt;
		//if (in_game_) game_.onTapIn(evt);
		break;
	  case in::tap_ctrl::move: // 移動
		//if (in_game_) game_.onTapMove(tapEvent_[evt->devnum_], evt);
		break;

	  case in::tap_ctrl::release: // タッチアウト
		//if (in_game_) game_.onTapOut(evt);
		tapEvent_[evt->devnum_] = in::tap_event::ptr();
		break;
      default:
        break;
	  }
	}

	// 加速度センサ
	virtual void on_acc_ctrl(in::acc_event::ptr evt) {
	  //game_.onAccEvent(evt);
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


  enum {
      BackgroundLayer = 1
  };
  void gameMain() {
		gr_.show_image("splash"
					   , "godai.png"
					   , align_center // positioning(align_center, size_f(2.0f,2.0f))
					   , BackgroundLayer
					   , fade_in(0.5f)
					   );
        wait_for_sec(1.0f); // 1.5秒
        gr_.hide_image("splash", fade_out(0.5f));
        wait_for_sec(1.0f);
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
