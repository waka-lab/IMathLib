#ifndef _IMATH_MATH_ACOS_HPP
#define _IMATH_MATH_ACOS_HPP

#include "IMathLib/math/math/math_traits.hpp"
#include "IMathLib/math/math/pi.hpp"
#include "IMathLib/math/math/asin.hpp"


namespace iml {

	//逆余弦
	template <class T>
	struct _Acos {
		using result_type = typename math_function_type<T>::type;

		static constexpr result_type __acos(const T& x) {
			return _Pi_<result_type>::pi / 2 - asin(x);
		}
	};
	template <class T>
	inline constexpr auto acos(const T& x) { return _Acos<T>::__acos(x); }

}


#endif