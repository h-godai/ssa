//
// ベクトル
//
#pragma once

#include <cmath>

namespace ngs {

template <typename Type>
class Vec2 {
	Type _sin(Type a);
	Type _cos(Type a);
	Type _sqrt(Type a);
	Type _acos(Type a);
	// FIXME:intだけstd::sqrt()とかの呼び出しが曖昧になるための措置

public:
	Type x, y;

	Vec2() : x(static_cast<Type>(0)), y(static_cast<Type>(0)) {}
	Vec2(Type xVal, Type yVal) : x(xVal), y(yVal) {}

	void set(Type xVal, Type yVal) {
		x = xVal;
		y = yVal;
	}
		
	Type dist(const Vec2& src) {											// FIXME:constにできない
		Type dx(x - src.x);
		Type dy(y - src.y);
		return _sqrt(dx * dx + dy * dy);
	}

	Type length() {																		// FIXME:constにできない
		Type a = x * x + y * y;
		return _sqrt(a);
	}

	void scale(const Type s) {
		x = x * s;
		y = y * s;
	}

	void unit() {
		Type l = length();
		if(l > static_cast<Type>(0))
		{
			x = x / l;
			y = y / l;
		}
		else
		{
			x = y = static_cast<Type>(0);
		}
	}

	Type dot(const Vec2& src) const {
		return x * src.x + y * src.y;
	}

	Type cross(const Vec2& src) const {
		return x * src.y - src.x * y;
	}

	void rotate(const Type r) {
		Type sin_r = _sin(r);
		Type cos_r = _cos(r);

		Type _x = x;
		Type _y = y;
		x = _x * cos_r - _y * sin_r;
		y = _x * sin_r + _y * cos_r;
	}

	Type angle(Vec2& src) {
		Type l1 = length();
		Type l2 = src.length();
		if((l1 <= static_cast<Type>(0)) || (l2 <= static_cast<Type>(0)))
		{
			return static_cast<Type>(0);
			// どちらかのベクトルの長さがゼロの場合、角度は０
		}
			
		Type value = dot(src) / (l1 * l2);
		Type r = static_cast<Type>(0);
		if(value < static_cast<Type>(1)) r = _acos(value);
			
		return r;
	}

	Type angleY() {
		Vec2 src(0, -1);
		Type r = angle(src);
		if(x < static_cast<Type>(0)) r = -r;

		return r;
	}

	Type angleX() {
		Vec2 src(1, 0);
		Type r = angle(src);
		if(y < static_cast<Type>(0)) r = -r;

		return r;
	}

	Vec2 normal() const {
		Vec2 dst(-y, x);
		dst.unit();
		return dst;
	}

	Vec2 refrect(const Vec2& src) const {
		Vec2 n = normal();
		float d = n.dot(src) * static_cast<Type>(2);
		float rx = src.x - d * n.x;
		float ry = src.y - d * n.y;
		Vec2 res(rx, ry);
		return res;
	}

	Vec2 parallel(const Vec2& src) const {
		Vec2 n = normal();
		float d = n.dot(src);
		float rx = src.x - d * n.x;
		float ry = src.y - d * n.y;
		Vec2 res(rx, ry);
		return res;
	}

	Vec2 operator-() const {
		return Vec2(-x, -y);
	}

	Vec2 operator+(const Vec2& vec) const {
		return Vec2(x + vec.x, y + vec.y);
	}

	Vec2& operator+=(const Vec2& vec) {
		x += vec.x;
		y += vec.y;
		return *this;
	}

	Vec2 operator-(const Vec2& vec) const {
		return Vec2(x - vec.x, y - vec.y);
	}

	Vec2& operator-=(const Vec2& vec) {
		x -= vec.x;
		y -= vec.y;
		return *this;
	}

	Vec2 operator*(const Type s) const {
		return Vec2(x * s, y * s);
	}

	Vec2& operator*=(const Type s) {
		scale(s);
		return *this;
	}

	Vec2 operator/(const Type s) const {
		return Vec2(x / s, y / s);
	}

	Vec2& operator/=(const Type s) {
		x /= s;
		y /= s;
		return *this;
	}
};

// intだけstd::関数の呼び出しが曖昧になるための措置
template<typename Type> Type Vec2<Type>::_sin(Type a) {
	return std::sin(a);
}
template<> int Vec2<int>::_sin(int a) {
	return std::sin(static_cast<float>(a));
}

template<typename Type> Type Vec2<Type>::_cos(Type a) {
	return std::cos(a);
}
template<> int Vec2<int>::_cos(int a) {
	return std::cos(static_cast<float>(a));
}

template<typename Type> Type Vec2<Type>::_sqrt(Type a) {
	return std::sqrt(a);
}
template<> int Vec2<int>::_sqrt(int a) {
	return std::sqrt(static_cast<float>(a));
}

template<typename Type> Type Vec2<Type>::_acos(Type a) {
	return std::acos(a);
}
template<> int Vec2<int>::_acos(int a) {
	return std::acos(static_cast<float>(a));
}
	
}

