#ifndef _IMATH_LINER_ALGEBRA_PRODUCT_MATRIX_HPP
#define _IMATH_LINER_ALGEBRA_PRODUCT_MATRIX_HPP

#include "IMathLib/math/liner_algebra/vector.hpp"
#include "IMathLib/math/liner_algebra/matrix.hpp"


namespace iml {

	//回転行列
	template <class T>
	inline matrix3<T> product_matrix(const vector3<T>& v) {
		return matrix3<T>(
			0, -v[2], v[1]
			, v[2], 0, -v[0]
			, -v[1], v[0], 0
			);
	}
}

#endif