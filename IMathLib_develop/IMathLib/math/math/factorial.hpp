#ifndef _IMATH_MATH_FACTORIAL_HPP
#define _IMATH_MATH_FACTORIAL_HPP

#include "IMathLib/math/math/math_traits.hpp"


namespace iml {

	//階乗(基本的に符号無し整数)
	template <class UIntegral>
	inline constexpr UIntegral fact(const UIntegral& x) {
		if (x == 0) return 1;
		return x * fact(x - 1);
	}
	template <class UIntegral, class T>
	inline constexpr T fact(const UIntegral& x, const T& init) {
		if (x == 0) return init;
		return init * x * fact(x - 1);
	}

	//二重階乗(基本的に符号無し整数)
	template <class UIntegral>
	inline constexpr UIntegral double_fact(const UIntegral& x) {
		if (x <= 2) return x;
		return x * double_fact(x - 2);
	}
	template <class UIntegral, class T>
	inline constexpr T double_fact(const UIntegral& x, const T& init) {
		if (x <= 2) return x * init;
		return init * x * double_fact(x - 2);
	}

	//多重階乗(基本的に符号無し整数)
	template <class UIntegral>
	inline constexpr UIntegral multi_fact(imsize_t n, const UIntegral& x) {
		if (x <= n) return x;
		return x * multi_fact(n, x - 2);
	}
	template <class UIntegral, class T>
	inline constexpr UIntegral multi_fact(imsize_t n, const UIntegral& x, const T& init) {
		if (x <= n) return x * init;
		return init * x * multi_fact(n, x - 2);
	}
}


#endif