#ifndef _IMATH_MATH_DIRICHELT_ETA_HPP
#define _IMATH_MATH_DIRICHELT_ETA_HPP

#include "IMathLib/math/math/math_traits.hpp"
#include "IMathLib/math/math/pow.hpp"

namespace iml {

	//ディリクレのイータ関数
	template <class T>
	struct _Dirichlet_eta {
		using result_type = typename math_function_type<T>::type;

		static constexpr result_type __dirichlet_eta(const T& x) {
			result_type x2 = 0.5, x1 = 0.5, x3 = 1, x4 = 1, x5 = 0;

			for (size_t i = 1; !error_evaluation(x2, x5); ++i) {
				x5 = x2;
				x1 /= 2;
				x3 = x4 = x1;
				for (size_t j = 1; j <= i; ++j) {
					//x3 *= -result_type(i + 1 - j) / j;
					//x4 += x3 * pow(j + 1, -x);
					x3 *= -pow(result_type(j) / (j + 1), x)*(i + 1 - j) / j;
					x4 += x3;
				}
				x2 += x4;
			}
			return x2;
		}
	};
	template <class T>
	inline constexpr auto dirichlet_eta(const T& x) { return _Dirichlet_eta<T>::__dirichlet_eta(x); }

}


#endif