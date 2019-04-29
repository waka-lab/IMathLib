#ifndef IMATH_MATH_MATH_FREXP_HPP
#define IMATH_MATH_MATH_FREXP_HPP

#include "IMathLib/math/math/math_traits.hpp"
#include "IMathLib/math/math/numerical_correction.hpp"
#include "IMathLib/math/math/abs.hpp"
#include "IMathLib/math/math/log.hpp"
#include "IMathLib/math/math/sgn.hpp"
#include "IMathLib/math/math/pow.hpp"
#include "IMathLib/utility/tuple.hpp"

namespace iml {

	//指数部と仮数部に分解する(10進数)
	inline constexpr pair<float, int_t> frexp10(float x) {
		float temp = log10(abs(x));
		int_t n = floor_integer(temp) - (temp < 0);
		float a = pow(10.f, temp - n);

		return pair<float, int_t>(sgn(x)*a, n);
	}
	inline constexpr pair<double, int_t> frexp10(double x) {
		double temp = log10(abs(x));
		int_t n = floor_integer(temp) - (temp < 0);
		double a = pow(10.f, temp - n);

		return pair<double, int_t>(sgn(x)*a, n);
	}
	inline constexpr pair<long double, int_t> frexp10(long double x) {
		long double temp = log10(abs(x));
		int_t n = floor_integer(temp) - (temp < 0);
		long double a = pow(10.f, temp - n);

		return pair<long double, int_t>(sgn(x)*a, n);
	}
}


#endif