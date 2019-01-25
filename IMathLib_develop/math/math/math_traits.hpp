#ifndef _IMATH_MATH_MATH_HPP
#define _IMATH_MATH_MATH_HPP

#include "IMathLib/IMathLib_config.hpp"
#include "IMathLib/utility/utility.hpp"

//デフォルトで計算される型
#ifndef _IMATH_DEFAULT_TYPE
#define _IMATH_DEFAULT_TYPE		double
#endif // _IMATH_DEFAULT_TYPE

//ベルヌーイ数の用意されるテーブル数(それを用いた関数の精度もこれに依存する)
#ifndef _IMATH_BERNOULLI_NUMBER_TABLE
#define _IMATH_BERNOULLI_NUMBER_TABLE		40
#endif // _IMATH_DEFAULT_TYPE


//数学関数


namespace iml {

	//From⊂Toかの判定(From→Toできるか)
	template <bool, class From, class To>
	struct _Is_inclusion : is_convertible<From, To> {};
	//どちらかにvoidがあればfalse
	template <class To>
	struct _Is_inclusion<false, void, To> {
		static constexpr bool value = false;
	};
	template <class From>
	struct _Is_inclusion<false, From, void> {
		static constexpr bool value = false;
	};
	//算術型の場合(N⊂Z⊂R)
	template <class From, class To>
	struct _Is_inclusion<true, From, To> :
		_Cat_base<is_same<From, To>::value ||
		(is_unsigned<From>::value && is_signed<To>::value) ||
		(is_unsigned<From>::value && is_floating_point<To>::value) ||
		(is_signed<From>::value && is_floating_point<To>::value)> {};
	template <class From, class To>
	struct is_inclusion :_Is_inclusion<(is_arithmetic<From>::value && is_arithmetic<To>::value), From, To> {};

	//全てのテンプレートパラメータがキャスト可能か
	template <class, class, bool>
	struct _Is_all_inclusion;
	template <class To, bool f>
	struct _Is_all_inclusion<arg_tuple<>, To, f> {
		static constexpr bool value = f;
	};
	template <class First, class... Types, class To, bool f>
	struct _Is_all_inclusion<arg_tuple<First, Types...>, To, f>
		: _Is_all_inclusion<arg_tuple<Types...>, To, is_inclusion<First, To>::value && f> {};
	template <class From, class To>
	struct is_all_inclusion;
	template <class... Types, class To>
	struct is_all_inclusion<arg_tuple<Types...>, To>
		: _Is_all_inclusion<arg_tuple<Types...>, To, sizeof...(Types) != 0> {};


	//包含関係で上位のものを選択(選択できなければvoid)
	template <class T1, class T2>
	struct inclusion_superior {
		using type = typename conditional<is_inclusion<T1, T2>::value, T2
			, typename conditional<is_inclusion<T2, T1>::value, T1, void>::type>::type;
	};


	//任意の2つの型の演算可能テスト
	template <bool, class, class>
	struct _Is_calcable;
	template <class T1, class T2>
	struct _Is_calcable<false, T1, T2> {
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
	struct _Is_calcable<true, T1, T2> {
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
	struct is_calcable :_Is_calcable<(is_arithmetic<T1>::value && is_arithmetic<T2>::value), T1, T2> {};


	//任意の2つの型の演算テスト(演算不可の場合はvoid型)
	template <bool, class, class>
	struct _Calculation_result;
	template <class T1, class T2>
	struct _Calculation_result<false, T1, T2> {
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
	struct _Calculation_result<true, T1, T2> {
		using type = typename inclusion_superior<T1, T2>::type;

		using add_type = typename conditional<is_calcable<T1, T2>::add_value, type, void>::type;
		using sub_type = typename conditional<is_calcable<T1, T2>::sub_value, type, void>::type;
		using mul_type = typename conditional<is_calcable<T1, T2>::mul_value, type, void>::type;
		using div_type = typename conditional<is_calcable<T1, T2>::div_value, type, void>::type;
		using add_eq_type = typename conditional<is_calcable<T1, T2>::add_eq_value, type, void>::type;
		using sub_eq_type = typename conditional<is_calcable<T1, T2>::sub_eq_value, type, void>::type;
		using mul_eq_type = typename conditional<is_calcable<T1, T2>::mul_eq_value, type, void>::type;
		using div_eq_type = typename conditional<is_calcable<T1, T2>::div_eq_value, type, void>::type;
	};
	template <class T1, class T2>
	struct calculation_result :_Calculation_result<(is_arithmetic<T1>::value && is_arithmetic<T2>::value), T1, T2> {};


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
	template<class T>
	struct is_algebraic_structure {
	private:
		template <class _T> static auto tester(_T*) ->decltype(declval<typename _T::algebraic_type>(), true_type());
		template <class _T> static false_type tester(...);
	public:
		static constexpr bool value = decltype(tester<T>(nullptr))::value;
	};


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
		static constexpr type positive_infinity() noexcept { return (max)()*(max)(); }
		static constexpr type negative_infinity() noexcept { return -(max)()*(max)(); }
		static constexpr type nan() noexcept { return positive_infinity() / positive_infinity(); }
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
		static constexpr type positive_infinity() noexcept { return (max)()*(max)(); }
		static constexpr type negative_infinity() noexcept { return -(max)()*(max)(); }
		static constexpr type nan() noexcept { return positive_infinity() / positive_infinity(); }
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
		static constexpr type positive_infinity() noexcept { return (max)()*(max)(); }
		static constexpr type negative_infinity() noexcept { return -(max)()*(max)(); }
		static constexpr type nan() noexcept { return positive_infinity() / positive_infinity(); }
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
	struct _Error_evaluation;
	//浮動小数点型の評価
	template <>
	struct _Error_evaluation<float> {
	private:
		static constexpr float eps = limits<float>::epsilon() * 10;

		static constexpr float __Abs(float x) {
			return (x < 0) ? -x : x;
		}
	public:
		static constexpr bool __error_evaluation(float x1, float x2) {
			return (x2 == 0) ? (__Abs(x1 - x2) < eps) : (__Abs((x1 - x2) / x2) < eps);
		}
	};
	template <>
	struct _Error_evaluation<double> {
	private:
		static constexpr double eps = limits<double>::epsilon() * 10;

