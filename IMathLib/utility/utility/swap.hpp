#ifndef IMATH_UTILITY_UTILITY_SWAP_HPP
#define IMATH_UTILITY_UTILITY_SWAP_HPP

#include "IMathLib/IMathLib_config.hpp"
#include "IMathLib/utility/type_traits/remove_decoration.hpp"

namespace iml {

	//スワップ
	template <class T>
	inline constexpr void swap(T& t1, T& t2) {
		T buf = move(t1);
		t1 = move(t2);
		t2 = move(buf);
	}
	//配列のスワップ
	template <size_t N, class T>
	inline constexpr void swap(T(&t1)[N], T(&t2)[N]) {
		for (size_t i = 0; i < N; ++i) swap(t1[i], t2[i]);
	}
}

#endif