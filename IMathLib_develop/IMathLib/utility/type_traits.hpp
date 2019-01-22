#ifndef _IMATH_UTILITIY_TYPE_TRAITS_HPP
#define _IMATH_UTILITIY_TYPE_TRAITS_HPP

//型判定の標準命令系の定義

#include "IMathLib/IMathLib_config.hpp"

namespace iml {
	//型のconst修飾の除去
	template<class T>
	struct remove_const {
		using type = T;
	};
	template<class T>
	struct remove_const<const T> {
		using type = T;
	};

	//型のvolatileの除去
	template<class T>
	struct remove_volatile {
		using type = T;
	};
	template<class T>
	struct remove_volatile<volatile T> {
		using type = T;
	};

	//型のconst-volatile修飾の除去
	template<class T>
	struct remove_cv {
		using type = typename remove_const<typename remove_volatile<T>::type>::type;
	};

	//参照の削除
	template <class T>
	struct remove_reference {
		using type = T;
	};
	template <class T>
	struct remove_reference<T&> {
		using type = T;
	};
	template <class T>
	struct remove_reference<T&&> {
		using type = T;
	};

	//const-volatile修飾とreferenceの除去
	template <class T>
	struct remove_cvr {
		using type = typename remove_reference<typename remove_cv<T>::type>::type;
	};
}

namespace iml {

	//整数型定数
	template<class T, T Val>
	struct integral_constant {
		static constexpr T value = Val;

		using value_type = T;
		using type = integral_constant<T, Val>;
		//保持している値を返す
		constexpr operator value_type() const noexcept { return value; }
		constexpr value_type operator()() const noexcept { return value; }

	};
	//bool型の定数
	template<bool Val>
	using bool_constant = integral_constant<bool, Val>;
	using true_type = bool_constant<true>;
	using false_type = bool_constant<false>;

	//戻り値の判定(C++03の名残なため多分消す)
	using yes_type = char;
	struct no_type { char value[8]; };


	//voidの判定
	template <class T>
	struct _Is_void : false_type {};
	template <>
	struct _Is_void<void> : true_type {};
	template <class T>
	struct is_void : _Is_void<typename remove_cv<T>::type> {};

	//nullptr型
	using nullptr_t = decltype(nullptr);

	//条件式が真のときのみ型が有効
	template<bool, class T = void>
	struct enable_if {};
	template<class T>
	struct enable_if<true, T> {
		using type = T;
	};

	//条件式によって型を切り替える
	template<bool, class T1, class T2>
	struct conditional {
		using type = T2;
	};
	template<class T1, class T2>
	struct conditional<true, T1, T2> {
		using type = T1;
	};

	//叙述関数の補助
	template<bool Val>
	struct _Cat_base : integral_constant<bool, Val> {};

	//整数型の判定
	template<class T>
	struct _Is_integral : false_type {};
	template<>
	struct _Is_integral<bool> : true_type {};
	template<>
	struct _Is_integral<char> : true_type {};
	template<>
	struct _Is_integral<unsigned char> : true_type {};
	template<>
	struct _Is_integral<wchar_t> : true_type {};
	template<>
	struct _Is_integral<short> : true_type {};
	template<>
	struct _Is_integral<unsigned short> : true_type {};
	template<>
	struct _Is_integral<int> : true_type {};
	template<>
	struct _Is_integral<unsigned int> : true_type {};
	template<>
	struct _Is_integral<long> : true_type {};
	template<>
	struct _Is_integral<unsigned long> : true_type {};
	template<>
	struct _Is_integral<char16_t> : true_type {};
	template<>
	struct _Is_integral<char32_t> : true_type {};
	template<>
	struct _Is_integral<long long> : true_type {};
	template<>
	struct _Is_integral<unsigned long long> : true_type {};
	//整数型の判定
	template<class T>
	struct is_integral : _Is_integral<typename remove_cv<T>::type> {};

	//浮動小数点型の判定
	template<class T>
	struct _Is_floating_point : false_type {};
	template<>
	struct _Is_floating_point<float> : true_type {};
	template<>
	struct _Is_floating_point<double> : true_type {};
	template<>
	struct _Is_floating_point<long double> : true_type {};
	//浮動小数点型の判定
	template<class T>
	struct is_floating_point : _Is_floating_point<typename remove_cv<T>::type> {};