		static constexpr double __Abs(double x) {
			return (x < 0) ? -x : x;
		}
	public:
		static constexpr bool __error_evaluation(double x1, double x2) {
			return (x2 == 0) ? (__Abs(x1 - x2) < eps) : (__Abs((x1 - x2) / x2) < eps);
		}
	};
	template <>
	struct _Error_evaluation<long double> {
	private:
		static constexpr long double eps = limits<long double>::epsilon() * 10;

		static constexpr long double __Abs(long double x) {
			return (x < 0) ? -x : x;
		}
	public:
		static constexpr bool __error_evaluation(long double x1, long double x2) {
			return (x2 == 0) ? (__Abs(x1 - x2) < eps) : (__Abs((x1 - x2) / x2) < eps);
		}
	};
	template <class T>
	inline constexpr bool error_evaluation(const T& x1, const T& x2) {
		return _Error_evaluation<T>::__error_evaluation(x1, x2);
	}

	//数学関数用の型のbindの補助
	template <class, class>
	struct _Math_function_type_bind;
	template <class T>
	struct _Math_function_type_bind<T, arg_tuple<>> {
		using type = T;
	};
	template <class T, class First, class... Types>
	struct _Math_function_type_bind<T, arg_tuple<First, Types...>>
		: _Math_function_type_bind<typename First::template rebind<T>::other, arg_tuple<Types...>> {};

	//数学関数用の型
	template <bool, class, class>
	struct _Math_function_type;
	template <class T, class... Types>
	struct _Math_function_type<false, T, arg_tuple<Types...>>
		//_Math_function_type_bindによるrebindの展開
		: _Math_function_type_bind<typename conditional<!is_floating_point<T>::value, _IMATH_DEFAULT_TYPE, T>::type, arg_tuple<Types...>> {};
	template <class T, class... Types>
	struct _Math_function_type<true, T, arg_tuple<Types...>>
		: _Math_function_type<is_algebraic_structure<typename T::algebraic_type>::value, typename T::algebraic_type, arg_tuple<T, Types...>> {};
	template <class T>
	struct math_function_type : _Math_function_type<is_algebraic_structure<T>::value, T, arg_tuple<>> {};
}


namespace iml {

	//デフォルトの数値評価関数の定義(数値特性や取得系)
	template <class T>
	struct numerical_evaluation_function {
		using type = T;
	private:
		using calc_type = typename conditional<is_integral<T>::value, double, T>::type;		//整数型ならば演算結果はdouble
	public:
		//切り上げ(天井関数)
		static type __ceil(const type& x) {
			return static_cast<imint_t>(x < 0 ? x : x + (x > static_cast<imint_t>(x)));
		}
		//切り捨て(床関数)
		static type __floor(const type& x) {
			return static_cast<imint_t>(x < 0 ? x - (x > static_cast<imint_t>(x)) : x);
		}
		//四捨五入
		static type __round(const type& x) {
			return static_cast<imint_t>(x < 0 ? x - 0.5 : x + 0.5);
		}
		//剰余
		static type __mod(const type& x, const type& y) {
			type n = __floor(x / y);
			return x - n * y;
		}
		//最大値
		static type(__max)(const type& x, const type& y) {
			return (x < y ? y : x);
		}
		//最小値
		static type(__min)(const type& x, const type& y) {
			return (x < y ? x : y);
		}
		//実部
		static type __real(const type& x) {
			return x;
		}
		//共役
		static type __conj(const type& x) {
			return x;
		}

	};

	//デフォルトの数学関数の定義(アルゴリズム的関数)
	template <class T>
	struct math_function {
		//result_type用のmath_functionも暗黙的に呼び出される
		using type = T;
		using result_type = typename conditional<is_integral<T>::value, double, T>::type;		//ほとんどの場合は整数型ならば演算結果はdouble

		//共役との積の実部の平方根:sqrt(real(x*conj(x)))
		template <bool, bool>
		struct __abs_struct {
			static auto __abs(const type& x) {
				auto temp = numerical_evaluation_function<type>::__real(x*numerical_evaluation_function<type>::__conj(x));
				return math_function<decltype(temp)>::__sqrt(temp);
			}
		};
		//unsigned型
		template <>
		struct __abs_struct<true, true> {
			static type __abs(const type& x) {
				return x;
			}
		};
		//signed型
		template <>
		struct __abs_struct<true, false> {
			static type __abs(const type& x) {
				return ((x < 0) ? -x : x);
			}
		};
		static auto __abs(const type& x) {
			return __abs_struct<is_arithmetic<type>::value, is_unsigned<type>::value>::__abs(x);
		}


		//平方根の整数近似
		static result_type __isqrt(const type& x) {
			result_type	i = 1, a = static_cast<result_type>(x) / 2;
			for (; a > 0; ++i) a -= i;
			return (i - 1);
		}
		//二分法用の補助
		static bool __sqrt_bisection(const result_type& x, const result_type& s1, const result_type& s2) {
			return x * x * s1*s2 + 1 < x*(s1 + s2);
		}
		//平方根
		static result_type __sqrt(const type& x) {
			result_type temp[2] = {}, x1 = (1 / (__isqrt(x) + 1));
			result_type h;

			if (x == 0) return 0;

			//基本的にニュートン法
			do {
				//誤差が大きくなったときは2分法に切り替える
				if (__abs(temp[0] - x1) > __abs(temp[1] - x1)) {
					//x1を初期化
					x1 = (temp[0] + temp[1]) / 2;
					//区間[temp0,temp1]の誤差を双方から考慮する
					while (!error_evaluation(x1, temp[0]) && !error_evaluation(x1, temp[1])) {
						((__sqrt_bisection(x, temp[0] * temp[0], x1*x1)) ? temp[1] : temp[0]) = x1;
						x1 = (temp[0] + temp[1]) / 2;
					}
					return x1 * x;
				}
				temp[1] = temp[0];
				temp[0] = x1;
				h = 1 - x * x1*x1;
				//五次
				x1 *= (1 + (64 * h + h * h*(48 + 40 * h + 35 * h*h)) / 128);
				//四次
				//x1 *= (1 + h*(8 + h*(6 + 5 * h)) / 16);
				//三次
				//x1 *= (1 + h*(4 + 3 * h) / 8);
				//二次
				//x1 *= (2 + h)*0.5;
			} while (!error_evaluation(x1, temp[0]));

			return x1 * x;
		}

