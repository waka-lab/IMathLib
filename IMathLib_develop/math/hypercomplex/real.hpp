#ifndef IMATH_MATH_HYPERCOMPLEX_REAL_HPP
#define IMATH_MATH_HYPERCOMPLEX_REAL_HPP

#include "IMathLib/math/math.hpp"
#include "IMathLib/math/hypercomplex/complex.hpp"
#include "IMathLib/math/hypercomplex/split_complex.hpp"
#include "IMathLib/math/hypercomplex/dual_numbers.hpp"
#include "IMathLib/math/hypercomplex/quaternion.hpp"
#include "IMathLib/math/hypercomplex/octonion.hpp"


namespace iml {

	template <class T>
	struct Real<complex<T>> {
		static constexpr auto _real_(const complex<T>& c) { return c[0]; }
	};
	template <class T>
	struct Real<split_complex<T>> {
		static constexpr auto _real_(const split_complex<T>& c) { return c[0]; }
	};
	template <class T>
	struct Real<dual_numbers<T>> {
		static constexpr auto _real_(const dual_numbers<T>& c) { return c[0]; }
	};
	template <class T>
	struct Real<quaternion<T>> {
		static constexpr auto _real_(const quaternion<T>& q) { return q[0]; }
	};
	template <class T>
	struct Real<octonion<T>> {
		static constexpr auto _real_(const octonion<T>& o) { return o[0]; }
	};
}

#endif