#ifndef _IMATH_MATH_E_HPP
#define _IMATH_MATH_E_HPP

#include "IMathLib/math/math/math_traits.hpp"

namespace iml {

	//ネイピア数(テイラー展開)
	template <class T>
	inline constexpr auto _E() {
		//浮動小数点型のみ
		using result_type = typename conditional<is_floating_point<T>::value, T, IMATH_DEFAULT_TYPE>::type;

		result_type a = 1, an = 1, b = 0;
		for (imsize_t i = 1; !error_evaluation(a, b); ++i) {
			b = a;
			an /= i;
			a += an;
		}
		return b;
	}

	//ライブラリ用のネイピア数
	template <class T>
	struct _E_ {
		static constexpr auto e = _E<T>();
	};
}


#endif