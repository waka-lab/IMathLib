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
	template <class T>
	constexpr bool is_void_v = is_void<T>::value;


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
	template <class T>
	constexpr bool is_integral_v = is_integral<T>::value;


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
	template <class T>
	constexpr bool is_floating_point_v = is_floating_point<T>::value;


	//算術型の判定
	template<class T>
	struct is_arithmetic : cat_bool<is_integral<T>::value || is_floating_point<T>::value> {};
	template <class T>
	constexpr bool is_arithmetic_v = is_arithmetic<T>::value;


	//符号付き算術型の判定
	template<class T>
	struct is_signed : cat_bool<is_arithmetic<T>::value && bool_constant<(T(-1) < T(0))>::value> {};
	template <class T>
	constexpr bool is_signed_v = is_signed<T>::value;
	//符号無し算術型の判定
	template<class T>
	struct is_unsigned : cat_bool<is_arithmetic<T>::value && bool_constant<(T(0) < T(-1))>::value> {};
	template <class T>
	constexpr bool is_unsigned_v = is_unsigned<T>::value;


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
	struct is_function_impl<R(Types...) volatile&> {
		using bool_type = true_type;
		using result_type = R;
		using arg_types = type_tuple<Types...>;
		static constexpr bool wrap = true;
	};
	template<class R, class... Types>
	struct is_function_impl<R(Types...) const volatile&> {
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
	struct is_function_impl<R(Types...) volatile&&> {
		using bool_type = true_type;
		using result_type = R;
		using arg_types = type_tuple<Types...>;
		static constexpr bool wrap = true;
	};
	template<class R, class... Types>
	struct is_function_impl<R(Types...) const volatile&&> {
		using bool_type = true_type;
		using result_type = R;
		using arg_types = type_tuple<Types...>;
		static constexpr bool wrap = true;
	};
	template <class F>
	struct is_function : is_function_impl<F>::bool_type {};
	template <class F>
	constexpr bool is_function_v = is_function<F>::value;


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
	struct is_member_function_pointer_impl<R(Class::*)(Types...) &&> {
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
	template <class T>
	constexpr bool is_member_function_pointer_v = is_member_function_pointer<T>::value;


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
	template <class T>
	constexpr bool is_member_object_pointer_v = is_member_object_pointer<T>::value;


	//配列の判定
	template<class>
	struct is_array : false_type {};
	template<class T, size_t N>
	struct is_array<T[N]> : true_type {};
	template<class T>
	struct is_array<T[]> : true_type {};
	template <class T>
	constexpr bool is_array_v = is_array<T>::value;


	//左辺値の判定
	template<class>
	struct is_lvalue_reference : false_type {};
	template<class T>
	struct is_lvalue_reference<T&> : true_type {};
	template <class T>
	constexpr bool is_lvalue_reference_v = is_lvalue_reference<T>::value;


	//右辺値の判定
	template<class>
	struct is_rvalue_reference : false_type {};
	template<class T>
	struct is_rvalue_reference<T&&> : true_type {};
	template <class T>
	constexpr bool is_rvalue_reference_v = is_rvalue_reference<T>::value;


	//参照型の判定
	template<class T>
	struct is_reference : cat_bool<is_lvalue_reference<T>::value || is_rvalue_reference<T>::value> {};
	template <class T>
	constexpr bool is_reference_v = is_reference<T>::value;


	//nullptrの判定
	template<class T>
	struct is_null_pointer : cat_bool<is_same<typename remove_cv<T>::type, nullptr_t>::value> {};
	template <class T>
	constexpr bool is_null_pointer_v = is_null_pointer<T>::value;


	//ポインタの判定(メンバポインタではない)
	template<class T>
	struct is_pointer_impl : false_type {};
	template<class T>
	struct is_pointer_impl<T*> : cat_bool<!is_member_object_pointer<T*>::value && !is_member_function_pointer<T*>::value> {};
	template<class T>
	struct is_pointer : is_pointer_impl<typename remove_cv<T>::type> {};
	template <class T>
	constexpr bool is_pointer_v = is_pointer<T>::value;


	//関数ポインタの判定
	template <class T>
	struct is_function_pointer
		: conditional<is_pointer<T>::value && is_function<typename remove_pointer<T>::type>::value, true_type, false_type>::type {};
	template <class T>
	constexpr bool is_function_pointer_v = is_function_pointer<T>::value;


	//クラスか共用体の判定
	template <class T>
	struct is_class_or_union_impl {
		template <class _T> static true_type tester(void(_T::*)(void));
		template <class _T> static false_type tester(...);

		using type = decltype(tester<T>(nullptr));
	};
	template <class T>
	struct is_class_or_union : is_class_or_union_impl<T>::type {};
	template <class T>
	constexpr bool is_class_or_union_v = is_class_or_union<T>::value;
	//共用体の判定
	template<class T>
	struct is_union : cat_bool<__is_union(T)> {};
	template <class T>
	constexpr bool is_union_v = is_union<T>::value;
	//クラスの判定
	template <class T>
	struct is_class : cat_bool<is_class_or_union<T>::value && !is_union<T>::value> {};
	template <class T>
	constexpr bool is_class_v = is_class<T>::value;


	//基本型の判定
	template<class T>
	struct is_fundamental : cat_bool<is_arithmetic<T>::value || is_void<T>::value || is_null_pointer<T>::value> {};
	template <class T>
	constexpr bool is_fundamental_v = is_fundamental<T>::value;


	//オブジェクト型の判定
	template<class T>
	struct is_object : cat_bool<!is_function<T>::value && !is_reference<T>::value && !is_void<T>::value> {};
	template <class T>
	constexpr bool is_object_v = is_object<T>::value;


	//列挙型の判定
	template <class T>
	struct is_enum : cat_bool<!is_arithmetic<T>::value && !is_reference<T>::value && !is_function<T>::value && !is_class_or_union<T>::value && !is_array<T>::value> {};
	template <class T>
	constexpr bool is_enum_v = is_enum<T>::value;


	//メンバへのポインタの判定
	template <class T>
	struct is_member_pointer : cat_bool<is_member_object_pointer<T>::value || is_member_function_pointer<T>::value> {};
	template <class T>
	constexpr bool is_member_pointer_v = is_member_pointer<T>::value;


	//スカラー型の判定
	template <class T>
	struct is_scalar : cat_bool<is_arithmetic<T>::value || is_enum<T>::value || is_pointer<T>::value || is_member_pointer<T>::value || is_null_pointer<T>::value> {};
	template <class T>
	constexpr bool is_scalar_v = is_scalar<T>::value;


	//const修飾の判定
	template <class>
	struct is_const : false_type {};
	template<class T>
	struct is_const<const T> : true_type {};
	template <class T>
	constexpr bool is_const_v = is_const<T>::value;


	//volatile修飾の判定
	template<class>
	struct is_volatile : false_type {};
	template<class T>
	struct is_volatile<volatile T> : true_type {};
	template <class T>
	constexpr bool is_volatile_v = is_volatile<T>::value;
}

#endif