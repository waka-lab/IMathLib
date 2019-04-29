#ifndef IMATH_MATH_MATH_NUMERICAL_CORRECTION_HPP
#define IMATH_MATH_MATH_NUMERICAL_CORRECTION_HPP

#include "IMathLib/math/math/math_traits.hpp"

//数値補正系関数

namespace iml {

	//床関数(切り捨て)
	template<class>
	struct Floor;
	//整数のとき
#define IMATH_FLOOR(TYPE)\
	template<>\
	struct Floor<TYPE> {\
		static constexpr bool _floor_(TYPE x) { return x; }\
	};
	IMATH_FLOOR(bool);
	IMATH_FLOOR(char);
	IMATH_FLOOR(unsigned char);
	IMATH_FLOOR(wchar_t);
	IMATH_FLOOR(short);
	IMATH_FLOOR(unsigned short);
	IMATH_FLOOR(int);
	IMATH_FLOOR(unsigned int);
	IMATH_FLOOR(long);
	IMATH_FLOOR(unsigned long);
	IMATH_FLOOR(char16_t);
	IMATH_FLOOR(char32_t);
	IMATH_FLOOR(long long);
	IMATH_FLOOR(unsigned long long);
#undef IMATH_FLOOR
	//浮動小数点型の場合
	template<>
	struct Floor<float> {
		static constexpr float _floor_(float x) {
			float result = 0;

			//xが正のとき
			while (x >= 1) {
				//size_tの上限を超えている場合
				if (x > (numeric_traits<size_t>::max)()) {
					x -= (numeric_traits<size_t>::max)();
					result += (numeric_traits<size_t>::max)();
				}
				else {
					result += static_cast<size_t>(x);
					break;
				}
			}
			//xが負のとき
			x = -x;				//符号反転
			while (x >= 1) {
				//size_tの上限を超えている場合(+1は必ずelseを通して終了させるため)
				if (x > (numeric_traits<size_t>::max)() + 1.0f) {
					x -= (numeric_traits<size_t>::max)();
					result -= (numeric_traits<size_t>::max)();
				}
				else {
					result -= static_cast<size_t>(x) + 1.0f;
					break;
				}
			}

			return result;
		}
	};
	template<>
	struct Floor<double> {
		static constexpr double _floor_(double x) {
			double result = 0;

			//xが正のとき
			while (x >= 1) {
				//size_tの上限を超えている場合
				if (x > (numeric_traits<size_t>::max)()) {
					x -= (numeric_traits<size_t>::max)();
					result += (numeric_traits<size_t>::max)();
				}
				else {
					result += static_cast<size_t>(x);
					break;
				}
			}
			//xが負のとき
			x = -x;				//符号反転
			while (x >= 1) {
				//size_tの上限を超えている場合(+1は必ずelseを通して終了させるため)
				if (x > (numeric_traits<size_t>::max)() + 1.0) {
					x -= (numeric_traits<size_t>::max)();
					result -= (numeric_traits<size_t>::max)();
				}
				else {
					result -= static_cast<size_t>(x) + 1.0;
					break;
				}
			}

			return result;
		}
	};
	template<>
	struct Floor<long double> {
		static constexpr long double _floor_(long double x) {
			long double result = 0;

			//xが正のとき
			while (x >= 1) {
				//size_tの上限を超えている場合
				if (x > (numeric_traits<size_t>::max)()) {
					x -= (numeric_traits<size_t>::max)();
					result += (numeric_traits<size_t>::max)();
				}
				else {
					result += static_cast<size_t>(x);
					break;
				}
			}
			//xが負のとき
			x = -x;				//符号反転
			while (x >= 1) {
				//size_tの上限を超えている場合(+1は必ずelseを通して終了させるため)
				if (x > (numeric_traits<size_t>::max)() + 1.0) {
					x -= (numeric_traits<size_t>::max)();
					result -= (numeric_traits<size_t>::max)();
				}
				else {
					result -= static_cast<size_t>(x) + 1.0;
					break;
				}
			}

			return result;
		}
	};
	template <class T>
	inline constexpr auto floor(const T& x) { return Floor<T>::_floor_(x); }

	//整数値として結果を得る
	template <class T>
	inline constexpr int_t floor_integer(const T& x) { return static_cast<int_t>(floor(x) + 0.1); }


