#ifndef _IMATH_MATH_BETA_HPP
#define _IMATH_MATH_BETA_HPP

#include "IMathLib/math/math/math_traits.hpp"
#include "IMathLib/math/math/gamma.hpp"
#include "IMathLib/math/math/exp.hpp"
#include "IMathLib/math/math/pow.hpp"

namespace iml {

	//ベータ関数
	template <class T>
	struct _Beta {
		using result_type = typename math_function_type<T>::type;

		//ベータ関数
		static constexpr result_type __beta(const T& a, const T& b) {
			return exp(lgamma(a) + lgamma(b) - lgamma(a + b));
		}
		//不完全ベータ関数
		static constexpr result_type __beta_impl(const T& a, const T& b, const T& x) {
			result_type x1 = 1, x2 = result_type(1) / a, x3 = 0;

			for (imsize_t i = 1; !error_evaluation(x2, x3); ++i) {
				x3 = x2;
				x1 *= -result_type(b - i) / i * x;
				x2 += x1 / (i + a);
			}
			return pow(x, a)*x2;
		}
		static constexpr result_type __beta(const T& a, const T& b, const T& x) {
			if (x == 0) return 0;
			if (x == 1) return __beta(a, b);
			//xが0.5より大きいときで場合分け
			return ((2 * x > 1) ? __beta(a, b) - __beta_impl(b, a, 1 - x) : __beta_impl(a, b, x));
		}
	};
	template <class T>
	inline constexpr auto beta(const T& a, const T& b) { return _Beta<T>::__beta(a, b); }
	template <class T>
	inline constexpr auto beta(const T& a, const T& b, const T& x) { return _Beta<T>::__beta(a, b, x); }


	//正規化ベータ関数
	template <class T>
	struct _Beta_i {
		using result_type = typename math_function_type<T>::type;

		static constexpr result_type __beta_i_impl(const T& a, const T& b, const T& x) {
			result_type x1 = 1, x2 = result_type(1) / a, x3 = 0;

			for (imsize_t i = 1; !error_evaluation(x2, x3); ++i) {
				x3 = x2;
				x1 *= -result_type(b - i) / i * x;
				x2 += x1 / (i + a);
			}
			return pow(x, a)*x2;
		}
		static constexpr result_type __beta_i(const T& a, const T& b, const T& x) {
			if (x == 0) return 0;
			if (x == 1) return 1;
			//xが0.5より大きいときで場合分け
			return ((2 * x > 1) ? 1 - __beta_i_impl(b, a, 1 - x) : __beta_i_impl(a, b, x));
		}
	};
	template <class T>
	inline constexpr auto beta_i(const T& a, const T& b, const T& x) { return _Beta_i<T>::__beta_i(a, b, x); }

}


#endif