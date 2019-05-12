#ifndef IMATH_MATH_HYPERCOMPLEX_VERSOR_HPP
#define IMATH_MATH_HYPERCOMPLEX_VERSOR_HPP

#include "IMathLib/math/hypercomplex/complex.hpp"
#include "IMathLib/math/hypercomplex/split_complex.hpp"
#include "IMathLib/math/hypercomplex/quaternion.hpp"
#include "IMathLib/math/hypercomplex/octonion.hpp"

#include "IMathLib/math/hypercomplex/abs.hpp"


namespace iml {

	template <class>
	struct Versor;
	template <class T>
	struct Versor<complex<T>> {
		static constexpr auto _versor_(const complex<T>& c) { return c / abs(c); }
	};
	template <class T>
	struct Versor<split_complex<T>> {
		static constexpr auto _versor_(const split_complex<T>& c) { c / abs(c); }
	};
	template <class T>
	struct Versor<quaternion<T>> {
		static constexpr auto _versor_(const quaternion<T>& q) { q / abs(q); }
	};
	template <class T>
	struct Versor<octonion<T>> {
		static constexpr auto _versor_(const octonion<T>& o) { o / abs(o); }
	};
	template <class T>
	inline constexpr auto versor(const T& x) { return Versor<T>::_versor_(x); }

}

#endif