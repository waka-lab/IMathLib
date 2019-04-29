#ifndef IMATH_MATH_MATH_EULER_CONSTANT_HPP
#define IMATH_MATH_MATH_EULER_CONSTANT_HPP

#include "IMathLib/math/math/math_traits.hpp"
#include "IMathLib/math/math/gamma.hpp"

namespace iml {

	//オイラーの定数(ディガンマ関数の特殊値-ψ(1)による導出)
	template <class T>
	struct Euler_constant {
		static constexpr auto _euler_constant_() { return -digamma<T>(1); }
	};

	//オイラーの定数
	template <class T>
	constexpr auto euler_constant = Euler_constant<T>::_euler_constant_();
}


#endif