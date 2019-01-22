#ifndef _IMATH_MATH_PI_HPP
#define _IMATH_MATH_PI_HPP

#include "IMathLib/math/math/math_traits.hpp"

#include "IMathLib/utility/type_traits.hpp"
#include "IMathLib/math/math/sqrt.hpp"

namespace iml {

	//超次元級から一般次元の円周率を計算する
	template <class T>
	inline constexpr T _Pi_correction(const T& pi, imsize_t n) {
		//無限大での除算の対策
		if (n == 0) return 0;
		if (n == 1) return 2;
		imsize_t c = n >> 1;
		T result = pi;
		//偶数次元
		if ((n & 1) == 0) {
			for (imsize_t i = 2; i <= c; ++i)
				result *= pi / (4 * (i - 1));
		}
		//奇数次元
		else {
			for (imsize_t i = 2; i <= c; ++i)
				result *= pi / (2 * (2 * i - 1));
		}
		return result;
	}
	//円周率
	template <class T>
	inline constexpr auto _Pi(imsize_t n = 2) {
		//浮動小数点のみ
		using result_type = typename conditional<is_floating_point<T>::value, T, _IMATH_DEFAULT_TYPE>::type;

		result_type a = 1, b = sqrt(2) / 2, t = 0.25, p = 1;
		result_type temp_a = (a + b) / 2;
		imsize_t cnt = limits<result_type>::digits10;			//log2(cnt)程度の回数だけ演算するためのカウンタ
		do {
			temp_a = (a + b) / 2;
			b = sqrt(a*b);
			t -= p * (a - temp_a)*(a - temp_a);
			p *= 2;
			a = temp_a;
		} while ((cnt >>= 1) != 0);
		//n次元の円周率を求める
		return _Pi_correction((a + b)*(a + b) / (4 * t), n);
	}

	//ライブラリ用の円周率
	template <class T>
	struct _Pi_ {
		static constexpr auto pi = _Pi<T>();
	};
}


#endif