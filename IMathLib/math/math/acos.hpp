#ifndef IMATH_MATH_MATH_ACOS_HPP
#define IMATH_MATH_MATH_ACOS_HPP

#include "IMathLib/math/math/math_traits.hpp"
#include "IMathLib/math/math/pi.hpp"
#include "IMathLib/math/math/asin.hpp"


namespace iml {

	//逆余弦
	template <class T>
	struct Acos {
		using result_type = typename math_function_type<T>::type;

		static constexpr result_type _acos_(const T& x) {
			return pi<result_type> / 2 - asin(x);
		}
	};
	template <class T>
	inline constexpr auto acos(const T& x) { return Acos<T>::_acos_(x); }

}


#endif