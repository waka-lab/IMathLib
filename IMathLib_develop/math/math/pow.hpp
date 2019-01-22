#ifndef _IMATH_MATH_POW_HPP
#define _IMATH_MATH_POW_HPP

#include "IMathLib/math/math/math_traits.hpp"
#include "IMathLib/math/math/numerical_correction.hpp"
#include "IMathLib/math/math/sgn.hpp"
#include "IMathLib/math/math/exp.hpp"
#include "IMathLib/math/math/log.hpp"
#include "IMathLib/math/math/ipow.hpp"


namespace iml {

	//任意の底に対する冪乗
	template <class T>
	struct _Pow {
		using result_type = typename math_function_type<T>::type;

		template <class S, class = typename enable_if<!is_integral<S>::value>::type>
		static constexpr result_type __pow(const T& x, const S& y) {

			if (x == 0 && y == 0) return result_type(1);
			else if (x == 0) return result_type(0);
			//(-1)^nはnが整数でないときは実数の範囲では演算不可(pow(T, imint_t)等で呼び出せばいい)
			else if (x < 0) return  limits<result_type>::nan();
	
			return exp(y*log(x));
		}
		//自然数冪
		static constexpr auto __pow(const T& x, imsize_t y) { return ipow(x, y); }
		//整数冪
		static constexpr auto __pow(const T& x, imint_t y) { return ipow(x, y); }
	};
	template <class T1, class T2>
	inline constexpr auto pow(const T1& x, const T2& y) { return _Pow<T1>::__pow(x, y); }

}


#endif