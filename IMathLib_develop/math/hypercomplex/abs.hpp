#ifndef IMATH_MATH_HYPERCOMPLEX_ABS_HPP
#define IMATH_MATH_HYPERCOMPLEX_ABS_HPP

#include "IMathLib/math/math.hpp"
#include "IMathLib/math/hypercomplex/complex.hpp"
#include "IMathLib/math/hypercomplex/split_complex.hpp"
#include "IMathLib/math/hypercomplex/quaternion.hpp"
#include "IMathLib/math/hypercomplex/octonion.hpp"


namespace iml {

	template <class T>
	struct Abs<complex<T>> {
		static constexpr auto _abs_(const complex<T>& c) {
			return sqrt(abs2(c[0]) + abs2(c[1]));
		}
	};
	template <class T>
	struct Abs<split_complex<T>> {
		static constexpr auto _abs_(const split_complex<T>& c) {
			return sqrt(abs2(c[0]) - abs2(c[1]));
		}
	};
	template <class T>
	struct Abs<quaternion<T>> {
		static constexpr auto _abs_(const quaternion<T>& q) {
			return sqrt(abs2(q[0]) + abs2(q[1]) + abs2(q[2]) + abs2(q[3]));
		}
	};
	template <class T>
	struct Abs<octonion<T>> {
		static constexpr auto _abs_(const octonion<T>& o) {
			return sqrt(abs2(o[0]) + abs2(o[1]) + abs2(o[2]) + abs2(o[3]) + abs2(o[4]) + abs2(o[5]) + abs2(o[6]) + abs2(o[7]));
		}
	};


	template <class T>
	struct Abs2<complex<T>> {
		static constexpr auto _abs_(const complex<T>& c) {
			return abs2(c[0]) + abs2(c[1]);
		}
	};
	template <class T>
	struct Abs2<split_complex<T>> {
		static constexpr auto _abs_(const split_complex<T>& c) {
			return abs2(c[0]) - abs2(c[1]);
		}
	};
	template <class T>
	struct Abs2<quaternion<T>> {
		static constexpr auto _abs2_(const quaternion<T>& q) {
			return abs2(q[0]) + abs2(q[1]) + abs2(q[2]) + abs2(q[3]);
		}
	};
	template <class T>
	struct Abs2<octonion<T>> {
		static constexpr auto _abs2_(const octonion<T>& o) {
			return abs2(o[0]) + abs2(o[1]) + abs2(o[2]) + abs2(o[3]) + abs2(o[4]) + abs2(o[5]) + abs2(o[6]) + abs2(o[7]);
		}
	};
}

#endif