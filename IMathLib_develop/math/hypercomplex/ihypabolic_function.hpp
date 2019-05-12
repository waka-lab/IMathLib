#ifndef IMATH_MATH_HYPERCOMPLEX_HYPABOLIC_FUNCTION_HPP
#define IMATH_MATH_HYPERCOMPLEX_HYPABOLIC_FUNCTION_HPP

#include "IMathLib/math/math.hpp"
#include "IMathLib/math/hypercomplex/complex.hpp"
#include "IMathLib/math/hypercomplex/dual_numbers.hpp"


namespace iml {

	template <class T>
	struct Asinh<dual_numbers<T>> {
		static constexpr auto _asinh_(const dual_numbers<T>& x) {
			return make_dual_numbers(asinh(x[0]), x[1] / (sqrt(1 + x[0] * x[0])));
		}
	};


	template <class T>
	struct Acosh<complex<T>> {
		static constexpr auto _acosh_(const complex<T>& x) {
			return log(x + sqrt(x - 1)*sqrt(x + 1));
		}
	};
	template <class T>
	struct Acosh<dual_numbers<T>> {
		static constexpr auto _acosh_(const dual_numbers<T>& x) {
			return make_dual_numbers(acosh(x[0]), x[1] / (sqrt(x[0] * x[0] - 1)));
		}
	};


	template <class T>
	struct Atanh<dual_numbers<T>> {
		static constexpr auto _atanh_(const dual_numbers<T>& x) {
			return make_dual_numbers(atanh(x[0]), x[1] / (1 - x[0] * x[0]));
		}
	};
}


#endif