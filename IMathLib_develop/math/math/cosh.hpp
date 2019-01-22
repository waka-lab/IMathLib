#ifndef _IMATH_MATH_COSH_HPP
#define _IMATH_MATH_COSH_HPP

#include "IMathLib/math/math/math_traits.hpp"
#include "IMathLib/math/math/math_comstant.hpp"

namespace iml {

	//双曲線余弦
	template <class T>
	struct _Cosh {
		using result_type = typename math_function_type<T>::type;

		static constexpr result_type __cosh(const T& x) {
			result_type temp = exp(x);
			return (temp + 1 / temp) / 2;
		}
	};
	template <class T>
	inline constexpr auto cosh(const T& x) { return _Cosh<T>::__cosh(x); }
}


#endif