#ifndef _IMATH_MATH_HRESNEL_INT_HPP
#define _IMATH_MATH_HRESNEL_INT_HPP

#include "IMathLib/math/math/math_traits.hpp"


namespace iml {

	//フレネル積分S(x)
	template <class T>
	struct _Fresnel_int_s {
		using result_type = typename math_function_type<T>::type;

		static constexpr result_type __fresnel_int_s(const T& x) {
			result_type temp = x * x * x;
			result_type x1 = temp, x2 = temp / 3, x3 = 0;
			temp *= x;

			for (size_t i = 1; !error_evaluation(x2, x3); ++i) {
				x3 = x2;
				x1 *= -(temp / (2 * i*(2 * i + 1)));
				x2 += x1 / (4 * i + 3);
			}
			return x2;
		}
	};
	template <class T>
	inline constexpr auto fresnel_int_s(const T& x) { return _Fresnel_int_s<T>::__fresnel_int_s(x); }


	//フレネル積分C(x)
	template <class T>
	struct _Fresnel_int_c {
		using result_type = typename math_function_type<T>::type;

		static constexpr result_type __fresnel_int_c(const T& x) {
			result_type x1 = x, x2 = x, x3 = 0;
			result_type temp = x * x * x * x;

			for (size_t i = 1; !error_evaluation(x2, x3); ++i) {
				x3 = x2;
				x1 *= -(temp / (2 * i*(2 * i - 1)));
				x2 += x1 / (4 * i + 1);
			}
			return x2;
		}
	};
	template <class T>
	inline constexpr auto fresnel_int_c(const T& x) { return _Fresnel_int_c<T>::__fresnel_int_c(x); }
}


#endif