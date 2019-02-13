#ifndef IMATH_MATH_MATH_MATH_TRAITS_HPP
#define IMATH_MATH_MATH_MATH_TRAITS_HPP

#include "IMathLib/IMathLib_config.hpp"
#include "IMathLib/utility/utility.hpp"

//デフォルトで計算される型
#ifndef IMATH_DEFAULT_TYPE
#define IMATH_DEFAULT_TYPE		double
#endif // IMATH_DEFAULT_TYPE

//ベルヌーイ数の用意されるテーブル数(それを用いた関数の精度もこれに依存する)
#ifndef IMATH_BERNOULLI_NUMBER_TABLE
#define IMATH_BERNOULLI_NUMBER_TABLE		40
#endif // IMATH_DEFAULT_TYPE


//浮動小数点の定数
#define IMATH_HUGE_NUM				1E+300			//巨大数
#define IMATH_INFINITYF				(float)(IMATH_HUGE_NUM*IMATH_HUGE_NUM)			//無限大
#define IMATH_INFINITYD				(double)(IMATH_HUGE_NUM*IMATH_HUGE_NUM)
#define IMATH_INFINITYLD			(long double)(IMATH_HUGE_NUM*IMATH_HUGE_NUM)
#define IMATH_NANF					(float)(IMATH_INFINITYF*0)						//非数
#define IMATH_NAND					(double)(IMATH_INFINITYD*0)
#define IMATH_NANLD					(long double)(IMATH_INFINITYLD*0)


//数学関数


namespace iml {

	//From⊂Toかの判定(From→Toできるか)
	template <class From, class To, bool = (is_arithmetic<From>::value && is_arithmetic<To>::value)>
	struct is_inclusion_impl : is_convertible<From, To> {};
	//どちらかにvoidがあればfalse
	template <class To>
	struct is_inclusion_impl<void, To, false> {
		static constexpr bool value = false;
	};
	template <class From>
	struct is_inclusion_impl<From, void, false> {
		static constexpr bool value = false;
	};
	//算術型の場合(N⊂Z⊂R)
	template <class From, class To>
	struct is_inclusion_impl<From, To, true> :
		cat_bool<is_same<From, To>::value ||
		(is_unsigned<From>::value && is_signed<To>::value) ||
		(is_unsigned<From>::value && is_floating_point<To>::value) ||
		(is_signed<From>::value && is_floating_point<To>::value)> {};
	template <class From, class To>
	struct is_inclusion : is_inclusion_impl<From, To> {};

	//全てのテンプレートパラメータがキャスト可能か
	template <class, class, bool>
	struct is_all_inclusion_impl;
	template <class To, bool f>
	struct is_all_inclusion_impl<type_tuple<>, To, f> {
		static constexpr bool value = f;
	};
	template <class First, class... Types, class To, bool f>
	struct is_all_inclusion_impl<type_tuple<First, Types...>, To, f>
		: is_all_inclusion_impl<type_tuple<Types...>, To, is_inclusion<First, To>::value && f> {};
	template <class From, class To>
	struct is_all_inclusion;
	template <class... Types, class To>
	struct is_all_inclusion<type_tuple<Types...>, To>
		: is_all_inclusion_impl<type_tuple<Types...>, To, sizeof...(Types) != 0> {};


	//包含関係で上位のものを選択(選択できなければvoid)
	template <class T1, class T2>
	struct inclusion_superior {
		using type = typename conditional<is_inclusion<T1, T2>::value, T2
			, typename conditional<is_inclusion<T2, T1>::value, T1, void>::type>::type;
	};


	//任意の2つの型の演算可能テスト
	template <class T1, class T2, bool = (is_arithmetic<T1>::value && is_arithmetic<T2>::value)>
	struct is_calcable_impl {
		using type = typename inclusion_superior<T1, T2>::type;

