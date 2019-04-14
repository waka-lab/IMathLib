#ifndef _IMATH_MATH_LOGE_2_HPP
#define _IMATH_MATH_LOGE_2_HPP

#include "IMathLib/math/math/math_traits.hpp"

namespace iml {

	//loge_2の取得
	template <class T>
	inline constexpr auto _Loge_2() {
		//浮動小数点のみ
		using result_type = typename conditional<is_floating_point<T>::value, T, IMATH_DEFAULT_TYPE>::type;

		result_type a = result_type(1) / result_type(3)
			, an = result_type(1) / result_type(3), b = 1;

		for (size_t i = 2; !error_evaluation(a, b); ++i) {
			b = a;
			an /= 9;
			a += an / (2 * i - 1);
		}
		return 2 * b;
	}

	//ライブラリ用のloge_2
	template <class T>
	struct _Loge_2_ {
		static constexpr auto loge_2 = _Loge_2<T>();
	};
}


#endif