#ifndef IMATH_IMATHLIB_CONFIG_HPP
#define IMATH_IMATHLIB_CONFIG_HPP

//各種設定によって実装にのみ依存するようにする


#include <intrin.h>
#include <cstdint>

namespace iml {

	//内部で用いるべきbit長ごとの型定義
	using imint8_t = std::int8_t;
	using imuint8_t = std::uint8_t;
	using imint16_t = std::int16_t;
	using imuint16_t = std::uint16_t;
	using imint32_t = std::int32_t;
	using imuint32_t = std::uint32_t;
	using imint64_t = std::int64_t;
	using imuint64_t = std::uint64_t;


	//ループカウンタ等で用いる整数型とそのリテラルの定義
#if defined IMATH_INT_32BIT
	using imsize_t = imuint32_t;
	inline constexpr imsize_t operator"" _imu(unsigned long long x) { return static_cast<imsize_t>(x); }
	using imint_t = imint32_t;
	inline constexpr imint_t operator"" _imi(unsigned long long x) { return static_cast<imint_t>(x); }
#elif defined IMATH_INT_64BIT
	using imsize_t = imuint64_t;
	inline constexpr imsize_t operator"" _imu(unsigned long long x) { return static_cast<imsize_t>(x); }
	using imint_t = imint64_t;
	inline constexpr imint_t operator"" _imi(unsigned long long x) { return static_cast<imint_t>(x); }
#else
#define IMATH_INT_32BIT
	using imsize_t = imuint32_t;
	inline constexpr imsize_t operator"" _imu(unsigned long long x) { return static_cast<imsize_t>(x); }
	using imint_t = imint32_t;
	inline constexpr imint_t operator"" _imi(unsigned long long x) { return static_cast<imint_t>(x); }
#endif

}


#endif