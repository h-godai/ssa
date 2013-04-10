//======================================== -*-tab-width:4-*- ==========//
/*!	@file
	@brief	ロゴ・クラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "logo.hpp"
#include "resource.hpp"

namespace core {

	using namespace img;
	using namespace gl;

	//-----------------------------------------------------------------//
	/*!
		@brief	初期化
	 */
	//-----------------------------------------------------------------//
	void logo::initialize()
	{
	  m_mobj.initialize(1024,1024); // 1024x1024ピクセルのテクスチャサイズで管理
	  m_button.initialize();

	  resource* res = get_resource(); // システムリソースのコンテキストを取得
	  if(res) {
		m_logo = res->create_mobj(m_mobj, "Logo"); // "Logo.png"の読み込み
		gl::glmobj::handle h = res->create_mobj(m_mobj, "playbutton"); // "playbutton.png"の読み込み
		//const Iimg* imf = res->load_image("playbutton");
		m_play = m_button.add(h, 384,384);
	  }

	  m_inten.initialize(60.0f);
	  m_inten.set_level_time(0.0f, 1.0f, 1.0f);	// 初期の輝度
	  
	  m_hold_time = 0;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	レンダリング
		@return 次の要求シーン・タイプ
	 */
	//-----------------------------------------------------------------//
	scene_type logo::render()
	{
		m_mobj.setup_matrix(1024, 768);

		::glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
//		::glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
//		::glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
//		::glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

		if(m_inten.is_max()) {
			++m_hold_time;
			if(m_hold_time > (3 * 60)) {
				//m_inten.set_level_time(1.0f, 0.0f, 1.5f);
			}
		}

		if(m_inten.is_min()) {
			//return render_root_menu;
		}

		m_inten.set_inten(img::rgbaf(1.0f, 1.0f, 1.0f));
		m_inten.service();

		::glEnable(GL_TEXTURE_2D);

		m_mobj.draw(m_logo, glmobj::normal, true, 0, 0);
		// m_mobj.draw(m_play, glmobj::normal, true, 256, 256);
		button::handle h = m_button.render(m_mobj, m_inten);
		if (h != 0) {
		  printf("button pushed %d\n", h);
		}

		return continuance;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	廃棄
	 */
	//-----------------------------------------------------------------//
	void logo::destroy()
	{
	}

}