	//算術型の判定
	template<class T>
	struct is_arithmetic : _Cat_base<is_integral<T>::value || is_floating_point<T>::value> {};

	//完全転送(ヘッダーの相互干渉の回避のための定義)
	template<class T>
	inline constexpr T&& _Forward(typename remove_reference<T>::type& arg) noexcept {
		return (static_cast<T&&>(arg));
	}
	template<class T>
	inline constexpr T&& _Forward(typename remove_reference<T>::type&& arg) noexcept {
		//左辺値参照でないときコンパイルエラー
		static_assert(!is_lvalue_reference<T>::value, "bad _Forward call");
		return (static_cast<T&&>(arg));
	}

	//任意の型の値比較
	template <class T>
	struct type_comparison {
		using value_type = T;

		//等価比較
		static constexpr bool eq(const T& v1, const T& v2) { return v1 == v2; }
		//小なり比較
		static constexpr bool lt(const T& v1, const T& v2) { return v1 < v2; }

		//任意の述語
		static constexpr bool pred(const T&) { return true; }
	};
	template <>
	struct type_comparison<void> {
		//等価比較
		template <class T1, class T2>
		static constexpr auto eq(T1&& v1, T2&& v2) -> decltype(_Forward<T1>(v1) == _Forward<T2>(v2)) {
			return _Forward<T1>(v1) == _Forward<T2>(v2);
		}
		//小なり比較
		template <class T1, class T2>
		static constexpr auto lt(T1&& v1, T2&& v2)  -> decltype(_Forward<T1>(v1) < _Forward<T2>(v2)) {
			return _Forward<T1>(v1) < _Forward<T2>(v2);
		}

		//任意の述語
		template <class T>
		static constexpr auto pred(T&&) { return true; }
	};

	//符号付き算術型の判定
	template<class T>
	struct is_signed : _Cat_base<is_arithmetic<T>::value && bool_constant<T(-1) < T(0)>::value> {};
	//符号無し算術型の判定
	template<class T>
	struct is_unsigned : _Cat_base<is_arithmetic<T>::value && bool_constant<T(0) < T(-1)>::value> {};

	//コンパイル時に決定される整数型のシーケンス
	template<class T, T... Vals>
	struct integer_sequence {
		static_assert(is_integral<T>::value, "integer_sequence<T, I...> requires T to be an integral type.");

		using type = integer_sequence<T, Vals...>;
		using value_type = T;

		static constexpr imsize_t size() noexcept { return (sizeof...(Vals)); }
	};
	//シーケンスの作成
	template<bool Negative, bool Zero, class Int_con, class Int_seq>
	struct _Make_seq {
		static_assert(!Negative, "make_integer_sequence<T, N> requires N to be non-negative.");
	};
	template<class T, T... Vals>
	struct _Make_seq<false, true, integral_constant<T, 0>, integer_sequence<T, Vals...>> : integer_sequence<T, Vals...> {};
	template<class T, T Ix, T... Vals>
	struct _Make_seq<false, false, integral_constant<T, Ix>, integer_sequence<T, Vals...>> :
		_Make_seq<false, Ix == 1, integral_constant<T, Ix - 1>, integer_sequence<T, Ix - 1, Vals...>> {};
	template<class T, T Size>
	using make_integer_sequence = typename _Make_seq<Size < 0, Size == 0, integral_constant<T, Size>, integer_sequence<T>>::type;

	//依存名に付加されたfalse値（static_assertの場合）
	template<class T>
	struct _Always_false {
		static constexpr bool value = false;
	};