	private:
		//超次元級から一般次元の円周率を計算する
		static result_type __pi_correction(result_type pi, imsize_t n) {
			//無限大での除算の対策
			if (n == 0) return 0;
			if (n == 1) return 2;
			imsize_t c = n >> 1;
			result_type result = pi;
			//偶数次元
			if ((n & 1) == 0) {
				for (imsize_t i = 2; i <= c; ++i)
					result *= pi / (4 * (i - 1));
			}
			//奇数次元
			else {
				for (imsize_t i = 2; i <= c; ++i)
					result *= pi / (2 * (2 * i - 1));
			}
			return result;
		}
	public:
		//円周率(ガウス=ルジャンドルのアルゴリズム)
		static result_type __pi(imsize_t n = 2) {
			result_type a = 1, b = __sqrt(2) / 2, t = 0.25, p = 1;
			result_type temp_a = (a + b) / 2;
			imsize_t cnt = limits<result_type>::digits10;			//log2(cnt)程度の回数だけ演算するためのカウンタ
			do {
				temp_a = (a + b) / 2;
				b = math_function<result_type>::__sqrt(a*b);
				t -= p * (a - temp_a)*(a - temp_a);
				p *= 2;
				a = temp_a;
			} while ((cnt >>= 1) != 1);
			//n次元の円周率を求める
			return __pi_correction((a + b)*(a + b) / (4 * t), n);
		}
		//ネイピア数(テイラー展開)
		static result_type __e() {
			result_type a = 1, an = 1, b = 0;
			for (imsize_t i = 1; !error_evaluation(a, b); ++i) {
				b = a;
				an /= i;
				a += an;
			}
			return b;
		}
		//log_e(2)
		static result_type __log_e_2() {
			result_type a = 1 / result_type(3), an = 1 / result_type(3), b = 1;

			for (imsize_t i = 2; !error_evaluation(a, b); ++i) {
				b = a;
				an /= 9;
				a += an / (2 * i - 1);
			}
			return 2 * b;
		}

		//正弦
		static result_type __sin(type x) {
			result_type x1 = x, x2 = x, buf = addition<result_type>::identity_element();
			x *= x;

			for (imsize_t i = 1; !error_evaluation(x2, buf); ++i) {
				buf = x2;
				x1 *= -(result_type(x) / (2 * i*(2 * i + 1)));
				x2 += x1;
			}
			return x2;
		}
		//余弦
		static result_type __cos(type x) {
			result_type x1 = 1, x2 = 1, buf = 0;
			x *= x;

			for (imsize_t i = 1; !error_evaluation(x2, buf); ++i) {
				buf = x2;
				x1 *= -(result_type(x) / (2 * i*(2 * i - 1)));
				x2 += x1;
			}
			return x2;
		}
		//正接
		static result_type __tan(const type& x) {
			return __sin(x) / __cos(x);
		}
		//逆正弦の主値
		static result_type __asin(const type& x) {
			static const result_type _pi = __pi();
			result_type x1 = x, x2 = x, buf = 0;
			result_type c = x * x;

			if (x == 1) return (_pi / 2);
			if (x == -1) return (-_pi / 2);

			for (imsize_t i = 1; !error_evaluation(x2, buf); ++i) {
				buf = x2;
				x1 *= c * (2 * i - 1) / (2 * i);
				x2 += x1 / (2 * i + 1);
			}
			return x2;
		}
		//逆余弦の主値
		static result_type __acos(const type& x) {
			static const result_type _pi2 = __pi() / 2;
			return (_pi2 - __asin(x));
		}
		//逆正接の主値
		static result_type __atan(const type& x) {
			return math_function<result_type>::__asin(x / __sqrt(1 + x * x));
		}
		//座標平面上の逆正接
		static result_type __atan2(const type& y, const type& x) {
			static const result_type _pi = __pi();

			if (x == 0)			//0と等しい
				return (y == 0) ? 0 : ((y < 0) ? -_pi / 2 : _pi / 2);
			else if (y == 0)
				return (x < 0) ? -_pi : 0;
			else if (x > 0)					//0以上(このときは0より大きい)
				return math_function<result_type>::__atan(result_type(y) / x);
			else
				return math_function<result_type>::__atan(result_type(y) / x) + ((y < 0) ? -_pi : _pi);
		}
		//指数関数
		static result_type __exp(type x) {
			static const result_type _e = __e();

			if (is_nan<result_type>(x)) return limits<result_type>::quiet_nan();
			else if (is_positive_inf<result_type>(x)) return limits<result_type>::positive_infinity();
			else if (is_negative_inf<result_type>(x)) return addition<result_type>::identity_element();

			result_type x1 = 1, x2 = 1, buf = 0;
			result_type c = x;
			imint_t index = static_cast<imint_t>(x);
			c -= index;
			c /= 32;			//早く収束させるために十分にcを小さくする(最大c=0.03125)
			for (imsize_t i = 1; !error_evaluation(x2, buf); ++i) {
				buf = x2;
				x1 *= (c / i);
				x2 += x1;
			}
			return math_function<result_type>::__pow(x2, 32)*math_function<result_type>::__pow(_e, index);
		}
		//対数関数
		static result_type __log(type x) {
			static const result_type loge2 = __log_e_2();

			if (x == 2) return loge2;
			else if (is_nan<result_type>(x)) return limits<result_type>::quiet_nan();
			else if (is_positive_inf<result_type>(x)) return limits<result_type>::positive_infinity();
			else if (x == 0) return limits<result_type>::negative_infinity();
			else if (x < 0) return limits<result_type>::quiet_nan();

			result_type x1, x2, buf = 0;
			result_type c = x;
			imint_t index = 0;			//冪指数

			//2の冪指数の取得(xを1に近くする)
			if (c < 0.75) {
				result_type temp = 1;
				for (imsize_t i = 0;; ++i, temp *= 2) {
					if (c*temp >= 0.75) {
						c *= temp;
						index -= i;
						break;
					}
				}
			}
			while (c > 2) {
				imsize_t temp = static_cast<imsize_t>(c), i;
				//tempより小さい最大の2の冪乗数を求める
				for (i = 1; (temp >> i) != 1; ++i);
				c /= 1 << i;
				index += i;
			}
			if (c > 1.5) { c /= 2; ++index; }

			c = (c - 1) / (1 + c);
			x1 = x2 = c; c *= c;
			for (imsize_t i = 2; !error_evaluation(x2, buf); ++i) {
				buf = x2;
				x1 *= c;
				x2 += x1 / (2 * i - 1);
			}
			return (2 * x2 + index * loge2);
		}
		static result_type __log10(const type& x) {
			static const result_type loge10 = __log(10);
			return (__log(x) / loge10);
		}
	private:
		//冪乗の分岐のための構造体
		//一般
		template <class S, bool flag1 = is_same<S, imint_t>::value, bool flag2 = is_same<S, imsize_t>::value>
		struct __pow_struct {
			static result_type __pow(const type& x, const type& y) {
				if (x == 0 && y == 0) return 1;
				if (x == 0) return 0;
				return math_function<result_type>::__exp(y*__log(x));
			}
		};
		//imint_t
		template <class S>
		struct __pow_struct<S, true, false> {
			static result_type __pow(const type& x, imint_t y) {
				if (x == 0 && y == 0) return 1;
				if (x == 0) return 0;
				if (y == 0) return 1;
				if (y == 1) return x;
				if (y == 2) return x * x;
				if (y < 0) {
					if ((y & 1) == 0) return math_function<result_type>::__pow(1 / __pow(x, (-y) >> 1), 2);
					else return math_function<result_type>::__pow(1 / __pow(x, ((-y - 1)) >> 1), 2) / x;
				}
				if ((y & 1) == 0) return __pow(__pow(x, y >> 1), 2);
				else return x * __pow(__pow(x, (y - 1) >> 1), 2);
			}
		};
		//imsize_t
		template <class S>
		struct __pow_struct<S, false, true> {
			//typeが群なら演算可能ため戻り値型はtype
			static type __pow(const type& x, imsize_t y) {
				if (x == 0 && y == 0) return 1;
				if (x == 0) return 0;
				if (y == 0) return 1;
				if (y == 1) return x;
				if (y == 2) return x * x;
				if ((y & 1) == 0) return __pow(__pow(x, y >> 1), 2);
				else return x * __pow(__pow(x, (y - 1) >> 1), 2);
			}
		};
	public:
		//冪乗
		template <class S>
		static auto __pow(const type& x, const S& y) {
			return __pow_struct<S>::__pow(x, y);
		}

