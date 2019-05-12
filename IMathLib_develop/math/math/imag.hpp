#ifndef IMATH_MATH_MATH_IMAG_HPP
#define IMATH_MATH_MATH_IMAG_HPP

#include "IMathLib/math/math/math_traits.hpp"

namespace iml {

	//実部取得(複素数等の多元環において有効)
	template <class T>
	struct Imag {
		static constexpr T _imag_(const T& x) { return x; }
	};
	template <class T>
	inline constexpr auto imag(const T& x) { return Imag<T>::_imag_(x); }
}


#endif