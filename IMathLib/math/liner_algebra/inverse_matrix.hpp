#ifndef IMATH_MATH_LINER_ALGEBRA_INVERSE_MATRIX_HPP
#define IMATH_MATH_LINER_ALGEBRA_INVERSE_MATRIX_HPP

#include "IMathLib/math/liner_algebra/matrix.hpp"
#include "IMathLib/math/liner_algebra/identity_matrix.hpp"


namespace iml {

	//逆行列(ガウスの消去法)
	template <class T, size_t N>
	inline matrix<T, N, N> inverse_matrix(const matrix<T, N, N>& x) {
		return multiplicative_inverse(x);
	}

}

#endif