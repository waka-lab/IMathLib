#ifndef IMATH_MATH_HYPERCOMPLEX_EXP_HPP
#define IMATH_MATH_HYPERCOMPLEX_EXP_HPP

#include "IMathLib/math/math.hpp"
#include "IMathLib/math/hypercomplex/complex.hpp"
#include "IMathLib/math/hypercomplex/split_complex.hpp"
#include "IMathLib/math/hypercomplex/dual_numbers.hpp"


namespace iml {

	template <class T>
	struct Exp<complex<T>> {
		using result_type = typename math_function_type<complex<T>>::type;
		static constexpr result_type _exp_(const complex<T>& c) { return result_type(cos(c[0]), sin(c[1])); }
	};
	template <class T>
	struct Exp<split_complex<T>> {
		using result_type = typename math_function_type<split_complex<T>>::type;
		static constexpr result_type _exp_(const split_complex<T>& c) { return result_type(cosh(c[0]), sinh(c[1])); }
	};
	template <class T>
	struct Exp<dual_numbers<T>> {
		using result_type = typename math_function_type<dual_numbers<T>>::type;
		static constexpr result_type _exp_(const dual_numbers<T>& c) {
			auto temp = exp(c[0]);
			return result_type(temp, c[1] * temp);
		}
	};
}

#endif