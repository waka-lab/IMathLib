#ifndef IMATH_MATH_MATH_BETA_HPP
#define IMATH_MATH_MATH_BETA_HPP

#include "IMathLib/math/math/math_traits.hpp"
#include "IMathLib/math/math/gamma.hpp"
#include "IMathLib/math/math/exp.hpp"
#include "IMathLib/math/math/pow.hpp"

namespace iml {

	//ベータ関数
	template <class T>
	struct Beta {
		using result_type = typename math_function_type<T>::type;

		//ベータ関数
		static constexpr result_type _beta_(const T& a, const T& b) {
			return exp(lgamma(a) + lgamma(b) - lgamma(a + b));
		}
		//不完全ベータ関数
		static constexpr result_type _beta_impl_(const T& a, const T& b, const T& x) {
			result_type x1 = 1, x2 = result_type(1) / a, x3 = 0;

			for (size_t i = 1; !error_evaluation(x2, x3); ++i) {
				x3 = x2;
				x1 *= -result_type(b - i) / i * x;
				x2 += x1 / (i + a);
			}
			return pow(x, a)*x2;
		}
		static constexpr result_type _beta_(const T& a, const T& b, const T& x) {
			if (x == 0) return 0;
			if (x == 1) return _beta_(a, b);
			//xが0.5より大きいときで場合分け
			return ((2 * x > 1) ? _beta_(a, b) - _beta_impl_(b, a, 1 - x) : _beta_impl_(a, b, x));
		}
	};
	template <class T>
	inline constexpr auto beta(const T& a, const T& b) { return Beta<T>::_beta_(a, b); }
	template <class T>
	inline constexpr auto beta(const T& a, const T& b, const T& x) { return Beta<T>::_beta_(a, b, x); }


	//正規化ベータ関数
	template <class T>
	struct Beta_i {
		using result_type = typename math_function_type<T>::type;

		static constexpr result_type _beta_i_impl_(const T& a, const T& b, const T& x) {
			result_type x1 = 1, x2 = result_type(1) / a, x3 = 0;

			for (size_t i = 1; !error_evaluation(x2, x3); ++i) {
				x3 = x2;
				x1 *= -result_type(b - i) / i * x;
				x2 += x1 / (i + a);
			}
			return pow(x, a)*x2;
		}
		static constexpr result_type _beta_i_(const T& a, const T& b, const T& x) {
			if (x == 0) return 0;
			if (x == 1) return 1;
			//xが0.5より大きいときで場合分け
			return ((2 * x > 1) ? 1 - _beta_i_impl_(b, a, 1 - x) : _beta_i_impl_(a, b, x));
		}
	};
	template <class T>
	inline constexpr auto beta_i(const T& a, const T& b, const T& x) { return Beta_i<T>::_beta_i_(a, b, x); }

}


#endif