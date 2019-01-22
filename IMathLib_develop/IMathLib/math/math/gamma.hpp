#ifndef _IMATH_MATH_GAMMA_HPP
#define _IMATH_MATH_GAMMA_HPP

#include "IMathLib/math/math/math_traits.hpp"
#include "IMathLib/math/math/bernoulli_number.hpp"
#include "IMathLib/math/math/pi.hpp"
#include "IMathLib/math/math/log.hpp"
#include "IMathLib/math/math/exp.hpp"
#include "IMathLib/math/math/pow.hpp"
#include "IMathLib/math/math/sin.hpp"
#include "IMathLib/math/math/tan.hpp"

namespace iml {

	//ガンマ関数の自然対数
	template <class T>
	struct _Lgamma {
		using result_type = typename math_function_type<T>::type;
		//x >= 0
		static constexpr result_type __lgamma(result_type x) {
			constexpr result_type log_const = log(2 * _Pi_<result_type>::pi) / 2;

			result_type k = 1;

			//ある程度xを大きくする(最大k=12!=479001600)
			while (x < 12) { k *= x;  ++x; }
			//B[2]=1/6
			result_type x1 = 1 / x, x2 = 1 / (6 * x), x3 = 0;
			result_type w = x * x;

			for (imsize_t i = 2; !error_evaluation(x2, x3) && (_IMATH_BERNOULLI_NUMBER_TABLE >= 2 * i); ++i) {
				x3 = x2;
				x1 /= w;
				x2 += x1 * bernoulli_number_table<_IMATH_BERNOULLI_NUMBER_TABLE, result_type>::b[2 * i] / (i*(2 * i - 1));
			}
			//kの補正を入れる
			return (log_const - x + (x - 0.5)*log(x) + x2 / 2 - log(k));
		}
	};
	template <class T>
	inline constexpr auto lgamma(const T& x) { return _Lgamma<T>::__lgamma(x); }


	//ガンマ関数
	template <class T>
	struct _Gamma {
		using result_type = typename math_function_type<T>::type;

		static constexpr result_type __gamma(const T& x) {
			//xが負数であるとき相反公式で反転
			return ((x < 0)
				? _Pi_<result_type>::pi / (sin(_Pi_<result_type>::pi * x)*exp(lgamma(1 - x)))
				: exp(lgamma(x)));
		}
	};
	template <class T>
	inline constexpr auto gamma(const T& x) { return _Gamma<T>::__gamma(x); }


	//第1種不完全ガンマ関数
	template <class T>
	struct _Gamma1 {
		using result_type = typename math_function_type<T>::type;

		static constexpr result_type __gamma1_impl(const result_type& a, const result_type& x) {
			result_type x1 = 1 / a, x2 = 1 / a, x3 = 0;

			for (imsize_t i = 1; !error_evaluation(x2, x3); ++i) {
				x3 = x2;
				x1 *= x / (a + i);
				x2 += x1;
			}
			return x2 * exp(-x)*pow(x, a);
		}
		static constexpr result_type __gamma2_impl(const result_type& a, const result_type& x) {
			result_type l1 = 1, l2 = x - a + 1, l3 = 0;
			result_type x1 = 1, x2 = 1 / (l1*l2), x3 = 0;

			for (imsize_t i = 1; !error_evaluation(x2, x3); ++i) {
				//一般化ラゲール陪多項式の更新
				//l3 = ((2 * (i + 1) - a - 1 + x)*l2 - ((i + 1) - a - 1)*l1) / (i + 1);
				l3 = ((2 * i - a + 1 + x)*l2 - (i - a)*l1) / (i + 1);
				l1 = l2; l2 = l3;

				x3 = x2;
				x1 *= (i - a) / (i + 1);
				x2 += x1 / (l1*l2);
			}
			return x2 * exp(-x)*pow(x, a);
		}
		static constexpr result_type __gamma1(const T& a, const T& x) {
			if (x == 0) return 0;
			return ((x >= 1 + a) ? gamma(a) - __gamma2_impl(a, x) : __gamma1_impl(a, x));
		}

