#ifndef IMATH_MATH_MATH_SQRT_HPP
#define IMATH_MATH_MATH_SQRT_HPP

#include "IMathLib/math/math/math_traits.hpp"

#include "IMathLib/utility/type_traits.hpp"
#include "IMathLib/math/math/abs.hpp"

namespace iml {

	//平方根の整数近似
	template <class T>
	struct Isqrt {
		using result_type = typename math_function_type<T>::type;

		static constexpr result_type _isqrt_(const T& x) {
			result_type	i = 1, a = static_cast<result_type>(x) / 2;
			for (; a > 0; ++i) a -= i;
			return i - 1;
		}
	};
	template <class T>
	inline constexpr auto isqrt(const T& x) { return Isqrt<T>::_isqrt_(x); }


	//平方根
	template <class T>
	struct Sqrt {
		using result_type = typename math_function_type<T>::type;

		static constexpr result_type _sqrt_(const T& x) {

			if ((x == 0) || (x == 1)) return x;

			result_type temp = 1, c = x;

			//1< x < 2^8 = 256
			while (c > 256) { c /= 256; temp *= 16; }
			while (c < 1) { c *= 256; temp /= 16; }
			//1 < x < 2^6 = 64
			while (c > 64) { c /= 64; temp *= 8; }
			//1 < x < 2^4 = 16
			while (c > 16) { c /= 16; temp *= 4; }
			//1 < x < 2^2 = 4
			while (c > 4) { c /= 4; temp *= 2; }
			result_type x1 = 1 / c, x2 = 0;
			result_type h = 1 - c * x1*x1;			//0 < h < 0.75
			//収束条件は単調増加が崩れる
			while (x2 + Error_evaluation<result_type>::epsilon() < x1) {
				x2 = x1;
				x1 *= (1 + (64 * h + h * h*(48 + 40 * h + 35 * h*h)) / 128);
				h = 1 - c * x1 * x1;
			}
			return temp * x1 * c;
		}
	};
	template <class T>
	inline constexpr auto sqrt(const T& x) { return Sqrt<T>::_sqrt_(x); }
}


#endif