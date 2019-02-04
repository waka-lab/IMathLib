#ifndef IMATH_UTILITY_TYPE_TRAITS_IS_CONVERTIBLE_HPP
#define IMATH_UTILITY_TYPE_TRAITS_IS_CONVERTIBLE_HPP

#include "IMathLib/utility/type_traits/integral_constant.hpp"
#include "IMathLib/utility/type_traits/add_decoration.hpp"
#include "IMathLib/utility/type_traits/declval.hpp"


//暗黙的に型変換が可能か


namespace iml {

	template <typename From>
	struct is_convertible_impl_abb_lvalue_reference: add_lvalue_reference<From>{};
	template <typename From>
	struct is_convertible_impl_abb_lvalue_reference<From[]>{
		using type = From[];
	};
	template <class From, class To>
	struct is_convertible_impl {
		using lvalue_type = typename is_convertible_impl_abb_lvalue_reference<From>::type;
		using rvalue_type = typename add_rvalue_reference<From>::type;

		static true_type tester(To);
		static false_type tester(...);

		using type = decltype(tester(static_cast<rvalue_type>(declval<lvalue_type>())));
	};
	//voidに関してはcv装飾関係ない
	template <> struct is_convertible_impl<void, void> : true_type {};
	template <> struct is_convertible_impl<void, void const> : true_type {};
	template <> struct is_convertible_impl<void, void const volatile> : true_type {};
	template <> struct is_convertible_impl<void, void volatile> : true_type {};
	template <> struct is_convertible_impl<void const, void> : true_type {};
	template <> struct is_convertible_impl<void const, void const> : true_type {};
	template <> struct is_convertible_impl<void const, void const volatile> : true_type {};
	template <> struct is_convertible_impl<void const, void volatile> : true_type {};
	template <> struct is_convertible_impl<void const volatile, void> : true_type {};
	template <> struct is_convertible_impl<void const volatile, void const> : true_type {};
	template <> struct is_convertible_impl<void const volatile, void const volatile> : true_type {};
	template <> struct is_convertible_impl<void const volatile, void volatile> : true_type {};
	template <> struct is_convertible_impl<void volatile, void> : true_type {};
	template <> struct is_convertible_impl<void volatile, void const> : true_type {};
	template <> struct is_convertible_impl<void volatile, void const volatile> : true_type {};
	template <> struct is_convertible_impl<void volatile, void volatile> : true_type {};
	template <class To> struct is_convertible_impl<void, To> : false_type {};
	template <class From> struct is_convertible_impl<From, void> : false_type {};
	template <class To> struct is_convertible_impl<void const, To> : false_type {};
	template <class From> struct is_convertible_impl<From, void const> : false_type {};
	template <class To> struct is_convertible_impl<void const volatile, To> : false_type {};
	template <class From> struct is_convertible_impl<From, void const volatile> : false_type {};
	template <class To> struct is_convertible_impl<void volatile, To> : false_type {};
	template <class From> struct is_convertible_impl<From, void volatile> : false_type {};
	//FromがToに暗黙的に変換可能かの判定
	template <class From, class To>
	struct is_convertible : is_convertible_impl<From, To>::type {};

}

#endif