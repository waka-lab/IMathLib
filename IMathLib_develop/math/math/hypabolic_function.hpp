#ifndef IMATH_MATH_MATH_HYPABOLIC_FUNCTION_HPP
#define IMATH_MATH_MATH_HYPABOLIC_FUNCTION_HPP

#include "IMathLib/math/math/math_traits.hpp"
#include "IMathLib/math/math/exp.hpp"


namespace iml {

	//双曲線正弦関数
	template <class T>
	struct Sinh {
		using result_type = typename math_function_type<T>::type;

		static constexpr result_type _sinh_(const T& x) {
			result_type temp = exp(x);
			return (temp - 1 / temp) / 2;
		}
	};
	template <class T>
	inline constexpr auto sinh(const T& x) { return Sinh<T>::_sinh_(x); }


	//双曲線余弦
	template <class T>
	struct Cosh {
		using result_type = typename math_function_type<T>::type;

		static constexpr result_type _cosh_(const T& x) {
			result_type temp = exp(x);
			return (temp + 1 / temp) / 2;
		}
	};
	template <class T>
	inline constexpr auto cosh(const T& x) { return Cosh<T>::_cosh_(x); }


	//双曲線正接関数
	template <class T>
	struct Tanh {
		using result_type = typename math_function_type<T>::type;

		static constexpr result_type _tanh_(const T& x) {
			result_type temp = exp(x);
			return (temp - 1 / temp) / (temp + 1 / temp);
		}
	};
	template <class T>
	inline constexpr auto tanh(const T& x) { return Tanh<T>::_tanh_(x); }

}


#endif