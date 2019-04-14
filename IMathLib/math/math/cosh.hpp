#ifndef IMATH_MATH_MATH_COSH_HPP
#define IMATH_MATH_MATH_COSH_HPP

#include "IMathLib/math/math/math_traits.hpp"
#include "IMathLib/math/math/math_comstant.hpp"

namespace iml {

	//双曲線余弦
	template <class T>
	struct Cosh {
		using result_type = typename math_function_type<T>::type;

		static constexpr result_type _cosh_(const T& x) {
			result_type temp = exp(x);
			return (temp + 1 / temp) / 2;
		}
	};
	template <class T>
	inline constexpr auto cosh(const T& x) { return Cosh<T>::_cosh_(x); }
}


#endif