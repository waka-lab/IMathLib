#ifndef _IMATH_MATH_REAL_HPP
#define _IMATH_MATH_REAL_HPP

#include "IMathLib/math/math/math_traits.hpp"

namespace iml {

	//実部取得(複素数等の多元環において有効)
	template <class T>
	struct _Real {

		static constexpr T __real(const T& x) { return x; }
	};
	template <class T>
	inline constexpr auto real(const T& x) { return _Real<T>::__real(x); }
}


#endif