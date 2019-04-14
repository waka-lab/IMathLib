#ifndef IMATH_UTILITY_TYPE_TRAITS_TYPE_COMPARISON_HPP
#define IMATH_UTILITY_TYPE_TRAITS_TYPE_COMPARISON_HPP

#include "IMathLib/utility/type_traits/integral_constant.hpp"


//型比較

namespace iml {

	//条件式が真のときのみ型が有効
	template<bool, class T = void>
	struct enable_if {};
	template<class T>
	struct enable_if<true, T> {
		using type = T;
	};
	template <bool F, class T = void>
	using enable_if_t = typename enable_if<F, T>::type;


	//条件式によって型を切り替える
	template<bool, class T1, class T2>
	struct conditional {
		using type = T2;
	};
	template<class T1, class T2>
	struct conditional<true, T1, T2> {
		using type = T1;
	};
	template <bool F, class T1, class T2>
	using conditional_t = typename conditional<F, T1, T2>::type;


	//同じ型か判定
	template<class, class>
	struct is_same : false_type {};
	template<class T>
	struct is_same<T, T> : true_type {};
	template <class T1, class T2>
	constexpr bool is_same_v = is_same<T1, T2>::value;


	//有効なパラメータのテスト(パラメータが存在するかのテスト)
	template<class...>
	struct _Param_tester { using type = void; };
	//任意の0以上の型をvoidに変換
	template<class... Types>
	using void_t = typename _Param_tester<Types...>::type;
	//C++17 ver
	//template <class...>
	//using void_t = void;
}

#endif