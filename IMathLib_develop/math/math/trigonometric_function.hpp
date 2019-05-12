#ifndef IMATH_MATH_MATH_TRIGONOMETRIC_FUNCTION_HPP
#define IMATH_MATH_MATH_TRIGONOMETRIC_FUNCTION_HPP

#include "IMathLib/math/math/math_traits.hpp"
#include "IMathLib/math/math/pi.hpp"
#include "IMathLib/math/math/numerical_correction.hpp"
#include "IMathLib/math/math/abs.hpp"


namespace iml {

	//余弦関数
	template <class T>
	struct Cos {
		using result_type = typename math_function_type<T>::type;

		//xを小さくするための補助
		static constexpr result_type _cos_impl_(result_type x) {
			size_t n = 0;

			//0 < x < 0.05 となるまでの操作回数をカウント
			while (20 * x >= 1) { x /= 4; ++n; }

			result_type x1 = 1, x2 = 1, x3 = 0;
			x *= x;

			for (size_t i = 1; !error_evaluation(x2, x3); ++i) {
				x3 = x2;
				x1 *= -x / (2 * i*(2 * i - 1));
				x2 += x1;
			}

			//4倍角の式で補正する
			while (n != 0) { x2 *= x2; x2 = 8 * x2 * x2 - 8 * x2 + 1; --n; }

			return x2;
		}

		static constexpr result_type _cos_(const T& x) {
			//周期性
			result_type temp = mod<result_type>(x, 2 * pi<result_type>);
			//正数値で演算する(余分な比較を省くため)
			return (temp < 0) ? _cos_impl_(-temp) : _cos_impl_(temp);
		}
	};
	template <class T>
	inline constexpr auto cos(const T& x) { return Cos<T>::_cos_(x); }


	//正弦関数
	template <class T>
	struct Sin {
		using result_type = typename math_function_type<T>::type;

		static constexpr result_type _sin_(const T& x) {
			return cos(pi<result_type> / 2 - x);
		}
	};
	template <class T>
	inline constexpr auto sin(const T& x) { return Sin<T>::_sin_(x); }


	//sinc関数
	template <class T>
	inline constexpr auto sinc(const T& x) {
		return (x == 0) ? 1 : sin(x) / x;
	}


	//正接関数
	template <class T>
	struct Tan {
		using result_type = typename math_function_type<T>::type;

		static constexpr result_type _tan_(const T& x) {
			return sin(x) / cos(x);
		}
	};
	template <class T>
	inline constexpr auto tan(const T& x) { return Tan<T>::_tan_(x); }
}


#endif