	//可変長引数のタプル
	template<class...>
	struct arg_tuple {};
	//arg_tupleを逆順にする
	template <class, class>
	struct _Reverse_arg_tuple;
	template <class... Inv, class First, class... Types>
	struct _Reverse_arg_tuple<arg_tuple<Inv...>, arg_tuple<First, Types...>>
		: public _Reverse_arg_tuple<arg_tuple<First, Inv...>, arg_tuple<Types...>> {};
	template <class... Inv>
	struct _Reverse_arg_tuple<arg_tuple<Inv...>, arg_tuple<>> {
		using type = arg_tuple<Inv...>;
	};
	template <class>
	struct reverse_arg_tuple;
	template <class... Types>
	struct reverse_arg_tuple<arg_tuple<Types...>>
		:_Reverse_arg_tuple<arg_tuple<>, arg_tuple<Types...>> {};
	//2つのシーケンスの結合
	template<class, class>
	struct cat_arg_tuple;
	template<class... Types1, class... Types2>
	struct cat_arg_tuple<arg_tuple<Types1...>, arg_tuple<Types2...>> {
		using type = arg_tuple<Types1..., Types2...>;
	};
	//N個の同じ型で構築
	template <class T, imsize_t N, class... Types>
	struct same_arg_tuple : same_arg_tuple<T, N - 1, Types..., T> {};
	template <class T, class... Types>
	struct same_arg_tuple<T, 0, Types...> {
		using type = arg_tuple<Types...>;
	};
	//多層構造となっているarg_tupleを1階層にまとめる
	template <class T>
	struct unit_arg_tuple {
		using type = arg_tuple<T>;
	};
	template <class, class>
	struct _Unit_arg_tuple;
	template <class... Types1, class T, class... Types2>
	struct _Unit_arg_tuple<arg_tuple<Types1...>, arg_tuple<T, Types2...>>
		: _Unit_arg_tuple<typename cat_arg_tuple<arg_tuple<Types1...>, typename unit_arg_tuple<T>::type>::type
		, arg_tuple<Types2...>> {};
	template <class T>
	struct _Unit_arg_tuple<T, arg_tuple<>> {
		using type = T;
	};
	template <class... Types>
	struct unit_arg_tuple<arg_tuple<Types...>> : _Unit_arg_tuple<arg_tuple<>, arg_tuple<Types...>> {};

	//関数型の判定
	template<class>
	struct _Is_function {
		using bool_type = false_type;
		static constexpr bool wrap = false;
	};
	template<class R, class... Types>
	struct _Is_function<R(Types...)> {
		using bool_type = true_type;				//関数型か
		using result_type = R;						//戻り値型
		using arg_types = arg_tuple<Types...>;		//引数型列
		static constexpr bool wrap = false;			//cv装飾等があるか
	};
	template<class R, class... Types>
	struct _Is_function<R(Types...) const> {
		using bool_type = true_type;
		using result_type = R;
		using arg_types = arg_tuple<Types...>;
		static constexpr bool wrap = true;
	};
	template<class R, class... Types>
	struct _Is_function<R(Types...) volatile> {
		using bool_type = true_type;
		using result_type = R;
		using arg_types = arg_tuple<Types...>;
		static constexpr bool wrap = true;
	};
	template<class R, class... Types>
	struct _Is_function<R(Types...) const volatile> {
		using bool_type = true_type;
		using result_type = R;
		using arg_types = arg_tuple<Types...>;
		static constexpr bool wrap = true;
	};
	template<class R, class... Types>
	struct _Is_function<R(Types...) &> {
		using bool_type = true_type;
		using result_type = R;
		using arg_types = arg_tuple<Types...>;
		static constexpr bool wrap = true;
	};
	template<class R, class... Types>
	struct _Is_function<R(Types...) const &> {
		using bool_type = true_type;
		using result_type = R;
		using arg_types = arg_tuple<Types...>;
		static constexpr bool wrap = true;
	};
	template<class R, class... Types>
	struct _Is_function<R(Types...) volatile &> {
		using bool_type = true_type;
		using result_type = R;
		using arg_types = arg_tuple<Types...>;
		static constexpr bool wrap = true;
	};
	template<class R, class... Types>
	struct _Is_function<R(Types...) const volatile &> {
		using bool_type = true_type;
		using result_type = R;
		using arg_types = arg_tuple<Types...>;
		static constexpr bool wrap = true;
	};
	template<class R, class... Types>
	struct _Is_function<R(Types...) &&> {
		using bool_type = true_type;
		using result_type = R;
		using arg_types = arg_tuple<Types...>;
		static constexpr bool wrap = true;
	};
	template<class R, class... Types>
	struct _Is_function<R(Types...) const &&> {
		using bool_type = true_type;
		using result_type = R;
		using arg_types = arg_tuple<Types...>;
		static constexpr bool wrap = true;
	};
	template<class R, class... Types>
	struct _Is_function<R(Types...) volatile &&> {
		using bool_type = true_type;
		using result_type = R;
		using arg_types = arg_tuple<Types...>;
		static constexpr bool wrap = true;
	};
	template<class R, class... Types>
	struct _Is_function<R(Types...) const volatile &&> {
		using bool_type = true_type;
		using result_type = R;
		using arg_types = arg_tuple<Types...>;
		static constexpr bool wrap = true;
	};
	template <class F>
	struct is_function : _Is_function<F>::bool_type {};
}