		static constexpr bool add_value = true;
		static constexpr bool sub_value = is_signed<type>::value;
		static constexpr bool mul_value = true;
		static constexpr bool div_value = is_floating_point<type>::value;
		static constexpr bool add_eq_value = is_inclusion<type, T1>::value;
		static constexpr bool sub_eq_value = is_inclusion<type, T1>::value && is_signed<type>::value;
		static constexpr bool mul_eq_value = is_inclusion<type, T1>::value;
		static constexpr bool div_eq_value = is_floating_point<T1>::value;
		static constexpr bool eq_value = true;
		static constexpr bool lt_value = true;
		static constexpr bool lt_eq_value = true;
		static constexpr bool gt_value = true;
		static constexpr bool gt_eq_value = true;
	};
	template <class T1, class T2>
	struct is_calcable_impl<T1, T2, false> {
	private:
		template <class _T1, class _T2> static auto add_tester(_T1*, _T2*) ->decltype(declval<_T1>() + declval<_T2>(), true_type());
		template <class _T1, class _T2> static auto sub_tester(_T1*, _T2*) ->decltype(declval<_T1>() - declval<_T2>(), true_type());
		template <class _T1, class _T2> static auto mul_tester(_T1*, _T2*) ->decltype(declval<_T1>() * declval<_T2>(), true_type());
		template <class _T1, class _T2> static auto div_tester(_T1*, _T2*) ->decltype(declval<_T1>() / declval<_T2>(), true_type());
		template <class _T1, class _T2> static auto add_eq_tester(_T1*, _T2*) ->decltype(declval<_T1>() += declval<_T2>(), true_type());
		template <class _T1, class _T2> static auto sub_eq_tester(_T1*, _T2*) ->decltype(declval<_T1>() -= declval<_T2>(), true_type());
		template <class _T1, class _T2> static auto mul_eq_tester(_T1*, _T2*) ->decltype(declval<_T1>() *= declval<_T2>(), true_type());
		template <class _T1, class _T2> static auto div_eq_tester(_T1*, _T2*) ->decltype(declval<_T1>() /= declval<_T2>(), true_type());
		template <class _T1, class _T2> static auto eq_tester(_T1*, _T2*) ->decltype(declval<_T1>() == declval<_T2>(), true_type());
		template <class _T1, class _T2> static auto lt_tester(_T1*, _T2*) ->decltype(declval<_T1>() < declval<_T2>(), true_type());
		template <class _T1, class _T2> static auto lt_eq_tester(_T1*, _T2*) ->decltype(declval<_T1>() <= declval<_T2>(), true_type());
		template <class _T1, class _T2> static auto gt_tester(_T1*, _T2*) ->decltype(declval<_T1>() > declval<_T2>(), true_type());
		template <class _T1, class _T2> static auto gt_eq_tester(_T1*, _T2*) ->decltype(declval<_T1>() >= declval<_T2>(), true_type());

		template <class _T1, class _T2> static false_type add_tester(...);
		template <class _T1, class _T2> static false_type sub_tester(...);
		template <class _T1, class _T2> static false_type mul_tester(...);
		template <class _T1, class _T2> static false_type div_tester(...);
		template <class _T1, class _T2> static false_type add_eq_tester(...);
		template <class _T1, class _T2> static false_type sub_eq_tester(...);
		template <class _T1, class _T2> static false_type mul_eq_tester(...);
		template <class _T1, class _T2> static false_type div_eq_tester(...);
		template <class _T1, class _T2> static false_type eq_tester(...);
		template <class _T1, class _T2> static false_type lt_tester(...);
		template <class _T1, class _T2> static false_type lt_eq_tester(...);
		template <class _T1, class _T2> static false_type gt_tester(...);
		template <class _T1, class _T2> static false_type gt_eq_tester(...);
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
		static constexpr bool lt_eq_value = decltype(lt_eq_tester<T1, T2>(nullptr, nullptr))::value;
		static constexpr bool gt_value = decltype(gt_tester<T1, T2>(nullptr, nullptr))::value;
		static constexpr bool gt_eq_value = decltype(gt_eq_tester<T1, T2>(nullptr, nullptr))::value;
	};
	template <class T1, class T2>
	struct is_calcable : is_calcable_impl<T1, T2> {};


