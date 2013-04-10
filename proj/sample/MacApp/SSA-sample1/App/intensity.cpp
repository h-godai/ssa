//=====================================================================//
/*!	@file
	@brief	Intensity（強度[輝度]）クラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "intensity.hpp"

namespace gl {

	//-----------------------------------------------------------------//
	/*!
		@brief	初期化
		@param[in]	rate	フレームレートを設定[Hz]
	 */
	//-----------------------------------------------------------------//
	void intensity::initialize(float rate)
	{
		m_inten_min = 0.0f;
		m_inten_max = 1.0f;
		m_frame_rate = rate;
		m_into_min = false;
		m_into_max = false;

		m_flash_count = 0;

		m_cycle_shade_speed = 0.1f;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	初期値と速度の設定
		@param[in]	first	輝度の初期値（0.0 to 1.0)
		@param[in]	limit	輝度の最終値（0.0 to 1.0)
		@param[in]	time	輝度変化の速度
	 */
	//-----------------------------------------------------------------//
	void intensity::set_level_time(float first, float limit, float time)
	{
		if(first < limit) {
			m_inten_min = first;
			m_inten_max = limit;
		} else {
			m_inten_min = limit;
			m_inten_max = first;
		}
		m_inten_level = first;
		m_speed = (limit - first) * (1.0f / (m_frame_rate * time));
// printf("%f\n", m_speed);
		m_into_min = false;
		m_into_max = false;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	毎フレームサービス
	 */
	//-----------------------------------------------------------------//
	void intensity::service()
	{
		m_inten_level += m_speed;

		if(m_speed < 0.0f && m_inten_level < m_inten_min) {
			m_inten_level = m_inten_min;
			m_into_min = true;
		} else if(m_speed > 0.0f && m_inten_level > m_inten_max) {
			m_inten_level = m_inten_max;
			m_into_max = true;
		} else {
			m_into_min = false;
			m_into_max = false;
		}

		// フラッシュ・カラー・アニメーション
		{
			unsigned char r, g, b, a;
			a = 255;
			unsigned int f = m_flash_count + 1;
			if(f & 1) r = 255; else r = 0;
			if(f & 2) g = 255; else g = 0;
			if(f & 4) b = 255; else b = 0;
			m_flash_color.set(r, g, b, a);
			++m_flash_count;
			if(m_flash_count >= 7) m_flash_count = 0;
		}

		// サイクル・シェード・レベル
		{
			m_cycle_shade_level += m_cycle_shade_speed;
			if(m_cycle_shade_level < 0.0f) {
				m_cycle_shade_speed = fabs(m_cycle_shade_speed);
				m_cycle_shade_level = 0.0f;
			} else if(m_cycle_shade_level >= 1.0f) {
				m_cycle_shade_speed = -fabs(m_cycle_shade_speed);
				m_cycle_shade_level = 1.0f;
			}
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	廃棄
	 */
	//-----------------------------------------------------------------//
	void intensity::destroy()
	{
	}

}

