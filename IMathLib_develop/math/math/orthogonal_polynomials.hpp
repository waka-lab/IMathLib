#ifndef _IMATH_MATH_ORTHOGONAL_PLOYNOMIALS_HPP
#define _IMATH_MATH_ORTHOGONAL_PLOYNOMIALS_HPP

#include "IMathLib/math/math/math_traits.hpp"
#include "IMathLib/math/math/sqrt.hpp"
#include "IMathLib/math/math/pow.hpp"
#include "IMathLib/math/math/factorial.hpp"

//直交多項式の実装

namespace iml {

	//ラゲール多項式(L_{n}(x))
	template <class T>
	inline constexpr auto laguerre_polynomials(size_t n, const T& x) {
		using result_type = typename math_function_type<T>::type;

		if (n == 0) return result_type(1);
		if (n == 1) return result_type(-x + 1);
		return ((2 * n - 1 - x)*laguerre_polynomials(n - 1, x) - (n - 1)*laguerre_polynomials(n - 2, x)) / n;
	}

	//ラゲール陪多項式(L_{n}^{k}(x))
	template <class T>
	inline constexpr auto associated_laguerre_polynomials(size_t n, size_t k, const T& x) {
		using result_type = typename math_function_type<T>::type;

		if (n == 0) return result_type(1);
		if (n == 1) return result_type(-x + k + 1);
		return ((2 * n + k - 1 - x)*associated_laguerre_polynomials(n - 1, k, x) - (n + k - 1)*associated_laguerre_polynomials(n - 2, k, x)) / n;
	}


	//ルジャンドル多項式(P_{k}(x))
	template <class T>
	inline constexpr auto legendre_polynomials(size_t k, const T& x) {
		using result_type = typename math_function_type<T>::type;

		if (k == 0) return result_type(1);
		if (k == 1) return result_type(x);
		return ((2 * k - 1)*x*legendre_polynomials(k - 1, x) - (k - 1)*legendre_polynomials(k - 2, x)) / k;
	}

	//ルジャンドル陪多項式(P_{k}^{m}(x))
	template <class T>
	inline constexpr auto associated_legendre_polynomials(size_t k, size_t m, const T& x) {
		using result_type = typename math_function_type<T>::type;

		if (m == k + 1) return result_type(0);
		if (m == k) {
			//mが奇数なら(整数+1/2)乗に分解
			if ((m & 1) == 1) return double_fact(2 * m - 1, pow(1 - x * x, (m - 1) / 2)*sqrt(1 - x * x));
			else return double_fact(2 * m - 1, pow(1 - x * x, m / 2));
		}
		return ((2 * k - 1)*x*associated_legendre_polynomials(k - 1, m, x) - (k + m - 1)*associated_legendre_polynomials(k - 2, m, x)) / (k - m);
	}

}


#endif