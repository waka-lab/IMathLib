#ifndef IMATH_UTILITY_TYPE_TRAITS_IS_TYPE_HPP
#define IMATH_UTILITY_TYPE_TRAITS_IS_TYPE_HPP

#include "IMathLib/utility/type_traits/integral_constant.hpp"
#include "IMathLib/utility/utility/tuple_type.hpp"
#include "IMathLib/utility/type_traits/remove_decoration.hpp"
#include "IMathLib/utility/type_traits/type_comparison.hpp"


//型判定(is a関係)

namespace iml {

	//nullptr型
	using nullptr_t = decltype(nullptr);
	//ポインタの減算の結果型
	using ptrdiff_t = decltype(static_cast<char*>(nullptr) - static_cast<char*>(nullptr));


	//voidの判定
	template <class T>
	struct is_void_impl : false_type {};
	template <>
	struct is_void_impl<void> : true_type {};
	template <class T>
	struct is_void : is_void_impl<typename remove_cv<T>::type> {};


	//整数型の判定
	template<class T>
	struct is_integral_impl : false_type {};
	template<>
	struct is_integral_impl<bool> : true_type {};
	template<>
	struct is_integral_impl<char> : true_type {};
	template<>
	struct is_integral_impl<unsigned char> : true_type {};
	template<>
	struct is_integral_impl<wchar_t> : true_type {};
	template<>
	struct is_integral_impl<short> : true_type {};
	template<>
	struct is_integral_impl<unsigned short> : true_type {};
	template<>
	struct is_integral_impl<int> : true_type {};
	template<>
	struct is_integral_impl<unsigned int> : true_type {};
	template<>
	struct is_integral_impl<long> : true_type {};
	template<>
	struct is_integral_impl<unsigned long> : true_type {};
	template<>
	struct is_integral_impl<char16_t> : true_type {};
	template<>
	struct is_integral_impl<char32_t> : true_type {};
	template<>
	struct is_integral_impl<long long> : true_type {};
	template<>
	struct is_integral_impl<unsigned long long> : true_type {};
	template<class T>
	struct is_integral : is_integral_impl<typename remove_cv<T>::type> {};


	//浮動小数点型の判定
	template<class T>
	struct is_floating_point_impl : false_type {};
	template<>
	struct is_floating_point_impl<float> : true_type {};
	template<>
	struct is_floating_point_impl<double> : true_type {};
	template<>
	struct is_floating_point_impl<long double> : true_type {};
	template<class T>
	struct is_floating_point : is_floating_point_impl<typename remove_cv<T>::type> {};


	//算術型の判定
	template<class T>
	struct is_arithmetic : cat_bool<is_integral<T>::value || is_floating_point<T>::value> {};


	//符号付き算術型の判定
	template<class T>
	struct is_signed : cat_bool<is_arithmetic<T>::value && bool_constant<(T(-1) < T(0))>::value> {};
	//符号無し算術型の判定
	template<class T>
	struct is_unsigned : cat_bool<is_arithmetic<T>::value && bool_constant<(T(0) < T(-1))>::value> {};


