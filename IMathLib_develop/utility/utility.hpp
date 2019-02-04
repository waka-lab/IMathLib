#ifndef IMATH_UTILITY_UTILITY_HPP
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
	template <bool f, class = typename enable_if<f>::type>
	class class_if {};
}

namespace iml {

	//任意の位置の型の取得
	template <imsize_t N, class First, class... Args>
	struct at_type :at_type<N - 1, Args...> {};
	template <class First, class... Args>
	struct at_type<0, First, Args...> {
		using type = First;
	};
	template <imsize_t N, class... Args>
	struct at_type<N, type_tuple<Args...>> : at_type<N, Args...> {};

	//任意の位置の整数パラメータの取得
	template <imsize_t N, imsize_t First, imsize_t... Indices>
	struct at_index : at_index<N - 1, Indices...> {};
	template <imsize_t First, imsize_t... Indices>
	struct at_index<0, First, Indices...> {
		static constexpr imsize_t value = First;
	};

	//自分自身を返す関数
	template <class T>
	inline constexpr T this_return(const T& x) { return x; }

	//プレースホルダー
	template <imsize_t N>
	using placeholder = integral_constant<imsize_t, N>;

}

#endif