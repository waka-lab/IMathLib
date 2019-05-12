#ifndef IMATH_MATH_MATH_LOG_INT_HPP
#define IMATH_MATH_MATH_LOG_INT_HPP

#include "IMathLib/math/math/math_traits.hpp"
#include "IMathLib/math/math/exp_int.hpp"
#include "IMathLib/math/math/ln2.hpp"
#include "IMathLib/math/math/log.hpp"

namespace iml {

	//対数積分
	template <class T>
	struct _Log_int {
		using result_type = typename math_function_type<T>::type;

		static constexpr result_type __log_int(const T& x) {
			return (x == 0) ? 0 : exp_int(log(x));
		}
	};
	template <class T>
	inline constexpr auto log_int(const T& x) { return _Log_int<T>::__log_int(x); }

	//補正対数積分
	template <class T>
	struct _Log_intc {
		using result_type = typename math_function_type<T>::type;

		static constexpr result_type __log_intc(const T& x) {
			constexpr auto temp = exp_int(_Loge_2_<result_type>::loge_2);
			return exp_int(log(x)) - temp;
		}
	};
	template <class T>
	inline constexpr auto log_intc(const T& x) { return _Log_intc<T>::__log_intc(x); }

}


#endif