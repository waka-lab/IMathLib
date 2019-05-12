#ifndef IMATH_MATH_HYPERCOMPLEX_ARG_HPP
#define IMATH_MATH_HYPERCOMPLEX_ARG_HPP

#include "IMathLib/math/math.hpp"
#include "IMathLib/math/hypercomplex/complex.hpp"
#include "IMathLib/math/hypercomplex/split_complex.hpp"
#include "IMathLib/math/hypercomplex/quaternion.hpp"


namespace iml {

	template <class T>
	struct Arg<complex<T>> {
		static constexpr auto _arg_(const complex<T>& c) {
			return atan2(c[0], c[1]);
		}
	};
	template <class T>
	struct Arg<split_complex<T>> {
		static constexpr auto _arg_(const split_complex<T>& c) {
			return atan2(c[0], c[1]);
		}
	};
	template <class T>
	struct Arg<quaternion<T>> {
		static constexpr auto _arg_(const quaternion<T>& q) { return vector<T, 3>(q[1], q[2], q[3]); }
	};
}

#endif