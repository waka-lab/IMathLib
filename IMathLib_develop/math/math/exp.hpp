#ifndef IMATH_MATH_MATH_EXP_HPP
#define IMATH_MATH_MATH_EXP_HPP

#include "IMathLib/math/math/math_traits.hpp"
#include "IMathLib/math/math/e.hpp"
#include "IMathLib/math/math/ipow.hpp"

namespace iml {

	//指数関数
	template <class T>
	struct Exp {
		using result_type = typename math_function_type<T>::type;

		static constexpr result_type _ipow_(result_type x) {
			//256 = 2^8より8回計算
			x *= x; x *= x; x *= x; x *= x; x *= x; x *= x; x *= x;
			return x *= x;
		}

		static constexpr result_type _exp_impl_(result_type x) {
			result_type x1 = 1, x2 = 1, x3 = 0;
			//これ超えたときは無限大になるけどそれでいい?
			int_t index = static_cast<int_t>(x);

			x -= index;
			x /= 256;			//0 < x < 1/256 = 1/2^8 = 0.00390625
			//収束条件は和の単調増加が崩れる(xは十分に小さいためError_evaluation<result_type>::epsを加算するコストにあわない)
			for (size_t i = 1; x3 < x2; ++i) {
				x3 = x2;
				x1 *= (x / i);
				x2 += x1;
			}
			return _ipow_(x2)*ipow(e<result_type>, index);
		}
		static constexpr result_type _exp_(const T& x) {
			return (x < 0) ? 1 / _exp_impl_(-x) : _exp_impl_(x);
		}
	};
	template <>
	struct Exp<size_t> {
		using result_type = typename math_function_type<size_t>::type;

		static constexpr result_type _exp_(size_t x) { return ipow(e<result_type>, x); }
	};
	template <>
	struct Exp<int_t> {
		using result_type = typename math_function_type<int_t>::type;

		static constexpr result_type _exp_(int_t x) { return ipow(e<result_type>, x); }
	};
	template <class T>
	inline constexpr auto exp(const T& x) { return Exp<T>::_exp_(x); }

	//シグモイド関数
	template <class T>
	inline constexpr auto sigmoid(const T& gain, const T& x) {
		return 1 / (1 + exp(-gain * x));
	}
}


#endif