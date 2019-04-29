#ifndef IMATH_MATH_HYPERCOMPLEX_QUATERNION_MATH_HPP
#define IMATH_MATH_HYPERCOMPLEX_QUATERNION_MATH_HPP

#include "IMathLib/math/math.hpp"
#include "IMathLib/math/hypercomplex/quaternion.hpp"

/*namespace iml {

	template <class T>
	struct numerical_evaluation_function<quaternion<T>> {
		using type = quaternion<T>;

		//共役
		static type __conj(const type& x) {
			return type(x.real(), -x.image());
		}
	};

	template <class T>
	struct math_function<quaternion<T>> {
		using type = quaternion<T>;

		//絶対値
		static T __abs(const type& x) {
			return sqrt<T>(x.real()*x.real() + x.image()*x.image());
		}
		//実部
		static T __real(const type& x) {
			return x.x[0];
		}
		//ノルム
		static T __norm(const type& x) {
			return (x.real()*x.real() + x.image()*x.image());
		}
		//ベルソル
		static type __versor(const type& x) {
			return x / __abs(x);
		}
	};
}

namespace iml {

	//ベルソル
	template <class T>
	inline quaternion<T> versor(const quaternion<T>& x) { return math_function<quaternion<T>>::__versor(x); }
}
*/

#endif