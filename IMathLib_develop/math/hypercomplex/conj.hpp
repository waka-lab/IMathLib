#ifndef IMATH_MATH_HYPERCOMPLEX_CONJ_HPP
#define IMATH_MATH_HYPERCOMPLEX_CONJ_HPP

#include "IMathLib/math/math.hpp"
#include "IMathLib/math/hypercomplex/complex.hpp"
#include "IMathLib/math/hypercomplex/split_complex.hpp"
#include "IMathLib/math/hypercomplex/dual_numbers.hpp"
#include "IMathLib/math/hypercomplex/quaternion.hpp"
#include "IMathLib/math/hypercomplex/octonion.hpp"


namespace iml {

	template <class T>
	struct Conj<complex<T>> {
		static constexpr auto _conj_(const complex<T>& c) {
			return complex<T>(c[0], -c[1]);
		}
	};
	template <class T>
	struct Conj<split_complex<T>> {
		static constexpr auto _conj_(const split_complex<T>& c) {
			return split_complex<T>(c[0], -c[1]);
		}
	};
	template <class T>
	struct Conj<dual_numbers<T>> {
		static constexpr auto _conj_(const dual_numbers<T>& c) {
			return dual_numbers<T>(c[0], -c[1]);
		}
	};
	template <class T>
	struct Conj<quaternion<T>> {
		static constexpr auto _conj_(const quaternion<T>& q) {
			return quaternion<T>(q[0], -q[1], -q[2], -q[3]);
		}
	};
	template <class T>
	struct Conj<octonion<T>> {
		static constexpr auto _conj_(const octonion<T>& o) {
			return octonion<T>(o[0], -o[1], -o[2], -o[3], -o[4], -o[5], -o[6], -o[7]);
		}
	};
}

#endif