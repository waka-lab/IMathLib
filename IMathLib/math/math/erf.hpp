#ifndef IMATH_MATH_MATH_ERF_HPP
#define IMATH_MATH_MATH_ERF_HPP

#include "IMathLib/math/math/math_traits.hpp"
#include "IMathLib/math/math/sqrt.hpp"
#include "IMathLib/math/math/pi.hpp"

namespace iml {

	//誤差関数
	template <class T>
	struct Erf {
		using result_type = typename math_function_type<T>::type;

		static constexpr result_type _erf_(const T& x) {
			result_type x1 = x, x2 = x, x3 = 0;
			result_type temp = x * x;
			constexpr result_type c = 2 / sqrt(pi<result_type>);

			for (size_t i = 1; !error_evaluation(x2, x3); ++i) {
				x3 = x2;
				x1 *= -temp / i;
				x2 += x1 / (2 * i + 1);
			}
			return c*x2;
		}
	};
	template <class T>
	inline constexpr auto erf(const T& x) { return Erf<T>::_erf_(x); }


	//相補誤差関数
	template <class T>
	struct Erfc {
		using result_type = typename math_function_type<T>::type;

		static constexpr result_type _erfc_(const T& x) {
			return 1 - erf(x);
		}
	};
	template <class T>
	inline constexpr auto erfc(const T& x) { return Erfc<T>::_erfc_(x); }
}


#endif