namespace iml {

	//同じ型か判定
	template<class, class>
	struct is_same : false_type {};
	template<class T>
	struct is_same<T, T> : true_type {};

	//メンバ関数のポインタの判定
	template<class>
	struct _Is_memfunptr {
		using bool_type = false_type;
	};
	template<class R, class Class, class... Types>
	struct _Is_memfunptr<R(Class::*)(Types...)> {
		using bool_type = true_type;
		using result_type = R;
		using class_type = Class;
	};
	template<class R, class Class, class... Types>
	struct _Is_memfunptr<R(Class::*)(Types...) const> {
		using bool_type = true_type;
		using result_type = R;
		using class_type = Class;
	};
	template<class R, class Class, class... Types>
	struct _Is_memfunptr<R(Class::*)(Types...) volatile> {
		using bool_type = true_type;
		using result_type = R;
		using class_type = Class;
	};
	template<class R, class Class, class... Types>
	struct _Is_memfunptr<R(Class::*)(Types...) const volatile> {
		using bool_type = true_type;
		using result_type = R;
		using class_type = Class;
	};
	template<class R, class Class, class... Types>
	struct _Is_memfunptr<R(Class::*)(Types...) &> {
		using bool_type = true_type;
		using result_type = R;
		using class_type = Class;
	};
	template<class R, class Class, class... Types>
	struct _Is_memfunptr<R(Class::*)(Types...) const &> {
		using bool_type = true_type;
		using result_type = R;
		using class_type = Class;
	};
	template<class R, class Class, class... Types>
	struct _Is_memfunptr<R(Class::*)(Types...) volatile &> {
		using bool_type = true_type;
		using result_type = R;
		using class_type = Class;
	};
	template<class R, class Class, class... Types>
	struct _Is_memfunptr<R(Class::*)(Types...) const volatile &> {
		using bool_type = true_type;
		using result_type = R;
		using class_type = Class;
	};
	template<class R, class Class, class... Types>
	struct _Is_memfunptr<R(Class::*)(Types...) && > {
		using bool_type = true_type;
		using result_type = R;
		using class_type = Class;
	};
	template<class R, class Class, class... Types>
	struct _Is_memfunptr<R(Class::*)(Types...) const &&> {
		using bool_type = true_type;
		using result_type = R;
		using class_type = Class;
	};
	template<class R, class Class, class... Types>
	struct _Is_memfunptr<R(Class::*)(Types...) volatile &&> {
		using bool_type = true_type;
		using result_type = R;
		using class_type = Class;
	};
	template<class R, class Class, class... Types>
	struct _Is_memfunptr<R(Class::*)(Types...) const volatile &&> {
		using bool_type = true_type;
		using result_type = R;
		using class_type = Class;
	};
	template<class T>
	struct is_member_function_pointer : _Is_memfunptr<typename remove_cv<T>::type>::bool_type {};

	//メンバオブジェクトのポインタの判定
	template<class T, bool = _Is_memfunptr<T>::bool_type::value>
	struct _Is_member_object_pointer {
		using bool_type = false_type;
	};
	template<class T1, class T2>
	struct _Is_member_object_pointer<T1 T2::*, false> {
		using bool_type = true_type;
		using class_type = T2;
	};
	template<class T>
	struct is_member_object_pointer : _Is_member_object_pointer<typename remove_cv<T>::type>::bool_type {};

