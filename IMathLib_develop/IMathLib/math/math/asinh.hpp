#ifndef _IMATH_MATH_ASINH_HPP
#define _IMATH_MATH_ASINH_HPP

#include "IMathLib/math/math/math_traits.hpp"
#include "IMathLib/math/math/math_comstant.hpp"
#include "IMathLib/math/math/sqrt.hpp"
#include "IMathLib/math/math/abs.hpp"
#include "IMathLib/math/math/log.hpp"
#include "IMathLib/math/math/sgn.hpp"

namespace iml {

	//逆双曲線正弦
	template <class T>
	struct _Asinh {
		using result_type = typename math_function_type<T>::type;

		static constexpr result_type __asinh(const T& x) {
			return sgn(x)*log(abs(x) + sqrt(x*x + 1));
		}
	};
	template <class T>
	inline constexpr auto asinh(const T& x) { return _Asinh<T>::__asinh(x); }

}


#endif