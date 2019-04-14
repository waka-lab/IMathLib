#ifndef _IMATH_LINER_ALGEBRA_NORM_HPP
#define _IMATH_LINER_ALGEBRA_NORM_HPP

#include "IMathLib/math/math.hpp"
#include "IMathLib/math/liner_algebra/vector.hpp"
#include "IMathLib/math/liner_algebra/matrix.hpp"


namespace iml {

	//ベクトルに限り絶対値の定義
	template <class T, size_t N>
	struct Abs<vector<T, N>> {
		using result_type = typename math_function_type<T>::type;

		static result_type _abs_(const vector<T, N>& x) {
			result_type temp = addition_traits<T>::identity_element();
			for (size_t i = 0; i < N; ++i) temp += abs(x[i])*abs(x[i]);
			return sqrt(temp);
		}
	};

	//マンハッタンノルム
	template <class T>
	struct _Manhattan_norm {
		using result_type = typename math_function_type<T>::type;

		static constexpr result_type __manhattan_norm(const T& x) {
			return abs(x);
		}
	};
	template <class T, size_t N>
	struct _Manhattan_norm<vector<T, N>> {
		using result_type = typename math_function_type<T>::type;

		static result_type __manhattan_norm(const vector<T, N>& x) {
			result_type temp = addition_traits<T>::identity_element();
			for (size_t i = 0; i < N; ++i) temp += abs(x[i]);
			return temp;
		}
	};
	template <class T, size_t M, size_t N>
	struct _Manhattan_norm<matrix<T, M, N>> {
		using result_type = typename math_function_type<T>::type;

		static result_type __manhattan_norm(const matrix<T, M, N>& x, size_t n) {
			result_type temp = addition_traits<T>::identity_element();
			for (size_t i = 0; i < dimension<M, N>::value; ++i) temp += abs(x[0][i]);
			return temp;
		}
	};
	template <class T>
	inline constexpr auto manhattan_norm(const T& x) { return _Manhattan_norm<T>::__manhattan_norm(x); }


	//ユークリッドノルム
	template <class T>
	struct _Euclid_norm {
		using result_type = typename math_function_type<T>::type;

		static constexpr result_type __euclid_norm(const T& x) {
			return abs(x);
		}
	};
	template <class T, size_t N>
	struct _Euclid_norm<vector<T, N>> {
		using result_type = typename math_function_type<T>::type;

		static result_type __euclid_norm(const vector<T, N>& x) {
			result_type temp = addition_traits<T>::identity_element();
			for (size_t i = 0; i < N; ++i) temp += abs(x[i])*abs(x[i]);
			return sqrt(temp);
		}
	};
	template <class T, size_t M, size_t N>
	struct _Euclid_norm<matrix<T, M, N>> {
		using result_type = typename math_function_type<T>::type;

		static result_type __euclid_norm(const matrix<T, M, N>& x, size_t n) {
			result_type temp = addition_traits<T>::identity_element();
			for (size_t i = 0; i < dimension<M, N>::value; ++i) temp += abs(x[0][i])*abs(x[0][i]);
			return sqrt(temp);
		}
	};
	template <class T>
	inline constexpr auto euclid_norm(const T& x) { return _Euclid_norm<T>::__euclid_norm(x); }


	//チェビシェフノルム
	template <class T>
	struct _Chebyshev_norm {
		using result_type = typename math_function_type<T>::type;

		static constexpr result_type __chebyshev_norm(const T& x) {
			return abs(x);
		}
	};
	template <class T, size_t N>
	struct _Chebyshev_norm<vector<T, N>> {
		using result_type = typename math_function_type<T>::type;

		static result_type __chebyshev_norm(const vector<T, N>& x) {
			result_type temp = addition_traits<T>::identity_element();
			for (size_t i = 0; i < N; ++i)  if (abs(x[i]) > temp) tenp = abs(x[i]);
			return temp;
		}
	};
	template <class T, size_t M, size_t N>
	struct _Chebyshev_norm<matrix<T, M, N>> {
		using result_type = typename math_function_type<T>::type;

		static result_type __chebyshev_norm(const matrix<T, M, N>& x, size_t n) {
			result_type temp = addition_traits<T>::identity_element();
			for (size_t i = 0; i < dimension<M, N>::value; ++i) if (abs(x[0][i]) > temp) tenp = abs(x[0][i]);
			return temp;
		}
	};
	template <class T>
	inline constexpr auto chebyshev_norm(const T& x) { return _Chebyshev_norm<T>::__chebyshev_norm(x); }


	//Lpノルム
	template <class T>
	struct _Lp_norm {
		using result_type = typename math_function_type<T>::type;

		static constexpr result_type __lp_norm(const T& x, size_t n) {
			return abs(x);
		}
	};
	template <class T, size_t N>
	struct _Lp_norm<vector<T, N>> {
		using result_type = typename math_function_type<T>::type;

		static result_type __lp_norm(const vector<T, N>& x, size_t n) {
			result_type temp = addition_traits<T>::identity_element();
			for (size_t i = 0; i < N; ++i) temp += pow(abs(x[i]), n);
			return pow(temp, result_type(1) / n);
		}
	};
	template <class T, size_t M, size_t N>
	struct _Lp_norm<matrix<T, M, N>> {
		using result_type = typename math_function_type<T>::type;

		static result_type __lp_norm(const matrix<T, M, N>& x, size_t n) {
			result_type temp = addition_traits<T>::identity_element();
			for (size_t i = 0; i < dimension<M, N>::value; ++i) temp += pow(abs(x[0][i]), n);
			return pow(temp, result_type(1) / n);
		}
	};
	template <class T>
	inline constexpr auto lp_norm(const T& x, size_t n) { return _Lp_norm<T>::__lp_norm(x, n); }

}

#endif