	//int型引数が_Wrap_intより優先されるようにラップ
	struct _Wrap_int { _Wrap_int(int) {} };
	//Tを変更しないでoperator()をなし
	template<class T>
	struct _Identity {
		using type = T;
	};

	//constの追加
	template<class T>
	struct add_const {
		using type = const T;
	};
	//volatileを加える
	template<class T>
	struct add_volatile {
		using type = volatile T;
	};
	//cvを加える
	template<class T>
	struct add_cv {
		using type = const volatile T;
	};

	//参照の追加
	template<class T, bool = _Is_function<T>::wrap || is_void<T>::value>
	struct _Add_reference {
		using l_value = T;
		using r_value = T;
	};
	template<class T>
	struct _Add_reference<T, false> {
		using l_value = T & ;
		using r_value = T && ;
	};
	//左辺値参照の追加
	template<class T>
	struct add_lvalue_reference {
		using type = typename _Add_reference<T>::l_value;
	};
	//右辺値参照の追加
	template<class T>
	struct add_rvalue_reference {
		using type = typename _Add_reference<T>::r_value;
	};

	//型だけの取得
	template<class T>
	typename add_rvalue_reference<T>::type declval() noexcept;

	//ポインタの減算の結果型
	using ptrdiff_t = decltype(declval<int*>() - declval<int*>());

	//配列から次数の削除
	template<class T>
	struct remove_extent {
		using type = T;
	};
	template<class T, imsize_t N>
	struct remove_extent<T[N]> {
		using type = T;
	};
	template<class T>
	struct remove_extent<T[]> {
		using type = T;
	};

	//配列から全ての次数の削除
	template<class T>
	struct remove_all_extents {
		using type = T;
	};
	template<class T, imsize_t N>
	struct remove_all_extents<T[N]> {
		using type = typename remove_all_extents<T>::type;
	};
	template<class T>
	struct remove_all_extents<T[]> {
		using type = typename remove_all_extents<T>::type;
	};

	//ポインタの除去
	template<class T>
	struct remove_pointer {
		using type = T;
	};
	template<class T>
	struct remove_pointer<T*> {
		using type = T;
	};
	template<class T>
	struct remove_pointer<T* const> {
		using type = T;
	};
	template<class T>
	struct remove_pointer<T* volatile> {
		using type = T;
	};
	template<class T>
	struct remove_pointer<T* const volatile> {
		using type = T;
	};

	//ポインターの追加
	template<class T, bool = _Is_function<T>::wrap>
	struct _Add_pointer {
		using type = T;
	};
	template<class T>
	struct _Add_pointer<T, false> {
		using type = typename remove_reference<T>::type*;
	};
	template<class T>
	struct add_pointer {
		using type = typename _Add_pointer<T>::type;
	};

	//配列の判定
	template<class T>
	struct is_array : false_type {};
	template<class T, imsize_t N>
	struct is_array<T[N]> : true_type {};
	template<class T>
	struct is_array<T[]> : true_type {};

	//左辺値の判定
	template<class T>
	struct is_lvalue_reference : false_type {};
	template<class T>
	struct is_lvalue_reference<T&> : true_type {};

	//右辺値の判定
	template<class T>
	struct is_rvalue_reference : false_type {};
	template<class T>
	struct is_rvalue_reference<T&&> : true_type {};

	//参照型の判定
	template<class T>
	struct is_reference : _Cat_base<is_lvalue_reference<T>::value || is_rvalue_reference<T>::value> {};

	//ポインタの判定(メンバポインタではない)
	template<class T>
	struct _Is_pointer : false_type {};
	template<class T>
	struct _Is_pointer<T*> : _Cat_base<!is_member_object_pointer<T*>::value && !is_member_function_pointer<T*>::value> {};
	template<class T>
	struct is_pointer : _Is_pointer<typename remove_cv<T>::type> {};
	//nullポインタの判定
	template<class T>
	struct is_null_pointer : _Cat_base<is_same<typename remove_cv<T>::type, nullptr_t>::value> {};


	//関数ポインタの判定
	template <class T>
	struct is_function_pointer
		: conditional<is_pointer<T>::value && is_function<typename remove_pointer<T>::type>::value, true_type, false_type >::type {};


