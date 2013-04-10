//
// ベクトル
//
#pragma once

namespace ngs {

	template <typename Type>
	class Vec4 {
	public:
		Type x, y, z, w;

		Vec4() : x(static_cast<Type>(0)), y(static_cast<Type>(0)), z(static_cast<Type>(0)), w(static_cast<Type>(0)) {}
		Vec4(Type xVal, Type yVal, Type zVal, Type wVal) : x(xVal), y(yVal), z(zVal), w(wVal) {}

		void set(Type xVal, Type yVal, Type zVal, Type wVal) {
			x = xVal;
			y = yVal;
			z = zVal;
			w = wVal;
		}
	};

}