	//任意の2つの型の演算テスト(演算不可の場合はvoid型)
	template <class T1, class T2, bool = (is_arithmetic<T1>::value && is_arithmetic<T2>::value)>
	struct calculation_result_impl {
		using type1 = typename inclusion_superior<T1, T2>::type;
		using type2 = is_calcable<T1, T2>;

		using add_type = typename conditional<type2::add_value, type1, void>::type;
		using sub_type = typename conditional<type2::sub_value, type1, void>::type;
		using mul_type = typename conditional<type2::mul_value, type1, void>::type;
		using div_type = typename conditional<type2::div_value, type1, void>::type;
		using add_eq_type = typename conditional<type2::add_eq_value, type1, void>::type;
		using sub_eq_type = typename conditional<type2::sub_eq_value, type1, void>::type;
		using mul_eq_type = typename conditional<type2::mul_eq_value, type1, void>::type;
		using div_eq_type = typename conditional<type2::div_eq_value, type1, void>::type;
	};
	template <class T1, class T2>
	struct calculation_result_impl<T1, T2, false> {
	private:
		template <bool>
		struct tester;
		template <>
		struct tester<true> {
			template <class _T1, class _T2> static auto add_tester() ->decltype(declval<_T1>() + declval<_T2>());
			template <class _T1, class _T2> static auto sub_tester() ->decltype(declval<_T1>() - declval<_T2>());
			template <class _T1, class _T2> static auto mul_tester() ->decltype(declval<_T1>() * declval<_T2>());
			template <class _T1, class _T2> static auto div_tester() ->decltype(declval<_T1>() / declval<_T2>());
			template <class _T1, class _T2> static auto add_eq_tester() ->decltype(declval<_T1>() += declval<_T2>());
			template <class _T1, class _T2> static auto sub_eq_tester() ->decltype(declval<_T1>() -= declval<_T2>());
			template <class _T1, class _T2> static auto mul_eq_tester() ->decltype(declval<_T1>() *= declval<_T2>());
			template <class _T1, class _T2> static auto div_eq_tester() ->decltype(declval<_T1>() /= declval<_T2>());
		};
		template <>
		struct tester<false> {
			template <class _T1, class _T2> static void add_tester();
			template <class _T1, class _T2> static void sub_tester();
			template <class _T1, class _T2> static void mul_tester();
			template <class _T1, class _T2> static void div_tester();
			template <class _T1, class _T2> static void add_eq_tester();
			template <class _T1, class _T2> static void sub_eq_tester();
			template <class _T1, class _T2> static void mul_eq_tester();
			template <class _T1, class _T2> static void div_eq_tester();
		};
	public:
		using add_type = decltype(tester<is_calcable<T1, T2>::add_value>::add_tester<T1, T2>());
		using sub_type = decltype(tester<is_calcable<T1, T2>::sub_value>::sub_tester<T1, T2>());
		using mul_type = decltype(tester<is_calcable<T1, T2>::mul_value>::mul_tester<T1, T2>());
		using div_type = decltype(tester<is_calcable<T1, T2>::div_value>::div_tester<T1, T2>());
		using add_eq_type = decltype(tester<is_calcable<T1, T2>::add_eq_value>::add_eq_tester<T1, T2>());
		using sub_eq_type = decltype(tester<is_calcable<T1, T2>::sub_eq_value>::sub_eq_tester<T1, T2>());
		using mul_eq_type = decltype(tester<is_calcable<T1, T2>::mul_eq_value>::mul_eq_tester<T1, T2>());
		using div_eq_type = decltype(tester<is_calcable<T1, T2>::div_eq_value>::div_eq_tester<T1, T2>());
	};
	template <class T1, class T2>
	struct calculation_result : calculation_result_impl<T1, T2> {};


