#ifndef _IMATH_UTILITY_UTILITY_HPP
#define _IMATH_UTILITY_UTILITY_HPP

//他のライブラリでも用いる汎用的な関数やクラス


#include "IMathLib/utility/type_traits.hpp"

namespace iml {

	//条件式に応じてクラスの構成でコンパイルエラーを引き起こす補助
	template <bool f, class = typename enable_if<f>::type>
	class class_if {};
}

namespace iml {

	//ムーブセマンティクス(ムーブへのキャスト)
	template <class T>
	inline constexpr typename remove_reference<T>::type&& move(T&& arg) noexcept {
		return (static_cast<typename remove_reference<T>::type&&>(arg));
	}

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

	//スワップ
	template <class T>
	inline constexpr void swap(T& t1, T& t2) {
		T buf = move(t1);
		t1 = move(t2);
		t2 = move(buf);
	}
	//配列のスワップ
	template <imsize_t N, class T>
	inline constexpr void swap(T(&t1)[N], T(&t2)[N]) {
		for (imsize_t i = 0; i < N; ++i) swap(t1[i], t2[i]);
	}
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
	struct at_type<N, arg_tuple<Args...>> : at_type<N, Args...> {};

	//任意の位置の整数パラメータの取得
	template <imsize_t N, imsize_t First, imsize_t... Indices>
	struct at_sign : at_sign<N - 1, Indices...> {};
	template <imsize_t First, imsize_t... Indices>
	struct at_sign<0, First, Indices...> {
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