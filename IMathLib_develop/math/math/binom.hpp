#ifndef IMATH_MATH_MATH_BINOM_HPP
#define IMATH_MATH_MATH_BINOM_HPP

#include "IMathLib/math/math/math_traits.hpp"


namespace iml {

	//二項係数
	inline constexpr auto binom(size_t n, size_t k) {
		using result_type = typename math_function_type<size_t>::type;

		result_type result = 1;
		if (k < (n / 2)) for (size_t i = 1; i <= k; ++i) result *= result_type(n - i + 1) / i;
		else for (size_t i = 1, end = n - k; i <= end; ++i) result *= result_type(n - i + 1) / i;
		return result;
	}
}


#endif