	//T1×T2→T3となるかの判定
	template <class T1, class T2, class T3>
	struct is_operation {
		static constexpr bool add_value = is_inclusion<typename calculation_result<T1, T2>::add_type, T3>::value;
		static constexpr bool sub_value = is_inclusion<typename calculation_result<T1, T2>::sub_type, T3>::value;
		static constexpr bool mul_value = is_inclusion<typename calculation_result<T1, T2>::mul_type, T3>::value;
		static constexpr bool div_value = is_inclusion<typename calculation_result<T1, T2>::div_type, T3>::value;
	};

	//閉じた演算であるかの判定(マグマの判定)
	template <class T>
	using is_magma = is_operation<T, T, T>;


	//加法パラメータの取得
	template<class>
	struct addition;
	template<>
	struct addition<bool> {
		//単位元の取得
		static constexpr bool identity_element() { return false; }
		//逆元の取得
		static constexpr bool inverse_element(bool x) { return !x; }
	};
	template<>
	struct addition<char> {
		//単位元の取得
		static constexpr char identity_element() { return 0; }
		//逆元の取得
		static constexpr char inverse_element(char x) { return -x; }
	};
	template<>
	struct addition<unsigned char> {
		//単位元の取得
		static constexpr unsigned char identity_element() { return 0; }
	};
	template<>
	struct addition<wchar_t> {
		//単位元の取得
		static constexpr wchar_t identity_element() { return 0; }
	};
	template<>
	struct addition<short> {
		//単位元の取得
		static constexpr short identity_element() { return 0; }
		//逆元の取得
		static constexpr short inverse_element(short x) { return -x; }
	};
	template<>
	struct addition<unsigned short> {
		//単位元の取得
		static constexpr unsigned char identity_element() { return 0; }
	};
	template<>
	struct addition<int> {
		//単位元の取得
		static constexpr int identity_element() { return 0; }
		//逆元の取得
		static constexpr int inverse_element(int x) { return -x; }
	};
	template<>
	struct addition<unsigned int> {
		//単位元の取得
		static constexpr unsigned int identity_element() { return 0; }
	};
	template<>
	struct addition<long> {
		//単位元の取得
		static constexpr long identity_element() { return 0; }
		//逆元の取得
		static constexpr long inverse_element(long x) { return -x; }
	};
	template<>
	struct addition<unsigned long> {
		//単位元の取得
		static constexpr unsigned long identity_element() { return 0; }
	};
	template<>
	struct addition<char16_t> {
		//単位元の取得
		static constexpr char16_t identity_element() { return 0; }
	};
	template<>
	struct addition<char32_t> {
		//単位元の取得
		static constexpr char32_t identity_element() { return 0; }
	};
	template<>
	struct addition<long long> {
		//単位元の取得
		static constexpr long long identity_element() { return 0; }
		//逆元の取得
		static constexpr long long inverse_element(long long x) { return -x; }
	};
	template<>
	struct addition<unsigned long long> {
		//単位元の取得
		static constexpr unsigned long long identity_element() { return 0; }
	};
	template<>
	struct addition<float> {
		//単位元の取得
		static constexpr float identity_element() { return 0; }
		//逆元の取得
		static constexpr float inverse_element(float x) { return -x; }
	};
	template<>
	struct addition<double> {
		//単位元の取得
		static constexpr double identity_element() { return 0; }
		//逆元の取得
		static constexpr double inverse_element(double x) { return -x; }
	};
	template<>
	struct addition<long double> {
		//単位元の取得
		static constexpr long double identity_element() { return 0; }
		//逆元の取得
		static constexpr long double inverse_element(long double x) { return -x; }
	};