	//関数型の判定
	template<class>
	struct is_function_impl {
		using bool_type = false_type;
		static constexpr bool wrap = false;
	};
	template<class R, class... Types>
	struct is_function_impl<R(Types...)> {
		using bool_type = true_type;				//関数型か
		using result_type = R;						//戻り値型
		using arg_types = type_tuple<Types...>;		//引数型列
		static constexpr bool wrap = false;			//cv装飾等があるか
	};
	template<class R, class... Types>
	struct is_function_impl<R(Types...) const> {
		using bool_type = true_type;
		using result_type = R;
		using arg_types = type_tuple<Types...>;
		static constexpr bool wrap = true;
	};
	template<class R, class... Types>
	struct is_function_impl<R(Types...) volatile> {
		using bool_type = true_type;
		using result_type = R;
		using arg_types = type_tuple<Types...>;
		static constexpr bool wrap = true;
	};
	template<class R, class... Types>
	struct is_function_impl<R(Types...) const volatile> {
		using bool_type = true_type;
		using result_type = R;
		using arg_types = type_tuple<Types...>;
		static constexpr bool wrap = true;
	};
	template<class R, class... Types>
	struct is_function_impl<R(Types...) &> {
		using bool_type = true_type;
		using result_type = R;
		using arg_types = type_tuple<Types...>;
		static constexpr bool wrap = true;
	};
	template<class R, class... Types>
	struct is_function_impl<R(Types...) const &> {
		using bool_type = true_type;
		using result_type = R;
		using arg_types = type_tuple<Types...>;
		static constexpr bool wrap = true;
	};
	template<class R, class... Types>
	struct is_function_impl<R(Types...) volatile &> {
		using bool_type = true_type;
		using result_type = R;
		using arg_types = type_tuple<Types...>;
		static constexpr bool wrap = true;
	};
	template<class R, class... Types>
	struct is_function_impl<R(Types...) const volatile &> {
		using bool_type = true_type;
		using result_type = R;
		using arg_types = type_tuple<Types...>;
		static constexpr bool wrap = true;
	};
	template<class R, class... Types>
	struct is_function_impl<R(Types...) && > {
		using bool_type = true_type;
		using result_type = R;
		using arg_types = type_tuple<Types...>;
		static constexpr bool wrap = true;
	};
	template<class R, class... Types>
	struct is_function_impl<R(Types...) const &&> {
		using bool_type = true_type;
		using result_type = R;
		using arg_types = type_tuple<Types...>;
		static constexpr bool wrap = true;
	};
	template<class R, class... Types>
	struct is_function_impl<R(Types...) volatile &&> {
		using bool_type = true_type;
		using result_type = R;
		using arg_types = type_tuple<Types...>;
		static constexpr bool wrap = true;
	};
	template<class R, class... Types>
	struct is_function_impl<R(Types...) const volatile &&> {
		using bool_type = true_type;
		using result_type = R;
		using arg_types = type_tuple<Types...>;
		static constexpr bool wrap = true;
	};
	template <class F>
	struct is_function : is_function_impl<F>::bool_type {};


	//メンバ関数のポインタの判定
	template<class>
	struct is_member_function_pointer_impl {
		using bool_type = false_type;
	};
	template<class R, class Class, class... Types>
	struct is_member_function_pointer_impl<R(Class::*)(Types...)> {
		using bool_type = true_type;
		using result_type = R;
		using class_type = Class;
	};
	template<class R, class Class, class... Types>
	struct is_member_function_pointer_impl<R(Class::*)(Types...) const> {
		using bool_type = true_type;
		using result_type = R;
		using class_type = Class;
	};
	template<class R, class Class, class... Types>
	struct is_member_function_pointer_impl<R(Class::*)(Types...) volatile> {
		using bool_type = true_type;
		using result_type = R;
		using class_type = Class;
	};
	template<class R, class Class, class... Types>
	struct is_member_function_pointer_impl<R(Class::*)(Types...) const volatile> {
		using bool_type = true_type;
		using result_type = R;
		using class_type = Class;
	};
	template<class R, class Class, class... Types>
	struct is_member_function_pointer_impl<R(Class::*)(Types...) &> {
		using bool_type = true_type;
		using result_type = R;
		using class_type = Class;
	};
	template<class R, class Class, class... Types>
	struct is_member_function_pointer_impl<R(Class::*)(Types...) const &> {
		using bool_type = true_type;
		using result_type = R;
		using class_type = Class;
	};
	template<class R, class Class, class... Types>
	struct is_member_function_pointer_impl<R(Class::*)(Types...) volatile &> {
		using bool_type = true_type;
		using result_type = R;
		using class_type = Class;
	};
	template<class R, class Class, class... Types>
	struct is_member_function_pointer_impl<R(Class::*)(Types...) const volatile &> {
		using bool_type = true_type;
		using result_type = R;
		using class_type = Class;
	};
	template<class R, class Class, class... Types>
	struct is_member_function_pointer_impl<R(Class::*)(Types...) && > {
		using bool_type = true_type;
		using result_type = R;
		using class_type = Class;
	};
	template<class R, class Class, class... Types>
	struct is_member_function_pointer_impl<R(Class::*)(Types...) const &&> {
		using bool_type = true_type;
		using result_type = R;
		using class_type = Class;
	};
	template<class R, class Class, class... Types>
	struct is_member_function_pointer_impl<R(Class::*)(Types...) volatile &&> {
		using bool_type = true_type;
		using result_type = R;
		using class_type = Class;
	};
	template<class R, class Class, class... Types>
	struct is_member_function_pointer_impl<R(Class::*)(Types...) const volatile &&> {
		using bool_type = true_type;
		using result_type = R;
		using class_type = Class;
	};
	template<class T>
	struct is_member_function_pointer : is_member_function_pointer_impl<typename remove_cv<T>::type>::bool_type {};


