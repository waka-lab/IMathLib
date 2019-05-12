#ifndef _IMATH_LINER_ALGEBRA_NORM_HPP
#define _IMATH_LINER_ALGEBRA_NORM_HPP

#include "IMathLib/math/math.hpp"
#include "IMathLib/math/liner_algebra/vector.hpp"
#include "IMathLib/math/liner_algebra/matrix.hpp"


namespace iml {

	template <class T, size_t N>
	struct Abs<vector<T, N>> {
		static constexpr auto _abs_(const vector<T, N>& x) {
			auto temp = abs2(x[0]);
			for (size_t i = 1; i < N; ++i) temp += abs2(x[i]);
			return sqrt(temp);
		}
	};
	template <class T, size_t N>
	struct Abs2<vector<T, N>> {
		static constexpr auto _abs_(const vector<T, N>& x) {
			auto temp = abs2(x[0]);
			for (size_t i = 1; i < N; ++i) temp += abs2(x[i]);
			return temp;
		}
	};

	//マンハッタンノルム
	template <class T>
	struct Manhattan_norm {
		static constexpr auto _manhattan_norm_(const T& x) {
			return abs(x);
		}
	};
	template <class T, size_t N>
	struct Manhattan_norm<vector<T, N>> {
		static auto _manhattan_norm_(const vector<T, N>& x) {
			auto temp = abs(x[0]);
			for (size_t i = 1; i < N; ++i) temp += abs(x[i]);
			return temp;
		}
	};
	template <class T, size_t M, size_t N>
	struct Manhattan_norm<matrix<T, M, N>> {
		static auto _manhattan_norm_(const matrix<T, M, N>& x, size_t n) {
			auto temp = abs(x[0][0]);
			for (size_t i = 1; i < M * N; ++i) temp += abs(x[0][i]);
			return temp;
		}
	};
	template <class T>
	inline constexpr auto manhattan_norm(const T& x) { return Manhattan_norm<T>::_manhattan_norm_(x); }


	//ユークリッドノルム
	template <class T>
	struct Euclid_norm {
		static constexpr auto _euclid_norm_(const T& x) {
			return abs(x);
		}
	};
	template <class T, size_t N>
	struct Euclid_norm<vector<T, N>> {
		static constexpr auto _euclid_norm_(const vector<T, N>& x) {
			auto temp = abs2(x[0]);
			for (size_t i = 1; i < N; ++i) temp += abs2(x[i]);
			return sqrt(temp);
		}
	};
	template <class T, size_t M, size_t N>
	struct Euclid_norm<matrix<T, M, N>> {
		static constexpr auto _euclid_norm_(const matrix<T, M, N>& x, size_t n) {
			auto temp = abs2(x[0][0]);
			for (size_t i = 1; i < M * N; ++i) temp += abs2(x[0][i]);
			return sqrt(temp);
		}
	};
	template <class T>
	inline constexpr auto euclid_norm(const T& x) { return Euclid_norm<T>::_euclid_norm_(x); }


	//チェビシェフノルム
	template <class T>
	struct Chebyshev_norm {
		static constexpr auto _hebyshev_norm_(const T& x) {
			return abs(x);
		}
	};
	template <class T, size_t N>
	struct Chebyshev_norm<vector<T, N>> {
		static constexpr auto _hebyshev_norm_(const vector<T, N>& x) {
			auto temp = abs(x[0]);
			for (size_t i = 0; i < N; ++i)  if (abs(x[i]) > temp) tenp = abs(x[i]);
			return temp;
		}
	};
	template <class T, size_t M, size_t N>
	struct Chebyshev_norm<matrix<T, M, N>> {
		static constexpr auto _hebyshev_norm_(const matrix<T, M, N>& x) {
			auto temp = abs(x[0]);
			for (size_t i = 1; i < M * N; ++i) if (abs(x[0][i]) > temp) tenp = abs(x[0][i]);
			return temp;
		}
	};
	template <class T>
	inline constexpr auto chebyshev_norm(const T& x) { return Chebyshev_norm<T>::_hebyshev_norm_(x); }


	//Lpノルム
	template <class T>
	struct Lp_norm {
		static constexpr auto _lp_norm_(const T& x, size_t n) {
			return abs(x);
		}
	};
	template <class T, size_t N>
	struct Lp_norm<vector<T, N>> {
		static constexpr auto _lp_norm_(const vector<T, N>& x, size_t n) {
			auto temp = pow(abs(x[0]), n);
			for (size_t i = 1; i < N; ++i) temp += pow(abs(x[i]), n);
			return nth_root(temp,  n);
		}
	};
	template <class T, size_t M, size_t N>
	struct Lp_norm<matrix<T, M, N>> {
		static constexpr auto _lp_norm_(const matrix<T, M, N>& x, size_t n) {
			auto temp = pow(abs(x[0][0]), n);
			for (size_t i = 1; i < M * N; ++i) temp += pow(abs(x[0][i]), n);
			return nth_root(temp, n);
		}
	};
	template <class T>
	inline constexpr auto lp_norm(const T& x, size_t n) { return Lp_norm<T>::_lp_norm_(x, n); }

}

#endif