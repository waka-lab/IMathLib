#ifndef _IMATH_MATH_TRIANGLE_INT_HPP
#define _IMATH_MATH_TRIANGLE_INT_HPP

#include "IMathLib/math/math/math_traits.hpp"
#include "IMathLib/math/math/exp_int.hpp"
#include "IMathLib/math/math/loge_2.hpp"
#include "IMathLib/math/math/log.hpp"

namespace iml {

	//正弦積分
	template <class T>
	struct _Sin_int {
		using result_type = typename math_function_type<T>::type;

		static constexpr result_type __sin_int(const T& x) {
			return (x == 0) ? 0 : exp_int(log(x));
		}
	};
	template <class T>
	inline constexpr auto sin_int(const T& x) { return _Sin_int<T>::__sin_int(x); }

	//余弦積分
	template <class T>
	struct _Cos_int {
		using result_type = typename math_function_type<T>::type;

		static constexpr result_type __cos_int(const T& x) {
			result_type c = x * x;
			result_type x1 = -c / 2, x2 = -c / 4, x3 = 0;

			for (imsize_t i = 2; !error_evaluation(x2, x3); ++i) {
				x3 = x2;
				x1 *= -c / (2 * i*(2 * i - 1));
				x2 += x1 / (2 * i);
			}
			return _Euler_constant_<result_type>::euler_constant + log(x) + x2;
		}
	};
	template <class T>
	inline constexpr auto cos_int(const T& x) { return _Cos_int<T>::__cos_int(x); }

}


#endif