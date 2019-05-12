#ifndef IMATH_MATH_MATH_IHYPABOLIC_FUNCTION_HPP
#define IMATH_MATH_MATH_IHYPABOLIC_FUNCTION_HPP

#include "IMathLib/math/math/math_traits.hpp"
#include "IMathLib/math/math/sqrt.hpp"
#include "IMathLib/math/math/abs.hpp"
#include "IMathLib/math/math/log.hpp"
#include "IMathLib/math/math/sgn.hpp"


namespace iml {

	//逆双曲線正弦
	template <class T>
	struct Asinh {
		static constexpr auto _asinh_(const T& x) {
			return log(x + sqrt(x*x + 1));
		}
	};
	template <class T>
	inline constexpr auto asinh(const T& x) { return Asinh<T>::_asinh_(x); }


	//逆双曲線余弦
	template <class T>
	struct Acosh {
		//実数の場合の定義
		static constexpr auto _acosh_(const T& x) {
			return log(x + sqrt(x*x - 1));
		}
	};
	template <class T>
	inline constexpr auto acosh(const T& x) { return Acosh<T>::_acosh_(x); }


	//逆双曲線正接
	template <class T>
	struct Atanh {
		using result_type = typename math_function_type<T>::type;

		static constexpr result_type _atanh_(const T& x) {
			return log(result_type(1 + x) / (1 - x)) / 2;
		}
	};
	template <class T>
	inline constexpr auto atanh(const T& x) { return Atanh<T>::_atanh_(x); }

}


#endif