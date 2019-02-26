#ifndef IMATH_MATH_MATH_ABS_HPP
#define IMATH_MATH_MATH_ABS_HPP

#include "IMathLib/math/math/math_traits.hpp"
#include "IMathLib/utility/type_traits.hpp"

namespace iml {

	//絶対値
	template<class>
	struct Abs;
	template<>
	struct Abs<bool> {
		static constexpr bool _abs_(bool x) { return x; }
	};
	template<>
	struct Abs<char> {
		static constexpr char _abs_(char x) { return (x < 0) ? -x : x; }
	};
	template<>
	struct Abs<unsigned char> {
		static constexpr unsigned char _abs_(unsigned char x) { return x; }
	};
	template<>
	struct Abs<wchar_t> {
		static constexpr wchar_t _abs_(wchar_t x) { return (x < 0) ? -x : x; }
	};
	template<>
	struct Abs<short> {
		static constexpr short _abs_(short x) { return (x < 0) ? -x : x; }
	};
	template<>
	struct Abs<unsigned short> {
		static constexpr unsigned short _abs_(unsigned short x) { return x; }
	};
	template<>
	struct Abs<int> {
		static constexpr int _abs_(int x) { return (x < 0) ? -x : x; }
	};
	template<>
	struct Abs<unsigned int> {
		static constexpr unsigned int _abs_(unsigned int x) { return x; }
	};
	template<>
	struct Abs<long> {
		static constexpr long _abs_(long x) { return (x < 0) ? -x : x; }
	};
	template<>
	struct Abs<unsigned long> {
		static constexpr unsigned long _abs_(unsigned long x) { return x; }
	};
	template<>
	struct Abs<char16_t> {
		static constexpr char16_t _abs_(char16_t x) { return x; }
	};
	template<>
	struct Abs<char32_t> {
		static constexpr char32_t _abs_(char32_t x) { return x; }
	};
	template<>
	struct Abs<long long> {
		static constexpr long long _abs_(long long x) { return (x < 0) ? -x : x; }
	};
	template<>
	struct Abs<unsigned long long> {
		static constexpr unsigned long long _abs_(unsigned long long x) { return x; }
	};
	template<>
	struct Abs<float> {
		static constexpr float _abs_(float x) { return (x < 0) ? -x : x; }
	};
	template<>
	struct Abs<double> {
		static constexpr double _abs_(double x) { return (x < 0) ? -x : x; }
	};
	template<>
	struct Abs<long double> {
		static constexpr long double _abs_(long double x) { return (x < 0) ? -x : x; }
	};
	template <class T>
	inline constexpr auto abs(const T& x) { return Abs<T>::_abs_(x); }
}


#endif