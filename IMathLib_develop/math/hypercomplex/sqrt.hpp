#ifndef IMATH_MATH_HYPERCOMPLEX_SQRT_HPP
#define IMATH_MATH_HYPERCOMPLEX_SQRT_HPP

#include "IMathLib/math/math.hpp"
#include "IMathLib/math/hypercomplex/complex.hpp"
#include "IMathLib/math/hypercomplex/dual_numbers.hpp"

#include "IMathLib/math/hypercomplex/abs.hpp"
#include "IMathLib/math/hypercomplex/arg.hpp"

namespace iml {

	template <class T>
	struct Sqrt<complex<T>> {
		//正の平方根
		static constexpr auto _sqrt_(const complex<T>& c) {
			auto temp = arg(c) / 2;
			auto temp2 = sqrt(abs(c));
			return make_complex(temp2 * cos(temp), temp2 * sin(temp));
		}
	};
	template <class T>
	struct Sqrt<dual_numbers<T>> {
		static constexpr auto _sqrt_(const dual_numbers<T>& c) {
			auto temp = sqrt(c[0]);
			return make_dual_numbers(temp, c[1] / (2 * temp));
		}
	};


	//複素数で計算することを前提とした平方根(符号無しの数に対しては定義しない)
	template <class>
	struct Csqrt;
	//符号あり数値
#define IMATH_CSQRT(TYPE)\
	template<>\
	struct Csqrt<TYPE> {\
		static constexpr auto _csqrt_(TYPE x) {\
			if (x < 0) {\
				auto temp = sqrt(-x);\
				return make_complex(decltype(temp)(0), temp);\
			}\
			else {\
				return make_complex(sqrt(x), 0); \
			}\
		}\
	};
	IMATH_CSQRT(char);
	IMATH_CSQRT(wchar_t);
	IMATH_CSQRT(short);
	IMATH_CSQRT(int);
	IMATH_CSQRT(long);
	IMATH_CSQRT(long long);
	IMATH_CSQRT(float);
	IMATH_CSQRT(double);
	IMATH_CSQRT(long double);
#undef IMATH_CSQRT
	template <class T>
	struct Csqrt<complex<T>> {
		//正の平方根
		static constexpr auto _csqrt_(const complex<T>& c) {
			auto temp = arg(c) / 2;
			auto temp2 = sqrt(abs(c));
			return make_complex(temp2 * cos(temp), temp2 * sin(temp));
		}
	};
	template <class T>
	inline constexpr auto csqrt(const T& x) { return Csqrt<T>::_csqrt_(x); }
}

#endif