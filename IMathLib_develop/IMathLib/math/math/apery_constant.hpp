#ifndef _IMATH_MATH_APERY_CONSTANT_HPP
#define _IMATH_MATH_APERY_CONSTANT_HPP

#include "IMathLib/math/math/math_traits.hpp"
#include "IMathLib/math/math/riemann_zeta.hpp"

namespace iml {

	//アペリーの定数(リーマンのゼータ関数の特殊値ζ(3)による導出)
	template <class T>
	inline constexpr auto _Apery_constant() { return riemann_zeta<T>(3); }


	//ライブラリ用のオイラーの定数
	template <class T>
	struct _Apery_constant_ {
		static constexpr auto apery_constant = _Apery_constant<T>();
	};
}


#endif