//
// 雑多な処理
//
#pragma once

#define HIT_EPSIRON  (0.00001)

#include "co_line.hpp"
#include "co_vec3.hpp"

namespace ngs {

// 切り上げて一番近い２のべき乗値を求める
int int2pow(int value) {
	int res = 1;

	while(res < (1 << 30))
	{
		if(res >= value)
		break;
		res *= 2;
	}

	return res;
}

// 指定値で切り上げ
//   ex)  ceilingvalue(14, 8) = 16
int ceilingvalue(int a, int b) {
	return (a % b) ? a + b - (a % b) : a;
}

// ビット数をカウントする
int countBitValue(u_int value) {
	int num = 0;
	u_int mask;

	mask = 0x1;
	for(int i = 0; i < ((int)sizeof(u_int) * 8); ++i)
	{
		if(value & mask)
		{
			++num;
		}
		mask <<= 1;
	}


	return num;
}

/* 一番下位のビットを求める */
int getLowBitValue(u_int value) {
	int i;
	u_int mask = 0x1;
	for(i = 0; i < ((int)sizeof(u_int) * 8); i += 1)
	{
		if(mask & value) break;
		mask <<= 1;
	}
	return i;
}

// リトルエンディアン値をCPUのエンディアンにして返す
u_int getIntValue(u_int value) {
#ifdef __BIG_ENDIAN__
	value = (value << 24) | ((value << 8) & 0xff0000) | ((value >> 8) & 0xff00) | (value >> 24);
#endif
	return value;
}

float Rad2Ang(float rad)
{
	return rad * 180.0f / PI;
}

float Ang2Rad(float ang)
{
	return ang * PI / 180.0f;
}

template<typename Type> bool checkCrossLine(const Line<Type>& l1, const Line<Type>& l2) {
	Type val;

	if(l1.start.x >= l1.end.x)
	{
		if(((l1.start.x < l2.start.x) && (l1.start.x < l2.end.x)) || ((l1.end.x > l2.start.x) && (l1.end.x > l2.end.x)))
		{
			return false;
		}
	}
	else
	{
		if(((l1.end.x < l2.start.x) && (l1.end.x < l2.end.x)) || ((l1.start.x > l2.start.x) && (l1.start.x > l2.end.x)))
		{
			return false;
		}
	}
	if(l1.start.y >= l1.end.y)
	{
		if(((l1.start.y < l2.start.y) && (l1.start.y < l2.end.y)) || ((l1.end.y > l2.start.y) && (l1.end.y > l2.end.y)))
		{
			return false;
		}
	}
	else
	{
		if(((l1.end.y < l2.start.y) && (l1.end.y < l2.end.y)) || ((l1.start.y > l2.start.y) && (l1.start.y > l2.end.y)))
		{
			return false;
		}
	}

	val = (Type)((l1.start.x - l1.end.x) * (l2.start.y - l1.start.y) + (l1.start.y - l1.end.y) * (l1.start.x - l2.start.x)) * (Type)((l1.start.x - l1.end.x) * (l2.end.y - l1.start.y) + (l1.start.y - l1.end.y) * (l1.start.x - l2.end.x));
	if(val > 0)
	{
		return false;
	}
	val = (Type)((l2.start.x - l2.end.x) * (l1.start.y - l2.start.y) + (l2.start.y - l2.end.y) * (l2.start.x - l1.start.x)) * (Type)((l2.start.x - l2.end.x) * (l1.end.y - l2.start.y) + (l2.start.y - l2.end.y) * (l2.start.x - l1.end.x));
	if(val > 0)
	{
		return false;
	}

	return true;
}
	
template<typename Type> bool crossLine(Vec2<Type>& res, const Line<Type>& line1, const Line<Type>& line2) {
	Type a1, a2, b1, b2, c1, c2;
	Type d;

	a1 = line1.end.y - line1.start.y;									// 直線の方程式(ax + by + c = 0)でのパラメータを求める
	b1 = line1.start.x - line1.end.x;
	c1 = -a1 * line1.start.x - b1 * line1.start.y;

	a2 = line2.end.y - line2.start.y;
	b2 = line2.start.x - line2.end.x;
	c2 = -a2 * line2.start.x - b2 * line2.start.y;

	d = b1 * a2 - b2 * a1;

	if(d == 0) return false;													// 平行
		
	res.x = (c1 * b2 - c2 * b1) / d;									// 交点を計算
	res.y = (a1 * c2 - a2 * c1) / d;

	return true;
}

template<typename Type> bool crossLine2(Vec2<Type>& res, const Line<Type>& line1, const Line<Type>& line2) {
	Vec2<Type> v = line2.start - line1.start;
	Vec2<Type> l1v = line1.end - line1.start;;
	Vec2<Type> l2v = line2.end - line2.start;;

	Type crs_v1_v2 = l1v.cross(l2v);
	if(crs_v1_v2 == static_cast<Type>(0)) return false;

	Type crs_v_v1 = v.cross(l1v);
	Type crs_v_v2 = v.cross(l2v);
	Type t1 = crs_v_v2 / crs_v1_v2;
	Type t2 = crs_v_v1 / crs_v1_v2;

	Type eps = static_cast<Type>(0.000001);
	if(t1 + eps < static_cast<Type>(0) || t1 - eps > static_cast<Type>(1) || t2 + eps < static_cast<Type>(0) || t2 - eps > static_cast<Type>(1))
	{
		return false;
	}

	res = line1.start + l1v * t1;
	return true;
}

bool sphereRayCollision(const float r, const Vec3<float>& center, const Vec3<float>& s, const Vec3<float>& v, Vec3<float>& hit_pos, float& hit_t)
{
	Vec3<float> u = s - center;

	float a = v.dot(v);
	float b = v.dot(u);
	float c = u.dot(u) - r * r;

	if(a - HIT_EPSIRON <= 0.0)
	{
		return false;
	}

	float isColli = b * b - a * c;
	if(isColli < 0.0)
	{
		return false;
	}

	hit_t = (-b - sqrt(b * b - a * c)) / a;
	hit_pos = s + v * hit_t;

	return true;
}

template<typename T> Vec2<T> calcAngle(const Vec3<T> vec)
{
	
}


class Bezier {
	float st_, ed_, p1_, p2_;
public:
	Bezier() : st_(0.0f), ed_(0.0f), p1_(0.0f), p2_(0.0f) {}
	Bezier(float st, float ed, float p1, float p2) : st_(st), ed_(ed), p1_(p1), p2_(p2) {}

	void set(float st, float ed, float p1, float p2) {
		st_ = st;
		ed_ = ed;
		p1_ = p1;
		p2_ = p2;
	}

	float pos(float t) {
		return (1.0f - t) * (1.0f - t) * (1.0f - t) * st_ + 3.0f * t * (1.0f - t) * (1.0f - t) * p1_ + 3.0f * t * t * (1.0f - t) * p2_ + t * t * t * ed_;
	}

};

float BezierLength(Bezier& bx, Bezier& by, int div) {
	float len = 0.0f;
	float t = 0.0f;
	float dt = 1.0f / div;

	for(int i = 0; i < div; ++i)
	{
		float x0 = bx.pos(t);
		float y0 = bx.pos(t);
		float x1 = bx.pos(t + dt);
		float y1 = bx.pos(t + dt);

		float l = (x1 - x0) * (x1 - x0) + (y1 - y0) * (y1 - y0);
		if(l > 0.0f) len += sqrtf(l);

		t += dt;
	}
		
	return len;
}

}
