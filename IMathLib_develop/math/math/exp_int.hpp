#ifndef IMATH_MATH_MATH_EXP_INT_HPP
#define IMATH_MATH_MATH_EXP_INT_HPP

#include "IMathLib/math/math/math_traits.hpp"
#include "IMathLib/math/math/abs.hpp"
#include "IMathLib/math/math/log.hpp"
#include "IMathLib/math/math/gamma.hpp"

namespace iml {

	//指数積分
	template <class T>
	struct Exp_int {
		using result_type = typename math_function_type<T>::type;

		static constexpr result_type _exp_int_(const T& x) {
			result_type x1 = x, x2 = x, x3 = 0;

			for (size_t i = 2; !error_evaluation(x2, x3); ++i) {
				x3 = x2;
				x1 *= result_type(x) / i;
				x2 += x1 / i;
			}
			return euler_constant<result_type> + log(abs(x)) + x2;
		}
	};
	template <class T>
	inline constexpr auto exp_int(const T& x) { return Exp_int<T>::_exp_int_(x); }

}


#endif