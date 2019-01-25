#ifndef _IMATH_MATH_COS_HPP
#define _IMATH_MATH_COS_HPP

#include "IMathLib/math/math/math_traits.hpp"
#include "IMathLib/math/math/pi.hpp"
#include "IMathLib/math/math/numerical_correction.hpp"


namespace iml {

	//余弦関数
	template <class T>
	struct _Cos {
		using result_type = typename math_function_type<T>::type;

		//xを小さくするための補助
		static constexpr result_type __cos_impl(result_type x) {
			
			//0 < x < 0.1
			if (10 * x >= 1) {
				x /= 2;
				result_type c = __cos_impl(x);
				return 2 * c * c - 1;
			}

			result_type x1 = 1, x2 = 1, x3 = 0;
			x *= x;

			for (imsize_t i = 1; !error_evaluation(x2, x3); ++i) {
				x3 = x2;
				x1 *= -x / (2 * i*(2 * i - 1));
				x2 += x1;
			}
			return x2;
		}

		static constexpr result_type __cos(const T& x) {
			//周期性
			result_type temp = mod<result_type>(x, 2 * _Pi_<result_type>::pi);
			//正数値で演算する(余分な比較を省くため)
			return (temp < 0) ? __cos_impl(-temp) : __cos_impl(temp);
		}
	};
	template <class T>
	inline constexpr auto cos(const T& x) { return _Cos<T>::__cos(x); }
}


#endif