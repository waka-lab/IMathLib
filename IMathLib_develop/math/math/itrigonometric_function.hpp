#ifndef IMATH_MATH_MATH_ITRIGONOMETRIC_FUNCTION_HPP
#define IMATH_MATH_MATH_ITRIGONOMETRIC_FUNCTION_HPP

#include "IMathLib/math/math/math_traits.hpp"
#include "IMathLib/math/math/pi.hpp"
#include "IMathLib/math/math/sqrt.hpp"


//逆三角関数
namespace iml {

	//逆正接
	template <class T>
	struct Atan {
		using result_type = typename math_function_type<T>::type;

		template <class U>
		static constexpr result_type _atan_impl_(const U& x) {
			result_type x1 = x, x2 = x, x3 = 0;
			result_type temp = x * x;

			for (size_t i = 1; !error_evaluation(x2, x3); ++i) {
				x3 = x2;
				x1 *= -temp;
				x2 += x1 / (2 * i + 1);
			}
			return x2;
		}
		static constexpr result_type _atan_(const T& x) {
			constexpr auto check_val1 = sqrt<result_type>(2) - 1;		//判定用の値を予め計算
			constexpr auto check_val2 = sqrt<result_type>(2) + 1;		//*

			//0 <= x <= sqrt(2)-1 にする
			if (x < 0) return -_atan_(-x);
			if (x > check_val2) return pi<result_type> / 4 + _atan_impl_(result_type(x - 1) / (x + 1));
			else if (x > 1) return pi<result_type> / 2 - _atan_impl_(result_type(1) / x);
			else if (x > check_val1) return pi<result_type> / 4 - _atan_impl_(result_type(1 - x) / (1 + x));
			else return _atan_impl_(x);
		}
	};
	//符号無し整数の場合は負数の補正を除去して定義
#define IMATH_ATAN(TYPE) \
	template <>\
	struct Atan<TYPE> {\
	using result_type = typename math_function_type<TYPE>::type;\
		template <class U>\
		static constexpr result_type _atan_impl_(U x) {\
			result_type x1 = x, x2 = x, x3 = 0;\
			result_type temp = x * x;\
			for (size_t i = 1; !error_evaluation(x2, x3); ++i) {\
				x3 = x2;\
				x1 *= -temp;\
				x2 += x1 / (2 * i + 1);\
			}\
			return x2;\
		}\
		static constexpr result_type _atan_(TYPE x) {\
			constexpr auto check_val1 = sqrt<result_type>(2) - 1;\
			constexpr auto check_val2 = sqrt<result_type>(2) + 1;\
			if (x > check_val2) return pi<result_type> / 4 + _atan_impl_(result_type(x - 1) / (x + 1));\
			else if (x > 1) return pi<result_type> / 2 - _atan_impl_(result_type(1) / x);\
			else if (x > check_val1) return pi<result_type> / 4 - _atan_impl_(result_type(1 - x) / (1 + x));\
			else return _atan_impl_(x);\
		}\
	};
	IMATH_ATAN(unsigned char);
	IMATH_ATAN(unsigned short);
	IMATH_ATAN(unsigned int);
	IMATH_ATAN(unsigned long);
	IMATH_ATAN(char16_t);
	IMATH_ATAN(char32_t);
	IMATH_ATAN(unsigned long long);
#undef IMATH_ATAN

	template <class T>
	inline constexpr auto atan(const T& x) { return Atan<T>::_atan_(x); }


	//座標平面上での逆正接
	template <class T>
	struct Atan2 {
		using result_type = typename math_function_type<T>::type;

		static constexpr result_type _atan2_(const T& y, const T& x) {
			if (x == 0) {
				if (y == 0) return 0;
				else if (y < 0) return -pi<result_type> / 2;
				else return pi<result_type> / 2;
			}
			else if (y == 0) return (x < 0) ? -pi<result_type> : 0;
			else if (x > 0) return atan(result_type(y) / x);
			else return atan(result_type(y) / x) + ((y < 0) ? -pi<result_type> : pi<result_type>);
		}
	};
	//符号無し整数の場合は負数の補正を除去して定義
#define IMATH_ATAN2(TYPE) \
	template <>\
	struct Atan2<TYPE> {\
		using result_type = typename math_function_type<TYPE>::type;\
		static constexpr result_type _atan2_(const TYPE& y, const TYPE& x) {\
			if (x == 0) return (y == 0) ? 0 : (pi<result_type> / 2);\
			else if (y == 0) return 0;\
			else if (x > 0) return atan(result_type(y) / x);\
			else return atan(result_type(y) / x) + pi<result_type>;\
		}\
	};
	IMATH_ATAN2(unsigned char);
	IMATH_ATAN2(unsigned short);
	IMATH_ATAN2(unsigned int);
	IMATH_ATAN2(unsigned long);
	IMATH_ATAN2(char16_t);
	IMATH_ATAN2(char32_t);
	IMATH_ATAN2(unsigned long long);
#undef IMATH_ATAN2
	template <class T>
	inline constexpr auto atan2(const T& y, const T& x) { return Atan2<T>::_atan2_(y, x); }


	//逆正弦
	template <class T>
	struct Asin {
		static constexpr auto _asin_(const T& x) {
			return 2 * atan(x / (1 + sqrt(1 - x * x)));
		}
	};
	template <class T>
	inline constexpr auto asin(const T& x) { return Asin<T>::_asin_(x); }


	//逆余弦
	template <class T>
	struct Acos {
		static constexpr auto _acos_(const T& x) { return pi<T> / 2 - asin(x); }
	};
	template <class T>
	inline constexpr auto acos(const T& x) { return Acos<T>::_acos_(x); }
}


#endif