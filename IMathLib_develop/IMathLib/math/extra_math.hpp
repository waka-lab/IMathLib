#ifndef _EXTRA_MATH_HPP
#define _EXTRA_MATH_HPP

#include "IMathLib/math/math.hpp"
#include "IMathLib/utility/utility.hpp"
#include "IMathLib/utility/functional.hpp"
#include "IMathLib/math/dual_numbers.hpp"

//数学関数の拡張関数群

namespace iml {

	//浮動小数点に対して指数と仮数部に分解する(10進数)
	inline float frexp(float x, imint_t* e) {
		float buf = log10(abs(x));
		*e = static_cast<imint_t>(buf);
		buf = pow<float>(10, buf - *e);
		if (buf == 0)
			*e = 0;
		else if (buf < 1) {
			buf *= 10;
			--*e;
		}
		return (x < 0) ? -buf : buf;
	}
	inline double frexp(double x, imint_t* e) {
		double buf = log10(abs(x));
		imint_t temp = static_cast<imint_t>(buf);
		buf = pow<double>(10, buf - temp);
		if (buf == 0)
			temp = 0;
		else if (buf < 1) {
			buf *= 10;
			--temp;
		}
		if (e != nullptr) *e = temp;
		return (x < 0) ? -buf : buf;
	}
	inline long double frexp(long double x, imint_t* e) {
		long double buf = log10(x);
		*e = static_cast<imint_t>(buf);
		buf = pow<long double>(10, buf - *e);
		if (buf == 0)
			*e = 0;
		else if (buf < 1) {
			buf *= 10;
			--*e;
		}
		return buf;
	}
	//配列から最大要素のポインタを返す
	template <class T>
	inline T* max_element(T* first, T* last) {
		T* p = first++;
		if (first == last) return first;
		while (first != last) {
			if (*p < *first) p = first;
			++first;
		}
		return p;
	}
	//複数要素から最大値を返す
	template <class First>
	inline First multi_max(const First& first) {
		return first;
	}
	template <class First, class... Rest>
	inline First multi_max(const First& first, const Rest&... rest) {
		First buf = multi_max(rest...);
		return (first < buf ? buf : first);
	}
	//配列から最小要素のポインタを返す
	template <class T>
	inline T* min_element(T* first, T* last) {
		T* p = first++;
		if (first == last) return first;
		while (first != last) {
			if (*p > *first) p = first;
			++first;
		}
		return p;
	}
	//複数要素から最小値を返す
	template <class First>
	inline First multi_min(const First& first) {
		return first;
	}
	template <class First, class... Rest>
	inline First multi_min(const First& first, const Rest&... rest) {
		First buf = multi_min(rest...);
		return (first < buf ? first : buf);
	}
	//配列要素の和をとる
	template <class T1, class T2>
	T1 accumulate(T1 first, T1 last, T2 init = T2()) {
		while (first != last)
			init = init + *first++;
		return init;
	}
	//最大公約数
	template <class T>
	T gcd(const T& m, const T& n) {
		if ((0 == m) || (0 == n))
			return 0;
		while (m != n) {
			if (m > n) m = m - n;
			else n = n - m;
		}
		return m;
	}
	//最小公倍数
	template <class T>
	T lcm(const T& m, const T& n) {
		if ((0 == m) || (0 == n))
			return 0;
		return ((m / gcd(m, n)) * n);
	}
	//素数判定
	template <class T = imsize_t>
	inline bool prime(const T& n) {
		if ((n == 0) || (n == 1)) return false;			//0と1は素数ではない
		if ((n % 2) == 0) return false;					//偶数
		//精々sqrt(n)まで
		for (T i = 3, k = isqrt(n) + 1; i < k; i += 2)
			if ((n%i) == 0) return true;
		return false;
	}
	//複数要素の関数補正による最大値の要素
	template <class F, class First>
	inline First function_max(F f, const First& first) {
		return first;
	}
	template <class F, class First, class... Rest>
	inline First function_max(F f, const First& first, const Rest&... rest) {
		First buf = function_max(f, rest...);
		return (f(first) < f(buf) ? buf : first);
	}
	//区分的に有界である1変数関数の最大値の近似
	template <class F, class T>
	inline T function_range_max(F f, const T& first, const T& last, const T& step= 1E-5) {
		if (last - first < step) return f((last + first) / 2);
		return max(function_range_max(f, first, (last + first) / 2, step)
			, function_range_max(f, (last + first) / 2, last, step));
	}
	//複数要素の関数補正による最小値の要素
	template <class F, class First>
	inline First function_min(F f, const First& first) {
		return first;
	}
	template <class F, class First, class... Rest>
	inline First function_min(F f, const First& first, const Rest&... rest) {
		First buf = function_min(f, rest...);
		return (f(first) < f(buf) ? buf : first);
	}
	//区分的に有界である1変数関数の最小値の近似
	template <class F, class T>
	inline T function_range_min(F f, const T& first, const T& last, const T& step = 1E-5) {
		if (last - first < step) return f((last + first) / 2);
		return max(function_range_min(f, first, (last + first) / 2, step)
			, function_range_min(f, (last + first) / 2, last, step));
	}
	//五点近似による数値微分
	template <class F, class T>
	inline T differential(F f, const T& x, const T& epsilon = 1E-5) {
		return (f(x - 2 * epsilon) - 8 * f(x - epsilon) + 8 * f(x + epsilon) - f(x + 2 * epsilon)) / (12 * epsilon);
	}
	//区分的に有界である1変数関数の数値積分(関数ポインタ,領域,最大誤差)
	template <class F, class T>
	inline T imint_tegral(F f, const T& first, const T& last, const T& epsilon = 1E-5) {
		auto	ffc = [&f](const T& x) {return abs<T>(forward<T>(f(x))); };			//関数の絶対値の最大値を返す
		T		f_max = first;		//区間での関数の最大値
		imsize_t	n;					//分割数
		T		step;				//分割幅
		T		result = 0;
		T		buf = 0;
		//関数の最大値探索
		f_max = function_range_max(ffc, first, last, epsilon);
		//分割数の取得
		n = static_cast<imsize_t>(sqrt(sqrt(abs(pow(last - first, 5)*f_max / (epsilon * 180)))));
		if ((n & 1) != 0) ++n;			//基数ならば偶数にする
		step = (last - first) / n;
		//分割数に応じてシンプソンの公式を適用
		result += f(first) + f(last);					//両端
		for (imsize_t i = 1; i <= n / 2 - 1; ++i)
			buf += f(first + step * 2 * i);
		result += 2 * buf; buf = 0;
		for (imsize_t i = 1; i <= n / 2; ++i)
			buf += f(first + step*(2 * i - 1));
		result += 4 * buf;
		return step / 3 * result;
	}
	//オイラーのトーシェント関数
	inline imsize_t euler_totient(imsize_t x) {
		imsize_t d = 3, t = x;

		if ((x & 1) == 0) {
			//t /= 2;
			t >>= 1;
			do { x >>= 1; } while ((x & 1) == 0);
		}
		while (x / d >= d) {
			if (x % d == 0) {
				t = t / d * (d - 1);
				do { x /= d; } while (x % d == 0);
			}
			d += 2;
		}
		return ((x > 1) ? t / x * (x - 1) : t);
	}
	//アッカーマン関数
	inline imint_t ackermann(imint_t x, imint_t y) {
		if (x == 0) return y + 1;
		if (y == 0) return ackermann(x - 1, 1);
		return ackermann(x - 1, ackermann(x, y - 1));
	}
	//第一種スターリング数(n > 0)
	inline imint_t stirling1(imint_t n, imint_t k) {
		if (k < 1 || k > n) return 0;
		if (k == n) return 1;
		return (n - 1) * stirling1(n - 1, k)
			+ stirling1(n - 1, k - 1);
	}
	//第二種スターリング数(n > 0)
	inline imint_t stirling2(imint_t n, imint_t k) {
		if (k < 1 || k > n) return 0;
		if (k == 1 || k == n) return 1;
		return k * stirling2(n - 1, k)
			+ stirling2(n - 1, k - 1);
	}

}

#endif