/* -*-tab-width:4-*-
   GameResource.hpp
   @brief ゲームのリソース定義部


*/

#ifndef APP_GAMERESOURCE_HPP
#define APP_GAMERESOURCE_HPP
#include <stdlib.h>
#include <vector>
#include <string>
#include "libTS/Util/policy.hpp"
#include "libTS/appfw/gl/draw_object.hpp"

namespace app {

  // BGM音楽のタイプ
  enum MusigTypes {
    RotorsMusic
  , GameMusic1
  , ComplateMusic
  };

  // 効果音のタイプ
  enum SoundTypes {
    TapSound1
  , TapSound2
  , CompletedSound
  };

  // ゲーム終了ステータス
  enum GameExitStatus {
	NoGameStatus
  , ShowEnding
  };
  // ゲームメニューのタイプ
  enum MenuTypes {
	SettingMenu
  , GameMenu
  , ExitMenu
  };

  // 表示レイヤーの定義
  enum DrawLayers {
	BackgroundLayer // 背景
	, ChipsLayer    // チップ
	, Croppinglayer  // チップを隠すためのパネル
	, DepthLayer
	, MenuLayer     // メニュー
  };

  // 画像ファイル
  enum ImageFileType {
	BackgroundPic
	, LogoPic
	, PuzzlePic1
	, PuzzlePic2
	, PuzzlePic3
	, PuzzlePic4
	, PuzzlePic5
	, PuzzlePic6
	, PuzzlePic7
	, GadgetPic    // gamestart, option, exit
	, SplashPic
	, MeshPic
	, MenuPuzzlePic1
	, MenuPuzzlePic2
	, MenuPuzzlePic3
	, MenuPuzzlePic4
	, MenuPuzzlePic5
	, MenuPuzzlePic6
	, MenuPuzzlePic7
  };
  const ::size_t NumOfPictures = GadgetPic - PuzzlePic1;

  const char* ImageFiles[] =
	{ "background.png"
	  , "rotatepuzzle.png"
	  , "puzzle-1-1536.png"  // バイク
	  , "puzzle-5.png"  // ワイングラス
	  , "puzzle-7.png"  // 猫
	  , "puzzle-3-1536.png"  // 美瑛
	  , "puzzle-6.png"  // 夕日
	  , "puzzle-4-1536.png"  // スキー
	  , "puzzle-2-1536.png"  // 森林公園
	  //, "gadget.png"  
	  , "gadget-1024.png"
	  , "ssabanner.png"
	  , "mesh.png"
	  , "puzzle-1.png"  // バイク
	  , "puzzle-5.png"  // ワイングラス
	  , "puzzle-7.png"  // 猫
	  , "puzzle-3.png"  // 美瑛
	  , "puzzle-6.png"  // 夕日
	  , "puzzle-4.png"  // スキー
	  , "puzzle-2.png"  // 森林公園
	};

  const char BestTimeFile[] = "besttime.dat";
  
  const int  CharSize = 128;
  const float CharScale = 0.5f;

  const rect_f BestTimeRect(0,2* CharSize,4* CharSize,CharSize);
  const rect_f NotYetRect(4* CharSize, 2* CharSize,4* CharSize,CharSize);
  const rect_f EasyRect(0* CharSize, 3* CharSize,3* CharSize,CharSize);
  const rect_f SecRect(4* CharSize, 3* CharSize,CharSize,CharSize);
  const rect_f MediumRect(4* CharSize, 3* CharSize,4* CharSize,CharSize);
  const rect_f HardRect(0* CharSize, 4* CharSize,3* CharSize,CharSize);
  const rect_f VeryHardRect(3* CharSize, 4* CharSize,5* CharSize,CharSize);
  const rect_f CopyrightRect(0* CharSize, 5* CharSize,8* CharSize,CharSize);
  const rect_f GiveUp1Rect(1* CharSize, 6* CharSize,3* CharSize,CharSize);
  const rect_f GiveUp2Rect(5* CharSize, 6* CharSize,3* CharSize,CharSize);
  const rect_f GiveArrowRect(0* CharSize, 6* CharSize,CharSize,CharSize);
  const rect_f BestTimeUpdatedRect(0* CharSize, 7* CharSize,8* CharSize,CharSize);

  const rect_f Lv3MeshRect(0,0,128,128);
  const rect_f Lv4MeshRect(128,0,128,128);
  const rect_f Lv5MeshRect(0,128,128,128);
  const rect_f Lv6MeshRect(128,128,128,128);
  const rect_f MeshRects[] = {Lv3MeshRect,Lv4MeshRect,Lv5MeshRect,Lv6MeshRect};


  // ゲームリソース管理
  class GameResource : public ts::singleton_policy<GameResource> {
	friend class ts::singleton_policy<GameResource>;
	std::vector<std::string> soundFileList_;
	std::vector<std::string> musicFileList_;
	std::vector<std::string> imageFileList_;
	app_info::ptr app_info_;
	volatile bool initialized_;
	GameResource() : initialized_(false) {}
	public:

	void initialize(app_info::ptr info) {
	  app_info_ = info;
	  soundFileList_.push_back(info->res_dir_ + "/b214.wav");
	  soundFileList_.push_back(info->res_dir_ + "/b210.wav");
	  soundFileList_.push_back(info->res_dir_ + "/b213.wav");

	  musicFileList_.push_back(info->res_dir_ + "/Rotors.mp3");
	  musicFileList_.push_back(info->res_dir_ + "/bgm1.mp3");
	  musicFileList_.push_back(info->res_dir_ + "/RiseOut-Transit.aif");


	  BOOST_FOREACH(const char* fn, ImageFiles) {
		imageFileList_.push_back(info->res_dir_ + "/" + fn);
	  }
	  initialized_ = true;
	}
	std::string bestTimeFile() {
	  return app_info_->doc_dir_ + "/" + BestTimeFile;
	}

	std::vector<std::string>& soundFileList() {
	  while (!initialized_) usleep(0);
	  assert(!soundFileList_.empty());
	  return soundFileList_;
	}
	std::vector<std::string>& musicFileList() {
	  while (!initialized_) usleep(0);
	  return musicFileList_;
	}

	const std::string& operator [] (ImageFileType ft) {
	  while (!initialized_) usleep(0);
	  assert(ft < imageFileList_.size());
	  return imageFileList_[ft];
	}
  };

}
#endif
