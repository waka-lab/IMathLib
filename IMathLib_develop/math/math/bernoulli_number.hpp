#ifndef IMATH_MATH_MATH_BERNOULLI_NUMBER_HPP
#define IMATH_MATH_MATH_BERNOULLI_NUMBER_HPP

#include "IMathLib/math/math/math_traits.hpp"
#include "IMathLib/math/math/pow.hpp"
#include "IMathLib/math/math/factorial.hpp"
#include "IMathLib/math/math/trigonometric_function.hpp"
#include "IMathLib/math/math/pi.hpp"
#include "IMathLib/utility/tuple.hpp"


namespace iml {

	//ベルヌーイ数のテーブル
	template <size_t, class, class>
	struct Bernoulli_number_table;
	template <size_t N, class T, size_t... Indices>
	struct Bernoulli_number_table<N, T, index_tuple<size_t, Indices...>> {
		using result_type = typename math_function_type<T>::type;

		static constexpr result_type __bernoulli_number(size_t n) {
			result_type bn[N + 1] = {};
			result_type x1 = 0, x2 = 0;
			
			if (n == 1) return -0.5;
			else if ((n & 1) == 1) return 0;			//1でない奇数のときは0
			else if (n == 0) return 1;

			bn[0] = 1; bn[1] = -0.5;
			for (size_t i = 2; i <= n; ++i) {
				//奇数なら次の計算
				if ((i & 1) == 1) continue;
				//奇数項が0であることを考慮
				x1 = 1;
				x2 = bn[0] + (i + 1)*bn[1];
				for (size_t j = 2; j <= i - 1; j += 2) {
					x1 *= result_type(i + 2 - j) / j * (i + 2 - (j - 1)) / (j - 1);
					x2 += x1 * bn[j];
				}
				bn[i] = -x2 / (i + 1);
			}
			return bn[n];
		}

		static constexpr result_type b[N] = { __bernoulli_number(Indices)... };
	};
	template <size_t N, class T = IMATH_DEFAULT_FLOATING_POINT>
	struct bernoulli_number_table : Bernoulli_number_table<N, T, typename index_range<size_t, 0, N>::type> {};

	//ベルヌーイ数
	template <class T>
	inline constexpr auto bernoulli_number(size_t n) {
		using result_type = typename math_function_type<T>::type;

		if (n == 1) return result_type(-0.5);
		else if ((n & 1) == 1) return result_type(0);			//1でない奇数のときは0
		else if (n == 0) return result_type(1);

		//i=1のとき
		result_type x1 = -1;
		result_type x2 = 0;
		for (size_t i = 0; i <= n; ++i) x2 -= result_type(i) / (i + 1);

		for (size_t i = 2; i <= n; ++i) {
			x1 *= -pow(result_type(i) / (i - 1), n);
			result_type temp1 = result_type(1) / (i + 1);
			result_type temp2 = 1;
			for (size_t j = i + 1; j <= n; ++j) {
				temp2 /= (1 - result_type(i) / j);
				temp1 += temp2 / (j + 1);
			}
			x2 += x1 * temp1;
		}
		return x2;
	}

	//周期ベルヌーイ多項式
	template <class T>
	struct Period_bernoulli_polynomial {
		using result_type = typename math_function_type<T>::type;

		//cos級数の場合
		static constexpr result_type _period_bernoulli_polynomial_impl1_(size_t k, const T& x) {
			result_type c1 = cos(2 * pi<result_type>*x), s1 = sin(2 * pi<result_type>*x);
			result_type c2 = c1, s2 = s1;

			//初期値の構築
			result_type x1 = 2;
			for (size_t i = 1; i <= k; ++i) x1 *= i / (2 * pi<result_type>);
			result_type x2 = c2 * x1, x3 = 0;

			size_t i = 2;
			for (i = 2; !error_evaluation(x2, x3); ++i) {
				//複素数の積に基づき回転
				result_type temp_c = c1 * c2 - s1 * s2;
				result_type temp_s = c1 * s2 + c2 * s1;
				c2 = temp_c;
				s2 = temp_s;

				x3 = x2;
				x1 *= pow(result_type(i - 1) / i, k);
				x2 += x1 * c2;
			}
			return x2;
		}
		//sin級数の場合
		static constexpr result_type _period_bernoulli_polynomial_impl2_(size_t k, const T& x) {
			result_type c1 = cos(2 * pi<result_type>*x), s1 = sin(2 * pi<result_type>*x);
			result_type c2 = c1, s2 = s1;

			//初期値の構築
			result_type x1 = 2;
			for (size_t i = 1; i <= k; ++i) x1 *= i / (2 * pi<result_type>);
			result_type x2 = s2 * x1, x3 = 0;

			for (size_t i = 2; !error_evaluation(x2, x3); ++i) {
				//複素数の積に基づき回転
				result_type temp_c = c1 * c2 - s1 * s2;
				result_type temp_s = c1 * s2 + c2 * s1;
				c2 = temp_c;
				s2 = temp_s;

				x3 = x2;
				x1 *= pow(result_type(i - 1) / i, k);
				x2 += x1 * s2;
			}
			return x2;
		}
		static constexpr result_type _period_bernoulli_polynomial_(size_t k, const T& x) {
			//4の剰余で場合分け
			switch (k & 3) {
			case 0:
				return -_period_bernoulli_polynomial_impl1_(k, x);
			case 1:
				return -_period_bernoulli_polynomial_impl2_(k, x);
			case 2:
				return _period_bernoulli_polynomial_impl1_(k, x);
			case 3:
				return _period_bernoulli_polynomial_impl2_(k, x);
			}
		}
	};
	template <class T>
	inline constexpr auto period_bernoulli_polynomial(size_t n, const T& x) { return Period_bernoulli_polynomial<T>::_period_bernoulli_polynomial_(n, x); }

}


#endif