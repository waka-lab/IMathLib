#ifndef _IMATH_LINER_ALGEBRA_LINER_MAPPING_HPP
#define _IMATH_LINER_ALGEBRA_LINER_MAPPING_HPP

#include "IMathLib/math/math.hpp"
#include "IMathLib/math/liner_algebra/vector.hpp"
#include "IMathLib/math/liner_algebra/matrix.hpp"


namespace iml {

	//線形写像
	template <class T, class S, imsize_t M, imsize_t N>
	inline vector<typename calculation_result<T, S>::mul_type, M> operator*(const matrix<T, M, N>& ma, const vector<S, N>& v) {
		vector<typename calculation_result<T, S>::mul_type, M> temp;
		for (imsize_t i = 0; i < M; ++i) {
			temp[i] = addition<typename calculation_result<T, S>::mul_type>::identity_element();
			for (imsize_t j = 0; j < N; ++j)
				temp[i] += ma[i][j] * v[j];
		}
		return temp;
	}
}

#endif