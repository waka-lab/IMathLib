#ifndef _IMATH_MATH_ATAN_HPP
#define _IMATH_MATH_ATAN_HPP

#include "IMathLib/math/math/math_traits.hpp"
#include "IMathLib/math/math/pi.hpp"
#include "IMathLib/math/math/sqrt.hpp"
#include "IMathLib/math/math/asin.hpp"


namespace iml {

	//逆正接
	template <class T>
	struct _Atan {
		using result_type = typename math_function_type<T>::type;

		static constexpr result_type __atan(const T& x) {
			return asin(x / sqrt(1 + x * x));
		}
	};
	template <class T>
	inline constexpr auto atan(const T& x) { return _Atan<T>::__atan(x); }


	//座標平面上での逆正接
	template <class T>
	struct _Atan2 {
		using result_type = typename conditional<is_arithmetic<T>::value && !is_floating_point<T>::value, double, T>::type;

		static constexpr result_type __atan2(const T& y, const T& x) {

			if (x == 0) {
				if (y == 0) return 0;
				else if (y < 0) return -_Pi_<result_type>::pi / 2;
				else return _Pi_<result_type>::pi / 2;
			}
			else if (y == 0) return (x < 0) ? -_Pi_<result_type>::pi : 0;
			else if (x > 0) return atan(result_type(y) / x);
			else return atan(result_type(y) / x)
				+ ((y < 0) ? -_Pi_<result_type>::pi : _Pi_<result_type>::pi);
		}
	};
	template <class T>
	inline constexpr auto atan2(const T& y, const T& x) { return _Atan2<T>::__atan2(y, x); }

}


#endif