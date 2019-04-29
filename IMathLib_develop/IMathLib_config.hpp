#ifndef IMATH_IMATHLIB_CONFIG_HPP
#define IMATH_IMATHLIB_CONFIG_HPP

//各種設定によって実装にのみ依存するようにする


#include <intrin.h>
#include <cstdint>

namespace iml {

	//ライブラリ内部で用いるべきビット長ごとの型定義
	using int8_t = std::int8_t;
	using uint8_t = std::uint8_t;
	using int16_t = std::int16_t;
	using uint16_t = std::uint16_t;
	using int32_t = std::int32_t;
	using uint32_t = std::uint32_t;
	using int64_t = std::int64_t;
	using uint64_t = std::uint64_t;


	//ループカウンタ等で用いる整数型とそのリテラルの定義
#if defined IMATH_INT_32BIT
	using size_t = uint32_t;
	inline constexpr size_t operator"" _imu(unsigned long long x) { return x; }
	using int_t = int32_t;
	inline constexpr int_t operator"" _imi(unsigned long long x) { return x; }
#elif defined IMATH_INT_64BIT
	using size_t = uint64_t;
	inline constexpr size_t operator"" _imu(unsigned long long x) { return x; }
	using int_t = int64_t;
	inline constexpr int_t operator"" _imi(unsigned long long x) { return x; }
#else
#define IMATH_INT_32BIT
	using size_t = uint32_t;
	inline constexpr size_t operator"" _imu(unsigned long long x) { return size_t(x); }
	using int_t = int32_t;
	inline constexpr int_t operator"" _imi(unsigned long long x) { return int_t(x); }
#endif

}


#endif