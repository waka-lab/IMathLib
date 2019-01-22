#ifndef _IMATH_MATH_ERF_HPP
#define _IMATH_MATH_ERF_HPP

#include "IMathLib/math/math/math_traits.hpp"
#include "IMathLib/math/math/sqrt.hpp"
#include "IMathLib/math/math/pi.hpp"

namespace iml {

	//誤差関数
	template <class T>
	struct _Erf {
		using result_type = typename math_function_type<T>::type;

		static constexpr result_type __erf(const T& x) {
			result_type x1 = x, x2 = x, x3 = 0;
			result_type temp = x * x;
			constexpr result_type c = 2 / sqrt(_Pi_<result_type>::pi);

			for (imsize_t i = 1; !error_evaluation(x2, x3); ++i) {
				x3 = x2;
				x1 *= -temp / i;
				x2 += x1 / (2 * i + 1);
			}
			return c*x2;
		}
	};
	template <class T>
	inline constexpr auto erf(const T& x) { return _Erf<T>::__erf(x); }


	//相補誤差関数
	template <class T>
	struct _Erfc {
		using result_type = typename conditional<is_arithmetic<T>::value && !is_floating_point<T>::value, double, T>::type;

		static constexpr result_type __erfc(const T& x) {
			return 1 - erf(x);
		}
	};
	template <class T>
	inline constexpr auto erfc(const T& x) { return _Erf<T>::__erfc(x); }
}


#endif