﻿#ifndef IMATH_MATH_MATH_MATH_TRAITS_HPP
#define IMATH_MATH_MATH_MATH_TRAITS_HPP

#include "IMathLib/IMathLib_config.hpp"
#include "IMathLib/utility/utility.hpp"
#include "IMathLib/math/math/numeric_traits.hpp"

//デフォルトで計算される型
#ifndef IMATH_DEFAULT_TYPE
#define IMATH_DEFAULT_TYPE		double
#endif // IMATH_DEFAULT_TYPE

//ベルヌーイ数の用意されるテーブル数(それを用いた関数の精度もこれに依存する)
#ifndef IMATH_BERNOULLI_NUMBER_TABLE
#define IMATH_BERNOULLI_NUMBER_TABLE		40
#endif // IMATH_DEFAULT_TYPE


namespace iml {

	//From⊂Toかの判定(From→Toできるか)
	template <class From, class To, bool = (is_arithmetic_v<From> && is_arithmetic_v<To>)>
	struct is_inclusion_impl : is_convertible<From, To> {};
	//どちらかにvoidがあればfalse
	template <class To>
	struct is_inclusion_impl<void, To, false> : false_type {};
	template <class From>
	struct is_inclusion_impl<From, void, false> : false_type {};
	//算術型の場合(表現桁数に関係なくN⊂Z⊂Rとして扱う)
	template <class From, class To>
	struct is_inclusion_impl<From, To, true> :
		cat_bool<
		(is_unsigned_v<From> && is_signed_v<To>) ||					//自然数型から整数型
		(is_unsigned_v<From> && is_floating_point_v<To>) ||			//自然数型から実数型
		(is_signed_v<From> && is_floating_point_v<To>) ||			//整数型から実数型
		//以下同じ代数的構造での表現可能な数値による分類(Fromの方が表現可能な数値がTo以下)
		(is_unsigned_v<From> && is_unsigned_v<To> && (numeric_traits<From>::max() <= numeric_traits<To>::max())) ||
		(is_signed_v<From> && is_signed_v<To> && (numeric_traits<From>::max() <= numeric_traits<To>::max())) ||
		(is_floating_point_v<From> && is_floating_point_v<To> && (numeric_traits<From>::max() <= numeric_traits<To>::max()))
		> {};
	template <class From, class To>
	struct is_inclusion : is_inclusion_impl<decay_t<From>, decay_t<To>> {};
	template <class From, class To>
	constexpr bool is_inclusion_v = is_inclusion<From, To>::value;


	//包含関係で上位のものを選択(選択できなければvoid)
	template <class T1, class T2>
	struct inclusion_superior {
		using type = conditional_t<is_inclusion_v<T1, T2>, T2
			, conditional_t<is_inclusion_v<T2, T1>, T1, void>>;
	};
	template <class T1, class T2>
	using inclusion_superior_t = typename inclusion_superior<T1, T2>::type;


	//任意の2つの型の演算可能テスト
	template <class T1, class T2, bool = (is_arithmetic_v<T1> && is_arithmetic_v<T2>)>
	struct is_calcable_impl {
		using type = inclusion_superior_t<T1, T2>;

