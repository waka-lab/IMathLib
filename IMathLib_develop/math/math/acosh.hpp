#ifndef _IMATH_MATH_ACOSH_HPP
#define _IMATH_MATH_ACOSH_HPP

#include "IMathLib/math/math/math_traits.hpp"
#include "IMathLib/math/math/math_comstant.hpp"
#include "IMathLib/math/math/sqrt.hpp"
#include "IMathLib/math/math/log.hpp"


namespace iml {

	//逆双曲線余弦
	template <class T>
	struct _Acosh {
		using result_type = typename math_function_type<T>::type;

		static constexpr result_type __acosh(const T& x) {
			return log(x + sqrt(x*x - 1));
		}
	};
	template <class T>
	inline constexpr auto acosh(const T& x) { return _Acosh<T>::__acosh(x); }

}


#endif