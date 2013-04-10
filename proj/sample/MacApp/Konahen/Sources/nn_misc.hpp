
#pragma once

#include <cmath>
#include "co_vec2.hpp"
#include "co_vec3.hpp"

namespace ngs {

// 3D→経度・緯度
Vec2<float> posToLoc(const Vec3<float>& pos)
{
	Vec3<float> v = pos;
	v.unit();

	float y = asin(v.y);
	float r = sqrt(1.0 - v.y * v.y);
	float x = (r > 0.0) ? asin(v.z / r) : 0.0;
	if(v.x > 0.0) x = ((x > 0) ? PI : -PI) - x;

	return Vec2<float>(x, y);
}

// 経度・緯度→2D
Vec2<float> locToPos2d(const Vec2<float> loc)
{
	float x = loc.x / -PI;
	float y = loc.y / (-PI / 2.0);

	return Vec2<float>(x, y);
}

// 経度・緯度→3D
Vec3<float> locToPos3d(const Vec2<float> loc)
{
	float y = sin(loc.y);
	float r = cos(loc.y);
	float z = sin(loc.x) * r;
	float x = cos(loc.x) * r;
		
	return Vec3<float>(-x, y, z);
}

}