		static constexpr bool add_value = true;
		static constexpr bool sub_value = is_signed_v<type>;
		static constexpr bool mul_value = true;
		static constexpr bool div_value = is_floating_point_v<type>;
		static constexpr bool add_eq_value = is_inclusion_v<type, T1>;
		static constexpr bool sub_eq_value = is_inclusion_v<type, T1> && is_signed_v<type>;
		static constexpr bool mul_eq_value = is_inclusion_v<type, T1>;
		static constexpr bool div_eq_value = is_inclusion_v<type, T1> && is_floating_point_v<T1>;
		static constexpr bool eq_value = true;
		static constexpr bool lt_value = true;
		static constexpr bool leq_value = true;
		static constexpr bool gt_value = true;
		static constexpr bool geq_value = true;
	};
	template <class T1, class T2>
	struct is_calcable_impl<T1, T2, false> {
	private:
		template <class S1, class S2> static auto add_tester(S1*, S2*) -> decltype(declval<S1>() + declval<S2>(), true_type());
		template <class S1, class S2> static auto sub_tester(S1*, S2*) -> decltype(declval<S1>() - declval<S2>(), true_type());
		template <class S1, class S2> static auto mul_tester(S1*, S2*) -> decltype(declval<S1>() * declval<S2>(), true_type());
		template <class S1, class S2> static auto div_tester(S1*, S2*) -> decltype(declval<S1>() / declval<S2>(), true_type());
		template <class S1, class S2> static auto add_eq_tester(S1*, S2*) -> decltype(declval<S1>() += declval<S2>(), true_type());
		template <class S1, class S2> static auto sub_eq_tester(S1*, S2*) -> decltype(declval<S1>() -= declval<S2>(), true_type());
		template <class S1, class S2> static auto mul_eq_tester(S1*, S2*) -> decltype(declval<S1>() *= declval<S2>(), true_type());
		template <class S1, class S2> static auto div_eq_tester(S1*, S2*) -> decltype(declval<S1>() /= declval<S2>(), true_type());
		template <class S1, class S2> static auto eq_tester(S1*, S2*) -> decltype(declval<S1>() == declval<S2>(), true_type());
		template <class S1, class S2> static auto lt_tester(S1*, S2*) -> decltype(declval<S1>() < declval<S2>(), true_type());
		template <class S1, class S2> static auto leq_tester(S1*, S2*) -> decltype(declval<S1>() <= declval<S2>(), true_type());
		template <class S1, class S2> static auto gt_tester(S1*, S2*) -> decltype(declval<S1>() > declval<S2>(), true_type());
		template <class S1, class S2> static auto geq_tester(S1*, S2*) -> decltype(declval<S1>() >= declval<S2>(), true_type());

		template <class S1, class S2> static false_type add_tester(...);
		template <class S1, class S2> static false_type sub_tester(...);
		template <class S1, class S2> static false_type mul_tester(...);
		template <class S1, class S2> static false_type div_tester(...);
		template <class S1, class S2> static false_type add_eq_tester(...);
		template <class S1, class S2> static false_type sub_eq_tester(...);
		template <class S1, class S2> static false_type mul_eq_tester(...);
		template <class S1, class S2> static false_type div_eq_tester(...);
		template <class S1, class S2> static false_type eq_tester(...);
		template <class S1, class S2> static false_type lt_tester(...);
		template <class S1, class S2> static false_type leq_tester(...);
		template <class S1, class S2> static false_type gt_tester(...);
		template <class S1, class S2> static false_type geq_tester(...);
	public:
		static constexpr bool add_value = decltype(add_tester<T1, T2>(nullptr, nullptr))::value;
		static constexpr bool sub_value = decltype(sub_tester<T1, T2>(nullptr, nullptr))::value;
		static constexpr bool mul_value = decltype(mul_tester<T1, T2>(nullptr, nullptr))::value;
		static constexpr bool div_value = decltype(div_tester<T1, T2>(nullptr, nullptr))::value;
		static constexpr bool add_eq_value = decltype(add_eq_tester<T1, T2>(nullptr, nullptr))::value;
		static constexpr bool sub_eq_value = decltype(sub_eq_tester<T1, T2>(nullptr, nullptr))::value;
		static constexpr bool mul_eq_value = decltype(mul_eq_tester<T1, T2>(nullptr, nullptr))::value;
		static constexpr bool div_eq_value = decltype(div_eq_tester<T1, T2>(nullptr, nullptr))::value;
		static constexpr bool eq_value = decltype(eq_tester<T1, T2>(nullptr, nullptr))::value;
		static constexpr bool lt_value = decltype(lt_tester<T1, T2>(nullptr, nullptr))::value;
		static constexpr bool leq_value = decltype(leq_tester<T1, T2>(nullptr, nullptr))::value;
		static constexpr bool gt_value = decltype(gt_tester<T1, T2>(nullptr, nullptr))::value;
		static constexpr bool geq_value = decltype(geq_tester<T1, T2>(nullptr, nullptr))::value;
	};
	template <class T1, class T2>
	struct is_calcable : is_calcable_impl<decay_t<T1>, decay_t<T2>> {};


