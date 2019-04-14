#ifndef IMATH_MATH_LINER_ALGEBRA_EXP_HPP
#define IMATH_MATH_LINER_ALGEBRA_EXP_HPP

#include "IMathLib/math/math.hpp"
#include "IMathLib/math/liner_algebra/matrix.hpp"
#include "IMathLib/math/liner_algebra/identity_matrix.hpp"


namespace iml {

	//行列指数関数
	template <class T, size_t N>
	struct Exp<matrix<T, N, N>> {
		using result_type = typename math_function_type<matrix<T, N, N>>::type;
		using type = typename math_function_type<T>::type;

		static constexpr result_type __exp(const matrix<T, N, N>& ma) {
			result_type x1 = identity_matrix<type, N>(), x2 = identity_matrix<type, N>(), x3{};
			for (size_t i = 1; !error_evaluation(x2, x3); ++i) {
				x3 = x2;
				x1 *= (x / i);
				x2 += x1;
			}
			return x2;
		}
	};
}

#endif