//
// クオータニオン
// ※型はfloatもしくはdoubleで使ってください
//
#pragma once

#include <cmath>
#include "co_vec3.hpp"

namespace ngs {

	template <typename T>
	class Quat {
	public:
		T x, y, z, w;

		Quat() : x(static_cast<T>(0)), y(static_cast<T>(0)), z(static_cast<T>(0)), w(static_cast<T>(1)) {}
		Quat(T xVal, T yVal, T zVal, T wVal) : x(xVal), y(yVal), z(zVal), w(wVal) {}

		void set(T xVal, T yVal, T zVal, T wVal) {
			x = xVal;
			y = yVal;
			z = zVal;
			w = wVal;
		}

		void rotate(T r, Vec3<T>& vct) {
			T sin_r = std::sin(r / static_cast<T>(2));

			x = vct.x * sin_r;
			y = vct.y * sin_r;
			z = vct.z * sin_r;
			w = std::cos(r / static_cast<T>(2));
		}

		Quat operator*(const Quat& src) const {
			T x_ = w * src.x + x * src.w + y * src.z - z * src.y;
			T y_ = w * src.y - x * src.z + y * src.w + z * src.x;
			T z_ = w * src.z + x * src.y - y * src.x + z * src.w;
			T w_ = w * src.w - x * src.x - y * src.y - z * src.z;
			return Quat(x_, y_, z_, w_);
		}

		Quat& operator*=(const Quat& src) {
			T x_ = w * src.x + x * src.w + y * src.z - z * src.y;
			T y_ = w * src.y - x * src.z + y * src.w + z * src.x;
			T z_ = w * src.z + x * src.y - y * src.x + z * src.w;
			T w_ = w * src.w - x * src.x - y * src.y - z * src.z;

			x = x_;
			y = y_;
			z = z_;
			w = w_;

			return *this;
		}
	};

}