	//任意の2つの型の演算テスト(演算不可の場合はvoid型)
	template <class T1, class T2>
	struct calculation_result_impl {
	private:
		template <bool>
		struct tester {
			template <class S1, class S2> static auto add_tester() -> decltype(declval<S1>() + declval<S2>());
			template <class S1, class S2> static auto sub_tester() -> decltype(declval<S1>() - declval<S2>());
			template <class S1, class S2> static auto mul_tester() -> decltype(declval<S1>() * declval<S2>());
			template <class S1, class S2> static auto div_tester() -> decltype(declval<S1>() / declval<S2>());
		};
		template <>
		struct tester<false> {
			template <class S1, class S2> static void add_tester();
			template <class S1, class S2> static void sub_tester();
			template <class S1, class S2> static void mul_tester();
			template <class S1, class S2> static void div_tester();
		};
	public:
		using add_type = decltype(tester<is_calcable<T1, T2>::add_value>::add_tester<T1, T2>());
		using sub_type = decltype(tester<is_calcable<T1, T2>::sub_value>::sub_tester<T1, T2>());
		using mul_type = decltype(tester<is_calcable<T1, T2>::mul_value>::mul_tester<T1, T2>());
		using div_type = decltype(tester<is_calcable<T1, T2>::div_value>::div_tester<T1, T2>());
	};
	template <class T1, class T2>
	struct calculation_result : calculation_result_impl<T1, T2> {};
	template <class T1, class T2>
	using add_result_t = typename calculation_result<T1, T2>::add_type;
	template <class T1, class T2>
	using sub_result_t = typename calculation_result<T1, T2>::sub_type;
	template <class T1, class T2>
	using mul_result_t = typename calculation_result<T1, T2>::mul_type;
	template <class T1, class T2>
	using div_result_t = typename calculation_result<T1, T2>::div_type;


	//T1×T2→T3となるかの判定
	template <class T1, class T2, class T3>
	struct is_operation {
		static constexpr bool add_value = is_inclusion_v<typename calculation_result<T1, T2>::add_type, T3>;
		static constexpr bool sub_value = is_inclusion_v<typename calculation_result<T1, T2>::sub_type, T3>;
		static constexpr bool mul_value = is_inclusion_v<typename calculation_result<T1, T2>::mul_type, T3>;
		static constexpr bool div_value = is_inclusion_v<typename calculation_result<T1, T2>::div_type, T3>;
	};


	//加法の特性
	template<class>
	struct addition_traits;
	//符号無し整数
#define IMATH_ADDITION_TRAITS(TYPE)\
	template<>\
	struct addition_traits<TYPE> {\
		/*単位元*/\
		static constexpr TYPE identity_element() { return 0; }\
		/*結合律*/\
		static constexpr bool associative_value = true;\
		/*消約律*/\
		static constexpr bool cancellative_value = false;\
		/*可換律*/\
		static constexpr bool commutative_value = true;\
	};
	IMATH_ADDITION_TRAITS(bool);
	IMATH_ADDITION_TRAITS(unsigned char);
	IMATH_ADDITION_TRAITS(unsigned short);
	IMATH_ADDITION_TRAITS(unsigned int);
	IMATH_ADDITION_TRAITS(unsigned long);
	IMATH_ADDITION_TRAITS(char16_t);
	IMATH_ADDITION_TRAITS(char32_t);
	IMATH_ADDITION_TRAITS(unsigned long long);
#undef IMATH_ADDITION_TRAITS
	//符号あり数値
#define IMATH_ADDITION_TRAITS(TYPE)\
	template<>\
	struct addition_traits<TYPE> {\
		/*単位元*/\
		static constexpr TYPE identity_element() { return 0; }\
		/*結合律*/\
		static constexpr bool associative_value = true;\
		/*消約律*/\
		static constexpr bool cancellative_value = true;\
		/*可換律*/\
		static constexpr bool commutative_value = true;\
	};
	IMATH_ADDITION_TRAITS(char);
	IMATH_ADDITION_TRAITS(wchar_t);
	IMATH_ADDITION_TRAITS(short);
	IMATH_ADDITION_TRAITS(int);
	IMATH_ADDITION_TRAITS(long);
	IMATH_ADDITION_TRAITS(long long);
	IMATH_ADDITION_TRAITS(float);
	IMATH_ADDITION_TRAITS(double);
	IMATH_ADDITION_TRAITS(long double);
#undef IMATH_ADDITION_TRAITS

