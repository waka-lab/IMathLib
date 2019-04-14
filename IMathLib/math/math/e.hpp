#ifndef IMATH_MATH_MATH_E_HPP
#define IMATH_MATH_MATH_E_HPP

#include "IMathLib/math/math/math_traits.hpp"

namespace iml {

	//ネイピア数(テイラー展開)
	template <class T>
	struct E {
		using result_type = typename math_function_type<T>::type;

		static constexpr result_type _e_() {
			result_type a = 1, an = 1, b = 0;
			for (size_t i = 1; !error_evaluation(a, b); ++i) {
				b = a;
				an /= i;
				a += an;
			}
			return b;
		}
	};


	//ネイピア数
	template <class T>
	constexpr auto e = E<T>::_e_();

}


#endif