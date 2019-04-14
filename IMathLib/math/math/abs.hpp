#ifndef IMATH_MATH_MATH_ABS_HPP
#define IMATH_MATH_MATH_ABS_HPP


namespace iml {

	//絶対値
	template<class>
	struct Abs;
	//符号無し整数
#define IMATH_ABS(TYPE)\
	template<>\
	struct Abs<TYPE> {\
		static constexpr TYPE _abs_(TYPE x) { return x; }\
	};
	IMATH_ABS(bool);
	IMATH_ABS(unsigned char);
	IMATH_ABS(unsigned short);
	IMATH_ABS(unsigned int);
	IMATH_ABS(unsigned long);
	IMATH_ABS(char16_t);
	IMATH_ABS(char32_t);
	IMATH_ABS(unsigned long long);
#undef IMATH_ABS
	//符号あり数値
#define IMATH_ABS(TYPE)\
	template<>\
	struct Abs<TYPE> {\
		static constexpr TYPE _abs_(TYPE x) { return (x < 0) ? -x : x; }\
	};
	IMATH_ABS(char);
	IMATH_ABS(wchar_t);
	IMATH_ABS(short);
	IMATH_ABS(int);
	IMATH_ABS(long);
	IMATH_ABS(long long);
	IMATH_ABS(float);
	IMATH_ABS(double);
	IMATH_ABS(long double);
#undef IMATH_ABS

	template <class T>
	inline constexpr auto abs(const T& x) { return Abs<T>::_abs_(x); }
}


#endif