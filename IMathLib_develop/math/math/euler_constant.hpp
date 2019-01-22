#ifndef _IMATH_MATH_EULER_CONSTANT_HPP
#define _IMATH_MATH_EULER_CONSTANT_HPP

#include "IMathLib/math/math/math_traits.hpp"
#include "IMathLib/math/math/gamma.hpp"

namespace iml {

	//オイラーの定数(ディガンマ関数の特殊値-ψ(1)による導出)
	template <class T>
	inline constexpr auto _Euler_constant() { return -digamma<T>(1); }


	//ライブラリ用のオイラーの定数
	template <class T>
	struct _Euler_constant_ {
		static constexpr auto euler_constant = _Euler_constant<T>();
	};
}


#endif