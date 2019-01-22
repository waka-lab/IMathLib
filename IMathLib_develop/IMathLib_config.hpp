#ifndef _IMATHLIB_CONFIG_HPP
#define _IMATHLIB_CONFIG_HPP

//各種設定によって実装にのみ依存するようにする


#include <intrin.h>
#include <cstdint>

namespace iml {

	//内部で用いるべきバイト長ごとの型定義
	using imint8_t = std::int8_t;
	using imuint8_t = std::uint8_t;
	using imint16_t = std::int16_t;
	using imuint16_t = std::uint16_t;
	using imint32_t = std::int32_t;
	using imuint32_t = std::uint32_t;
	using imint64_t = std::int64_t;
	using imuint64_t = std::uint64_t;


	//ループカウンタ等で用いる整数型とそのリテラルの定義
#if defined _IMATH_INT_32BIT_
	using imsize_t = imuint32_t;
	inline constexpr imsize_t operator"" _imu(unsigned long long x) { return static_cast<imsize_t>(x); }
	using imint_t = imint32_t;
	inline constexpr imint_t operator"" _imi(unsigned long long x) { return static_cast<imint_t>(x); }
#elif defined _IMATH_INT_64BIT_
	using imsize_t = imuint64_t;
	inline constexpr imsize_t operator"" _imu(unsigned long long x) { return static_cast<imsize_t>(x); }
	using imint_t = imint64_t;
	inline constexpr imint_t operator"" _imi(unsigned long long x) { return static_cast<imint_t>(x); }
#else
#define _IMATH_INT_32BIT_
	using imsize_t = imuint32_t;
	inline constexpr imsize_t operator"" _imu(unsigned long long x) { return static_cast<imsize_t>(x); }
	using imint_t = imint32_t;
	inline constexpr imint_t operator"" _imi(unsigned long long x) { return static_cast<imint_t>(x); }
#endif

}

//フォーマット指定子の定義
#ifdef _IMATH_INT_32BIT_
#define _IMF_D_		"%d"
#define _IMF_U_		"%u"
#elif defined _IMATH_INT_64BIT_
#define _IMF_D_		"%ld"
#define _IMF_U_		"%lu"
#endif


#endif