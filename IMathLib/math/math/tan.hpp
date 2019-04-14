#ifndef _IMATH_MATH_TAN_HPP
#define _IMATH_MATH_TAN_HPP

#include "IMathLib/math/math/math_traits.hpp"
#include "IMathLib/math/math/sin.hpp"
#include "IMathLib/math/math/cos.hpp"

namespace iml {

	//正接関数
	template <class T>
	struct _Tan {
		using result_type = typename math_function_type<T>::type;

		static constexpr result_type __tan(const T& x) {
			return sin(x) / cos(x);
		}
	};
	template <class T>
	inline constexpr auto tan(const T& x) { return _Tan<T>::__tan(x); }
}


#endif