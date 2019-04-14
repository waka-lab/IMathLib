#ifndef IMATH_MATH_MATH_GAMMA_HPP
#define IMATH_MATH_MATH_GAMMA_HPP

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
	struct Lgamma {
		using result_type = typename math_function_type<T>::type;
		//x >= 0 を仮定
		static constexpr result_type _lgamma_(result_type x) {
			constexpr result_type log_const = log(2 * pi<result_type>) / 2;

			result_type k = 1;

			//ある程度xを大きくする(最大k=12!=479001600)
			while (x < 12) { k *= x;  ++x; }
			//B[2]=1/6
			result_type x1 = 1 / x, x2 = 1 / (6 * x), x3 = 0;
			result_type w = x * x;

			for (size_t i = 2; !error_evaluation(x2, x3) && (IMATH_BERNOULLI_NUMBER_TABLE >= 2 * i); ++i) {
				x3 = x2;
				x1 /= w;
				x2 += x1 * bernoulli_number_table<IMATH_BERNOULLI_NUMBER_TABLE, result_type>::b[2 * i] / (i*(2 * i - 1));
			}
			//kの補正を入れる
			return (log_const - x + (x - 0.5)*log(x) + x2 / 2 - log(k));
		}
	};
	template <class T>
	inline constexpr auto lgamma(const T& x) { return Lgamma<T>::_lgamma_(x); }


	//ガンマ関数
	template <class T>
	struct Gamma {
		using result_type = typename math_function_type<T>::type;

		static constexpr result_type _gamma_(const T& x) {
			//xが負数であるとき相反公式で反転
			return ((x < 0)
				? pi<result_type> / (sin(pi<result_type> * x)*exp(lgamma(1 - x)))
				: exp(lgamma(x)));
		}
	};
	template <class T>
	inline constexpr auto gamma(const T& x) { return Gamma<T>::_gamma_(x); }


	//第1種不完全ガンマ関数
	template <class T>
	struct Gamma1 {
		using result_type = typename math_function_type<T>::type;

		static constexpr result_type _gamma1_impl_(const result_type& a, const result_type& x) {
			result_type x1 = 1 / a, x2 = 1 / a, x3 = 0;

			for (size_t i = 1; !error_evaluation(x2, x3); ++i) {
				x3 = x2;
				x1 *= x / (a + i);
				x2 += x1;
			}
			return x2 * exp(-x)*pow(x, a);
		}
		static constexpr result_type _gamma2_impl_(const result_type& a, const result_type& x) {
			result_type l1 = 1, l2 = x - a + 1, l3 = 0;
			result_type x1 = 1, x2 = 1 / (l1*l2), x3 = 0;

			for (size_t i = 1; !error_evaluation(x2, x3); ++i) {
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
		static constexpr result_type _gamma1_(const T& a, const T& x) {
			if (x == 0) return 0;
			return ((x >= 1 + a) ? gamma(a) - _gamma2_impl_(a, x) : _gamma1_impl_(a, x));
		}

		//ガンマ関数値が渡された場合のものを定義
		static constexpr result_type _gamma1_(const T& a, const T& x, const result_type& ga) {
			if (x == 0) return 0;
			return ((x >= 1 + a) ? ga - _gamma2_impl_(a, x) : _gamma1_impl_(a, x));
		}
	};
	template <class T>
	inline constexpr auto gamma1(const T& a, const T& x) { return Gamma1<T>::_gamma1_(a, x); }


	//第2種不完全ガンマ関数
	template <class T>
	struct Gamma2 {
		using result_type = typename math_function_type<T>::type;

		static constexpr result_type _gamma1_impl_(const result_type& a, const result_type& x) {
			result_type x1 = 1 / a, x2 = 1 / a, x3 = 0;

			for (size_t i = 1; !error_evaluation(x2, x3); ++i) {
				x3 = x2;
				x1 *= x / (a + i);
				x2 += x1;
			}
			return x2 * exp(-x)*pow(x, a);
		}
		static constexpr result_type _gamma2_impl_(const result_type& a, const result_type& x) {
			result_type l1 = 1, l2 = x - a + 1, l3 = 0;
			result_type x1 = 1, x2 = 1 / (l1*l2), x3 = 0;

			for (size_t i = 1; !error_evaluation(x2, x3); ++i) {
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
		static constexpr result_type _gamma2_(const T& a, const T& x) {
			return ((x < 1 + a) ? gamma(a) - _gamma1_impl_(a, x) : _gamma2_impl_(a, x));
		}

		//ガンマ関数値が渡された場合のものを定義
		static constexpr result_type _gamma2_(const T& a, const T& x, const result_type& ga) {
			return ((x < 1 + a) ? ga - _gamma1_impl_(a, x) : _gamma2_impl_(a, x));
		}
	};
	template <class T>
	inline constexpr auto gamma2(const T& a, const T& x) { return Gamma2<T>::_gamma2_(a, x); }


	//第1種不完全ガンマ関数による正規化ガンマ関数
	template <class T>
	inline constexpr auto gamma_p(const T& a, const T& x) {
		if (a == 0) return static_cast<typename Gamma1<T>::result_type>(1);
		auto temp = gamma(a);
		return Gamma1<T>::_gamma1_(a, x, temp) / temp;
	}

	//第2種不完全ガンマ関数による正規化ガンマ関数
	template <class T>
	inline constexpr auto gamma_q(const T& a, const T& x) {
		if (a == 0) return static_cast<typename Gamma2<T>::result_type>(0);
		auto temp = gamma(a);
		return Gamma2<T>::_gamma2_(a, x, temp) / temp;
	}


	//ディガンマ関数
	template <class T>
	struct Digamma {
		using result_type = typename math_function_type<T>::type;

		static constexpr result_type _digamma_impl_(result_type x) {
			result_type k = 0;

			//ある程度xを大きくする
			while (x < 10) { k += 1 / x;  ++x; }
			result_type w = x * x;
			//B[2]=1/6
			result_type x1 = 1 / w, x2 = 1 / (6 * w), x3 = 0;

			for (size_t i = 2; !error_evaluation(x2, x3) && (IMATH_BERNOULLI_NUMBER_TABLE >= 2 * i); ++i) {
				x3 = x2;
				x1 /= w;
				x2 += x1*bernoulli_number_table<IMATH_BERNOULLI_NUMBER_TABLE, result_type>::b[2 * i] / i;
			}
			//kの補正を入れる
			return (log(x) - 1 / (2 * x) - x2 / 2 - k);
		}
		static constexpr result_type _digamma_(const T& x) {
			//xが負数であるとき相反公式で反転
			return ((x < 0)
				? _digamma_impl_(1 - x) - pi<result_type> / tan(pi<result_type> * x)
				: _digamma_impl_(x));
		}
	};
	template <class T>
	inline constexpr auto digamma(const T& x) { return Digamma<T>::_digamma_(x); }
}


#endif