	//乗法の特性
	template <class T>
	struct multiplication_traits;
	//整数
#define IMATH_MULTIPLICATIVE_TRAITS(TYPE)\
	template<>\
	struct multiplication_traits<TYPE> {\
		/*単位元*/\
		static constexpr TYPE identity_element() { return 1; }\
		/*吸収元(唯一の場合は加法単位元を返す)*/\
		static constexpr TYPE absorbing_element() { return 0; }\
		/*結合律*/\
		static constexpr bool associative_value = true;\
		/*消約律*/\
		static constexpr bool cancellative_value = false;\
		/*可換律*/\
		static constexpr bool commutative_value = true;\
		/*分配律*/\
		static constexpr bool distributive_value = true;\
	};
	IMATH_MULTIPLICATIVE_TRAITS(bool);
	IMATH_MULTIPLICATIVE_TRAITS(unsigned char);
	IMATH_MULTIPLICATIVE_TRAITS(unsigned short);
	IMATH_MULTIPLICATIVE_TRAITS(unsigned int);
	IMATH_MULTIPLICATIVE_TRAITS(unsigned long);
	IMATH_MULTIPLICATIVE_TRAITS(char16_t);
	IMATH_MULTIPLICATIVE_TRAITS(char32_t);
	IMATH_MULTIPLICATIVE_TRAITS(unsigned long long);
	IMATH_MULTIPLICATIVE_TRAITS(char);
	IMATH_MULTIPLICATIVE_TRAITS(wchar_t);
	IMATH_MULTIPLICATIVE_TRAITS(short);
	IMATH_MULTIPLICATIVE_TRAITS(int);
	IMATH_MULTIPLICATIVE_TRAITS(long);
	IMATH_MULTIPLICATIVE_TRAITS(long long);
#undef IMATH_MULTIPLICATIVE_TRAITS
	//浮動小数点
#define IMATH_MULTIPLICATIVE_TRAITS(TYPE)\
	template<>\
	struct multiplication_traits<TYPE> {\
		/*単位元*/\
		static constexpr TYPE identity_element() { return 1; }\
		/*吸収元*/\
		static constexpr TYPE absorbing_element() { return 0; }\
		/*結合律*/\
		static constexpr bool associative_value = true;\
		/*消約律*/\
		static constexpr bool cancellative_value = true;\
		/*可換律*/\
		static constexpr bool commutative_value = true;\
		/*分配律*/\
		static constexpr bool distributive_value = true;\
	};
	IMATH_MULTIPLICATIVE_TRAITS(float);
	IMATH_MULTIPLICATIVE_TRAITS(double);
	IMATH_MULTIPLICATIVE_TRAITS(long double);
#undef IMATH_MULTIPLICATIVE_TRAITS

	//単位元が存在する
	template <class T>
	struct is_exist_identity_element {
	private:
		template <class S> static auto tester(S*) ->decltype(S::identity_element(), true_type());
		template <class S> static false_type tester(...);
	public:
		using add_type = decltype(tester<addition_traits<T>>(nullptr));
		using mul_type = decltype(tester<multiplication_traits<T>>(nullptr));
	};
	//加法単位元が存在する
	template <class T>
	struct is_exist_additive_identity : is_exist_identity_element<T>::add_type {};
	template <class T>
	constexpr bool is_exist_additive_identity_v = is_exist_additive_identity<T>::value;
	//加法単位元の判定
	template <class T>
	inline constexpr bool is_additive_identity(const T& x) { return x == addition_traits<T>::identity_element(); }
	//乗法単位元が存在する
	template <class T>
	struct is_exist_multiplicative_identity : is_exist_identity_element<T>::mul_type {};
	template <class T>
	constexpr bool is_exist_multiplicative_identity_v = is_exist_multiplicative_identity<T>::value;
	//乗法単位元の判定
	template <class T>
	inline constexpr bool is_multiplicative_identity(const T& x) { return x == multiplication_traits<T>::identity_element(); }


