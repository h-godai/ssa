//=====================================================================//
/*!	@file
	@brief	リソース・クラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "resource.hpp"
#include "IGLcore.hpp"
#include "Iimg.hpp"

namespace core {

	using namespace gl;

	static const char* se_names[] = {
		0
	};

	static resource* g_res = 0;

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	リソースジュールの生成
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	void create_resource()
	{
		if(g_res == 0) {
			g_res = new resource;
		}
		g_res->initialize();
	}

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	リソースモジュールを返す
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	resource* get_resource()
	{
		return g_res;
	}

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	リソースモジュールの廃棄
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	void destroy_resource()
	{
		delete g_res;
		g_res = 0;
	}

	using namespace img;

	//-----------------------------------------------------------------//
	/*!
		@brief	初期化
	 */
	//-----------------------------------------------------------------//
	void resource::initialize()
	{
		m_png.initialize();
		m_sound.initialize(8);

		IGLcore* Igl = get_glcore();
		if(Igl == 0) return;

		fio::fpathman fpm = Igl->at_fpathman();

		std::string s;
		fpm.previous();
		const fio::wstring& w = fpm.get_path();
		fio::utf16_to_utf8(w.c_str(), s);

		const char* p;
		int i = 0;
		while((p = se_names[i]) != 0) {
			std::string ss;
			ss = s;
#ifndef IPHONE_IPAD
			ss += "/se/";
#endif
			ss += p;
			ss += ".wav";
//			printf("SE: '%s'\n", ss.c_str());
			m_sound.load_se(ss.c_str());
			++i;
		}

	}


	//-----------------------------------------------------------------//
	/*!
		@brief	モーション・オブジェクトを作成してハンドルを返す
		@param[in]	mobj	glmobj を渡す。
		@param[in]	base_name	ベース名
		@return	glmobj のハンドル
	 */
	//-----------------------------------------------------------------//
	gl::glmobj::handle resource::create_mobj(gl::glmobj& mobj, const char* base_name)
	{
		IGLcore* Igl = get_glcore();
		if(Igl == 0) return 0;
		std::string s;
		fio::fpathman fpm = Igl->at_fpathman();		
		fpm.previous();
		const fio::wstring& w = fpm.get_path();
		fio::utf16_to_utf8(w.c_str(), s);
		
		fio::fileio f;
//		s = "../res/";
		s += "/";
		s += base_name;
		s += ".png";
		if(!f.open(s.c_str(), "rb")) {
			printf("Can't open input file: '%s'\n", s.c_str());
			return 0;
		}
		m_png.load(f);
		f.close();

		gl::glmobj::handle h = 0;
		const Iimg* imf = m_png.get_image_if();
		if(imf) {
			h = mobj.install(imf);
		}
		return h;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	廃棄
	 */
	//-----------------------------------------------------------------//
	void resource::destroy()
	{
		m_sound.destroy();
		m_png.destroy();
	}


}