		//双曲線関数
		static result_type __sinh(const type& x) {
			return ((__exp(x) - __exp(-x)) * type(0.5));
		}
		static result_type __cosh(const type& x) {
			return ((__exp(x) + __exp(-x)) * type(0.5));
		}
		static result_type __tanh(const type& x) {
			result_type x1 = __exp(x), x2 = __exp(-x);
			return ((x1 - x2) / (x1 + x2));
		}
		//逆双曲線関数
		static result_type __asinh(const type& x) {
			result_type x1 = (x < 0) ? -x : x;
			x1 = math_function<result_type>::__log(x1 + __sqrt(x1*x1 + 1));
			return ((x < 0) ? -x1 : x1);
		}
		static result_type __acosh(const type& x) {
			return math_function<result_type>::__log(x + __sqrt(x*x - 1));
		}
		static result_type __atanh(const type& x) {
			return (math_function<result_type>::__log((1 + x) / (1 - x)) / 2);
		}
		//誤差関数
		static result_type __erf(const type& x) {
			result_type x1 = x, x2 = x, buf = 0;
			result_type c = x * x;
			static const result_type _2spi = 2 / math_function<result_type>::__sqrt(__pi());

			for (imsize_t i = 1; !error_evaluation(x2, buf); ++i) {
				buf = x2;
				x1 *= -c / i;
				x2 += x1 / (2 * i + 1);
			}
			return (_2spi*x2);
		}
		//相補誤差関数
		static result_type __erfc(const type& x) {
			return (1 - __erf(x));
		}

