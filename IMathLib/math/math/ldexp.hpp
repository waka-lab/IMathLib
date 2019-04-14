#ifndef IMATH_MATH_MATH_LDEXP_HPP
#define IMATH_MATH_MATH_LDEXP_HPP

#include "IMathLib/IMathLib_config.hpp"

namespace iml {

	//指数部と仮数部から浮動小数点を復元
	template <class>
	struct Ldexp2;
	template <>
	struct Ldexp2<float> {
		template <class Int>
		static constexpr float _ldexp2_(float x, Int n) {
			constexpr float lshift = uint16_t(-1) + 1.f;
			constexpr float rshift = 1.f / lshift;

			if (n >= 0) {
				while (n >= 16) {
					x *= lshift;
					n -= 16;
				}
				if (n) x *= uint16_t(1) << n;
			}
			else {
				while (n <= -16) {
					x *= rshift;
					n += 16;
				}
				if (n) x *= rshift * 2 * (uint16_t(1) << (15 + n));
			}
			return x;
		}
	};
	template <>
	struct Ldexp2<double> {
		template <class Int>
		static constexpr double _ldexp2_(double x, Int n) {
			constexpr double lshift = uint64_t(-1) + 1.;
			constexpr double rshift = 1. / lshift;

			if (n >= 0) {
				while (n >= 64) {
					x *= lshift;
					n -= 64;
				}
				if (n) x *= uint64_t(1) << n;
			}
			else {
				while (n <= -64) {
					x *= rshift;
					n += 64;
				}
				if (n) x *= rshift * 2 * (uint64_t(1) << (63 + n));
			}
			return x;
		}
	};
	template <>
	struct Ldexp2<long double> {
		template <class Int>
		static constexpr long double _ldexp2_(long double x, Int n) {
			constexpr long double lshift = uint64_t(-1) + 1.l;
			constexpr long double rshift = 1.l / lshift;

			if (n >= 0) {
				while (n >= 64) {
					x *= lshift;
					n -= 64;
				}
				if (n) x *= uint64_t(1) << n;
			}
			else {
				while (n <= -64) {
					x *= rshift;
					n += 64;
				}
				if (n) x *= rshift * 2 * (uint64_t(1) << (63 + n));
			}
			return x;
		}
	};
	template <class T, class Int>
	inline constexpr auto ldexp2(T x, Int n) { return Ldexp2<T>::_ldexp2_(x, n); }

}


#endif