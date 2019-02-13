#ifndef IMATH_MATH_MATH_COS_HPP
#define IMATH_MATH_MATH_COS_HPP

#include "IMathLib/math/math/math_traits.hpp"
#include "IMathLib/math/math/pi.hpp"
#include "IMathLib/math/math/numerical_correction.hpp"


namespace iml {

	//余弦関数
	template <class T>
	struct Cos {
		using result_type = typename math_function_type<T>::type;

		//xを小さくするための補助
		static constexpr result_type _cos_impl_(result_type x) {
			
			//0 < x < 0.05
			if (20 * x >= 1) {
				x /= 2;
				result_type c = _cos_impl_(x);
				return 2 * c * c - 1;
			}

			result_type x1 = 1, x2 = 1, x3 = 2;
			x *= x;

			//収束条件は級数の偶数項の単調減少が崩れる
			for (imsize_t i = 1; x1 + Error_evaluation<result_type>::eps < x3; ++i) {
				x3 = x1;
				x1 *= -x / (2 * i*(2 * i - 1));
				++i;
				x2 += x1;
				x1 *= -x / (2 * i*(2 * i - 1));
				x2 += x1;
			}
			/*for (imsize_t i = 1; !error_evaluation(x2, x3); ++i) {
				x3 = x2;
				x1 *= -x / (2 * i*(2 * i - 1));
				x2 += x1;
			}*/
			return x2;
		}

		static constexpr result_type _cos_(const T& x) {
			//周期性
			result_type temp = mod<result_type>(x, 2 * _Pi_<result_type>::pi);
			//正数値で演算する(余分な比較を省くため)
			return (temp < 0) ? _cos_impl_(-temp) : _cos_impl_(temp);
		}
	};
	template <class T>
	inline constexpr auto cos(const T& x) { return Cos<T>::_cos_(x); }
}


#endif