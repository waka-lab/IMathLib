#ifndef _IMATH_UTILITY_TYPE_TRAITS_IS_CONVERTIBLE_HPP
#define _IMATH_UTILITY_TYPE_TRAITS_IS_CONVERTIBLE_HPP



//暗黙的に型変換が可能か


namespace iml {

	template <typename From>
	struct _Is_convertible_abb_lvalue_reference: add_lvalue_reference<From>{};
	template <typename From>
	struct _Is_convertible_abb_lvalue_reference<From[]>{
		using type = From[];
	};
	template <class From, class To>
	struct _Is_convertible {
		using lvalue_type = typename _Is_convertible_abb_lvalue_reference<From>::type;
		using rvalue_type = typename add_rvalue_reference<From>::type;

		static true_type tester(To);
		static false_type tester(...);

		using type = decltype(tester(static_cast<rvalue_type>(declval<lvalue_type>())));
	};
	//voidに関してはcv装飾関係ない
	template <> struct _Is_convertible<void, void> : true_type {};
	template <> struct _Is_convertible<void, void const> : true_type {};
	template <> struct _Is_convertible<void, void const volatile> : true_type {};
	template <> struct _Is_convertible<void, void volatile> : true_type {};
	template <> struct _Is_convertible<void const, void> : true_type {};
	template <> struct _Is_convertible<void const, void const> : true_type {};
	template <> struct _Is_convertible<void const, void const volatile> : true_type {};
	template <> struct _Is_convertible<void const, void volatile> : true_type {};
	template <> struct _Is_convertible<void const volatile, void> : true_type {};
	template <> struct _Is_convertible<void const volatile, void const> : true_type {};
	template <> struct _Is_convertible<void const volatile, void const volatile> : true_type {};
	template <> struct _Is_convertible<void const volatile, void volatile> : true_type {};
	template <> struct _Is_convertible<void volatile, void> : true_type {};
	template <> struct _Is_convertible<void volatile, void const> : true_type {};
	template <> struct _Is_convertible<void volatile, void const volatile> : true_type {};
	template <> struct _Is_convertible<void volatile, void volatile> : true_type {};
	template <class To> struct _Is_convertible<void, To> : false_type {};
	template <class From> struct _Is_convertible<From, void> : false_type {};
	template <class To> struct _Is_convertible<void const, To> : false_type {};
	template <class From> struct _Is_convertible<From, void const> : false_type {};
	template <class To> struct _Is_convertible<void const volatile, To> : false_type {};
	template <class From> struct _Is_convertible<From, void const volatile> : false_type {};
	template <class To> struct _Is_convertible<void volatile, To> : false_type {};
	template <class From> struct _Is_convertible<From, void volatile> : false_type {};
	//FromがToに暗黙的に変換可能かの判定
	template <class From, class To>
	struct is_convertible : _Is_convertible<From, To>::type {};

}

#endif