#ifndef _IMATH_MATH_STIRLING_NUMBER_HPP
#define _IMATH_MATH_STIRLING_NUMBER_HPP

#include "IMathLib/math/math/math_traits.hpp"

namespace iml {

	//第1種スターリング数(n > 0)
	inline constexpr int_t stirling_number1(int_t n, int_t k) {
		if (k < 1 || k > n) return 0;
		if (k == n) return 1;
		return (n - 1) * stirling_number1(n - 1, k)
			+ stirling_number1(n - 1, k - 1);
	}


	//第2種スターリング数(n > 0)
	inline constexpr int_t stirling_number2(int_t n, int_t k) {
		if (k < 1 || k > n) return 0;
		if (k == 1 || k == n) return 1;
		return k * stirling_number2(n - 1, k)
			+ stirling_number2(n - 1, k - 1);
	}
}


#endif