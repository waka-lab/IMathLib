#ifndef IMATH_UTILITY_UTILITY_TYPE_COMPARISON_HPP
#define IMATH_UTILITY_UTILITY_TYPE_COMPARISON_HPP

#include "IMathLib/utility/utility/forward.hpp"

//型の値比較

namespace iml {

	//任意の型の値比較
	template <class T = void>
	struct type_comparison {
		using value_type = T;

		//等価比較
		static constexpr bool eq(const T& v1, const T& v2) { return v1 == v2; }
		//小なり比較
		static constexpr bool lt(const T& v1, const T& v2) { return v1 < v2; }
	};
	template <>
	struct type_comparison<void> {
		//等価比較
		template <class T1, class T2>
		static constexpr auto eq(T1&& v1, T2&& v2) -> decltype(forward<T1>(v1) == forward<T2>(v2)) {
			return forward<T1>(v1) == forward<T2>(v2);
		}
		//小なり比較
		template <class T1, class T2>
		static constexpr auto lt(T1&& v1, T2&& v2)  -> decltype(forward<T1>(v1) < forward<T2>(v2)) {
			return forward<T1>(v1) < forward<T2>(v2);
		}
	};
}

#endif