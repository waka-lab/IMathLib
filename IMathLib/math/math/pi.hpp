#ifndef IMATH_MATH_MATH_PI_HPP
#define IMATH_MATH_MATH_PI_HPP

#include "IMathLib/math/math/math_traits.hpp"

#include "IMathLib/utility/type_traits.hpp"
#include "IMathLib/math/math/sqrt.hpp"

namespace iml {

	template <class T>
	struct Pi {
		using result_type = typename math_function_type<T>::type;

		//超次元級から一般次元の円周率を計算する
		static constexpr result_type _pi_correction_(const result_type& pi, size_t n) {
			//無限大での除算の対策
			if (n == 0) return 0;
			if (n == 1) return 2;
			size_t c = n >> 1;
			result_type result = pi;
			//偶数次元
			if ((n & 1) == 0) {
				for (size_t i = 2; i <= c; ++i)
					result *= pi / (4 * (i - 1));
			}
			//奇数次元
			else {
				for (size_t i = 2; i <= c; ++i)
					result *= pi / (2 * (2 * i - 1));
			}
			return result;
		}
		//円周率
		static constexpr result_type _pi_(size_t n = 2) {
			result_type a = 1, b = sqrt(2) / 2, t = 0.25, p = 1;
			result_type temp_a = (a + b) / 2;
			size_t cnt = numeric_traits<result_type>::digits10;			//log2(cnt)程度の回数だけ演算するためのカウンタ
			do {
				temp_a = (a + b) / 2;
				b = sqrt(a*b);
				t -= p * (a - temp_a)*(a - temp_a);
				p *= 2;
				a = temp_a;
			} while ((cnt >>= 1) != 0);
			//n次元の円周率を求める
			return _pi_correction_((a + b)*(a + b) / (4 * t), n);
		}
	};

	//円周率
	template <class T>
	constexpr auto pi = Pi<T>::_pi_();
}


#endif