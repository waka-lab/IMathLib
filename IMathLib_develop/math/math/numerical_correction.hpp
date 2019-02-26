#ifndef _IMATH_MATH_NUMERICAL_CORRECTION_HPP
#define _IMATH_MATH_NUMERICAL_CORRECTION_HPP

#include "IMathLib/math/math/math_traits.hpp"

//数値補正系関数

namespace iml {

	//床関数(切り捨て)
	template<class>
	struct _Floor;
	template<>
	struct _Floor<bool> {
		static constexpr bool __floor(bool x) { return x; }
	};
	template<>
	struct _Floor<char> {
		static constexpr char __floor(char x) { return x; }
	};
	template<>
	struct _Floor<unsigned char> {
		static constexpr unsigned char __floor(unsigned char x) { return x; }
	};
	template<>
	struct _Floor<wchar_t> {
		static constexpr wchar_t __floor(wchar_t x) { return x; }
	};
	template<>
	struct _Floor<short> {
		static constexpr short __floor(short x) { return x; }
	};
	template<>
	struct _Floor<unsigned short> {
		static constexpr unsigned short __floor(unsigned short x) { return x; }
	};
	template<>
	struct _Floor<int> {
		static constexpr int __floor(int x) { return x; }
	};
	template<>
	struct _Floor<unsigned int> {
		static constexpr unsigned int __floor(unsigned int x) { return x; }
	};
	template<>
	struct _Floor<long> {
		static constexpr long __floor(long x) { return x; }
	};
	template<>
	struct _Floor<unsigned long> {
		static constexpr unsigned long __floor(unsigned long x) { return x; }
	};
	template<>
	struct _Floor<char16_t> {
		static constexpr char16_t __floor(char16_t x) { return x; }
	};
	template<>
	struct _Floor<char32_t> {
		static constexpr char32_t __floor(char32_t x) { return x; }
	};
	template<>
	struct _Floor<long long> {
		static constexpr long long __floor(long long x) { return x; }
	};
	template<>
	struct _Floor<unsigned long long> {
		static constexpr unsigned long long __floor(unsigned long long x) { return x; }
	};
	template<>
	struct _Floor<float> {
		static constexpr float __floor(float x) {
			float result = 0;

			//xが正のとき
			while (x >= 1) {
				//imsize_tの上限を超えている場合
				if (x > (limits<imsize_t>::max)()) {
					x -= (limits<imsize_t>::max)();
					result += (limits<imsize_t>::max)();
				}
				else {
					result += static_cast<imsize_t>(x);
					break;
				}
			}
			//xが負のとき
			x = -x;				//符号反転
			while (x >= 1) {
				//imsize_tの上限を超えている場合(+1は必ずelseを通して終了させるため)
				if (x > (limits<imsize_t>::max)() + 1.0f) {
					x -= (limits<imsize_t>::max)();
					result -= (limits<imsize_t>::max)();
				}
				else {
					result -= static_cast<imsize_t>(x) + 1.0f;
					break;
				}
			}

			return result;
		}
	};
	template<>
	struct _Floor<double> {
		static constexpr double __floor(double x) {
			double result = 0;

			//xが正のとき
			while (x >= 1) {
				//imsize_tの上限を超えている場合
				if (x > (limits<imsize_t>::max)()) {
					x -= (limits<imsize_t>::max)();
					result += (limits<imsize_t>::max)();
				}
				else {
					result += static_cast<imsize_t>(x);
					break;
				}
			}
			//xが負のとき
			x = -x;				//符号反転
			while (x >= 1) {
				//imsize_tの上限を超えている場合(+1は必ずelseを通して終了させるため)
				if (x > (limits<imsize_t>::max)() + 1.0) {
					x -= (limits<imsize_t>::max)();
					result -= (limits<imsize_t>::max)();
				}
				else {
					result -= static_cast<imsize_t>(x) + 1.0;
					break;
				}
			}

			return result;
		}
	};
	template<>
	struct _Floor<long double> {
		static constexpr long double __floor(long double x) {
			long double result = 0;

			//xが正のとき
			while (x >= 1) {
				//imsize_tの上限を超えている場合
				if (x > (limits<imsize_t>::max)()) {
					x -= (limits<imsize_t>::max)();
					result += (limits<imsize_t>::max)();
				}
				else {
					result += static_cast<imsize_t>(x);
					break;
				}
			}
			//xが負のとき
			x = -x;				//符号反転
			while (x >= 1) {
				//imsize_tの上限を超えている場合(+1は必ずelseを通して終了させるため)
				if (x > (limits<imsize_t>::max)() + 1.0) {
					x -= (limits<imsize_t>::max)();
					result -= (limits<imsize_t>::max)();
				}
				else {
					result -= static_cast<imsize_t>(x) + 1.0;
					break;
				}
			}

			return result;
		}
	};
	template <class T>
	inline constexpr auto floor(const T& x) { return _Floor<T>::__floor(x); }