	//可逆元ならば逆元を返す(存在しない場合は例外を出す)
	//可逆元の存在しない代数系では定義しない
	template <class>
	struct Inverse_element;
	//符号なし整数
#define IMATH_INVERSE_ELEMENT(TYPE)\
	template <> struct Inverse_element<TYPE> {};
	IMATH_INVERSE_ELEMENT(bool);
	IMATH_INVERSE_ELEMENT(unsigned char);
	IMATH_INVERSE_ELEMENT(unsigned short);
	IMATH_INVERSE_ELEMENT(unsigned int);
	IMATH_INVERSE_ELEMENT(unsigned long);
	IMATH_INVERSE_ELEMENT(char16_t);
	IMATH_INVERSE_ELEMENT(char32_t);
	IMATH_INVERSE_ELEMENT(unsigned long long);
#undef IMATH_INVERSE_ELEMENT
	//符号あり整数
#define IMATH_INVERSE_ELEMENT(TYPE)\
	template <>\
	struct Inverse_element<TYPE> {\
		static constexpr TYPE _additive_inverse_(TYPE x) { return -x; }\
	};
	IMATH_INVERSE_ELEMENT(char);
	IMATH_INVERSE_ELEMENT(wchar_t);
	IMATH_INVERSE_ELEMENT(short);
	IMATH_INVERSE_ELEMENT(int);
	IMATH_INVERSE_ELEMENT(long);
	IMATH_INVERSE_ELEMENT(long long);
#undef IMATH_INVERSE_ELEMENT
	//浮動小数点
#define IMATH_INVERSE_ELEMENT(TYPE)\
	template <>\
	struct Inverse_element<TYPE> {\
		static constexpr TYPE _additive_inverse_(TYPE x) { return -x; }\
		static constexpr TYPE _multiplicative_inverse_(TYPE x) { return 1 / x; }\
	};
	IMATH_INVERSE_ELEMENT(float);
	IMATH_INVERSE_ELEMENT(double);
	IMATH_INVERSE_ELEMENT(long double);
#undef IMATH_INVERSE_ELEMENT
	//加法逆元の取得
	template <class T>
	inline constexpr auto additive_inverse(const T& x) { return Inverse_element<T>::_additive_inverse_(x); }
	//乗法逆元の取得
	template <class T>
	inline constexpr auto multiplicative_inverse(const T& x) { return Inverse_element<T>::_multiplicative_inverse_(x); }


	//逆元が存在する(乗法については吸収元以外で逆元が存在)(逆演算が存在する)
	template <class T>
	struct is_exist_inverse_element {
	private:
		template <class S> static auto add_tester(S*) -> decltype(-declval<S>(), true_type());
		template <class S> static false_type add_tester(...);
		template <class S1, class S2> static auto mul_tester(S1*, S2*) -> decltype(S1::identity_element() / declval<S2>(), true_type());
		template <class S1, class S2> static false_type mul_tester(...);
	public:
		using add_type = decltype(add_tester<T>(nullptr));
		using mul_type = decltype(mul_tester<multiplication_traits<T>, T>(nullptr, nullptr));
	};
	//加法逆元が存在する
	template <class T>
	struct is_exist_additive_inverse : is_exist_inverse_element<T>::add_type {};
	template <class T>
	constexpr bool is_exist_additive_inverse_v = is_exist_additive_inverse<T>::value;
	//乗法逆元が存在する
	template <class T>
	struct is_exist_multiplicative_inverse : is_exist_inverse_element<T>::mul_type {};
	template <class T>
	constexpr bool is_exist_multiplicative_inverse_v = is_exist_multiplicative_inverse<T>::value;


