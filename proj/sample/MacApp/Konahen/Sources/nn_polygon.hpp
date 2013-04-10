
#pragma once

#include <vector>
#include "co_vec2.hpp"

namespace ngs {

template <typename T>
class Polygon {
	std::vector<Vec2<T> > vtx_;

public:
	void setVtx(const Vec2<T>& pos)
	{
		vtx_.push_back(pos);
	}
	void setVtx(const std::vector<Vec2<T> >& vtx)
	{
		vtx_ = vtx;
	}
	const std::vector<Vec2<T> >& vtx() const
	{
		return vtx_;
	}
	
	void clear()
	{
		vtx_.clear();
	}
	
	size_t numVtx() const
	{
		return vtx_.size();
	}

	bool intension(const Vec2<T>& pos)
	{
		int cou = 0;
		size_t num = numVtx();
		for(size_t i = 0; i < num; ++i)
		{
			T x1, y1, x2, y2;

			x1 = vtx_[i].x;
			y1 = vtx_[i].y;

			int idx = ((i + 1) != num) ? i + 1 : 0;
			x2 = vtx_[idx].x;
			y2 = vtx_[idx].y;

			if(y1 > y2)
			{
				std::swap(x1, x2);
				std::swap(y1, y2);
			}

			if(pos.y >= y1 && pos.y < y2)
			{
        // y = pos.y の位置でのスキャンライン上の交差位置を求める
        T xx = (x2 - x1) * (pos.y - y1) / (y2 - y1) + x1;
        if(xx < pos.x) ++cou;
			}
		}
		return (cou & 1);
	}
	
};

}
