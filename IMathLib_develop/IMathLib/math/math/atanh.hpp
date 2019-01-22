#ifndef _IMATH_MATH_ATANH_HPP
#define _IMATH_MATH_ATANH_HPP

#include "IMathLib/math/math/math_traits.hpp"
#include "IMathLib/math/math/pi.hpp"
#include "IMathLib/math/math/sqrt.hpp"
#include "IMathLib/math/math/log.hpp"


namespace iml {

	//逆双曲線正接
	template <class T>
	struct _Atanh {
		using result_type = typename math_function_type<T>::type;

		static constexpr result_type __atanh(const T& x) {
			return log(result_type(1 + x) / (1 - x)) / 2;
		}
	};
	template <class T>
	inline constexpr auto atanh(const T& x) { return _Atanh<T>::__atanh(x); }
}


#endif