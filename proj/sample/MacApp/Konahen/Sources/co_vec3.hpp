//
// ベクトル
//
#pragma once

#include <cmath>

namespace ngs {

	template <typename Type>
	class Vec3 {
		Type _sin(Type a);
		Type _cos(Type a);
		Type _sqrt(Type a);
		Type _acos(Type a);
		// FIXME:intだけstd::sqrt()とかの呼び出しが曖昧になるための措置

	public:
		Type x, y, z;

		Vec3() : x(static_cast<Type>(0)), y(static_cast<Type>(0)), z(static_cast<Type>(0)) {}
		Vec3(Type xVal, Type yVal, Type zVal) : x(xVal), y(yVal), z(zVal) {}

		void set(Type xVal, Type yVal, Type zVal) {
			x = xVal;
			y = yVal;
			z = zVal;
		}
		
		Type dist(Vec3& src) {						// FIXME:constにできない
			Type dx(x - src.x);
			Type dy(y - src.y);
			Type dz(z - src.z);
			return _sqrt(dx * dx + dy * dy + dz *dz);
		}

		Type length() {								// FIXME:constにできない
			Type a = x * x + y * y + z * z;
			return _sqrt(a);
		}

		void scale(const Type s) {
			x = x * s;
			y = y * s;
			z = z * s;
		}

		void unit() {
			Type l = length();
			if(l > static_cast<Type>(0))
			{
				x = x / l;
				y = y / l;
				z = z / l;
			}
			else
			{
				x = y = z = static_cast<Type>(0);
			}
		}

		Type dot(const Vec3& src) const {
			return x * src.x + y * src.y + z * src.z;
		}

		Vec3 cross(const Vec3& src) const {
			return Vec3(y * src.z - z * src.y, z * src.x - x * src.z, x * src.y - y * src.x);
		}

		Vec3 operator-() const {
			return Vec3(-x, -y, -z);
		}

		Vec3 operator+(const Vec3& vec) const {
			return Vec3(x + vec.x, y + vec.y, z + vec.z);
		}

		Vec3& operator+=(const Vec3& vec) {
			x += vec.x;
			y += vec.y;
			z += vec.z;
			return *this;
		}

		Vec3 operator-(const Vec3& vec) const {
			return Vec3(x - vec.x, y - vec.y, z - vec.z);
		}

		Vec3& operator-=(const Vec3& vec) {
			x -= vec.x;
			y -= vec.y;
			z -= vec.z;
			return *this;
		}

		Vec3 operator*(const Type s) const {
			return Vec3(x * s, y * s, z * s);
		}

		Vec3& operator*=(const Type s) {
			scale(s);
			return *this;
		}

		Vec3 operator/(const Type s) const {
			return Vec3(x / s, y / s, z / s);
		}

		Vec3& operator/=(const Type s) {
			x /= s;
			y /= s;
			z /= s;
			return *this;
		}
	};

	// intだけstd::関数の呼び出しが曖昧になるための措置
	template<typename Type> Type Vec3<Type>::_sin(Type a) {
		return std::sin(a);
	}
	template<> int Vec3<int>::_sin(int a) {
		return static_cast<int>(std::sin(static_cast<float>(a)));
	}

	template<typename Type> Type Vec3<Type>::_cos(Type a) {
		return std::cos(a);
	}
	template<> int Vec3<int>::_cos(int a) {
		return static_cast<int>(std::cos(static_cast<float>(a)));
	}

	template<typename Type> Type Vec3<Type>::_sqrt(Type a) {
		return std::sqrt(a);
	}
	template<> int Vec3<int>::_sqrt(int a) {
		return static_cast<int>(std::sqrt(static_cast<float>(a)));
	}

	template<typename Type> Type Vec3<Type>::_acos(Type a) {
		return std::acos(a);
	}
	template<> int Vec3<int>::_acos(int a) {
		return static_cast<int>(std::acos(static_cast<float>(a)));
	}
	
}

