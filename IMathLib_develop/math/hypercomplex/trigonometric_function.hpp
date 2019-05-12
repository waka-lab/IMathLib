#ifndef IMATH_MATH_HYPERCOMPLEX_TRIGONOMETRIC_FUNCTION_HPP
#define IMATH_MATH_HYPERCOMPLEX_TRIGONOMETRIC_FUNCTION_HPP

#include "IMathLib/math/math.hpp"
#include "IMathLib/math/hypercomplex/complex.hpp"
#include "IMathLib/math/hypercomplex/split_complex.hpp"
#include "IMathLib/math/hypercomplex/dual_numbers.hpp"


namespace iml {

	template <class T>
	struct Cos<complex<T>> {
		static constexpr auto _cos_(const complex<T>& c) {
			//cos(z) = (e^(i z) + e^(-i z)) / 2 = cos(x)cosh(y) - i sin(x)sinh(y)
			auto temp = exp(c[1]);
			auto temp2 = 1 / temp;
			return make_complex(cos(c[0]) * (temp + temp2) / 2, -sin(c[0])* (temp - temp2) / 2);
		}
	};
	template <class T>
	struct Cos<split_complex<T>> {};
	template <class T>
	struct Cos<dual_numbers<T>> {
		static constexpr auto _cos_(const dual_numbers<T>& c) {
			return make_dual_numbers(cos(c[0]), -c[1] * sin(c[0]));
		}
	};


	template <class T>
	struct Sin<complex<T>> {
		static constexpr auto _sin_(const complex<T>& c) {
			//sin(z) = (e^(i z) + e^(-i z)) / 2 = sin(x)cosh(y) + i cos(x)sinh(y)
			auto temp = exp(c[1]);
			auto temp2 = 1 / temp;
			return make_complex(sin(c[0]) * (temp + temp2) / 2, cos(c[0])* (temp - temp2) / 2);
		}
	};
	template <class T>
	struct Sin<split_complex<T>> {};
	template <class T>
	struct Sin<dual_numbers<T>> {
		static constexpr auto _sin_(const dual_numbers<T>& c) {
			return make_dual_numbers(sin(c[0]), c[1] * cos(c[0]));
		}
	};


	template <class T>
	struct Tan<split_complex<T>> {};
	template <class T>
	struct Tan<dual_numbers<T>> {
		static constexpr auto _tan_(const dual_numbers<T>& c) {
			auto temp = cos(c[0]);
			return make_dual_numbers(tan(c[0]), c[1] / (temp * temp));
		}
	};
}


#endif