#ifndef _IMATH_MATH_MATH_CONSTANT_HPP
#define _IMATH_MATH_MATH_CONSTANT_HPP

#include "IMathLib/math/math/math_traits.hpp"

#include "IMathLib/math/math/pi.hpp"
#include "IMathLib/math/math/e.hpp"
#include "IMathLib/math/math/loge_2.hpp"
#include "IMathLib/math/math/euler_constant.hpp"
#include "IMathLib/math/math/apery_constant.hpp"


namespace iml {

	//数学定数
	template <class T>
	struct math_constant {
		//円周率
		static constexpr auto pi = _Pi<T>();
		//ネイピア数
		static constexpr auto e = _E<T>();
		//loge_2
		static constexpr auto loge_2 = _Loge_2<T>();
		//オイラーの定数
		static constexpr auto euler_constant = _Euler_constant<T>();
		//アペリーの定数
		static constexpr auto apery_constant = _Apery_constant<T>();
	};

}


#endif