	//(唯一の)吸収元が存在する
	template <class T>
	struct is_exist_absorbing_element_impl {
	private:
		template <class S> static auto tester(S*) ->decltype(S::absorbing_element(), true_type());
		template <class S> static false_type tester(...);
	public:
		using type = decltype(tester<multiplication_traits<T>>(nullptr));
	};
	template <class T>
	struct is_exist_absorbing_element : is_exist_absorbing_element_impl<T>::type {};
	template <class T>
	constexpr bool is_exist_absorbing_element_v = is_exist_absorbing_element<T>::value;
	//(唯一の)吸収元の判定
	template <class T>
	inline constexpr bool is_absorbing_element(const T& x) { return x == multiplication_traits<T>::absorbing_element(); }


	//代数的構造上の代数的構造である(algebraic_typeが内部で定義されている)ことの判定
	template <class, class = void>
	struct is_algebraic_structure : false_type {};
	template <class T>
	struct is_algebraic_structure<T, void_t<typename T::algebraic_type>> : true_type {};
	template <class T>
	constexpr bool is_algebraic_structure_v = is_algebraic_structure<T>::value;


	//マグマ(亜群)の判定(閉じた演算であるかの判定)
	template <class T>
	struct is_magma {
		static constexpr bool add_value = is_operation<T, T, T>::add_value;
		static constexpr bool mul_value = is_operation<T, T, T>::add_value;
	};
	//半群の判定(結合律を満たすマグマ)
	template <class T>
	struct is_semi_group{
		static constexpr bool add_value = is_magma<T>::add_value && addition_traits<T>::associative_value;
		static constexpr bool mul_value = is_magma<T>::mul_value && multiplication_traits<T>::associative_value;
	};
	//準群の判定(消約律を満たすマグマ)
	template <class T>
	struct is_quasi_group {
		static constexpr bool add_value = is_magma<T>::add_value && addition_traits<T>::cancellative_value;
		static constexpr bool mul_value = is_magma<T>::mul_value && multiplication_traits<T>::cancellative_value;
	};
	//ループ(擬群)の判定(逆元の存在するマグマもしくは単位元の存在する準群)
	template <class T>
	struct is_loop {
		static constexpr bool add_value = is_magma<T>::add_value && is_exist_additive_inverse_v<T>;
		static constexpr bool mul_value = is_magma<T>::mul_value && is_exist_multiplicative_inverse_v<T>;
	};
	//モノイドの判定(単位元の存在する半群)
	template <class T>
	struct is_monoid {
		static constexpr bool add_value = is_semi_group<T>::add_value && is_exist_additive_identity_v<T>;
		static constexpr bool mul_value = is_semi_group<T>::mul_value && is_exist_multiplicative_identity_v<T>;
	};
	//群の判定(逆元の存在する半群もしくはモノイド)
	template <class T>
	struct is_group {
		static constexpr bool add_value = is_semi_group<T>::add_value && is_exist_additive_inverse_v<T>;
		static constexpr bool mul_value = is_semi_group<T>::mul_value && is_exist_multiplicative_inverse_v<T>;
	};
	//アーベル群(可換群)の判定
	template <class T>
	struct is_abelian_group {
		static constexpr bool add_value = is_group<T>::add_value && addition_traits<T>::commutative_value;
		static constexpr bool mul_value = is_group<T>::mul_value && multiplication_traits<T>::commutative_value;
	};
	//環の判定(加法についてアーベル群で乗法について半群)
	template <class T>
	struct is_ring : bool_constant<is_abelian_group<T>::add_value && is_semi_group<T>::mul_value> {};
	template <class T>
	constexpr bool is_ring_v = is_ring<T>::value;
	//単位環の判定(乗法単位元の存在する環)
	template <class T>
	struct is_unitary_ring : bool_constant<is_ring_v<T> && is_exist_multiplicative_identity_v<T>> {};
	template <class T>
	constexpr bool is_unitary_ring_v = is_unitary_ring<T>::value;
	//可換環の判定(乗法について可換な環)
	template <class T>
	struct is_commutative_ring : bool_constant<is_ring_v<T> && multiplication_traits<T>::commutative_value> {};
	template <class T>
	constexpr bool is_commutative_ring_v = is_commutative_ring<T>::value;
	//整域の判定(零因子の存在しない可換環)
	template <class T>
	struct is_integral_domain : bool_constant<is_commutative_ring_v<T> && is_exist_absorbing_element<T>::mul_value> {};
	template <class T>
	constexpr bool is_integral_domain_v = is_integral_domain<T>::value;
	//斜体の判定(乗法逆元の存在する単位環)
	template <class T>
	struct is_skew_field : bool_constant<is_unitary_ring_v<T> && is_exist_multiplicative_inverse_v<T>> {};
	template <class T>
	constexpr bool is_skew_field_v = is_skew_field<T>::value;
	//可換体の判定(乗法について可換な斜体)
	template <class T>
	struct is_commutative_field : bool_constant<is_skew_field_v<T> && multiplication_traits<T>::commutative_value> {};
	template <class T>
	constexpr bool is_commutative_field_v = is_commutative_field<T>::value;


