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
			
			//0 < x < 1/10
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
	//余弦関数
	template <>
	struct _Cos<double> {

		//xを小さくするための補助
		static double __cos(double x) {

			static double waru[5] = { 1.0 / (3 * 4 * 5 * 6 * 7 * 8 * 9 * 10),-1.0 / (3 * 4 * 5 * 6 * 7 * 8),1.0 / (3 * 4 * 5 * 6),-1.0 / (3 * 4),1.0 };
			double y, *p = waru;
			int i, q;
			q=(int)(x/(2.0*_Pi_<double>::pi));
			x=x-q*(2.0*_Pi_<double>::pi);
			x = x / 32.0;
			x = x * x;
			y = -1.0 / (3 * 4 * 5 * 6 * 7 * 8 * 9 * 10 * 11 * 12);
			do {
				y = y * x + (*p);
				p++;
			} while (p < waru + 5);
			y = y * x;
			for (i = 0; i < 5; i++) y = y * (4.0 - y);
			return 1.0 - y / 2.0;
		}
	};
	template <class T>
	inline constexpr auto cos(const T& x) { return _Cos<T>::__cos(x); }
}


#endif