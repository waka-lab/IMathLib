#ifndef IMATH_MATH_HYPERCOMPLEX_ITRIGONOMETRIC_FUNCTION_HPP
#define IMATH_MATH_HYPERCOMPLEX_ITRIGONOMETRIC_FUNCTION_HPP

#include "IMathLib/math/math.hpp"
#include "IMathLib/math/hypercomplex/complex.hpp"
#include "IMathLib/math/hypercomplex/dual_numbers.hpp"

#include "IMathLib/math/hypercomplex/log.hpp"
#include "IMathLib/math/hypercomplex/sqrt.hpp"


//逆三角関数
namespace iml {

	//逆正弦
	template <class T>
	struct Asin<complex<T>> {
		static constexpr auto _asin_(const complex<T>& x) {
			//-i * log(i * x + sqrt(1 - x * x))
			auto temp = make_complex(-x[1], x[0]);
			auto temp2 = log(temp + sqrt(1 - x * x));
			return make_complex(temp2[1], -temp[0]);
		}
	};
	template <class T>
	struct Asin<dual_numbers<T>> {
		static constexpr auto _asin_(const dual_numbers<T>& x) {
			return dual_numbers(asin(x[0]), x[1] / sqrt(1 - x[0] * x[0]));
		}
	};


	//逆余弦
	template <class T>
	struct Acos<dual_numbers<T>> {
		static constexpr auto _acos_(const dual_numbers<T>& x) {
			return dual_numbers(acos(x[0]), -x[1] / sqrt(1 - x[0] * x[0]));
		}
	};


	//逆正接
	template <class T>
	struct Atan<complex<T>> {
		using calc_type = typename math_function_type<T>::type;

		static constexpr auto _atan_(const complex<T>& x) {
			//i / 2 * log((1 - i * x) / (1 + i * x))
			auto temp = make_complex<calc_type>(-x[1], x[0]);
			auto temp2 = log((1 - temp) / (1 + temp)) / 2;
			return make_complex(-temp2[1], temp2[0]);
		}
	};
	template <class T>
	struct Atan<dual_numbers<T>> {
		static constexpr auto _atan_(const dual_numbers<T>& x) {
			return dual_numbers(atan(x[0]), -x[1] / (1 + x[0] * x[0]));
		}
	};
}


#endif