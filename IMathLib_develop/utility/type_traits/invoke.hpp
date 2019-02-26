#ifndef IMATH_UTILITY_TYPE_TRAITS_INVOKE_HPP
#define IMATH_UTILITY_TYPE_TRAITS_INVOKE_HPP

#include "IMathLib/utility/utility/forward.hpp"
#include "IMathLib/utility/type_traits/is_type.hpp"
#include "IMathLib/utility/type_traits/is_base_of.hpp"
#include "IMathLib/utility/type_traits/decay.hpp"


//INVOKEに従ったオブジェクトの呼び出し

namespace iml {

	//オブジェクトのメンバ関数へのポインタの呼び出し
	struct invoker_pmf_object {
		template<class Decay, class T, class... Args>
		static constexpr auto call(Decay pmf, T&& arg, Args&&... args) { return (forward<T>(arg).*pmf)(forward<Args>(args)...); }
	};
	//スマートポインタでオブジェクトのメンバ関数へのポインタの呼び出し
	struct invoker_pmf_pointer {
		template<class Decay, class T, class... Args>
		static constexpr auto call(Decay pmf, T&& arg, Args&&... args) { return ((*forward<T>(arg)).*pmf)(forward<Args>(args)...); }
	};
	//オブジェクトのメンバデータへのポインタの呼び出し
	struct invoker_pmd_object {
		template<class Decay, class T>
		static constexpr auto call(Decay pmd, T&& arg) { return forward<T>(arg).*pmd; }
	};
	//スマートポインタでオブジェクトのメンバデータへのポインタの呼び出し
	struct invoker_pmd_pointer {
		template<class Decay, class T>
		static constexpr auto call(Decay pmd, T&& arg) { return (*forward<T>(arg)).*pmd; }
	};
	//関数オブジェクトの呼び出し
	struct invoker_functor {
		template<class F, class... Args>
		static constexpr auto call(F&& obj, Args&&... args) { return forward<F>(obj)(forward<Args>(args)...); }
	};

	//オブジェクトがクラスメンバに関するものかの判別
	template <class F, class, class Decay = typename decay<F>::type, bool = is_member_function_pointer<Decay>::value, bool = is_member_object_pointer<Decay>::value>
	struct invoker_impl;
	//メンバ関数へのポインタのとき(FのクラスがTの基底なら通常の呼び出しでそうでないならスマートポインタ)
	template <class F, class T, class Decay>
	struct invoker_impl<F, T, Decay, true, false> :
		conditional<is_base_of<typename is_member_function_pointer_impl<Decay>::Class_type, typename decay<T>::type>::value,
		invoker_pmf_object, invoker_pmf_pointer>::type {};
	//メンバデータへのポインタのとき(FのクラスがTの基底なら通常の呼び出しでそうでないならポインタ)
	template <class F, class T, class Decay>
	struct invoker_impl<F, T, Decay, false, true> :
		conditional<is_base_of<typename is_member_object_pointer_impl<Decay>::class_type, typename decay<T>::type>::value,
		invoker_pmd_object, invoker_pmd_pointer>::type {};
	//関数オブジェクト
	template <class F, class T, class Decay>
	struct invoker_impl<F, T, Decay, false, false> : invoker_functor {};

	//オブジェクトを呼び出す
	template <class F, class... Args>
	struct invoker;
	//引数がないときはクラスメンバではない
	template <class F>
	struct invoker<F> : invoker_functor {};
	//引数が存在するときはクラスメンバかファンクタ
	template<class F, class FirstT, class... Args>
	struct invoker<F, FirstT, Args...> : invoker_impl<F, FirstT> {};

	//呼び出し可能なオブジェクトを呼び出す
	template<class F, class... Args>
	inline constexpr auto invoke(F&& obj, Args&&... args) -> decltype(invoker<F, Args...>::call(forward<F>(obj), forward<Args>(args)...)) {
		return invoker<F, Args...>::call(forward<F>(obj), forward<Args>(args)...);
	}


	//オブジェクトの呼び出しの戻り値型の取得
	template <class F, class... Args>
	struct invoke_result {
		using type = decltype(invoke(declval<F&>(), declval<Args&>()...));
	};
}

#endif