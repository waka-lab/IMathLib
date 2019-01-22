#ifndef _IMATH_MATH_ABS_HPP
#define _IMATH_MATH_ABS_HPP

#include "IMathLib/math/math/math_traits.hpp"
#include "IMathLib/utility/type_traits.hpp"

namespace iml {

	//絶対値
	template<class>
	struct _Abs;
	template<>
	struct _Abs<bool> {
		static constexpr bool __abs(bool x) { return x; }
	};
	template<>
	struct _Abs<char> {
		static constexpr char __abs(char x) { return (x < 0) ? -x : x; }
	};
	template<>
	struct _Abs<unsigned char> {
		static constexpr unsigned char __abs(unsigned char x) { return x; }
	};
	template<>
	struct _Abs<wchar_t> {
		static constexpr wchar_t __abs(wchar_t x) { return (x < 0) ? -x : x; }
	};
	template<>
	struct _Abs<short> {
		static constexpr short __abs(short x) { return (x < 0) ? -x : x; }
	};
	template<>
	struct _Abs<unsigned short> {
		static constexpr unsigned short __abs(unsigned short x) { return x; }
	};
	template<>
	struct _Abs<int> {
		static constexpr int __abs(int x) { return (x < 0) ? -x : x; }
	};
	template<>
	struct _Abs<unsigned int> {
		static constexpr unsigned int __abs(unsigned int x) { return x; }
	};
	template<>
	struct _Abs<long> {
		static constexpr long __abs(long x) { return (x < 0) ? -x : x; }
	};
	template<>
	struct _Abs<unsigned long> {
		static constexpr unsigned long __abs(unsigned long x) { return x; }
	};
	template<>
	struct _Abs<char16_t> {
		static constexpr char16_t __abs(char16_t x) { return x; }
	};
	template<>
	struct _Abs<char32_t> {
		static constexpr char32_t __abs(char32_t x) { return x; }
	};
	template<>
	struct _Abs<long long> {
		static constexpr long long __abs(long long x) { return (x < 0) ? -x : x; }
	};
	template<>
	struct _Abs<unsigned long long> {
		static constexpr unsigned long long __abs(unsigned long long x) { return x; }
	};
	template<>
	struct _Abs<float> {
		static constexpr float __abs(float x) { return (x < 0) ? -x : x; }
	};
	template<>
	struct _Abs<double> {
		static constexpr double __abs(double x) { return (x < 0) ? -x : x; }
	};
	template<>
	struct _Abs<long double> {
		static constexpr long double __abs(long double x) { return (x < 0) ? -x : x; }
	};
	template <class T>
	inline constexpr auto abs(const T& x) { return _Abs<T>::__abs(x); }
}


#endif