	//整数値として結果を得る
	template <class T>
	inline constexpr imint_t floor_integer(const T& x) { return static_cast<imint_t>(floor(x) + 0.1); }


	//天井関数(切り上げ)
	template<class>
	struct _Ceil;
	template<>
	struct _Ceil<bool> {
		static constexpr bool __ceil(bool x) { return x; }
	};
	template<>
	struct _Ceil<char> {
		static constexpr char __ceil(char x) { return x; }
	};
	template<>
	struct _Ceil<unsigned char> {
		static constexpr unsigned char __ceil(unsigned char x) { return x; }
	};
	template<>
	struct _Ceil<wchar_t> {
		static constexpr wchar_t __ceil(wchar_t x) { return x; }
	};
	template<>
	struct _Ceil<short> {
		static constexpr short __ceil(short x) { return x; }
	};
	template<>
	struct _Ceil<unsigned short> {
		static constexpr unsigned short __ceil(unsigned short x) { return x; }
	};
	template<>
	struct _Ceil<int> {
		static constexpr int __ceil(int x) { return x; }
	};
	template<>
	struct _Ceil<unsigned int> {
		static constexpr unsigned int __ceil(unsigned int x) { return x; }
	};
	template<>
	struct _Ceil<long> {
		static constexpr long __ceil(long x) { return x; }
	};
	template<>
	struct _Ceil<unsigned long> {
		static constexpr unsigned long __ceil(unsigned long x) { return x; }
	};
	template<>
	struct _Ceil<char16_t> {
		static constexpr char16_t __ceil(char16_t x) { return x; }
	};
	template<>
	struct _Ceil<char32_t> {
		static constexpr char32_t __ceil(char32_t x) { return x; }
	};
	template<>
	struct _Ceil<long long> {
		static constexpr long long __ceil(long long x) { return x; }
	};
	template<>
	struct _Ceil<unsigned long long> {
		static constexpr unsigned long long __ceil(unsigned long long x) { return x; }
	};
	template<>
	struct _Ceil<float> {
		static constexpr float __ceil(float x) {
			float result = 0;

			//xが正のとき
			while (x >= 1) {
				//imsize_tの上限を超えている場合(+1は必ずelseを通して終了させるため)
				if (x > (limits<imsize_t>::max)() + 1.0f) {
					x -= (limits<imsize_t>::max)();
					result += (limits<imsize_t>::max)();
				}
				else {
					result += static_cast<imsize_t>(x) + 1.0f;
					break;
				}
			}
			//xが負のとき
			x = -x;				//符号反転
			while (x >= 1) {
				//imsize_tの上限を超えている場合
				if (x > (limits<imsize_t>::max)()) {
					x -= (limits<imsize_t>::max)();
					result -= (limits<imsize_t>::max)();
				}
				else {
					result -= static_cast<imsize_t>(x);
					break;
				}
			}

			return result;
		}
	};
	template<>
	struct _Ceil<double> {
		static constexpr double __ceil(double x) {
			double result = 0;

			//xが正のとき
			while (x >= 1) {
				//imsize_tの上限を超えている場合(+1は必ずelseを通して終了させるため)
				if (x > (limits<imsize_t>::max)() + 1.0) {
					x -= (limits<imsize_t>::max)();
					result += (limits<imsize_t>::max)();
				}
				else {
					result += static_cast<imsize_t>(x) + 1.0;
					break;
				}
			}
			//xが負のとき
			x = -x;				//符号反転
			while (x >= 1) {
				//imsize_tの上限を超えている場合
				if (x > (limits<imsize_t>::max)()) {
					x -= (limits<imsize_t>::max)();
					result -= (limits<imsize_t>::max)();
				}
				else {
					result -= static_cast<imsize_t>(x);
					break;
				}
			}

			return result;
		}
	};
	template<>
	struct _Ceil<long double> {
		static constexpr long double __ceil(long double x) {
			long double result = 0;

			//xが正のとき
			while (x >= 1) {
				//imsize_tの上限を超えている場合(+1は必ずelseを通して終了させるため)
				if (x > (limits<imsize_t>::max)() + 1.0) {
					x -= (limits<imsize_t>::max)();
					result += (limits<imsize_t>::max)();
				}
				else {
					result += static_cast<imsize_t>(x) + 1.0;
					break;
				}
			}
			//xが負のとき
			x = -x;				//符号反転
			while (x >= 1) {
				//imsize_tの上限を超えている場合
				if (x > (limits<imsize_t>::max)()) {
					x -= (limits<imsize_t>::max)();
					result -= (limits<imsize_t>::max)();
				}
				else {
					result -= static_cast<imsize_t>(x);
					break;
				}
			}

			return result;
		}
	};
	template <class T>
	inline constexpr auto ceil(const T& x) { return _Ceil<T>::__ceil(x); }

