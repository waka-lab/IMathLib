#ifndef IMATH_MATH_MATH_REAL_HPP
#define IMATH_MATH_MATH_REAL_HPP

#include "IMathLib/math/math/math_traits.hpp"

namespace iml {

	//実部取得(複素数等の多元環において有効)
	template <class T>
	struct Real {
		static constexpr T _real_(const T& x) { return x; }
	};
	template <class T>
	inline constexpr auto real(const T& x) { return Real<T>::_real_(x); }
}


#endif