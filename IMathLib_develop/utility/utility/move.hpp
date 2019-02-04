#ifndef IMATH_UTILITY_UTILITY_MOVE_HPP
#define IMATH_UTILITY_UTILITY_MOVE_HPP

#include "IMathLib/utility/type_traits/remove_decoration.hpp"

namespace iml {

	//ムーブセマンティクス(右辺値へのキャスト)
	template <class T>
	inline constexpr typename remove_reference<T>::type&& move(T&& arg) noexcept {
		return (static_cast<typename remove_reference<T>::type&&>(arg));
	}
}

#endif