	//天井関数(切り上げ)
	template<class>
	struct Ceil;
	//整数のとき
#define IMATH_CEIL(TYPE)\
	template<>\
	struct Ceil<TYPE> {\
		static constexpr bool _ceil_(TYPE x) { return x; }\
	};
	IMATH_CEIL(bool);
	IMATH_CEIL(char);
	IMATH_CEIL(unsigned char);
	IMATH_CEIL(wchar_t);
	IMATH_CEIL(short);
	IMATH_CEIL(unsigned short);
	IMATH_CEIL(int);
	IMATH_CEIL(unsigned int);
	IMATH_CEIL(long);
	IMATH_CEIL(unsigned long);
	IMATH_CEIL(char16_t);
	IMATH_CEIL(char32_t);
	IMATH_CEIL(long long);
	IMATH_CEIL(unsigned long long);
#undef IMATH_CEIL
	//浮動小数点型の場合
	template<>
	struct Ceil<float> {
		static constexpr float _ceil_(float x) {
			float result = 0;

			//xが正のとき
			while (x >= 1) {
				//size_tの上限を超えている場合(+1は必ずelseを通して終了させるため)
				if (x > (numeric_traits<size_t>::max)() + 1.0f) {
					x -= (numeric_traits<size_t>::max)();
					result += (numeric_traits<size_t>::max)();
				}
				else {
					result += static_cast<size_t>(x) + 1.0f;
					break;
				}
			}
			//xが負のとき
			x = -x;				//符号反転
			while (x >= 1) {
				//size_tの上限を超えている場合
				if (x > (numeric_traits<size_t>::max)()) {
					x -= (numeric_traits<size_t>::max)();
					result -= (numeric_traits<size_t>::max)();
				}
				else {
					result -= static_cast<size_t>(x);
					break;
				}
			}

			return result;
		}
	};
	template<>
	struct Ceil<double> {
		static constexpr double _ceil_(double x) {
			double result = 0;

			//xが正のとき
			while (x >= 1) {
				//size_tの上限を超えている場合(+1は必ずelseを通して終了させるため)
				if (x > (numeric_traits<size_t>::max)() + 1.0) {
					x -= (numeric_traits<size_t>::max)();
					result += (numeric_traits<size_t>::max)();
				}
				else {
					result += static_cast<size_t>(x) + 1.0;
					break;
				}
			}
			//xが負のとき
			x = -x;				//符号反転
			while (x >= 1) {
				//size_tの上限を超えている場合
				if (x > (numeric_traits<size_t>::max)()) {
					x -= (numeric_traits<size_t>::max)();
					result -= (numeric_traits<size_t>::max)();
				}
				else {
					result -= static_cast<size_t>(x);
					break;
				}
			}

			return result;
		}
	};
	template<>
	struct Ceil<long double> {
		static constexpr long double _ceil_(long double x) {
			long double result = 0;

			//xが正のとき
			while (x >= 1) {
				//size_tの上限を超えている場合(+1は必ずelseを通して終了させるため)
				if (x > (numeric_traits<size_t>::max)() + 1.0) {
					x -= (numeric_traits<size_t>::max)();
					result += (numeric_traits<size_t>::max)();
				}
				else {
					result += static_cast<size_t>(x) + 1.0;
					break;
				}
			}
			//xが負のとき
			x = -x;				//符号反転
			while (x >= 1) {
				//size_tの上限を超えている場合
				if (x > (numeric_traits<size_t>::max)()) {
					x -= (numeric_traits<size_t>::max)();
					result -= (numeric_traits<size_t>::max)();
				}
				else {
					result -= static_cast<size_t>(x);
					break;
				}
			}

			return result;
		}
	};
	template <class T>
	inline constexpr auto ceil(const T& x) { return Ceil<T>::_ceil_(x); }

	//整数値として結果を得る
	template <class T>
	inline constexpr int_t ceil_integer(const T& x) { return static_cast<int_t>(ceil(x) + 0.1); }


	//四捨五入
	template<class>
	struct Round;
	//整数のとき
#define IMATH_ROUND(TYPE)\
	template<>\
	struct Round<TYPE> {\
		static constexpr bool _round_(TYPE x) { return x; }\
	};
	IMATH_ROUND(bool);
	IMATH_ROUND(char);
	IMATH_ROUND(unsigned char);
	IMATH_ROUND(wchar_t);
	IMATH_ROUND(short);
	IMATH_ROUND(unsigned short);
	IMATH_ROUND(int);
	IMATH_ROUND(unsigned int);
	IMATH_ROUND(long);
	IMATH_ROUND(unsigned long);
	IMATH_ROUND(char16_t);
	IMATH_ROUND(char32_t);
	IMATH_ROUND(long long);
	IMATH_ROUND(unsigned long long);
#undef IMATH_ROUND
	//浮動小数点型の場合
	template<>
	struct Round<float> {
		static constexpr float _round_(float x) { return (x < 0) ? ceil(x - 0.5f) : floor(x + 0.5f); }
	};
	template<>
	struct Round<double> {
		static constexpr double _round_(double x) { return (x < 0) ? ceil(x - 0.5) : floor(x + 0.5); }
	};
	template<>
	struct Round<long double> {
		static constexpr long double _round_(long double x) { return (x < 0) ? ceil(x - 0.5) : floor(x + 0.5); }
	};
	template <class T>
	inline constexpr auto round(const T& x) { return Round<T>::_round_(x); }

	//整数値として結果を得る
	template <class T>
	inline constexpr int_t round_integer(const T& x) { return static_cast<int_t>(round(x) + 0.1); }


	//0方向への丸め
	template <class T>
	struct Trunc {
		static constexpr T _trunc_(const T& x) { return (x < 0) ? ceil(x) : floor(x); }
	};
	template <class T>
	inline constexpr auto trunc(const T& x) { return Trunc<T>::_trunc_(x); }

	//整数値として結果を得る
	template <class T>
	inline constexpr int_t trunc_integer(const T& x) { return static_cast<int_t>(trunc(x) + 0.1); }


	//剰余
	template <class T>
	struct Mod {

		static constexpr T _mod_(const T& x, const T& y) { return x - trunc(x / y) * y; }
	};
	template <class T>
	inline constexpr auto mod(const T& x, const T& y) { return Mod<T>::_mod_(x, y); }

}


#endif