		//ガンマ関数値が渡された場合のものを定義
		static constexpr result_type __gamma1(const T& a, const T& x, const result_type& ga) {
			if (x == 0) return 0;
			return ((x >= 1 + a) ? ga - __gamma2_impl(a, x) : __gamma1_impl(a, x));
		}
	};
	template <class T>
	inline constexpr auto gamma1(const T& a, const T& x) { return _Gamma1<T>::__gamma1(a, x); }


	//第2種不完全ガンマ関数
	template <class T>
	struct _Gamma2 {
		using result_type = typename math_function_type<T>::type;

		static constexpr result_type __gamma1_impl(const result_type& a, const result_type& x) {
			result_type x1 = 1 / a, x2 = 1 / a, x3 = 0;

			for (imsize_t i = 1; !error_evaluation(x2, x3); ++i) {
				x3 = x2;
				x1 *= x / (a + i);
				x2 += x1;
			}
			return x2 * exp(-x)*pow(x, a);
		}
		static constexpr result_type __gamma2_impl(const result_type& a, const result_type& x) {
			result_type l1 = 1, l2 = x - a + 1, l3 = 0;
			result_type x1 = 1, x2 = 1 / (l1*l2), x3 = 0;

			for (imsize_t i = 1; !error_evaluation(x2, x3); ++i) {
				//一般化ラゲール陪多項式の更新
				//l3 = ((2 * (i + 1) - a - 1 + x)*l2 - ((i + 1) - a - 1)*l1) / (i + 1);
				l3 = ((2 * i - a + 1 + x)*l2 - (i - a)*l1) / (i + 1);
				l1 = l2; l2 = l3;

				x3 = x2;
				x1 *= (i - a) / (i + 1);
				x2 += x1 / (l1*l2);
			}
			return x2 * exp(-x)*pow(x, a);
		}
		static constexpr result_type __gamma2(const T& a, const T& x) {
			return ((x < 1 + a) ? gamma(a) - __gamma1_impl(a, x) : __gamma2_impl(a, x));
		}

		//ガンマ関数値が渡された場合のものを定義
		static constexpr result_type __gamma2(const T& a, const T& x, const result_type& ga) {
			return ((x < 1 + a) ? ga - __gamma1_impl(a, x) : __gamma2_impl(a, x));
		}
	};
	template <class T>
	inline constexpr auto gamma2(const T& a, const T& x) { return _Gamma2<T>::__gamma2(a, x); }


	//第1種不完全ガンマ関数による正規化ガンマ関数
	template <class T>
	inline constexpr auto gamma_p(const T& a, const T& x) {
		if (a == 0) return static_cast<typename _Gamma1<T>::result_type>(1);
		auto temp = gamma(a);
		return _Gamma1<T>::__gamma1(a, x, temp) / temp;
	}

	//第2種不完全ガンマ関数による正規化ガンマ関数
	template <class T>
	inline constexpr auto gamma_q(const T& a, const T& x) {
		if (a == 0) return static_cast<typename _Gamma2<T>::result_type>(0);
		auto temp = gamma(a);
		return _Gamma2<T>::__gamma2(a, x, temp) / temp;
	}


	//ディガンマ関数
	template <class T>
	struct _Digamma {
		using result_type = typename math_function_type<T>::type;

		static constexpr result_type __digamma_impl(result_type x) {
			result_type k = 0;

			//ある程度xを大きくする
			while (x < 10) { k += 1 / x;  ++x; }
			result_type w = x * x;
			//B[2]=1/6
			result_type x1 = 1 / w, x2 = 1 / (6 * w), x3 = 0;

			for (imsize_t i = 2; !error_evaluation(x2, x3) && (_IMATH_BERNOULLI_NUMBER_TABLE >= 2 * i); ++i) {
				x3 = x2;
				x1 /= w;
				x2 += x1*bernoulli_number_table<_IMATH_BERNOULLI_NUMBER_TABLE, result_type>::b[2 * i] / i;
			}
			//kの補正を入れる
			return (log(x) - 1 / (2 * x) - x2 / 2 - k);
		}
		static constexpr result_type __digamma(const T& x) {
			//xが負数であるとき相反公式で反転
			return ((x < 0)
				? __digamma_impl(1 - x) - _Pi_<result_type>::pi / tan(_Pi_<result_type>::pi * x)
				: __digamma_impl(x));
		}
	};
	template <class T>
	inline constexpr auto digamma(const T& x) { return _Digamma<T>::__digamma(x); }
}


#endif