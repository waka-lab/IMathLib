#ifndef IMATH_MATH_MATH_LN2_HPP
#define IMATH_MATH_MATH_LN2_HPP

#include "IMathLib/math/math/math_traits.hpp"

namespace iml {

	//loge_2の取得
	template <class T>
	inline constexpr auto _ln2_() -> decltype(real_as_math_constant_t<T>()) {
		using result_type = real_as_math_constant_t<T>;

		result_type a = result_type(1) / 3, an = result_type(1) / 3, b = 1;

		for (size_t i = 2; !error_evaluation(a, b); ++i) {
			b = a;
			an /= 9;
			a += an / (2 * i - 1);
		}
		return 2 * b;
	}


	//ln(2)
	template <class T>
	constexpr auto ln2 = _ln2_<T>();
}


#endif