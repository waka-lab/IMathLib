#ifndef _IMATH_MATH_EXP_HPP
#define _IMATH_MATH_EXP_HPP

#include "IMathLib/math/math/math_traits.hpp"
#include "IMathLib/math/math/e.hpp"
#include "IMathLib/math/math/ipow.hpp"

namespace iml {

	//指数関数
	template <class T>
	struct _Exp {
		using result_type = typename math_function_type<T>::type;

		static constexpr result_type __ipow(result_type x, imsize_t y) {
			//yが2の冪であることから最適化
			while (y > 1) { x *= x; y /= 2; }
			return x;
		}

		static constexpr result_type __exp_impl(result_type x) {
			result_type x1 = 1, x2 = 1, x3 = 0;
			//これ超えたときは無限大になるけどそれでいい?
			imint_t index = static_cast<imint_t>(x);

			x -= index;
			x /= 256;			//0 < x < 1/256=0.00390625
			//収束条件は単調増加が崩れる
			for (imsize_t i = 1; x3 < x2; ++i) {
				x3 = x2;
				x1 *= (x / i);
				x2 += x1;
			}
			return __ipow(x2, 256)*ipow(_E_<result_type>::e, index);
		}
		static constexpr result_type __exp(const T& x) {
			return (x < 0) ? 1 / __exp_impl(-x) : __exp_impl(x);
		}
	};
	template <>
	struct _Exp<imsize_t> {
		using result_type = typename math_function_type<imsize_t>::type;

		static constexpr result_type __exp(imsize_t x) { return ipow(_E_<result_type>::e, x); }
	};
	template <>
	struct _Exp<imint_t> {
		using result_type = typename math_function_type<imint_t>::type;

		static constexpr result_type __exp(imint_t x) { return ipow(_E_<result_type>::e, x); }
	};
	template <class T>
	inline constexpr auto exp(const T& x) { return _Exp<T>::__exp(x); }

	//シグモイド関数
	template <class T>
	inline constexpr auto sigmoid(const T& gain, const T& x) {
		return 1 / (1 + exp(-gain * x));
	}
}


#endif