﻿#ifndef IMATH_UTILITY_UTILITY_HPP
#define IMATH_UTILITY_UTILITY_HPP

//汎用的なクラスや関数


#include "IMathLib/utility/utility/forward.hpp"
#include "IMathLib/utility/utility/move.hpp"
#include "IMathLib/utility/utility/swap.hpp"
#include "IMathLib/utility/utility/tuple_type.hpp"
#include "IMathLib/utility/utility/type_comparison.hpp"


#include "IMathLib/utility/type_traits.hpp"

namespace iml {

	//条件式に応じてクラスの構成でコンパイルエラーを引き起こす補助
	template <bool F, class = enable_if_t<F>>
	class class_if {};
}

namespace iml {

	//任意の位置の型の取得
	template <size_t N, class First, class... Args>
	struct at_type :at_type<N - 1, Args...> {};
	template <class First, class... Args>
	struct at_type<0, First, Args...> {
		using type = First;
	};
	template <size_t N, class First, class... Args>
	using at_type_t = typename at_type<N, First, Args...>::type;
	template <size_t, class>
	struct at_type_tuple;
	template <size_t N, class... Args>
	struct at_type_tuple<N, type_tuple<Args...>> : at_type<N, Args...> {};
	template <class...>
	class tuple;
	template <size_t N, class... Args>
	struct at_type_tuple<N, tuple<Args...>> : at_type<N, Args...> {};
	template <size_t N, class Tuple>
	using at_type_tuple_t = typename at_type_tuple<N, Tuple>::type;


	//任意の位置の整数パラメータの取得
	template <size_t N, size_t First, size_t... Indices>
	struct at_index : at_index<N - 1, Indices...> {};
	template <size_t First, size_t... Indices>
	struct at_index<0, First, Indices...> {
		static constexpr size_t value = First;
	};
	template <size_t, class>
	struct at_index_tuple;
	template <size_t N, size_t... Indices>
	struct at_index_tuple<N, index_tuple<size_t, Indices...>> : at_index<N, Indices...> {};

	//自分自身を返す関数
	template <class T>
	inline constexpr T this_return(const T& x) { return x; }

	//プレースホルダー
	template <size_t N>
	using placeholder = integral_constant<size_t, N>;

}

#endif