	//相対誤差評価の補助構造体
	//整数型の場合
	template <class T>
	struct Error_evaluation {
		static constexpr T epsilon() { return 0; }
		static constexpr bool _error_evaluation_(const T& x1, const T& x2) { return x1 == x2; }
	};
	//浮動小数点型の評価
#define IMATH_ERROR_EVALUATION(TYPE) \
	template <>\
	struct Error_evaluation<TYPE> {\
		static constexpr TYPE epsilon() { return numeric_traits<TYPE>::epsilon() * 10; }\
		static constexpr bool _error_evaluation_(TYPE x1, TYPE x2) {\
			return (x2 == 0) ? (abs(x1 - x2) < epsilon()) : (abs((x1 - x2) / x2) < epsilon());\
		}\
	};
	IMATH_ERROR_EVALUATION(float);
	IMATH_ERROR_EVALUATION(double);
	IMATH_ERROR_EVALUATION(long double);
#undef IMATH_ERROR_EVALUATION
	template <class T>
	inline constexpr bool error_evaluation(const T& x1, const T& x2) {
		return Error_evaluation<T>::_error_evaluation_(x1, x2);
	}


	//最下層の型をTでrebindする(type_tupleには階層の低い型から順に並んでいる)
	template <class T, class>
	struct algebraic_type_rebind;
	template <class T>
	struct algebraic_type_rebind<T, type_tuple<>> {
		using type = T;
	};
	template <class T, class First, class... Types>
	struct algebraic_type_rebind<T, type_tuple<First, Types...>>
		: algebraic_type_rebind<typename First::template rebind<T>::other, type_tuple<Types...>> {};


	//数学関数用の型(最下層の型を(基本的に)実数体にする)
	template <class T, class, bool = is_algebraic_structure_v<T>>
	struct math_function_type_impl {};
	template <class T, class... Types>
	struct math_function_type_impl<T, type_tuple<Types...>, false>
		: algebraic_type_rebind<conditional_t<!is_floating_point_v<T>, IMATH_DEFAULT_TYPE, T>, type_tuple<Types...>> {};
	template <class T, class... Types>
	struct math_function_type_impl<T, type_tuple<Types...>, true>
		: math_function_type_impl<typename T::algebraic_type, type_tuple<T, Types...>> {};
	template <class T>
	struct math_function_type : math_function_type_impl<T, type_tuple<>> {};


	//完全に移行完了するまで残しておく
	template <class T>
	struct math_function;

	/*
		//超幾何級数(r:分子の数,s:分母の数,_r:r個の要素,_s:s個の要素,z)
		static result_type __hypergeometric_series(size_t r, size_t s, const type* _r, const type* _s, const type& z) {
			result_type x1 = 1, x2 = 1, buf = 0;

			if (r > s + 1) return 0;							//発散
			else if ((r == s + 1) && (abs(z) > 1)) return 0;	//*

			for (size_t i = 1; !error_evaluation(x2, buf); ++i) {
				buf = x2;
				for (size_t j = 0; j < r; ++j)			//分子
					x1 *= (_r[j] + i - 1);
				for (size_t j = 0; j < s; ++j)			//分母
					x1 /= (_s[j] + i - 1);
				x1 *= (z / i);
				x2 += x1;
			}
			return x2;
		}
	*/
}


#endif