		//二項係数
		static result_type __comb(imsize_t n, imsize_t k) {
			result_type result = 1;
			if (k > n) return 0;
			for (imsize_t i = 1; i <= k; ++i)
				result *= result_type(n - i + 1) / i;
			return result;
		}
		//ベルヌーイ数
		static result_type __bernoulli_number(imsize_t n) {
			result_type *Bk;
			result_type x1, x2;
			if ((n != 1) && ((n & 1) == 1)) return 0;			//1でない奇数のときは0
			else if (n == 0) return 1;
			else if (n == 1) return -0.5;
			Bk = new result_type[n + 1];
			Bk[0] = 1; Bk[1] = -0.5;
			for (imsize_t i = 2; i <= n; ++i) {
				if ((i & 1) == 1) {
					Bk[i] = 0;
					continue;
				}
				//Bk[2n+1]=0(n != 0)を考慮したもの
				x1 = Bk[0];
				x2 = Bk[0] + (i + 1)*Bk[1];
				for (imsize_t j = 2; j <= i - 1; j += 2) {
					x1 *= Bk[j] / Bk[j - 2] * (i + 2 - j) / j * (i + 2 - (j - 1)) / (j - 1);
					x2 += x1;
				}
				/*x2 = Bk[0] + (i + 1)*Bk[1];
				for (size_t j = 2; j <= i - 1; j += 2)
				x2 += comb<N + 4>(i + 1, j)*Bk[j];
				*/
				Bk[i] = -x2 / (i + 1);
			}
			x2 = Bk[n];
			delete[] Bk;
			return x2;
		}
		//sinc関数(sin(x)/x)
		static result_type __sinc(type x) {
			if (__abs(x) >= 1) return __sin(x) / x;				//十分にxが大きいときsin関数をそのまま使う

			result_type x1 = 1, x2 = 1, buf = 0;
			x *= x;

			for (imsize_t i = 1; !error_evaluation(x2, buf); ++i) {
				buf = x2;
				x1 *= -(x / (2 * i*(2 * i + 1)));
				x2 += x1;
			}
			return x2;
		}
		//バーンスタイン多項式
		static result_type __bernstein_polynomial(const type& x, const type *beta, imsize_t n) {
			result_type x1 = __pow(1 - x, n);
			result_type x2 = beta[0] * x1;
			for (imsize_t i = 1; i <= n; ++i) {
				x1 *= x / (1 - x)*(n + 1 - i) / i;
				x2 += x1 * beta[i];
			}
			return x2;
		}
		//ベジェ曲線
		static result_type __bezier_curve(const type& x, const type *beta, imsize_t n) {
			return __bernstein_polynomial(x, beta, n);
		}
		//シグモイド関数
		static result_type __sigmoid(const type& gain, const type& x) {
			return 1 / (1 + __exp(-gain * x));
		}
		//フレネル積分S(x)
		static result_type __fresnel_integrals_s(type x) {
			result_type x1 = x * x*x, x2 = x * x*x / 3, buf = 0;
			x *= x;
			x *= x;

			for (imsize_t i = 1; !error_evaluation(x2, buf); ++i) {
				buf = x2;
				x1 *= -(x / (2 * i*(2 * i + 1)));
				x2 += x1 / (4 * i + 3);
			}
			return x2;
		}
		//フレネル積分C(x)
		static result_type __fresnel_integrals_c(type x) {
			result_type x1 = x, x2 = x, buf = 0;
			x *= x;
			x *= x;

			for (imsize_t i = 1; !error_evaluation(x2, buf); ++i) {
				buf = x2;
				x1 *= -(x / (2 * i*(2 * i - 1)));
				x2 += x1 / (4 * i + 1);
			}
			return x2;
		}
		//ガンマ関数の自然対数
		static result_type __lgamma(type x) {
			result_type x1 = 0, x2 = 0, buf = 0;
			result_type k = 1, B = 1 / result_type(6), w = 0;
			static const result_type _pi = __pi();
			static const result_type _log_2pi_s2 = math_function<result_type>::__log(2 * _pi) / 2;

			//xが負数であるとき相反公式で反転(log(pi/sin(pi*x))は複素数でないときNanになることもある)
			if (x < 0) return math_function<result_type>::__log(_pi / math_function<result_type>::__sin(_pi * x)) - __lgamma(1 - x);

			//ある程度xを大きくする(最大k=12!=479001600)
			while (__abs(x) < 12) { k *= x;  ++x; }
			x1 = x2 = result_type(1) / (12 * x);
			w = x * x;
			for (imsize_t i = 2; !error_evaluation(x2, buf); ++i) {
				buf = x2;
				x1 /= B;
				B = __bernoulli_number(2 * i);
				x1 *= (2 * i - 2)*(2 * i - 3)*B / (2 * i*(2 * i - 1)*w);
				x2 += x1;
				//計算精度的都合で不可
				//x1 /= w;
				//x2 += x1*__bernoulli_number(2 * i) / (2 * i*(2 * i - 1));
			}
			//kの補正を入れる
			return (_log_2pi_s2 - x + (x - 0.5)*__log(x) + x2 - __log(k));
		}
		//ガンマ関数
		static result_type __gamma(const type& x) {
			static const result_type _pi = __pi();
			return (x < 0) ? _pi / (math_function<result_type>::__sin(_pi * x) * math_function<result_type>::__exp(__lgamma(1 - x))) : math_function<result_type>::__exp(__lgamma(x));
		}
		//第一種不完全ガンマ関数
		static result_type __gamma1(const type& a, const type& x) {
			result_type x1 = result_type(1) / a, x2 = result_type(1) / a, buf = 0;

			if (x == 0) return 0;
			if (x >= 1 + a) return __gamma(a) - __gamma2(a, x);

			for (imsize_t i = 1; !error_evaluation(x2, buf); ++i) {
				buf = x2;
				x1 *= x / result_type(a + i);
				x2 += x1;
			}
			return x2 * math_function<result_type>::__exp(-x)*math_function<result_type>::__pow(x, a);
		}
		//第二種不完全ガンマ関数
		static result_type __gamma2(const type& a, const type& x) {
			result_type x1 = 1, x2 = 1, buf = 0;
			result_type l1 = 1, l2 = x - a + 1, lbuf;

			if (x < 1 + a) return __gamma(a) - __gamma1(a, x);

			for (imsize_t i = 1; !error_evaluation(x2, buf); ++i) {
				//一般化ラゲール陪多項式の更新
				//lbuf = ((2 * (i + 1) - a - 1 + x)*l2 - ((i + 1) - a - 1)*l1) / (i + 1);
				lbuf = result_type((2 * i - a + 1 + x)*l2 - (i - a)*l1) / (i + 1);
				l1 = l2; l2 = lbuf;

				buf = x2;
				x1 *= result_type(i - a) / (i + 1);
				x2 += x1 / (l1*l2);
			}
			return x2 * math_function<result_type>::__exp(-x)*math_function<result_type>::__pow(x, a);
		}

