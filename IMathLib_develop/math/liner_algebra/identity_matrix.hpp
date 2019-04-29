#ifndef _IMATH_LINER_ALGEBRA_IDENTITY_MATRIX_HPP
#define _IMATH_LINER_ALGEBRA_IDENTITY_MATRIX_HPP

#include "IMathLib/math/liner_algebra/vector.hpp"
#include "IMathLib/math/liner_algebra/matrix.hpp"


namespace iml {

	//単位行列
	template <class T, size_t N>
	inline matrix<T, N, N> identity_matrix() {
		return multiplication_traits<matrix<T, N, N>>::identity_element();
	}


	//スカラー行列
	template <class T, size_t N>
	inline matrix<T, N, N> scalar_matrix(const T& a) {
		matrix<T, N, N> temp;
		for (size_t i = 0; i < N; ++i) temp[i][i] = a;
		return temp;
	}


	//スケール行列
	template <class T, size_t N>
	inline matrix<T, N, N> scale_matrix(const vector<T, N>& v) {
		matrix<T, N, N> temp;
		for (size_t i = 0; i < N; ++i) temp[i][i] = v[i];
		return temp;
	}
}

#endif