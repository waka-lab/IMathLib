#ifndef _EXTRA_MATH_HPP
#define _EXTRA_MATH_HPP

#include "IMathLib/math/math.hpp"
#include "IMathLib/utility/utility.hpp"
#include "IMathLib/utility/functional.hpp"
#include "IMathLib/math/hypercomplex.hpp"

//数学関数の拡張関数群

namespace iml {

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
	template <class T = size_t>
	inline bool prime(const T& n) {
		if ((n == 0) || (n == 1)) return false;			//0と1は素数ではない
		if ((n % 2) == 0) return false;					//偶数
		//精々sqrt(n)まで
		for (T i = 3, k = isqrt(n) + 1; i < k; i += 2)
			if ((n%i) == 0) return true;
		return false;
	}
	//オイラーのトーシェント関数
	inline size_t euler_totient(size_t x) {
		size_t d = 3, t = x;

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
	inline int_t ackermann(int_t x, int_t y) {
		if (x == 0) return y + 1;
		if (y == 0) return ackermann(x - 1, 1);
		return ackermann(x - 1, ackermann(x, y - 1));
	}
	//第一種スターリング数(n > 0)
	inline int_t stirling1(int_t n, int_t k) {
		if (k < 1 || k > n) return 0;
		if (k == n) return 1;
		return (n - 1) * stirling1(n - 1, k)
			+ stirling1(n - 1, k - 1);
	}
	//第二種スターリング数(n > 0)
	inline int_t stirling2(int_t n, int_t k) {
		if (k < 1 || k > n) return 0;
		if (k == 1 || k == n) return 1;
		return k * stirling2(n - 1, k)
			+ stirling2(n - 1, k - 1);
	}

}

#endif