	private:
		//ディガンマ関数のRe(z)>0の部分の処理
		static result_type __digamma_re(type x) {
			result_type x1 = 0, x2 = 0, buf = 0;
			result_type k = 0, B = 1 / result_type(6), w = 0;

			//ある程度xを大きくする(最大k=12!=479001600)
			while (__abs(x) < 12) { k += 1 / x;  ++x; }
			w = x * x;
			x1 = x2 = 1 / (12 * w);
			for (imsize_t i = 2; !error_evaluation(x2, buf); ++i) {
				buf = x2;
				x1 /= B;
				B = __bernoulli_number(2 * i);
				x1 *= (i - 1)*B / (i*w);
				x2 += x1;
				//計算精度的都合で不可
				//x1 /= w;
				//x2 += x1*__bernoulli_number(2 * i) / (2 * i);
			}
			//kの補正を入れる
			return (__log(x) - 1 / (2 * x) - x2 - k);
		}
	public:
		//ディガンマ関数
		static result_type __digamma(const type& x) {
			static const result_type _pi = __pi();
			return ((x < 0) ? __digamma_re(1 - x) - _pi / math_function<result_type>::__tan(_pi * x) : __digamma_re(x));
		}
		//オイラーの定数
		static result_type __euler_constant() {
			return -__digamma_re(1);
		}
		//ベータ関数
		static result_type __beta(const type& x, const type& y) {
			static const result_type _pi = __pi();
			//相反公式を全ての場合に適応していく(実数の場合において実数の範囲で議論をするため)
			//sin(x+pi)=-sin(x)とsin(x+2pi)=sin(x)を適用してsin(0)のとき等の精度を改善する
			if ((x < 0) && (x + y < 0) && (imint_t(y) == y)) {
				return ((y < 0) ? _pi / math_function<result_type>::__sin(_pi * y) : 1)*
					(((imint_t(y) & 1) == 1) ? -1 : 1)*
					math_function<result_type>::__exp(
						-__lgamma(1 - x)
						+ ((y < 0) ? -__lgamma(1 - y) : __lgamma(y))
						+ __lgamma(1 - x - y)
					);
			}
			if ((y < 0) && (x + y < 0) && (imint_t(x) == x)) {
				return ((x < 0) ? _pi / math_function<result_type>::__sin(_pi * x) : 1)*
					(((imint_t(x) & 1) == 1) ? -1 : 1)*
					math_function<result_type>::__exp(
					((x < 0) ? -__lgamma(1 - x) : __lgamma(x))
						- __lgamma(1 - y)
						+ __lgamma(1 - x - y)
					);
			}
			return ((x < 0) ? _pi / math_function<result_type>::__sin(_pi * x) : 1)*
				((y < 0) ? _pi / math_function<result_type>::__sin(_pi * y) : 1)*
				((x + y < 0) ? math_function<result_type>::__sin(_pi * (x + y) / _pi) : 1)*
				math_function<result_type>::__exp(
				((x < 0) ? -__lgamma(1 - x) : __lgamma(x))
					+ ((y < 0) ? -__lgamma(1 - y) : __lgamma(y))
					- ((x + y < 0) ? -__lgamma(1 - x - y) : __lgamma(x + y))
				);
			//return math_function<result_type>::__exp(__lgamma(x) + __lgamma(y) - __lgamma(x + y));
		}
		//不完全ベータ関数
		static result_type __beta(const type& a, const type& b, const type& x) {
			result_type x1 = 1, x2 = result_type(1) / a, buf = 0;

			if (x == 0) return 0;
			if (x == 1) return __beta(a, b);
			//自明な特異点
			if (a == 0) return limits<result_type>::quiet_nan();
			if ((b > 0) && (a < 0) && (imint_t(a) == a) && (-a < numerical_evaluation_function<type>::__ceil(b))) return limits<result_type>::quiet_nan();
			//定義不可の領域を調整
			if (b == -a) return __pow(x, a)*__pow(1 - x, b) / a;

			if (x > 0.5) return __beta(a, b) - __beta(b, a, 1 - x);

			for (imsize_t i = 1; !error_evaluation(x2, buf); ++i) {
				buf = x2;
				x1 *= -result_type(b - i) / i * x;
				x2 += x1 / (i + a);
			}
			return __pow(x, a)*x2;
		}
		//正則ベータ関数
		static result_type __regularized_beta(const type& a, const type& b, const type& x) {
			return (x > 0.5) ? (1 - __beta(b, a, 1 - x)) / __beta(a, b) : __beta(a, b, x) / __beta(a, b);
		}
		//ディリクレのイータ関数
		static result_type __dirichlet_eta(const type& x) {
			result_type x2 = 0.5, x1 = 0.5, x3 = 1, x4 = 1, buf = 0;
			static const result_type _pi = __pi();

			//負数のときは相反公式で反転(計算精度的都合上)
			if (x < 0) return __pow(2, x)*math_function<result_type>::__pow(_pi, x - 1)*math_function<result_type>::__sin(_pi*x / 2)
				*__gamma(1 - x)*__dirichlet_eta(1 - x);
			for (imsize_t i = 1; !error_evaluation(x2, buf); ++i) {
				buf = x2;
				x1 /= 2;
				x3 = x4 = x1;
				for (imsize_t j = 1; j <= i; ++j) {
					x3 *= -result_type(i + 1 - j) / j;
					x4 += x3 * __pow(j + 1, -x);
				}
				x2 += x4;
			}
			return x2;
		}
		//リーマンゼータ関数
		static result_type __riemann_zeta(const type& x) {
			//相反公式の適用を考慮する
			//return __dirichlet_eta(x) / (1 - __pow(2, 1 - ((x < 0) ? 1 - x : x)));
			return __dirichlet_eta(x) / (1 - __pow(2, ((x < 0) ? x : 1 - x)));
		}
		//リーマンのクシー関数
		static result_type __riemann_xi(const type& x) {
			static const result_type _pi = __pi();
			return x * (x - 1)*math_function<result_type>::__pow(_pi, -result_type(x) / 2)
				*math_function<result_type>::__gamma(result_type(x) / 2)*__riemann_zeta(x) / 2;
		}

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
		//指数積分
		static result_type __expint(const type& x) {
			static const result_type euler = __euler_constant();
			result_type x1 = x, x2 = x, buf = 0;

			for (imsize_t i = 2; !error_evaluation(x2, buf); ++i) {
				buf = x2;
				x1 *= (x*(i - 1) / (i*i));
				x2 += x1;
			}
			return euler + __log(x) + x2;
		}
		//対数積分
		static result_type __logint(const type& x) {
			return (x == 0) ? 0 : math_function<result_type>::__expint(__log(x));
		}
		//正弦積分
		static result_type __sinint(type x) {
			result_type x1 = x, x2 = x, buf = 0;
			x *= x;

			for (imsize_t i = 1; !error_evaluation(x2, buf); ++i) {
				buf = x2;
				x1 *= -(x *(2 * i - 1) / (2 * i*(2 * i + 1)*(2 * i + 1)));
				x2 += x1;
			}
			return x2;
		}
		//余弦積分
		static result_type __cosint(type x) {
			static const result_type euler = __euler_constant();
			result_type x1 = -x * x / 4, x2 = -x * x / 4, buf = 0, logx = __log(x);
			x *= x;

			for (imsize_t i = 2; !error_evaluation(x2, buf); ++i) {
				buf = x2;
				x1 *= -(x *(i - 1) / (2 * i*i*(2 * i - 1)));
				x2 += x1;
			}
			return euler + logx + x2;
		}

