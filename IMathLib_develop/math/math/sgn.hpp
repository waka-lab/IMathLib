#ifndef _IMATH_MATH_SGN_HPP
#define _IMATH_MATH_SGN_HPP

#include "IMathLib/math/math/math_traits.hpp"

namespace iml {

	//符号関数
	template<class>
	struct Sgn;
	template<>
	struct Sgn<bool> {
		static constexpr int_t _sgn_(bool x) { return x ? 1 : -1; }
	};
	template<>
	struct Sgn<char> {
		static constexpr int_t _sgn_(char x) { return (x == 0) ? 0 : ((x < 0) ? -1 : 1); }
	};
	template<>
	struct Sgn<unsigned char> {
		static constexpr int_t _sgn_(unsigned char x) { return (x == 0) ? 0 : 1; }
	};
	template<>
	struct Sgn<wchar_t> {
		static constexpr int_t _sgn_(wchar_t x) { return (x == 0) ? 0 : 1; }
	};
	template<>
	struct Sgn<short> {
		static constexpr int_t _sgn_(short x) { return (x == 0) ? 0 : ((x < 0) ? -1 : 1); }
	};
	template<>
	struct Sgn<unsigned short> {
		static constexpr int_t _sgn_(unsigned short x) { return (x == 0) ? 0 : 1; }
	};
	template<>
	struct Sgn<int> {
		static constexpr int_t _sgn_(int x) { return (x == 0) ? 0 : ((x < 0) ? -1 : 1); }
	};
	template<>
	struct Sgn<unsigned int> {
		static constexpr int_t _sgn_(unsigned int x) { return (x == 0) ? 0 : 1; }
	};
	template<>
	struct Sgn<long> {
		static constexpr int_t _sgn_(long x) { return (x == 0) ? 0 : ((x < 0) ? -1 : 1); }
	};
	template<>
	struct Sgn<unsigned long> {
		static constexpr int_t _sgn_(unsigned long x) { return (x == 0) ? 0 : 1; }
	};
	template<>
	struct Sgn<char16_t> {
		static constexpr int_t _sgn_(char16_t x) { return (x == 0) ? 0 : 1; }
	};
	template<>
	struct Sgn<char32_t> {
		static constexpr int_t _sgn_(char32_t x) { return (x == 0) ? 0 : 1; }
	};
	template<>
	struct Sgn<long long> {
		static constexpr int_t _sgn_(long long x) { return (x == 0) ? 0 : ((x < 0) ? -1 : 1); }
	};
	template<>
	struct Sgn<unsigned long long> {
		static constexpr int_t _sgn_(unsigned long long x) { return (x == 0) ? 0 : 1; }
	};
	template<>
	struct Sgn<float> {
		static constexpr int_t _sgn_(float x) { return (x == 0) ? 0 : ((x < 0) ? -1 : 1); }
	};
	template<>
	struct Sgn<double> {
		static constexpr int_t _sgn_(double x) { return (x == 0) ? 0 : ((x < 0) ? -1 : 1); }
	};
	template<>
	struct Sgn<long double> {
		static constexpr int_t _sgn_(long double x) { return (x == 0) ? 0 : ((x < 0) ? -1 : 1); }
	};
	template <class T>
	inline constexpr int_t sgn(const T& x) { return Sgn<T>::_sgn_(x); }
}


#endif