	//乗法パラメータの取得
	template <class T>
	struct multiplicative;
	template<>
	struct multiplicative<bool> {
		//単位元の取得
		static constexpr bool identity_element() { return true; }
		//吸収元
		static constexpr bool absorbing_element() { return false; }
	};
	template<>
	struct multiplicative<char> {
		//単位元の取得
		static constexpr char identity_element() { return 1; }
		//吸収元
		static constexpr char absorbing_element() { return 0; }
	};
	template<>
	struct multiplicative<unsigned char> {
		//単位元の取得
		static constexpr unsigned char identity_element() { return 1; }
		//吸収元
		static constexpr unsigned char absorbing_element() { return 0; }
	};
	template<>
	struct multiplicative<wchar_t> {
		//単位元の取得
		static constexpr wchar_t identity_element() { return 1; }
		//吸収元
		static constexpr wchar_t absorbing_element() { return 0; }
	};
	template<>
	struct multiplicative<short> {
		//単位元の取得
		static constexpr short identity_element() { return 1; }
		//吸収元
		static constexpr short absorbing_element() { return 0; }
	};
	template<>
	struct multiplicative<unsigned short> {
		//単位元の取得
		static constexpr unsigned short identity_element() { return 1; }
		//吸収元
		static constexpr unsigned short absorbing_element() { return 0; }
	};
	template<>
	struct multiplicative<int> {
		//単位元の取得
		static constexpr int identity_element() { return 1; }
		//吸収元
		static constexpr int absorbing_element() { return 0; }
	};
	template<>
	struct multiplicative<unsigned int> {
		//単位元の取得
		static constexpr unsigned int identity_element() { return 1; }
		//吸収元
		static constexpr unsigned int absorbing_element() { return 0; }
	};
	template<>
	struct multiplicative<long> {
		//単位元の取得
		static constexpr long identity_element() { return 1; }
		//吸収元
		static constexpr long absorbing_element() { return 0; }
	};
	template<>
	struct multiplicative<unsigned long> {
		//単位元の取得
		static constexpr unsigned long identity_element() { return 1; }
		//吸収元
		static constexpr unsigned long absorbing_element() { return 0; }
	};
	template<>
	struct multiplicative<char16_t> {
		//単位元の取得
		static constexpr char16_t identity_element() { return 1; }
		//吸収元
		static constexpr char16_t absorbing_element() { return 0; }
	};
	template<>
	struct multiplicative<char32_t> {
		//単位元の取得
		static constexpr char32_t identity_element() { return 1; }
		//吸収元
		static constexpr char32_t absorbing_element() { return 0; }
	};
	template<>
	struct multiplicative<long long> {
		//単位元の取得
		static constexpr long long identity_element() { return 1; }
		//吸収元
		static constexpr long long absorbing_element() { return 0; }
	};
	template<>
	struct multiplicative<unsigned long long> {
		//単位元の取得
		static constexpr unsigned long long identity_element() { return 1; }
		//吸収元
		static constexpr unsigned long long absorbing_element() { return 0; }
	};
	template<>
	struct multiplicative<float> {
		//単位元の取得
		static constexpr float identity_element() { return 1; }
		//逆元の取得
		static constexpr float inverse_element(float x) { return 1 / x; }
		//吸収元
		static constexpr float absorbing_element() { return 0; }
	};
	template<>
	struct multiplicative<double> {
		//単位元の取得
		static constexpr double identity_element() { return 1; }
		//逆元の取得
		static constexpr double inverse_element(double x) { return 1 / x; }
		//吸収元
		static constexpr double absorbing_element() { return 0; }
	};
	template<>
	struct multiplicative<long double> {
		//単位元の取得
		static constexpr long double identity_element() { return 1; }
		//逆元の取得
		static constexpr long double inverse_element(long double x) { return 1 / x; }
		//吸収元
		static constexpr long double absorbing_element() { return 0; }
	};

