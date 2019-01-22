#ifndef _IMATH_MATH_TANH_HPP
#define _IMATH_MATH_TANH_HPP

#include "IMathLib/math/math/math_traits.hpp"
#include "IMathLib/math/math/exp.hpp"

namespace iml {

	//双曲線正接関数
	template <class T>
	struct _Tanh {
		using result_type = typename math_function_type<T>::type;

		static constexpr result_type __tanh(const T& x) {
			result_type temp = exp(x);
			return (temp - 1 / temp) / (temp + 1 / temp);
		}
	};
	template <class T>
	inline constexpr auto tanh(const T& x) { return _Tanh<T>::__tanh(x); }
}


#endif