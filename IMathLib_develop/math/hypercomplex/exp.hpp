#ifndef IMATH_MATH_HYPERCOMPLEX_EXP_HPP
#define IMATH_MATH_HYPERCOMPLEX_EXP_HPP

#include "IMathLib/math/math.hpp"
#include "IMathLib/math/hypercomplex/complex.hpp"
#include "IMathLib/math/hypercomplex/split_complex.hpp"
#include "IMathLib/math/hypercomplex/dual_numbers.hpp"


namespace iml {

	template <class T>
	struct Exp<complex<T>> {
		static constexpr auto _exp_(const complex<T>& c) {
			auto temp = exp(c[0]);
			return make_complex(temp * cos(c[1]), temp * sin(c[1]));
		}
	};
	template <class T>
	struct Exp<split_complex<T>> {
		static constexpr auto _exp_(const split_complex<T>& c) {
			auto temp = exp(c[0]);
			return make_split_complex(temp * cosh(c[1]), temp * sinh(c[1]));
		}
	};
	template <class T>
	struct Exp<dual_numbers<T>> {
		static constexpr auto _exp_(const dual_numbers<T>& c) {
			auto temp = exp(c[0]);
			return make_dual_numbers(temp, c[1] * temp);
		}
	};
}

#endif