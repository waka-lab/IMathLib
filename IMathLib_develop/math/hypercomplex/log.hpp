#ifndef IMATH_MATH_HYPERCOMPLEX_LOG_HPP
#define IMATH_MATH_HYPERCOMPLEX_LOG_HPP

#include "IMathLib/math/math.hpp"
#include "IMathLib/math/hypercomplex/complex.hpp"
#include "IMathLib/math/hypercomplex/split_complex.hpp"
#include "IMathLib/math/hypercomplex/dual_numbers.hpp"

#include "IMathLib/math/hypercomplex/abs.hpp"
#include "IMathLib/math/hypercomplex/arg.hpp"


namespace iml {

	template <class T>
	struct Log<complex<T>> {
		static constexpr auto _log_(const complex<T>& c) {
			return make_complex(log(abs(c)), arg(c));
		}
	};
	template <class T>
	struct Log<split_complex<T>> {
		static constexpr auto _log_(const split_complex<T>& c) {
			return make_split_complex(log(abs(c)), arg(c));
		}
	};
	template <class T>
	struct Log<dual_numbers<T>> {
		static constexpr auto _log_(const dual_numbers<T>& c) {
			return make_dual_numbers(log(c[0]), c[1] / c[0]);
		}
	};
}

#endif