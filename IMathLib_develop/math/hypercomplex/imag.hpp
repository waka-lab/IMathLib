#ifndef IMATH_MATH_HYPERCOMPLEX_IMAG_HPP
#define IMATH_MATH_HYPERCOMPLEX_IMAG_HPP

#include "IMathLib/math/math.hpp"
#include "IMathLib/math/hypercomplex/complex.hpp"
#include "IMathLib/math/hypercomplex/split_complex.hpp"
#include "IMathLib/math/hypercomplex/dual_numbers.hpp"
#include "IMathLib/math/hypercomplex/quaternion.hpp"
#include "IMathLib/math/hypercomplex/octonion.hpp"


namespace iml {

	template <class T>
	struct Imag<complex<T>> {
		static constexpr auto _imag_(const complex<T>& c) { return c[1]; }
	};
	template <class T>
	struct Imag<split_complex<T>> {
		static constexpr auto _imag_(const split_complex<T>& c) { return c[1]; }
	};
	template <class T>
	struct Imag<dual_numbers<T>> {
		static constexpr auto _imag_(const dual_numbers<T>& c) { return c[1]; }
	};
	template <class T>
	struct Imag<quaternion<T>> {
		static constexpr auto _imag_(const quaternion<T>& q) { return vector<T, 3>(q[1], q[2], q[3]); }
	};
	template <class T>
	struct Imag<octonion<T>> {
		static constexpr auto _imag_(const octonion<T>& o) { return vector<T, 7>(o[1], o[2], o[3], o[4], o[5], o[6], o[7]); }
	};
}

#endif