	//加法単位元が存在する
	template <class T>
	struct is_exist_add_identity_element {
	private:
		template <class _T> static auto tester(_T*) ->decltype(_T::identity_element(), true_type());
		template <class _T> static false_type tester(...);
	public:
		static constexpr bool value = decltype(tester<addition<T>>(nullptr))::value;
	};
	//加法逆元が存在する
	template <class T>
	struct is_exist_add_inverse_element {
	private:
		template <class _T1, class _T2> static auto tester(_T1*, _T2*) ->decltype(_T1::inverse_element(declval<_T2>()), true_type());
		template <class _T1, class _T2> static false_type tester(...);
	public:
		static constexpr bool value = decltype(tester<addition<T>, T>(nullptr, nullptr))::value;
	};
	//乗法単位元が存在する
	template <class T>
	struct is_exist_mul_identity_element {
	private:
		template <class _T> static auto tester(_T*) ->decltype(_T::identity_element(), true_type());
		template <class _T> static false_type tester(...);
	public:
		static constexpr bool value = decltype(tester<multiplicative<T>>(nullptr))::value;
	};
	//乗法逆元が存在する
	template <class T>
	struct is_exist_mul_inverse_element {
	private:
		template <class _T1, class _T2> static auto tester(_T1*, _T2*) ->decltype(_T1::inverse_element(declval<_T2>()), true_type());
		template <class _T1, class _T2> static false_type tester(...);
	public:
		static constexpr bool value = decltype(tester<multiplicative<T>, T>(nullptr, nullptr))::value;
	};
	//乗法吸収元が存在する
	template <class T>
	struct is_exist_mul_absorbing_element {
	private:
		template <class _T> static auto tester(_T*) ->decltype(_T::absorbing_element(), true_type());
		template <class _T> static false_type tester(...);
	public:
		static constexpr bool value = decltype(tester<multiplicative<T>>(nullptr))::value;
	};

	//ループ(代数的構造)の判定
	template <class T>
	struct is_loop {
		static constexpr bool add_value = is_magma<T>::add_value && is_exist_add_identity_element<T>::value && is_exist_add_inverse_element<T>::value;
		static constexpr bool mul_value = is_magma<T>::mul_value && is_exist_mul_identity_element<T>::value && is_exist_mul_inverse_element<T>::value;
	};

	//代数的構造上の代数的構造である(algebraic_typeが内部で定義されている)ことの判定
	template <class, class = void>
	struct is_algebraic_structure : false_type {};
	template <class T>
	struct is_algebraic_structure<T, void_t<typename T::algebraic_type>> : true_type {};

	//数値型に対する極限や不定形の定義
	template <class T>
	struct limits;
	template <>
	struct limits<char> {
		using type = char;

		static constexpr imsize_t digits = 7;
		static constexpr imsize_t digits10 = 2;
		static constexpr type(min)() noexcept { return -127 - 1; }
		static constexpr type(max)() noexcept { return 127; }
	};
	template <>
	struct limits<unsigned char> {
		using type = unsigned char;

		static constexpr imsize_t digits = 8;
		static constexpr imsize_t digits10 = 2;
		static constexpr type(min)() noexcept { return 0; }
		static constexpr type(max)() noexcept { return 0XFF; }
	};
	template <>
	struct limits<imint16_t> {
		using type = imint16_t;

		static constexpr imsize_t digits = 15;
		static constexpr imsize_t digits10 = 4;
		static constexpr type(min)() noexcept { return -32767 - 1; }
		static constexpr type(max)() noexcept { return 32767; }
	};
	template <>
	struct limits<imuint16_t> {
		using type = imuint16_t;

		static constexpr imsize_t digits = 16;
		static constexpr imsize_t digits10 = 4;
		static constexpr type(min)() noexcept { return 0; }
		static constexpr type(max)() noexcept { return 65535; }
	};
	template <>
	struct limits<imint32_t> {
		using type = imint32_t;

		static constexpr imsize_t digits = 31;
		static constexpr imsize_t digits10 = 9;
		static constexpr type(min)() noexcept { return -2147483647L - 1; }
		static constexpr type(max)() noexcept { return 2147483647L; }
	};
	template <>
	struct limits<imuint32_t> {
		using type = imuint32_t;

