#ifndef IMATH_MATH_MATH_ATAN_HPP
#define IMATH_MATH_MATH_ATAN_HPP

#include "IMathLib/math/math/math_traits.hpp"
#include "IMathLib/math/math/pi.hpp"
#include "IMathLib/math/math/sqrt.hpp"
#include "IMathLib/math/math/asin.hpp"


namespace iml {

	//逆正接
	template <class T>
	struct Atan {
		using result_type = typename math_function_type<T>::type;

		static constexpr result_type _atan_(const T& x) {
			return asin(x / sqrt(1 + x * x));
		}
	};
	template <class T>
	inline constexpr auto atan(const T& x) { return Atan<T>::_atan_(x); }


	//座標平面上での逆正接
	template <class T>
	struct Atan2 {
		using result_type = typename math_function_type<T>::type;

		static constexpr result_type _atan2_(const T& y, const T& x) {

			if (x == 0) {
				if (y == 0) return 0;
				else if (y < 0) return -pi<result_type> / 2;
				else return pi<result_type> / 2;
			}
			else if (y == 0) return (x < 0) ? -pi<result_type> : 0;
			else if (x > 0) return atan(result_type(y) / x);
			else return atan(result_type(y) / x)
				+ ((y < 0) ? -pi<result_type> : pi<result_type>);
		}
	};
	template <class T>
	inline constexpr auto atan2(const T& y, const T& x) { return Atan2<T>::_atan2_(y, x); }

}


#endif