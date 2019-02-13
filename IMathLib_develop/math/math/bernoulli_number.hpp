#ifndef _IMATH_MATH_BERNOULLI_NUMBER_HPP
#define _IMATH_MATH_BERNOULLI_NUMBER_HPP

#include "IMathLib/math/math/math_traits.hpp"
#include "IMathLib/math/math/pow.hpp"
#include "IMathLib/math/math/factorial.hpp"
#include "IMathLib/math/math/cos.hpp"
#include "IMathLib/math/math/sin.hpp"
#include "IMathLib/math/math/pi.hpp"
#include "IMathLib/utility/tuple.hpp"


namespace iml {

	//ベルヌーイ数のテーブル
	template <imsize_t, class, class>
	struct _Bernoulli_number_table;
	template <imsize_t N, class T, imsize_t... Indices>
	struct _Bernoulli_number_table<N, T, index_imu_tuple<Indices...>> {
		//浮動小数点のみ
		using result_type = typename conditional<is_floating_point<T>::value, T, IMATH_DEFAULT_TYPE>::type;

		static constexpr result_type __bernoulli_number(imsize_t n) {
			result_type bn[N + 1] = {};
			result_type x1 = 0, x2 = 0;
			
			if (n == 1) return -0.5;
			else if ((n & 1) == 1) return 0;			//1でない奇数のときは0
			else if (n == 0) return 1;

			bn[0] = 1; bn[1] = -0.5;
			for (imsize_t i = 2; i <= n; ++i) {
				//奇数なら次の計算
				if ((i & 1) == 1) continue;
				//奇数項が0であることを考慮
				x1 = 1;
				x2 = bn[0] + (i + 1)*bn[1];
				for (imsize_t j = 2; j <= i - 1; j += 2) {
					x1 *= result_type(i + 2 - j) / j * (i + 2 - (j - 1)) / (j - 1);
					x2 += x1 * bn[j];
				}
				bn[i] = -x2 / (i + 1);
			}
			return bn[n];
		}

		static constexpr result_type b[N] = { __bernoulli_number(Indices)... };
	};
	template <imsize_t N, class T = IMATH_DEFAULT_TYPE>
	struct bernoulli_number_table : _Bernoulli_number_table<N, T, typename index_imu_range<0, N>::type> {};

	//ベルヌーイ数
	template <class T>
	inline constexpr auto bernoulli_number(imsize_t n) {
		//浮動小数点のみ
		using result_type = typename conditional<is_floating_point<T>::value, T, IMATH_DEFAULT_TYPE>::type;

		if (n == 1) return result_type(-0.5);
		else if ((n & 1) == 1) return result_type(0);			//1でない奇数のときは0
		else if (n == 0) return result_type(1);

		//i=1のとき
		result_type x1 = -1;
		result_type x2 = 0;
		for (imsize_t i = 0; i <= n; ++i) x2 -= result_type(i) / (i + 1);

		for (imsize_t i = 2; i <= n; ++i) {
			x1 *= -pow(result_type(i) / (i - 1), n);
			result_type temp1 = result_type(1) / (i + 1);
			result_type temp2 = 1;
			for (imsize_t j = i + 1; j <= n; ++j) {
				temp2 /= (1 - result_type(i) / j);
				temp1 += temp2 / (j + 1);
			}
			x2 += x1 * temp1;
		}
		return x2;
	}

	//周期ベルヌーイ多項式
	template <class T>
	struct _Period_bernoulli_polynomial {
		using result_type = typename math_function_type<T>::type;

		//cos
		static constexpr result_type __period_bernoulli_polynomial_impl1(imsize_t k, const T& x) {
			result_type c1 = cos(2 * _Pi_<result_type>::pi*x), s1 = sin(2 * _Pi_<result_type>::pi*x);
			result_type c2 = c1, s2 = s1;

			//初期値の構築
			result_type x1 = 2;
			for (imsize_t i = 1; i <= k; ++i) x1 *= i / (2 * _Pi_<result_type>::pi);
			result_type x2 = c2 * x1, x3 = 0;

			imsize_t i = 2;
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
		//sin
		static constexpr result_type __period_bernoulli_polynomial_impl2(imsize_t k, const T& x) {
			result_type c1 = cos(2 * _Pi_<result_type>::pi*x), s1 = sin(2 * _Pi_<result_type>::pi*x);
			result_type c2 = c1, s2 = s1;

			//初期値の構築
			result_type x1 = 2;
			for (imsize_t i = 1; i <= k; ++i) x1 *= i / (2 * _Pi_<result_type>::pi);
			result_type x2 = s2 * x1, x3 = 0;

			for (imsize_t i = 2; !error_evaluation(x2, x3); ++i) {
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
		static constexpr result_type __period_bernoulli_polynomial(imsize_t k, const T& x) {
			//4の剰余で場合分け
			switch (k & 3) {
			case 0:
				return -__period_bernoulli_polynomial_impl1(k, x);
			case 1:
				return -__period_bernoulli_polynomial_impl2(k, x);
			case 2:
				return __period_bernoulli_polynomial_impl1(k, x);
			case 3:
				return __period_bernoulli_polynomial_impl2(k, x);
			}
		}
	};
	template <class T>
	inline constexpr auto period_bernoulli_polynomial(imsize_t n, const T& x) { return _Period_bernoulli_polynomial<T>::__period_bernoulli_polynomial(n, x); }

}


#endif