		static constexpr imsize_t digits = 32;
		static constexpr imsize_t digits10 = 9;
		static constexpr type(min)() noexcept { return 0L; }
		static constexpr type(max)() noexcept { return 4294967295UL; }
	};
	template <>
	struct limits<imint64_t> {
		using type = imint64_t;

		static constexpr imsize_t digits = 63;
		static constexpr imsize_t digits10 = 18;
		static constexpr type(min)() noexcept { return -9223372036854775807LL - 1; }
		static constexpr type(max)() noexcept { return 9223372036854775807LL; }
	};
	template <>
	struct limits<imuint64_t> {
		using type = imuint64_t;

		static constexpr imsize_t digits = 64;
		static constexpr imsize_t digits10 = 19;
		static constexpr type(min)() noexcept { return 0ULL; }
		static constexpr type(max)() noexcept { return 18446744073709551615ULL; }
	};
	template <>
	struct limits<float> {
		using type = float;

		static constexpr imsize_t digits = 24;
		static constexpr imsize_t digits10 = 6;
		static constexpr type(min)() noexcept { return -3.402823466E+38f; }
		static constexpr type(max)() noexcept { return 3.402823466E+38f; }
		static constexpr type normalisation() { return 1.175494351E-38f; }
		static constexpr type positive_infinity() noexcept { return IMATH_INFINITYF; }
		static constexpr type negative_infinity() noexcept { return -IMATH_INFINITYF; }
		static constexpr type nan() noexcept { return IMATH_NANF; }
		static constexpr type epsilon() noexcept { return 1.192092896E-07f; }

		static constexpr bool is_positive_infinity(type x) { return x == positive_infinity(); }
		static constexpr bool is_negative_infinity(type x) { return x == negative_infinity(); }
		static constexpr bool is_nan(type x) { return !(x >= 0 || x < 0); }
	};
	template <>
	struct limits<double> {
		using type = double;

		static constexpr imsize_t digits = 53;
		static constexpr imsize_t digits10 = 15;
		static constexpr type(min)() noexcept { return -1.7976931348623158E+308; }
		static constexpr type(max)() noexcept { return 1.7976931348623158E+308; }
		static constexpr type normalisation() { return 2.2250738585072014E-308; }
		static constexpr type positive_infinity() noexcept { return IMATH_INFINITYD; }
		static constexpr type negative_infinity() noexcept { return -IMATH_INFINITYD; }
		static constexpr type nan() noexcept { return IMATH_NAND; }
		static constexpr type epsilon() noexcept { return 2.2204460492503131E-016; }

		static constexpr bool is_positive_infinity(type x) { return x == positive_infinity(); }
		static constexpr bool is_negative_infinity(type x) { return x == negative_infinity(); }
		static constexpr bool is_nan(type x) { return !(x >= 0 || x < 0); }
	};
	template <>
	struct limits<long double> {
		using type = long double;

		static constexpr imsize_t digits = 53;
		static constexpr imsize_t digits10 = 15;
		static constexpr type(min)() noexcept { return -1.7976931348623158E+308; }
		static constexpr type(max)() noexcept { return 1.7976931348623158E+308; }
		static constexpr type normalisation() { return 2.2250738585072014E-308; }
		static constexpr type positive_infinity() noexcept { return IMATH_INFINITYLD; }
		static constexpr type negative_infinity() noexcept { return -IMATH_INFINITYLD; }
		static constexpr type nan() noexcept { return IMATH_NANLD; }
		static constexpr type epsilon() noexcept { return 2.2204460492503131E-016; }

		static constexpr bool is_positive_infinity(type x) { return x == positive_infinity(); }
		static constexpr bool is_negative_infinity(type x) { return x == negative_infinity(); }
		static constexpr bool is_nan(type x) { return !(x >= 0 || x < 0); }
	};

