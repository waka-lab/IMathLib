#ifndef _IMATH_MATH_EXP_INT_HPP
#define _IMATH_MATH_EXP_INT_HPP

#include "IMathLib/math/math/math_traits.hpp"
#include "IMathLib/math/math/abs.hpp"
#include "IMathLib/math/math/log.hpp"
#include "IMathLib/math/math/euler_constant.hpp"

namespace iml {

	//指数積分
	template <class T>
	struct _Exp_int {
		using result_type = typename math_function_type<T>::type;

		static constexpr result_type __exp_int(const T& x) {
			result_type x1 = x, x2 = x, x3 = 0;

			for (imsize_t i = 2; !error_evaluation(x2, x3); ++i) {
				x3 = x2;
				x1 *= result_type(x) / i;
				x2 += x1 / i;
			}
			return _Euler_constant_<result_type>::euler_constant + log(abs(x)) + x2;
		}
	};
	template <class T>
	inline constexpr auto exp_int(const T& x) { return _Exp_int<T>::__exp_int(x); }

}


#endif