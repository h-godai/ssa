
#pragma once

#include <iostream>
#include <vector>
#include "co_vec2.hpp"

namespace ngs {


const int D_CLICK_TIME = 30;												// ダブルクリックとみなす間隔

enum MOUSE_BUTTON {
	MOUSE_LEFT				= (1 << 0),											// マウス左ボタン
	MOUSE_MIDDLE			= (1 << 1),											// マウス中央ボタン
	MOUSE_RIGHT				= (1 << 2),											// マウス右ボタン
	MOUSE_D_LEFT			= (1 << 3),											// マウス左ボタンダブルクリック
	MOUSE_MULTI_TOUCH = (1 << 4),											// マルチタップ
};

enum MOUSE_STATE {
	MOUSE_PUSH,
	MOUSE_PULL,
};


class Mouse
{
	std::vector<Vec2<int> > pos_;
	std::vector<Vec2<int> > dPos_;

	Vec2<int> size_;																	// マウスの取り得る座標
	Vec2<int> disp_ofs_;															// 表示時のオフセット
	Vec2<int> disp_size_;															// 表示時のサイズ

	u_int btn_p_;																			// press
	u_int btn_td_;																		// push
	u_int btn_tu_;																		// pull

	u_int m_btn_p_;
	u_int m_btn_td_;
	u_int m_btn_tu_;
	
	int d_left_count_;																// 左ダブルクリック用のカウンタ
	Vec2<int> l_pos_;																	// 左ダブルクリック用の位置

	
	Mouse() : size_(640, 480), disp_size_(640, 480),
						btn_p_(), btn_td_(), btn_tu_(), m_btn_p_(), m_btn_td_(), m_btn_tu_(),
						d_left_count_()
	{
		pos_.resize(2);
		dPos_.resize(2);
	}

public:
	static Mouse& instance() {
		static Mouse mouse;
		return mouse;
	}

	~Mouse() {}

	void cursorCallBack(const int index, const int x, const int y)
	{
		dPos_[index].set(x, disp_size_.y - y);

		// int mx = (float)(x - disp_ofs_.x) * ((float)size_.x / (float)(disp_size_.x - (disp_ofs_.x * 2)));
		// int my = (float)(y - disp_ofs_.y) * ((float)size_.y / (float)(disp_size_.y - (disp_ofs_.y * 2))); 
		int mx = (float)(x - disp_ofs_.x) * ((float)size_.x / (float)(disp_size_.x - (disp_ofs_.x * 2))) - size_.x / 2;
		int my = (float)(y - disp_ofs_.y) * ((float)size_.y / (float)(disp_size_.y - (disp_ofs_.y * 2))) - size_.y / 2; 
		pos_[index].set(mx, my);
	}

	void cursorCallBack(const int x, const int y)
	{
		cursorCallBack(0, x, y);
	}

	void buttonCallBack(const enum MOUSE_BUTTON button, const enum MOUSE_STATE state)
	{
		switch(state)
		{
		case MOUSE_PUSH:
			{
				m_btn_p_ |= static_cast<u_int>(button);
				m_btn_td_ |= static_cast<u_int>(button);

				if(m_btn_td_ & MOUSE_LEFT)
				{
					if((d_left_count_ > 0) && (l_pos_.dist(pos_[0]) < 2))
					{
						m_btn_td_ &= ~static_cast<u_int>(MOUSE_LEFT);
						m_btn_td_ |= static_cast<u_int>(MOUSE_D_LEFT);
						d_left_count_ = 0;
					}
					else
					{
						d_left_count_ = D_CLICK_TIME;
						l_pos_ = pos_[0];
					}
				}
			}
			break;
			
		case MOUSE_PULL:
			{
				m_btn_p_ &= ~static_cast<u_int>(button);
				m_btn_tu_ |= static_cast<u_int>(button);
			}
			break;
		}
	}

	void resize(const Vec2<int>& size, const Vec2<int>& dispOfs, const Vec2<int>& dispSize)
	{
		size_ = size;
		disp_ofs_ = dispOfs;
		disp_size_ = dispSize;
	}

	void update()
	{
		btn_p_ = m_btn_p_;
		btn_td_ = m_btn_td_;
		btn_tu_ = m_btn_tu_;
		m_btn_td_ = m_btn_tu_ = 0;

		if(d_left_count_ > 0) --d_left_count_;
	}

	const Vec2<int>& pos(const int index) const {
		return pos_[index];
	}
	const Vec2<int>& pos() const {
		return pos(0);
	}
	const std::vector<Vec2<int> >& posAll() const {
		return pos_;
	}
	const Vec2<int>& d_pos(const int index) const {
		return dPos_[index];
	}
	const Vec2<int>& d_pos() const {
		return d_pos(0);
	}

	bool press(const u_int btn) const {
		return (btn_p_ & btn) ? true : false;
	}
	bool push(const u_int btn) const {
		return (btn_td_ & btn) ? true : false;
	}
	bool pull(const u_int btn) const {
		return (btn_tu_ & btn) ? true : false;
	}
};

}