	//整数値として結果を得る
	template <class T>
	inline constexpr imint_t ceil_integer(const T& x) { return static_cast<imint_t>(ceil(x) + 0.1); }


	//四捨五入
	template<class>
	struct _Round;
	template<>
	struct _Round<bool> {
		static constexpr bool __round(bool x) { return x; }
	};
	template<>
	struct _Round<char> {
		static constexpr char __round(char x) { return x; }
	};
	template<>
	struct _Round<unsigned char> {
		static constexpr unsigned char __round(unsigned char x) { return x; }
	};
	template<>
	struct _Round<wchar_t> {
		static constexpr wchar_t __round(wchar_t x) { return x; }
	};
	template<>
	struct _Round<short> {
		static constexpr short __round(short x) { return x; }
	};
	template<>
	struct _Round<unsigned short> {
		static constexpr unsigned short __round(unsigned short x) { return x; }
	};
	template<>
	struct _Round<int> {
		static constexpr int __round(int x) { return x; }
	};
	template<>
	struct _Round<unsigned int> {
		static constexpr unsigned int __round(unsigned int x) { return x; }
	};
	template<>
	struct _Round<long> {
		static constexpr long __round(long x) { return x; }
	};
	template<>
	struct _Round<unsigned long> {
		static constexpr unsigned long __round(unsigned long x) { return x; }
	};
	template<>
	struct _Round<char16_t> {
		static constexpr char16_t __round(char16_t x) { return x; }
	};
	template<>
	struct _Round<char32_t> {
		static constexpr char32_t __round(char32_t x) { return x; }
	};
	template<>
	struct _Round<long long> {
		static constexpr long long __round(long long x) { return x; }
	};
	template<>
	struct _Round<unsigned long long> {
		static constexpr unsigned long long __round(unsigned long long x) { return x; }
	};
	template<>
	struct _Round<float> {
		static constexpr float __round(float x) { return (x < 0) ? ceil(x - 0.5f) : floor(x + 0.5f); }
	};
	template<>
	struct _Round<double> {
		static constexpr double __round(double x) { return (x < 0) ? ceil(x - 0.5) : floor(x + 0.5); }
	};
	template<>
	struct _Round<long double> {
		static constexpr long double __round(long double x) { return (x < 0) ? ceil(x - 0.5) : floor(x + 0.5); }
	};
	template <class T>
	inline constexpr auto round(const T& x) { return _Round<T>::__round(x); }

	//整数値として結果を得る
	template <class T>
	inline constexpr imint_t round_integer(const T& x) { return static_cast<imint_t>(round(x) + 0.1); }


	//0方向への丸め
	template <class T>
	struct _Trunc {
		static constexpr T __trunc(const T& x) { return (x < 0) ? ceil(x) : floor(x); }
	};
	template <class T>
	inline constexpr auto trunc(const T& x) { return _Trunc<T>::__trunc(x); }

	//整数値として結果を得る
	template <class T>
	inline constexpr imint_t trunc_integer(const T& x) { return static_cast<imint_t>(trunc(x) + 0.1); }


	//剰余
	template <class T>
	struct Mod {

		static constexpr T _mod_(const T& x, const T& y) { return x - trunc(x / y) * y; }
	};
	template <class T>
	inline constexpr auto mod(const T& x, const T& y) { return Mod<T>::_mod_(x, y); }

}


#endif