#ifndef _IMATH_MATH_ASIN_HPP
#define _IMATH_MATH_ASIN_HPP

#include "IMathLib/math/math/math_traits.hpp"
#include "IMathLib/math/math/pi.hpp"

namespace iml {

	//逆正弦
	template <class T>
	struct _Asin {
		using result_type = typename math_function_type<T>::type;

		static constexpr result_type __asin(const T& x) {
			result_type x1 = x, x2 = x, x3 = 0;
			result_type temp = x * x;

			if (x == 1) return (_Pi_<result_type>::pi / 2);
			if (x == -1) return (-_Pi_<result_type>::pi / 2);
			if ((x < -1) || (x > 1)) return limits<result_type>::nan();

			for (imsize_t i = 1; !error_evaluation(x2, x3); ++i) {
				x3 = x2;
				x1 *= temp * (2 * i - 1) / (2 * i);
				x2 += x1 / (2 * i + 1);
			}
			return x2;
		}
	};
	template <class T>
	inline constexpr auto asin(const T& x) { return _Asin<T>::__asin(x); }

}


#endif