	//正の無限大の判定
	template <class T>
	inline bool is_positive_inf(const T& x) {
		return limits<T>::is_positive_infinity(x);
	}
	//負の無限大の判定
	template <class T>
	inline bool is_negative_inf(const T& x) {
		return limits<T>::is_negative_infinity(x);
	}
	//無限大の判定
	template <class T>
	inline bool is_inf(const T& x) {
		return is_positive_inf(x) || is_negative_inf(x);
	}
	//非数の判定
	template <class T>
	inline bool is_nan(const T& x) {
		return limits<T>::is_quiet_nan(x) || limits<T>::is_signaling_nan(x);
	}

	//相対誤差評価の補助構造体
	template <class T>
	struct Error_evaluation;
	//浮動小数点型の評価
	template <>
	struct Error_evaluation<float> {
	private:
		static constexpr float _abs_(float x) {
			return (x < 0) ? -x : x;
		}
	public:
		static constexpr float eps = limits<float>::epsilon() * 10;

		static constexpr bool __error_evaluation(float x1, float x2) {
			return (x2 == 0) ? (_abs_(x1 - x2) < eps) : (_abs_((x1 - x2) / x2) < eps);
		}
	};
	template <>
	struct Error_evaluation<double> {
	private:
		static constexpr double _abs_(double x) {
			return (x < 0) ? -x : x;
		}
	public:
		static constexpr double eps = limits<double>::epsilon() * 10;

		static constexpr bool __error_evaluation(double x1, double x2) {
			return (x2 == 0) ? (_abs_(x1 - x2) < eps) : (_abs_((x1 - x2) / x2) < eps);
		}
	};
	template <>
	struct Error_evaluation<long double> {
	private:
		static constexpr long double _abs_(long double x) {
			return (x < 0) ? -x : x;
		}
	public:
		static constexpr double eps = limits<double>::epsilon() * 10;

		static constexpr bool __error_evaluation(long double x1, long double x2) {
			return (x2 == 0) ? (_abs_(x1 - x2) < eps) : (_abs_((x1 - x2) / x2) < eps);
		}
	};
	template <class T>
	inline constexpr bool error_evaluation(const T& x1, const T& x2) {
		return Error_evaluation<T>::__error_evaluation(x1, x2);
	}


	//最下層の型のrebind(type_tupleには階層の低い型から順に並んでいる)
	//Tは置き換える型
	template <class, class>
	struct algebraic_type_rebind;
	template <class T>
	struct algebraic_type_rebind<T, type_tuple<>> {
		using type = T;
	};
	template <class T, class First, class... Types>
	struct algebraic_type_rebind<T, type_tuple<First, Types...>>
		: algebraic_type_rebind<typename First::template rebind<T>::other, type_tuple<Types...>> {};


	//数学関数用の型(最下層の型を(基本的に)実数体にする)
	template <class T, class, bool = is_algebraic_structure<T>::value>
	struct math_function_type_impl {};
	template <class T, class... Types>
	struct math_function_type_impl<T, type_tuple<Types...>, false>
		: algebraic_type_rebind<typename conditional<!is_floating_point<T>::value, IMATH_DEFAULT_TYPE, T>::type, type_tuple<Types...>> {};
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
		static result_type __hypergeometric_series(imsize_t r, imsize_t s, const type* _r, const type* _s, const type& z) {
			result_type x1 = 1, x2 = 1, buf = 0;

			if (r > s + 1) return 0;							//発散
			else if ((r == s + 1) && (abs(z) > 1)) return 0;	//*

			for (imsize_t i = 1; !error_evaluation(x2, buf); ++i) {
				buf = x2;
				for (imsize_t j = 0; j < r; ++j)			//分子
					x1 *= (_r[j] + i - 1);
				for (imsize_t j = 0; j < s; ++j)			//分母
					x1 /= (_s[j] + i - 1);
				x1 *= (z / i);
				x2 += x1;
			}
			return x2;
		}
	*/
}


#endif