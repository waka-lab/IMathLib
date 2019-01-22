#ifndef _IMATH_MATH_SIN_HPP
#define _IMATH_MATH_SIN_HPP

#include "IMathLib/math/math/math_traits.hpp"
#include "IMathLib/math/math/pi.hpp"
#include "IMathLib/math/math/numerical_correction.hpp"
#include "IMathLib/math/math/abs.hpp"
#include "IMathLib/math/math/cos.hpp"


namespace iml {

	//正弦関数
	template <class T>
	struct _Sin {
		using result_type = typename math_function_type<T>::type;

		static constexpr result_type __sin(const T& x) {
			return cos(_Pi_<result_type>::pi / 2 - x);
		}
	};
	template <class T>
	inline constexpr auto sin(const T& x) { return _Sin<T>::__sin(x); }


	//sinc関数
	template <class T>
	inline constexpr auto sinc(const T& x) {
		return (x == 0) ? 1 : sin(x) / x;
	}
}


#endif