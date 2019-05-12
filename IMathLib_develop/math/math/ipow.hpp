#ifndef _IMATH_MATH_IPOW_HPP
#define _IMATH_MATH_IPOW_HPP

#include "IMathLib/math/math/math_traits.hpp"


namespace iml {

	//自然数冪(積に関してマグマであることを要求)
	template <class T>
	inline constexpr T ipow_impl(T x, size_t y) {

		/*switch (y) {
		case 0: return 1;
		case 1: return x;
		case 2: return x * x;
		default:
			if ((y & 1) == 0) return ipow(ipow(x, y >> 1), size_t(2));
			else return x * ipow(ipow(x, (y - 1) >> 1), size_t(2));
		}*/
		T temp = 1;
		while (y > 1) {
			if (y & 1) temp *= x;
			x *= x;
			y /= 2;
		}
		return temp * x;
	}
	template <class T>
	inline constexpr T ipow(const T& x, size_t y) {
		if (y == 0) return 1;
		if (x == 0) return 0;
		return ipow_impl(x, y);
	}
	//整数冪
	template <class T>
	inline constexpr auto ipow(const T& x, int_t y) {
		using result_type = typename math_function_type<T>::type;
		return (y < 0) ? result_type(1) / ipow(x, size_t(-y)) : result_type(ipow(x, size_t(y)));
	}

}


#endif