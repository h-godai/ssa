//=====================================================================//
/*!	@file
	@brief	メイン・ルート・クラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "mainroot.hpp"
#include "logo.hpp"
//#include "root_menu.hpp"
//#include "music_player.hpp"

using namespace gl;
using namespace core;

//-----------------------------------------------------------------//
/*!
	@brief	初期化
*/
//-----------------------------------------------------------------//
void mainroot::initialize()
{
	create_resource();

	m_next_scene = render_logo;
//	m_next_scene = mus_player;
}


//-----------------------------------------------------------------//
/*!
	@brief	毎フレームサービス
	@return 終了なら「true」を返す
*/
//-----------------------------------------------------------------//
bool mainroot::service()
{
	IGLcore* Igl = get_glcore();
	if(Igl == 0) return true;

	// 何かエラーが発生したら、何もしない〜
	if(m_next_scene == error) {
		return false;
	}

	resource* res = get_resource();
	if(res == 0) return false;
	res->service();

	// 次のシーンを準備
	if(m_next_scene != continuance) {
		delete m_scene;
		m_scene = 0;

		bool err = false;
		switch(m_next_scene) {
		case error:
#ifndef NDEBUG
			printf("scene return type error......\n");
			fflush(stdout);
#endif
			break;

		case render_logo:
			m_scene = dynamic_cast<IScene*>(new logo);
			m_scene->initialize();
			break;

#if 0
		case render_root_menu:
			m_scene = dynamic_cast<IScene*>(new root_menu);
			m_scene->initialize();
			break;

		case mus_player:
			m_scene = dynamic_cast<IScene*>(new music_player);
			m_scene->initialize();
			break;
#endif

		default:
			break;
		}
		if(err) {
			m_next_scene = error;
		} else {
			m_next_scene = continuance;
		}
		return false;
	}

	if(m_scene) {
		bool render = true;
		switch(m_scene->get_scene_type()) {

		case render_logo:
#if 0
		case render_root_menu:
		case mus_player:
#endif
			break;
		case continuance:
		default:
			render = false;
			break;
		}

		if(render) {
			m_next_scene = m_scene->render();
		}

	}
	return false;
}


//-----------------------------------------------------------------//
/*!
	@brief	廃棄
*/
//-----------------------------------------------------------------//
void mainroot::destroy()
{
	if(m_scene) {
		delete m_scene;
		m_scene = 0;
	}
	destroy_resource();
}