		//ラゲールの多項式(L_{n}(x))
		static result_type __laguerre(imsize_t n, const type& x) {
			result_type x1 = 1, x2 = 1;
			result_type c = x;
			for (imsize_t i = 1; i <= n; ++i) {
				x1 *= -c * (n - i + 1) / (i*i);
				x2 += x1;
			}
			return x2;
		}
		//ラゲールの陪多項式(L_{n}^{k}(x))
		static result_type __associated_laguerre(imsize_t n, imsize_t k, const type& x) {
			result_type x1 = 1, x2 = -x + k + 1;
			result_type buf;
			for (imsize_t i = 2; i <= n; ++i) {
				buf = ((2 * n + k - 1 - x)*x2 - (n + k - 1)*x1) / n;
				x1 = x2; x2 = buf;
			}
			return (n == 0) ? 1 : x2;
		}

		//ルジャンドルの多項式(P_{k}(x))
		static result_type __legendre(imsize_t k, const type& x) {
			result_type x1 = k + 1, x2;
			result_type c = x;
			//初期値の構成
			for (imsize_t i = 2; i <= k; ++i)
				x1 *= result_type(k + i) / i;
			x2 = x1 * __pow(x, k);
			for (imsize_t i = 1; i <= k / 2; ++i) {
				x1 *= -result_type(k - i + 1)*(k - 2 * i + 2)*(k - 2 * i + 2 - 1) / (i*(2 * k - 2 * i + 2)*(2 * k - 2 * i + 2 - 1));
				x2 += x1 * __pow(x, k - 2 * i);			//0^0=1を用いるため
			}
			return x2 / __pow(2, k);
		}
		//ルジャンドルの陪関数(P_{k}^{m}(x))
		static result_type __associated_legendre(imsize_t k, imsize_t m, const type& x) {
			if (k < m) { return 0; }			//例外

			result_type x1 = k + 1, x2;
			result_type c = x * x;
			//初期値の構成
			for (imsize_t i = 2; i <= k - m; ++i)
				x1 *= result_type(k + i) / i;
			for (imsize_t i = (numerical_evaluation_function<imsize_t>::__max)(k - m + 1, 2); i <= k; ++i)
				x1 *= (k + i);
			x2 = x1 * __pow(x, k - m);

			for (imsize_t i = 1; i <= (k - m) / 2; ++i) {
				x1 *= -result_type(k - i + 1)*(k - 2 * i + 2 - m)*(k - 2 * i + 2 - 1 - m) / ((2 * k - 2 * i + 2)*(2 * k - 2 * i + 2 - 1)*i);
				x2 += x1 * __pow(x, k - 2 * i - m);
			}
			return x2 * math_function<result_type>::__pow(1 - c, result_type(m) / 2) / __pow(2, k);
		}


		/*//ルジャンドルの第一種完全楕円積分
		template <class T>
		inline T ellint_1(const T& k) {

		}
		//ルジャンドルの第一種楕円積分
		template <class T>
		inline T ellint_1(const T& phi, const T& k) {

		}
		//ルジャンドルの第二種完全楕円積分
		template <class T>
		inline T ellint_2(const T& k) {

		}
		//ルジャンドルの第二種楕円積分
		template <class T>
		inline T ellint_2(const T& phi, const T& k) {

		}
		//ルジャンドルの第三種楕円積分
		template <class T>
		inline T ellint_3(const T& n, const T& phi, const T& k) {

		}*/
	};
}

//数学関数
namespace iml {
	
