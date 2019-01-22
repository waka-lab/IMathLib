#ifndef _IMATH_MATH_SGN_HPP
#define _IMATH_MATH_SGN_HPP

#include "IMathLib/math/math/math_traits.hpp"

namespace iml {

	//符号関数
	template<class>
	struct _Sgn;
	template<>
	struct _Sgn<bool> {
		static constexpr imint_t __sgn(bool x) { return x ? 1 : -1; }
	};
	template<>
	struct _Sgn<char> {
		static constexpr imint_t __sgn(char x) { return (x == 0) ? 0 : ((x < 0) ? -1 : 1); }
	};
	template<>
	struct _Sgn<unsigned char> {
		static constexpr imint_t __sgn(unsigned char x) { return (x == 0) ? 0 : 1; }
	};
	template<>
	struct _Sgn<wchar_t> {
		static constexpr imint_t __sgn(wchar_t x) { return (x == 0) ? 0 : 1; }
	};
	template<>
	struct _Sgn<short> {
		static constexpr imint_t __sgn(short x) { return (x == 0) ? 0 : ((x < 0) ? -1 : 1); }
	};
	template<>
	struct _Sgn<unsigned short> {
		static constexpr imint_t __sgn(unsigned short x) { return (x == 0) ? 0 : 1; }
	};
	template<>
	struct _Sgn<int> {
		static constexpr imint_t __sgn(int x) { return (x == 0) ? 0 : ((x < 0) ? -1 : 1); }
	};
	template<>
	struct _Sgn<unsigned int> {
		static constexpr imint_t __sgn(unsigned int x) { return (x == 0) ? 0 : 1; }
	};
	template<>
	struct _Sgn<long> {
		static constexpr imint_t __sgn(long x) { return (x == 0) ? 0 : ((x < 0) ? -1 : 1); }
	};
	template<>
	struct _Sgn<unsigned long> {
		static constexpr imint_t __sgn(unsigned long x) { return (x == 0) ? 0 : 1; }
	};
	template<>
	struct _Sgn<char16_t> {
		static constexpr imint_t __sgn(char16_t x) { return (x == 0) ? 0 : 1; }
	};
	template<>
	struct _Sgn<char32_t> {
		static constexpr imint_t __sgn(char32_t x) { return (x == 0) ? 0 : 1; }
	};
	template<>
	struct _Sgn<long long> {
		static constexpr imint_t __sgn(long long x) { return (x == 0) ? 0 : ((x < 0) ? -1 : 1); }
	};
	template<>
	struct _Sgn<unsigned long long> {
		static constexpr imint_t __sgn(unsigned long long x) { return (x == 0) ? 0 : 1; }
	};
	template<>
	struct _Sgn<float> {
		static constexpr imint_t __sgn(float x) { return (x == 0) ? 0 : ((x < 0) ? -1 : 1); }
	};
	template<>
	struct _Sgn<double> {
		static constexpr imint_t __sgn(double x) { return (x == 0) ? 0 : ((x < 0) ? -1 : 1); }
	};
	template<>
	struct _Sgn<long double> {
		static constexpr imint_t __sgn(long double x) { return (x == 0) ? 0 : ((x < 0) ? -1 : 1); }
	};
	template <class T>
	inline constexpr imint_t sgn(const T& x) { return _Sgn<T>::__sgn(x); }
}


#endif