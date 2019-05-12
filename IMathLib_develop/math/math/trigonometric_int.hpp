#ifndef IMATH_MATH_MATH_TRIGONOMETRIC_INT_HPP
#define IMATH_MATH_MATH_TRIGONOMETRIC_INT_HPP

#include "IMathLib/math/math/math_traits.hpp"
#include "IMathLib/math/math/exp_int.hpp"
#include "IMathLib/math/math/ln2.hpp"
#include "IMathLib/math/math/log.hpp"

namespace iml {

	//正弦積分
	template <class T>
	struct Sin_int {
		using result_type = typename math_function_type<T>::type;

		static constexpr result_type _sin_int_(const T& x) {
			return (x == 0) ? 0 : exp_int(log(x));
		}
	};
	template <class T>
	inline constexpr auto sin_int(const T& x) { return Sin_int<T>::_sin_int_(x); }

	//余弦積分
	template <class T>
	struct Cos_int {
		using result_type = typename math_function_type<T>::type;

		static constexpr result_type _cos_int_(const T& x) {
			result_type c = x * x;
			result_type x1 = -c / 2, x2 = -c / 4, x3 = 0;

			for (size_t i = 2; !error_evaluation(x2, x3); ++i) {
				x3 = x2;
				x1 *= -c / (2 * i*(2 * i - 1));
				x2 += x1 / (2 * i);
			}
			return euler_constant<result_type> + log(x) + x2;
		}
	};
	template <class T>
	inline constexpr auto cos_int(const T& x) { return Cos_int<T>::_cos_int_(x); }

}


#endif