	//クラスか共用体の判定
	template <class T>
	struct _Is_class_or_union {
		template <class _T> static true_type tester(void(_T::*)(void));
		template <class _T> static false_type tester(...);

		using type = decltype(tester<T>(nullptr));
	};
	template <class T>
	struct is_class_or_union : _Is_class_or_union<T>::type {};
	//共用体の判定
	template<class T>
	struct is_union : _Cat_base<__is_union(T)> {};
	//クラスの判定
	template <class T>
	struct is_class : _Cat_base<is_class_or_union<T>::value && !is_union<T>::value> {};


	//基本型の判定
	template<class T>
	struct is_fundamental : _Cat_base<is_arithmetic<T>::value || is_void<T>::value || is_null_pointer<T>::value> {};

	//オブジェクト型の判定
	template<class T>
	struct is_object : _Cat_base<!is_function<T>::value && !is_reference<T>::value && !is_void<T>::value> {};

}

#include "IMathLib/utility/type_traits/is_convertible.hpp"

namespace iml {
	
	//列挙型の判定
	template <class T>
	struct is_enum : _Cat_base<!is_arithmetic<T>::value && !is_reference<T>::value && !is_function<T>::value && !is_class_or_union<T>::value && !is_array<T>::value> {};

	//メンバへのポインタの判定
	template <class T>
	struct is_member_pointer : _Cat_base<is_member_object_pointer<T>::value || is_member_function_pointer<T>::value> {};

	//スカラー型の判定
	template <class T>
	struct is_scalar : _Cat_base<is_arithmetic<T>::value || is_enum<T>::value || is_pointer<T>::value || is_member_pointer<T>::value || is_null_pointer<T>::value> {};
	
	//const修飾の判定
	template <class T>
	struct is_const : false_type {};
	template<class T>
	struct is_const<const T> : true_type {};

	//volatile修飾の判定
	template<class T>
	struct is_volatile : false_type {};
	template<class T>
	struct is_volatile<volatile T> : true_type {};

	//関数が呼び出し可能かの判定
	template <class F, class... Types>
	struct _Is_callable {
	private:
		template <class _F> static auto tester(_F) -> decltype(declval<_F>()(declval<Types>()...), true_type());
		template <class _F> static false_type tester(...);
	public:
		using type = decltype(tester<F>(declval<F>()));
	};
	template <class F, class... Types>
	struct is_callable : _Is_callable<F, Types...>::type {};


	//アドレスを返す
	template <class T, bool>
	struct _Addressof {
		//関数のアドレス
		static constexpr T* __addressof(T& val) noexcept { return val; }
	};
	template <class T>
	struct _Addressof<T, false> {
		//オブジェクトのアドレス
		static constexpr T* __addressof(T& val) noexcept {
			return (reinterpret_cast<T*>(&const_cast<char&>(reinterpret_cast<const volatile char&>(val))));
		}
	};
	template<class T>
	inline constexpr T* addressof(T& val) noexcept {
		return _Addressof<T, is_function<T>::value>::__addressof(val);
	}
	template <class T>
	constexpr T* addressof(const T&&) = delete;

	
	//BaseがDerivedの基底クラスであるかの判定
	template <class Base, class Derived>
	struct _Is_base_of {
		using ncvB = typename remove_cv<Base>::type;
		using ncvD = typename remove_cv<Derived>::type;
		//クラスであり同じ型かポインタが暗黙的に変換可能か
		static constexpr bool value =
			(is_same<ncvB, ncvD>::value && is_class<ncvB>::value) ||
			(is_class<ncvB>::value && is_class<ncvD>::value && is_convertible<ncvD*, ncvB*>::value);
	};
	template <class Base, class Derived>
	struct is_base_of : _Cat_base<_Is_base_of<Base, Derived>::value> {};
	template <class Base, class Derived>
	struct is_base_of<Base, Derived&> : false_type {};
	template <class Base, class Derived>
	struct is_base_of<Base&, Derived&> : false_type {};
	template <class Base, class Derived>
	struct is_base_of<Base&, Derived> : false_type {};
	