	//メンバオブジェクト(変数)のポインタの判定
	template<class T, bool = is_member_function_pointer_impl<T>::bool_type::value>
	struct is_member_object_pointer_impl {
		using bool_type = false_type;
	};
	template<class T1, class T2>
	struct is_member_object_pointer_impl<T1 T2::*, false> {
		using bool_type = true_type;
		using class_type = T2;
	};
	template<class T>
	struct is_member_object_pointer : is_member_object_pointer_impl<typename remove_cv<T>::type>::bool_type {};


	//配列の判定
	template<class>
	struct is_array : false_type {};
	template<class T, imsize_t N>
	struct is_array<T[N]> : true_type {};
	template<class T>
	struct is_array<T[]> : true_type {};


	//左辺値の判定
	template<class>
	struct is_lvalue_reference : false_type {};
	template<class T>
	struct is_lvalue_reference<T&> : true_type {};


	//右辺値の判定
	template<class>
	struct is_rvalue_reference : false_type {};
	template<class T>
	struct is_rvalue_reference<T&&> : true_type {};


	//参照型の判定
	template<class T>
	struct is_reference : cat_bool<is_lvalue_reference<T>::value || is_rvalue_reference<T>::value> {};


	//nullptrの判定
	template<class T>
	struct is_null_pointer : cat_bool<is_same<typename remove_cv<T>::type, nullptr_t>::value> {};


	//ポインタの判定(メンバポインタではない)
	template<class T>
	struct is_pointer_impl : false_type {};
	template<class T>
	struct is_pointer_impl<T*> : cat_bool<!is_member_object_pointer<T*>::value && !is_member_function_pointer<T*>::value> {};
	template<class T>
	struct is_pointer : is_pointer_impl<typename remove_cv<T>::type> {};


	//関数ポインタの判定
	template <class T>
	struct is_function_pointer
		: conditional<is_pointer<T>::value && is_function<typename remove_pointer<T>::type>::value, true_type, false_type>::type {};


	//クラスか共用体の判定
	template <class T>
	struct is_class_or_union_impl {
		template <class _T> static true_type tester(void(_T::*)(void));
		template <class _T> static false_type tester(...);

		using type = decltype(tester<T>(nullptr));
	};
	template <class T>
	struct is_class_or_union : is_class_or_union_impl<T>::type {};
	//共用体の判定
	template<class T>
	struct is_union : cat_bool<__is_union(T)> {};
	//クラスの判定
	template <class T>
	struct is_class : cat_bool<is_class_or_union<T>::value && !is_union<T>::value> {};


	//基本型の判定
	template<class T>
	struct is_fundamental : cat_bool<is_arithmetic<T>::value || is_void<T>::value || is_null_pointer<T>::value> {};


	//オブジェクト型の判定
	template<class T>
	struct is_object : cat_bool<!is_function<T>::value && !is_reference<T>::value && !is_void<T>::value> {};


	//列挙型の判定
	template <class T>
	struct is_enum : cat_bool<!is_arithmetic<T>::value && !is_reference<T>::value && !is_function<T>::value && !is_class_or_union<T>::value && !is_array<T>::value> {};


	//メンバへのポインタの判定
	template <class T>
	struct is_member_pointer : cat_bool<is_member_object_pointer<T>::value || is_member_function_pointer<T>::value> {};


	//スカラー型の判定
	template <class T>
	struct is_scalar : cat_bool<is_arithmetic<T>::value || is_enum<T>::value || is_pointer<T>::value || is_member_pointer<T>::value || is_null_pointer<T>::value> {};


	//const修飾の判定
	template <class>
	struct is_const : false_type {};
	template<class T>
	struct is_const<const T> : true_type {};


	//volatile修飾の判定
	template<class>
	struct is_volatile : false_type {};
	template<class T>
	struct is_volatile<volatile T> : true_type {};

}

#endif