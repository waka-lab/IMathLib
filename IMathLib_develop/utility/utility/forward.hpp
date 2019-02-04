#ifndef IMATH_UTILITY_UTILITY_FORWARD_HPP
#define IMATH_UTILITY_UTILITY_FORWARD_HPP

#include "IMathLib/utility/type_traits/remove_decoration.hpp"


namespace iml {


	//完全転送(テンプレート引数をそのまま別関数に渡す)
	template<class T>
	inline constexpr T&& forward(typename remove_reference<T>::type& arg) noexcept {
		return (static_cast<T&&>(arg));
	}
	template<class T>
	inline constexpr T&& forward(typename remove_reference<T>::type&& arg) noexcept {
		//左辺値参照でないときコンパイルエラー
		static_assert(!is_lvalue_reference<T>::value, "bad forward call");
		return (static_cast<T&&>(arg));
	}

}

#endif