	//配列と関数ポインタに関して関数テンプレートと同様に推論された型を取得(配列ならポインタに、関数型なら関数ポインタに)
	template<class T>
	struct decay {
		using T1 = typename remove_reference<T>::type;

		using type = typename conditional<is_array<T1>::value, typename remove_extent<T1>::type *,
			typename conditional<is_function<T1>::value, typename add_pointer<T1>::type, typename remove_cv<T1>::type>::type>::type;
	};

	//有効なパラメータのテスト(パラメータが存在するかのテスト)
	template<class...>
	struct _Param_tester {
		using type = void;
	};
	template<class... Types>
	using void_t = typename _Param_tester<Types...>::type;

	//オブジェクトのメンバ関数へのポインタの呼び出し
	struct _Invoker_pmf_object {
		template<class Decayed, class T, class... Types>
		static auto _Call(Decayed pmf, T&& arg, Types&&... args) -> decltype((iml::_Forward<T>(arg).*pmf)(iml::_Forward<Types>(args)...)) {
			return ((iml::_Forward<T>(arg).*pmf)(iml::_Forward<Types>(args)...));
		}
	};
	//スマートポインタでオブジェクトのメンバ関数へのポインタの呼び出し
	struct _Invoker_pmf_pointer {
		template<class Decayed, class T, class... Types>
		static auto _Call(Decayed pmf, T&& arg, Types&&... args) -> decltype(((*iml::_Forward<T>(arg)).*pmf)(iml::_Forward<Types>(args)...)) {
			return (((*iml::_Forward<T>(arg)).*pmf)(iml::_Forward<Types>(args)...));
		}
	};
	//オブジェクトのメンバデータへのポインタの呼び出し
	struct _Invoker_pmd_object {
		template<class Decayed, class T>
		static auto _Call(Decayed pmd, T&& arg) -> decltype(iml::_Forward<T>(arg).*pmd) {
			return (iml::_Forward<T>(arg).*pmd);
		}
	};
	//スマートポインタでオブジェクトのメンバデータへのポインタの呼び出し
	struct _Invoker_pmd_pointer {
		template<class Decayed, class T>
		static auto _Call(Decayed pmd, T&& arg) -> decltype((*iml::_Forward<T>(arg)).*pmd) {
			return ((*iml::_Forward<T>(arg)).*pmd);
		}
	};
	//関数オブジェクトの呼び出し
	struct _Invoker_functor {
		template<class Callable, class... Types>
		static auto _Call(Callable&& obj, Types&&... args) -> decltype(iml::_Forward<Callable>(obj)(iml::_Forward<Types>(args)...)) {
			return (iml::_Forward<Callable>(obj)(iml::_Forward<Types>(args)...));
		}
	};

	template <class Callable, class T, class Decayed = typename decay<Callable>::type,
		bool _Is_pmf = is_member_function_pointer<Decayed>::value, bool _Is_pmd = is_member_object_pointer<Decayed>::value>
		struct _Invoker1;
	//メンバ関数へのポインタ
	template <class Callable, class T, class Decayed>
	struct _Invoker1<Callable, T, Decayed, true, false> :
		conditional<is_base_of<typename _Is_memfunptr<Decayed>::Class_type, typename decay<T>::type>::value,
		_Invoker_pmf_object, _Invoker_pmf_pointer>::type {};
	//メンバデータへのポインタ
	template <class Callable, class T, class Decayed>
	struct _Invoker1<Callable, T, Decayed, false, true> :
		conditional<is_base_of<typename _Is_member_object_pointer<Decayed>::class_type, typename decay<T>::type>::value,
		_Invoker_pmd_object, _Invoker_pmd_pointer>::type {};

	//関数オブジェクト
	template <class Callable, class T, class Decayed>
	struct _Invoker1<Callable, T, Decayed, false, false> : _Invoker_functor {};

	template <class Callable, class... Types>
	struct _Invoker;
	//引数0
	template <class Callable>
	struct _Invoker<Callable> : _Invoker_functor {};
	//1以上の引数
	template<class Callable, class T, class... Types>
	struct _Invoker<Callable, T, Types...> : _Invoker1<Callable, T> {};