	/*//切り上げ(天井関数)
	template <class T>
	inline auto ceil(const T& x) { return numerical_evaluation_function<T>::__ceil(x); }
	//切り捨て(床関数)
	template <class T>
	inline auto floor(const T& x) { return numerical_evaluation_function<T>::__floor(x); }
	//四捨五入
	template <class T>
	inline auto round(const T& x) { return numerical_evaluation_function<T>::__round(x); }
	//剰余
	template <class T>
	inline auto mod(const T& x, const T& y) { return numerical_evaluation_function<T>::__mod(x, y); }
	*///最大値
	template <class T>
	inline auto (max)(const T& x, const T& y) { return (numerical_evaluation_function<T>::__max)(x, y); }
	//最小値
	template <class T>
	inline auto (min)(const T& x, const T& y) { return (numerical_evaluation_function<T>::__min)(x, y); }
	/*//共役
	template <class T>
	inline auto conj(const T& x) { return numerical_evaluation_function<T>::__conj(x); }

	//絶対値
	template <class T>
	inline auto abs(const T& x) { return math_function<T>::__abs(x); }
	//平方根の整数近似
	template <class T>
	inline auto isqrt(const T& x) { return math_function<T>::__isqrt(x); }
	//平方根
	template <class T>
	inline auto sqrt(const T& x) { return math_function<T>::__sqrt(x); }

	//円周率
	template <class T>
	inline auto pi(imsize_t n = 2) { return math_function<T>::__pi(n); }
	//ネイピア数(テイラー展開)
	template <class T>
	inline auto e() { return math_function<T>::__e(); }
	//log_e(2)
	template <class T>
	inline auto log_e_2() { return math_function<T>::__log_e_2(); };

	//正弦
	template <class T>
	inline auto sin(const T& x) { return math_function<T>::__sin(x); }
	//余弦
	template <class T>
	inline auto cos(const T& x) { return math_function<T>::__cos(x); }
	//正接
	template <class T>
	inline auto tan(const T& x) { return math_function<T>::__tan(x); }
	//逆正弦の主値
	template <class T>
	inline auto asin(const T& x) { return math_function<T>::__asin(x); }
	//逆余弦の主値
	template <class T>
	inline auto acos(const T& x) { return math_function<T>::__acos(x); }
	//逆正接の主値
	template <class T>
	inline auto atan(const T& x) { return math_function<T>::__atan(x); }
	//座標平面上の逆正接
	template <class T>
	inline auto atan2(const T& y, const T& x) { return math_function<T>::__atan2(x, y); }
	//指数関数
	template <class T>
	inline auto exp(const T& x) { return math_function<T>::__exp(x); }
	//対数関数
	template <class T>
	inline auto log(const T& x) { return math_function<T>::__log(x); }
	template <class T>
	inline auto log10(const T& x) { return math_function<T>::__log10(x); }
	//冪乗
	template <class T, class S>
	inline auto pow(const T& x, const S& y) { return math_function<T>::__pow(x, y); }
	//双曲線関数
	template <class T>
	inline auto sinh(const T& x) { return math_function<T>::__sinh(x); }
	template <class T>
	inline auto cosh(const T& x) { return math_function<T>::__cosh(x); }
	template <class T>
	inline auto tanh(const T& x) { return math_function<T>::__tanh(x); }
	//逆双曲線関数
	template <class T>
	inline auto asinh(const T& x) { return math_function<T>::__asinh(x); }
	template <class T>
	inline auto acosh(const T& x) { return math_function<T>::__acosh(x); }
	template <class T>
	inline auto atanh(const T& x) { return math_function<T>::__atanh(x); }
	//誤差関数
	template <class T>
	inline auto erf(const T& x) { return math_function<T>::__erf(x); }
	//相補誤差関数
	template <class T>
	inline auto erfc(const T& x) { return math_function<T>::__erfc(x); }
	//二項係数
	template <class T>
	inline auto comb(imsize_t n, imsize_t k) { return math_function<T>::__comb(n, k); }
	//ベルヌーイ数
	template <class T>
	inline auto bernoulli_number(imsize_t n) { return math_function<T>::__bernoulli_number(n); }
	//sinc関数(sin(x)/x)
	template <class T>
	inline auto sinc(const T& x) { return math_function<T>::__sinc(x); }
	//バーンスタイン多項式
	template <class T>
	inline auto bernstein_polynomial(const T& x, const T *beta, imsize_t n) { return math_function<T>::__bernstein_polynomial(x, beta, n); }
	//ベジェ曲線
	template <class T>
	inline auto bezier_curve(const T& x, const T *beta, imsize_t n) { return math_function<T>::__bezier_curve(x, beta, n); }
	//シグモイド関数
	template <class T>
	inline auto sigmoid(const T& gain, const T& x) { return math_function<T>::__sigmoid(gain, x); }
	//フレネル積分S(x)
	template <class T>
	inline auto fresnel_integrals_s(const T& x) { return math_function<T>::__fresnel_integrals_s(x); }
	//フレネル積分C(x)
	template <class T>
	inline auto fresnel_integrals_c(const T& x) { return math_function<T>::__fresnel_integrals_c(x); }
	//ガンマ関数の自然対数
	template <class T>
	inline auto lgamma(const T& x) { return math_function<T>::__lgamma(x); }
	//ガンマ関数
	template <class T>
	inline auto gamma(const T& x) { return math_function<T>::__gamma(x); }
	//第一種不完全ガンマ関数
	template <class T>
	inline auto gamma1(const T& a, const T& x) { return math_function<T>::__gamma1(a, x); }
	//第二種不完全ガンマ関数
	template <class T>
	inline auto gamma2(const T& a, const T& x) { return math_function<T>::__gamma2(a, x); }
	//ディガンマ関数
	template <class T>
	inline auto digamma(const T& x) { return math_function<T>::__digamma(x); }
	//オイラーの定数
	template <class T>
	inline auto euler_constant() { return math_function<T>::__euler_constant(); }
	//ベータ関数
	template <class T>
	inline auto beta(const T& x, const T& y) { return math_function<T>::__beta(x, y); }
	template <class T>
	inline auto beta(const T& a, const T& b, const T& x) { return math_function<T>::__beta(a, b, x); }
	template <class T>
	inline auto regularized_beta(const T& a, const T& b, const T& x) { return math_function<T>::__regularized_beta(a, b, x); }
	//ディリクレのイータ関数
	template <class T>
	inline auto dirichlet_eta(const T& x) { return math_function<T>::__dirichlet_eta(x); }
	//リーマンゼータ関数
	template <class T>
	inline auto riemann_zeta(const T& x) { return math_function<T>::__riemann_zeta(x); }
	//リーマンのクシー関数
	template <class T>
	inline auto riemann_xi(const T& x) { return math_function<T>::__riemann_xi(x); }
	//超幾何級数(r:分子の数,s:分母の数,_r:r個の要素,_s:s個の要素,z)
	template <class T>
	inline auto hypergeometric_series(imsize_t r, imsize_t s, const T* _r, const T* _s, const T& z) {
		return math_function<T>::__hypergeometric_series(r, s, _r, _s, z);
	}
	//指数積分
	template <class T>
	inline auto expint(const T& x) { return math_function<T>::__expint(x); }
	//指数積分
	template <class T>
	inline auto logint(const T& x) { return math_function<T>::__logint(x); }
	//正弦積分
	template <class T>
	inline auto sinint(const T& x) { return math_function<T>::__sinint(x); }
	//余弦積分
	template <class T>
	inline auto cosint(const T& x) { return math_function<T>::__cosint(x); }
	//ラゲールの多項式(L_{n}(x))
	template <class T>
	inline auto laguerre(imsize_t n, const T& x) { return math_function<T>::__laguerre(n, x); }
	//ラゲールの陪多項式(L_{n}^{k}(x))
	template <class T>
	inline auto associated_laguerre(imsize_t n, imsize_t k, const T& x) { return math_function<T>::__associated_laguerre(n, k, x); }
	//ルジャンドルの多項式(P_{k}(x))
	template <class T>
	inline auto  legendre(imsize_t k, const T& x) { return math_function<T>::__legendre(k, x); }
	//ルジャンドルの陪関数(P_{k}^{m}(x))
	template <class T>
	static auto associated_legendre(imsize_t k, imsize_t m, const T& x) { return math_function<T>::__associated_legendre(k, m, x); }
	*/
}


namespace iml {

	//∇とテンソル等の二項演算の定義で使うかもしれない

	//二項演算(T1×T2→T3という意味で用いる)
	template <class T1, class T2>
	struct binary_operation {
		using first_type = T1;
		using second_type = T2;

		//加法
		static auto addition(const first_type& x1, const second_type& x2) {
			return x1 + x2;
		}
		//減法
		static auto subtraction(const first_type& x1, const second_type& x2) {
			return x1 + iml::addition<second_type>::inverse_element(x2);
		}
		//乗法
		static auto multiplication(const first_type& x1, const second_type& x2) {
			return x1 * x2;
		}
		//除法
		static auto division(const first_type& x1, const second_type& x2) {
			return x1 * multiplicative<second_type>::inverse_element(x2);
		}
		//内積
		static auto inner_product(const first_type& x1, const second_type& x2) {
			return x1 * conj(x2);
		}
	};
}


#endif