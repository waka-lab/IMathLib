#ifndef IMATH_MATH_MATH_SIN_HPP
#define IMATH_MATH_MATH_SIN_HPP

#include "IMathLib/math/math/math_traits.hpp"
#include "IMathLib/math/math/pi.hpp"
#include "IMathLib/math/math/numerical_correction.hpp"
#include "IMathLib/math/math/abs.hpp"
#include "IMathLib/math/math/cos.hpp"


namespace iml {

	//正弦関数
	template <class T>
	struct Sin {
		using result_type = typename math_function_type<T>::type;

		static constexpr result_type _sin_(const T& x) {
			return cos(pi<result_type> / 2 - x);
		}
	};
	template <class T>
	inline constexpr auto sin(const T& x) { return Sin<T>::_sin_(x); }


	//sinc関数
	template <class T>
	inline constexpr auto sinc(const T& x) {
		return (x == 0) ? 1 : sin(x) / x;
	}
}


#endif