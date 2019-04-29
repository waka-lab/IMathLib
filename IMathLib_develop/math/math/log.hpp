#ifndef IMATH_MATH_MATH_LOG_HPP
#define IMATH_MATH_MATH_LOG_HPP

#include "IMathLib/math/math/math_traits.hpp"
#include "IMathLib/math/math/loge_2.hpp"

namespace iml {

	//対数関数
	template <class T>
	struct Log {
		using result_type = typename math_function_type<T>::type;

		static constexpr result_type _log_impl_(const T& x) {

			if (x == 2) return _Loge_2_<result_type>::loge_2;
			else if (x == 0) return numeric_traits<result_type>::negative_infinity();
			else if (x < 0) return numeric_traits<result_type>::nan();
			
			result_type c = x;
			int_t index = 0;			//冪指数

			//xを1に近くする
			//1< x < 2^8 = 256
			while (c > 256) { c /= 256; index += 8; }
			//1 < x < 2^6 = 64
			while (c > 64) { c /= 64; index += 6; }
			//1 < x < 2^4 = 16
			while (c > 16) { c /= 16; index += 4; }
			//1 < x < 2^2 = 4
			while (c > 4) { c /= 4; index += 2; }
			//1 < x < 2
			while (c > 2) { c /= 2; ++index; }

			//テイラー展開の計算
			c = (c - 1) / (1 + c);			//0 < c < 1/3
			result_type x1 = c, x2 = c, x3 = 0;
			c *= c;
			//収束条件は和の単調増加が崩れる
			for (size_t i = 2; x3 + Error_evaluation<result_type>::epsilon() < x2; ++i) {
				x3 = x2;
				x1 *= c;
				x2 += x1 / (2 * i - 1);
			}
			return 2 * x2 + index * _Loge_2_<result_type>::loge_2;
		}
		static constexpr result_type _log_(const T& x) {
			if (x == 0) return numeric_traits<result_type>::positive_infinity();
			//xを1以上にする
			return (x < 1) ? (-_log_impl_(1 / x)) : _log_impl_(x);
		}
	};
	template <class T>
	inline constexpr auto log(const T& x) { return Log<T>::_log_(x); }

	//常用対数
	template <class T>
	struct Log10 {
		using result_type = typename conditional<is_arithmetic<T>::value && !is_floating_point<T>::value, double, T>::type;

		static constexpr result_type _log10_(const T& x) {
			constexpr auto c = log<result_type>(10);
			return log(x) / c;
		}
	};
	template <class T>
	inline constexpr auto log10(const T& x) { return Log10<T>::_log10_(x); }

}


#endif