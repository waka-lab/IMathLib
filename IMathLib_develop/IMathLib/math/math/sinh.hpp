#ifndef _IMATH_MATH_SINH_HPP
#define _IMATH_MATH_SINH_HPP

#include "IMathLib/math/math/math_traits.hpp"
#include "IMathLib/math/math/math_comstant.hpp"
#include "IMathLib/math/math/exp.hpp"


namespace iml {

	//双曲線正弦関数
	template <class T>
	struct _Sinh {
		using result_type = typename math_function_type<T>::type;

		static constexpr result_type __sin(const T& x) {
			result_type temp = exp(x);
			return (temp - 1 / temp) / 2;
		}
	};
	template <class T>
	inline constexpr auto sinh(const T& x) { return _Sinh<T>::__sinh(x); }

}


#endif