	//呼び出し可能なオブジェクトを呼び出す
	template<class Callable, class... Types>
	inline auto invoke(Callable&& obj, Types&&... args) -> decltype(_Invoker<Callable, Types...>::_Call(iml::_Forward<Callable>(obj), iml::_Forward<Types>(args)...)) {
		return (_Invoker<Callable, Types...>::_Call(iml::_Forward<Callable>(obj), iml::_Forward<Types>(args)...));
	}

	//デフォルト
	template <class T, class = void>
	struct _Weak_result_type {};
	//T::result_typeが存在する
	template <class T>
	struct _Weak_result_type<T, void_t<typename T::result_type>> {
		using result_type = typename T::result_type;
	};

	//デフォルト
	template <class T, class = void>
	struct _Weak_argument_type : _Weak_result_type<T> {};
	//T::argument_typeが存在する
	template <class T>
	struct _Weak_argument_type<T, void_t<typename T::argument_type>> : _Weak_result_type<T> {
		using argument_type = typename T::argument_type;
	};

	//デフォルト
	template <class T, class = void>
	struct _Weak_binary_args : _Weak_argument_type<T> {};
	//両方の型が存在する
	template <class T>
	struct _Weak_binary_args<T, void_t<typename T::first_argument_type, typename T::second_argument_type>> : _Weak_argument_type<T> {
		typedef typename T::first_argument_type first_argument_type;
		typedef typename T::second_argument_type second_argument_type;
	};

	//ネストされた型の提供
	template <class T>
	struct _Weak_types {
		using _Is_f_or_pf = is_function<typename remove_pointer<T>::type>;
		using _Is_pmf = _Is_memfunptr<typename remove_cv<T>::type>;
		using type = typename conditional<_Is_f_or_pf::bool_type::value, _Is_f_or_pf,
			typename conditional<_Is_pmf::bool_type::value, _Is_pmf,
			_Weak_binary_args<T>>::type>::type;
	};

	//割り当て可能な参照のために待機
	template <class T>
	class reference_wrapper : public _Weak_types<T>::type {
		T *ptr;
	public:
		static_assert(is_object<T>::value || is_function<T>::value,
			"reference_wrapper<T> requires T to be an object type or a function type.");
		using type = T;

		reference_wrapper(T& val) noexcept: ptr(std::addressof(val)) {}

		//リファレンスの取得
		operator T&() const noexcept { return *ptr; }
		T& get() const noexcept { return *ptr; }
		//オブジェクトの呼び出し
		template<class... Types>
		auto operator()(Types&&... args) const -> decltype(invoke(get(), iml::_Forward<Types>(args)...)) {
			return invoke(get(), _Forward<Types>(args)...);
		}
		reference_wrapper(T&&) = delete;
	};

	//reference_wrapper<T>の作成
	template<class T>
	inline reference_wrapper<T> ref(T& val) noexcept {
		return reference_wrapper<T>(val);
	}
	template<class T>
	inline reference_wrapper<T> ref(reference_wrapper<T> val) noexcept {
		return (ref(val.get()));
	}
	template<class T>
	void ref(const T&&) = delete;
	template<class T>
	inline reference_wrapper<const T> cref(const T& _Val) noexcept {
		return (reference_wrapper<const T>(_Val));
	}
	template<class T>
	inline reference_wrapper<const T> cref(reference_wrapper<T> val) noexcept {
		return (cref(val.get()));
	}
	template<class T>
	void cref(const T&&) = delete;

	//reference_wrapperでないときそのまま
	template<class T>
	struct _Unrefwrap_helper {
		using type = T;
		static constexpr bool _Is_refwrap = false;
	};
	//reference_wrapperから参照の作成
	template<class T>
	struct _Unrefwrap_helper<reference_wrapper<T>> {
		using type = T&;
		static constexpr bool _Is_refwrap = true;
	};
	//reference_wrapperのラップの解除
	template<class T>
	struct _Unrefwrap {
		typedef typename decay<T>::type _Ty1;
		typedef typename _Unrefwrap_helper<_Ty1>::type type;
		static constexpr bool _Is_refwrap = _Unrefwrap_helper<_Ty1>::_Is_refwrap;
	};
}

#endif