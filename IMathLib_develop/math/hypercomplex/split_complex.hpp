#ifndef IMATH_MATH_HYPERCOMPLEX_SPLIT_COMPLEX_HPP
#define IMATH_MATH_HYPERCOMPLEX_SPLIT_COMPLEX_HPP

#include "IMathLib/utility/utility/tuple_type.hpp"
#include "IMathLib/math/math/math_traits.hpp"
#include "IMathLib/math/math/type_parameter.hpp"
#include "IMathLib/container/array.hpp"


namespace iml {

	template <class>
	class split_complex;


	//分解型複素数型のパラメータ
	template <class T, class Re, class Im>
	using split_complex_parameter = type_parameter<split_complex<T>, type_tuple<Re, Im>>;
	template <class T, class ReP, class ImP>
	struct type_parameter<split_complex<T>, type_tuple<type_parameter<T, ReP>, type_parameter<T, ImP>>> {
		using type = split_complex<T>;
		static constexpr type value = type(type_parameter<T, ReP>::value, type_parameter<T, ImP>::value);

		template <class = enable_if_t<is_exist_additive_inverse_v<T>>>
		auto operator-() const { return split_complex_parameter<T, decltype(-type_parameter<T, ReP>()), decltype(-type_parameter<T, ImP>())>(); }
		auto operator+() const { return *this; }

		template <class T, T Val, class = enable_if_t<(Val >= 0) && (Val < 2)>>
			auto operator[](int_parameter<T, Val>) const { return at_type_tuple_t<Val, type_tuple<type_parameter<T, ReP>, type_parameter<T, ImP>>>(); }
	};


	template <class Base, class T, bool = is_math_type_has_base<T>::value, bool = is_same<Base, T>::value>
	class split_complex_base;


	//2項演算の十分条件を補完するメタ関数の定義
	namespace dec {
		//[name]1 : M(T1)×M(T2)			//上位の数学型の場合をも含む
		//[name]2 : M(T1)×T2
		//[name]3 : T1×M(T2)

#define SPLIT_COMPLEX_DECISION(NAME, TERMS)\
		template <bool, class T1, class T2> struct split_complex_##NAME : cat_bool<TERMS> {};\
		template <class T1, class T2> struct split_complex_##NAME##<false, T1, T2> : false_type {};\
		template <bool F, class T1, class T2> constexpr bool split_complex_##NAME##_v = split_complex_##NAME##<F, T1, T2>::value;
		SPLIT_COMPLEX_DECISION(add1, (is_calcable<T1, T2>::add_value && addition_traits<T1>::commutative_value && addition_traits<T2>::commutative_value));
		SPLIT_COMPLEX_DECISION(add2, (is_inclusion_v<T1, add_result_t<T1, T2>> && addition_traits<T1>::commutative_value && addition_traits<T2>::commutative_value));
		SPLIT_COMPLEX_DECISION(add3, (is_inclusion_v<T2, add_result_t<T1, T2>> && addition_traits<T1>::commutative_value && addition_traits<T2>::commutative_value));
		SPLIT_COMPLEX_DECISION(sub1, (is_calcable<T1, T2>::sub_value && addition_traits<T1>::commutative_value && addition_traits<T2>::commutative_value));
		SPLIT_COMPLEX_DECISION(sub2, (is_inclusion_v<T1, sub_result_t<T1, T2>> && addition_traits<T1>::commutative_value && addition_traits<T2>::commutative_value));
		SPLIT_COMPLEX_DECISION(sub3, (is_inclusion_v<T2, sub_result_t<T1, T2>> && addition_traits<T1>::commutative_value && addition_traits<T2>::commutative_value));
		SPLIT_COMPLEX_DECISION(mul1, (is_calcable<T1, T2>::mul_value && is_ring_v<T1> && is_ring_v<T2>));
		SPLIT_COMPLEX_DECISION(mul2, (is_calcable<T1, T2>::mul_value && is_ring_v<T2>));
		SPLIT_COMPLEX_DECISION(mul3, (is_calcable<T1, T2>::mul_value && is_ring_v<T1>));
		SPLIT_COMPLEX_DECISION(div2, (is_calcable<T1, T2>::div_value && is_skew_field_v<T2> && is_ring_v<T1>));
		SPLIT_COMPLEX_DECISION(eq1, (is_calcable<T1, T2>::eq_value));
		SPLIT_COMPLEX_DECISION(eq2, (is_calcable<T1, T2>::eq_value));
		SPLIT_COMPLEX_DECISION(eq3, (is_calcable<T1, T2>::eq_value));
#undef SPLIT_COMPLEX_DECISION
	}


	//分解型複素数型
	template <class T>
	class split_complex : public value_list_input<array_iterator<T>> {
		template <class> friend class split_complex;
		T x_m[2];
	public:
		constexpr split_complex() : x_m{} {}
		constexpr split_complex(const T& re) : x_m{ re } {}
		template <class U, class = enable_if_t<is_inclusion_v<U, T>>>
		constexpr split_complex(const U& re) : x_m{ re } {}
		constexpr split_complex(const T& re, const T& im) : x_m{ re,im } {}
		template <class U, class = enable_if_t<is_inclusion_v<U, T>>>
		constexpr split_complex(const U& re, const U& im) : x_m{ re,im } {}
		constexpr split_complex(const split_complex& c) : x_m{ c.x_m[0],c.x_m[1] } {}
		template <class U, class = enable_if_t<is_inclusion_v<U, T>>>
		constexpr split_complex(const split_complex<U>& c) : x_m{ c.x_m[0],c.x_m[1] } {}

		using base_type = T;
		using iterator = array_iterator<T>;
		using const_iterator = array_iterator<const T>;

		template <class Other>
		struct rebind {
			using other = split_complex<Other>;
		};
		template <class Other>
		using rebind_t = split_complex<Other>;

		constexpr iterator begin() noexcept { return iterator(x_m); }
		constexpr const_iterator begin() const noexcept { return const_iterator(x_m); }
		constexpr iterator end() noexcept { return iterator(x_m + 2); }
		constexpr const_iterator end() const noexcept { return const_iterator(x_m + 2); }

		//単項演算
		template <class = enable_if_t<is_exist_additive_inverse_v<T>>>
		constexpr split_complex operator-() const { return split_complex(-this->x_m[0], -this->x_m[1]); }
		constexpr split_complex operator+() const { return split_complex(*this); }
		//代入演算
		split_complex& operator=(const split_complex& c) {
			if (this != addressof(c)) { this->x_m[0] = c.x_m[0]; this->x_m[1] = c.x_m[1]; }
			return *this;
		}
		template <class U, class = enable_if_t<is_inclusion_v<U, T>>>
		split_complex& operator=(const split_complex<U>& c) {
			this->x_m[0] = c.x_m[0]; this->x_m[1] = c.x_m[1];
			return *this;
		}
		template <class U, class = enable_if_t<is_inclusion_v<U, T>>>
		split_complex& operator=(const U& n) {
			this->x_m[0] = n; this->x_m[1] = addition_traits<T>::identity_element();
			return *this;
		}
		template <class U, class = enable_if_t<dec::split_complex_add1_v<is_operation<T, U, T>::add_value && !is_rscalar_operation_v<split_complex, split_complex<U>>, T, U>>>
		split_complex& operator+=(const split_complex<U>& c) {
			this->x_m[0] += c.x_m[0]; this->x_m[1] += c.x_m[1];
			return *this;
		}
		template <class U, class = enable_if_t<dec::split_complex_add2_v<is_operation<T, U, T>::add_value && is_rscalar_operation_v<split_complex, U>, T, U>>>
		split_complex& operator+=(const U& n) {
			this->x_m[0] += n;
			return *this;
		}
		template <class U, class = enable_if_t<dec::split_complex_sub1_v<is_operation<T, U, T>::sub_value && !is_rscalar_operation_v<split_complex, split_complex<U>>, T, U>>>
		split_complex& operator-=(const split_complex<U>& c) {
			this->x_m[0] -= c.x_m[0]; this->x_m[1] -= c.x_m[1];
			return *this;
		}
		template <class U, class = enable_if_t<dec::split_complex_sub2_v<is_operation<T, U, T>::sub_value && is_rscalar_operation_v<split_complex, U>, T, U>>>
		split_complex& operator-=(const U& n) {
			this->x_m[0] -= n;
			return *this;
		}
		template <class U, class = enable_if_t<dec::split_complex_mul1_v<is_operation<T, U, T>::mul_value && !is_rscalar_operation_v<split_complex, split_complex<U>>, T, U>>>
		split_complex& operator*=(const split_complex<U>& c) {
			T temp[2] = { this->x_m[0] * c.x_m[0] + this->x_m[1] * c.x_m[1], this->x_m[0] * c.x_m[1] + this->x_m[1] * c.x_m[0] };
			this->x_m[0] = temp[0]; this->x_m[1] = temp[1];
			return *this;
		}
		template <class U, class = enable_if_t<dec::split_complex_mul2_v<is_operation<T, U, T>::mul_value && is_rscalar_operation_v<split_complex, U>, T, U>>>
		split_complex& operator*=(const U& k) {
			this->x_m[0] *= k; this->x_m[1] *= k;
			return *this;
		}
		template <class U, class = enable_if_t<dec::split_complex_div2_v<is_operation<T, U, T>::div_value && is_rscalar_operation_v<split_complex, U>, T, U>>>
		split_complex& operator/=(const U& k) {
			this->x_m[0] /= k; this->x_m[1] /= k;
			return *this;
		}
		//2項演算
		using value_list_input<array_iterator<T>>::operator<<;

		//添え字演算
		const constexpr T& operator[](size_t index) const { return this->x_m[index]; }
		constexpr T& operator[](size_t index) { return this->x_m[index]; }

		//ストリーム出力
		friend std::ostream& operator<<(std::ostream& os, const split_complex& n) {
			os << '(' << n.x_m[0] << ',' << n.x_m[1] << ')';
			return os;
		}
		friend std::wostream& operator<<(std::wostream& os, const split_complex& n) {
			os << L'(' << n.x_m[0] << L',' << n.x_m[1] << L')';
			return os;
		}
	};


	template <class T1, class T2, class = enable_if_t<dec::split_complex_add1_v<!is_scalar_operation_v<split_complex<T1>, split_complex<T2>>, T1, T2>>>
	inline constexpr auto operator+(const split_complex<T1>& lhs, const split_complex<T2>& rhs) {
		return split_complex<add_result_t<T1, T2>>(lhs[0] + rhs[0], lhs[1] + rhs[1]);
	}
	template <class T1, class T2, class Re1, class Im1, class Re2, class Im2, class = enable_if_t<dec::split_complex_add1_v<!is_scalar_operation_v<split_complex<T1>, split_complex<T2>>, T1, T2>>>
	inline auto operator+(split_complex_parameter<T1, Re1, Im1>, split_complex_parameter<T2, Re2, Im2>) {
		return split_complex_parameter<add_result_t<T1, T2>, decltype(Re1() + Re2()), decltype(Im1() + Im2())>();
	}
	template <class T1, class T2, class = enable_if_t<dec::split_complex_add2_v<is_rscalar_operation_v<split_complex<T1>, T2>, T1, T2>>>
	inline constexpr auto operator+(const split_complex<T1>& lhs, const T2& rhs) {
		return split_complex<add_result_t<T1, T2>>(lhs[0] + rhs, lhs[1]);
	}
	template <class T1, class T2, class Re, class Im, class Param, class = enable_if_t<dec::split_complex_add2_v<is_rscalar_operation_v<split_complex<T1>, T2>, T1, T2>>>
	inline auto operator+(split_complex_parameter<T1, Re, Im>, type_parameter<T2, Param> rhs) {
		return split_complex_parameter<add_result_t<T1, T2>, decltype(Re() + rhs), Im>();
	}
	template <class T1, class T2, class = enable_if_t<dec::split_complex_add3_v<is_lscalar_operation_v<T1, split_complex<T2>>, T1, T2>>>
	inline constexpr auto operator+(const T1& lhs, const split_complex<T2>& rhs) {
		return split_complex<add_result_t<T1, T2>>(lhs + rhs[0], rhs[1]);
	}
	template <class T1, class T2, class Param, class Re, class Im, class = enable_if_t<dec::split_complex_add3_v<is_lscalar_operation_v<T1, split_complex<T2>>, T1, T2>>>
	inline auto operator+(type_parameter<T1, Param> lhs, split_complex_parameter<T2, Re, Im>) {
		return split_complex_parameter<add_result_t<T1, T2>, decltype(lhs + Re()), Im>();
	}

	template <class T1, class T2, class = enable_if_t<dec::split_complex_sub1_v<!is_scalar_operation_v<split_complex<T1>, split_complex<T2>>, T1, T2>>>
	inline constexpr auto operator-(const split_complex<T1>& lhs, const split_complex<T2>& rhs) {
		return split_complex<sub_result_t<T1, T2>>(lhs[0] - rhs[0], lhs[1] - rhs[1]);
	}
	template <class T1, class T2, class Re1, class Im1, class Re2, class Im2, class = enable_if_t<dec::split_complex_sub1_v<!is_scalar_operation_v<split_complex<T1>, split_complex<T2>>, T1, T2>>>
	inline auto operator-(split_complex_parameter<T1, Re1, Im1>, split_complex_parameter<T2, Re2, Im2>) {
		return split_complex_parameter<sub_result_t<T1, T2>, decltype(Re1() - Re2()), decltype(Im1() - Im2())>();
	}
	template <class T1, class T2, class = enable_if_t<dec::split_complex_sub2_v<is_rscalar_operation_v<split_complex<T1>, T2>, T1, T2>>>
	inline constexpr auto operator-(const split_complex<T1>& lhs, const T2& rhs) {
		return split_complex<sub_result_t<T1, T2>>(lhs[0] - rhs, lhs[1]);
	}
	template <class T1, class T2, class Re, class Im, class Param, class = enable_if_t<dec::split_complex_sub2_v<is_rscalar_operation_v<split_complex<T1>, T2>, T1, T2>>>
	inline auto operator-(split_complex_parameter<T1, Re, Im>, type_parameter<T2, Param> rhs) {
		return split_complex_parameter<sub_result_t<T1, T2>, decltype(Re() - rhs), Im>();
	}
	template <class T1, class T2, class = enable_if_t<dec::split_complex_sub3_v<is_lscalar_operation_v<T1, split_complex<T2>>, T1, T2>>>
	inline constexpr auto operator-(const T1& lhs, const split_complex<T2>& rhs) {
		return split_complex<sub_result_t<T1, T2>>(lhs - rhs[0], -rhs[1]);
	}
	template <class T1, class T2, class Param, class Re, class Im, class = enable_if_t<dec::split_complex_sub3_v<is_lscalar_operation_v<T1, split_complex<T2>>, T1, T2>>>
	inline auto operator-(type_parameter<T1, Param> lhs, split_complex_parameter<T2, Re, Im>) {
		return split_complex_parameter<sub_result_t<T1, T2>, decltype(lhs - Re()), decltype(-Im())>();
	}

	template <class T1, class T2, class = enable_if_t<dec::split_complex_mul1_v<!is_scalar_operation_v<split_complex<T1>, split_complex<T2>>, T1, T2>>>
	inline constexpr auto operator*(const split_complex<T1>& lhs, const split_complex<T2>& rhs) {
		return split_complex<mul_result_t<T1, T2>>(lhs[0] * rhs[0] + lhs[1] * rhs[1], lhs[0] * rhs[1] + lhs[1] * rhs[0]);
	}
	template <class T1, class T2, class Re1, class Im1, class Re2, class Im2, class = enable_if_t<dec::split_complex_mul1_v<!is_scalar_operation_v<split_complex<T1>, split_complex<T2>>, T1, T2>>>
	inline auto operator*(split_complex_parameter<T1, Re1, Im1>, split_complex_parameter<T2, Re2, Im2>) {
		return split_complex_parameter<mul_result_t<T1, T2>, decltype(Re1() * Re2() + Im1() * Im2()), decltype(Re1() * Im2() + Im1() * Re2())>();
	}
	template <class T1, class T2, class = enable_if_t<dec::split_complex_mul2_v<is_rscalar_operation_v<split_complex<T1>, T2>, T1, T2>>>
	inline constexpr auto operator*(const split_complex<T1>& lhs, const T2& rhs) {
		return split_complex<mul_result_t<T1, T2>>(lhs[0] * rhs, lhs[1] * rhs);
	}
	template <class T1, class T2, class Re, class Im, class Param, class = enable_if_t<dec::split_complex_mul2_v<is_rscalar_operation_v<split_complex<T1>, T2>, T1, T2>>>
	inline auto operator*(split_complex_parameter<T1, Re, Im>, type_parameter<T2, Param> rhs) {
		return split_complex_parameter<mul_result_t<T1, T2>, decltype(Re() * rhs), decltype(Im() * rhs)>();
	}
	template <class T1, class T2, class = enable_if_t<dec::split_complex_mul3_v<is_rscalar_operation_v<T1, split_complex<T2>>, T1, T2>>>
	inline constexpr auto operator*(const T1& lhs, const split_complex<T2>& rhs) {
		return split_complex<mul_result_t<T1, T2>>(lhs * rhs[0], lhs * rhs[1]);
	}
	template <class T1, class T2, class Param, class Re, class Im, class = enable_if_t<dec::split_complex_mul3_v<is_lscalar_operation_v<T1, split_complex<T2>>, T1, T2>>>
	inline auto operator*(type_parameter<T1, Param> lhs, split_complex_parameter<T2, Re, Im>) {
		return split_complex_parameter<mul_result_t<T1, T2>, decltype(lhs * Re()), decltype(lhs * Im())>();
	}

	template <class T1, class T2, class = enable_if_t<dec::split_complex_div2_v<is_rscalar_operation_v<split_complex<T1>, T2>, T1, T2>>>
	inline constexpr auto operator/(const split_complex<T1>& lhs, const T2& rhs) {
		return split_complex<div_result_t<T1, T2>>(lhs[0] / rhs, lhs[1] / rhs);
	}
	template <class T1, class T2, class Re, class Im, class Param, class = enable_if_t<dec::split_complex_div2_v<is_rscalar_operation_v<split_complex<T1>, T2>, T1, T2>>>
	inline auto operator/(split_complex_parameter<T1, Re, Im>, type_parameter<T2, Param> rhs) {
		return split_complex_parameter<div_result_t<T1, T2>, decltype(Re() / rhs), decltype(Im() / rhs)>();
	}


	//比較演算
	template <class T1, class T2, class = enable_if_t<dec::split_complex_eq1_v<!is_scalar_operation_v<split_complex<T1>, split_complex<T2>>, T1, T2>>>
	inline constexpr bool operator==(const split_complex<T1>& lhs, const split_complex<T2>& rhs) {
		return (lhs[0] == rhs[0]) && (lhs[1] == rhs[1]);
	}
	template <class T1, class T2, class Re1, class Im1, class Re2, class Im2, class = enable_if_t<dec::split_complex_eq1_v<!is_scalar_operation_v<split_complex<T1>, split_complex<T2>>, T1, T2>>>
	inline auto operator==(split_complex_parameter<T1, Re1, Im1>, split_complex_parameter<T2, Re2, Im2>) {
		return (Re1() == Re2()) && (Im1() == Im2());
	}
	template <class T1, class T2, class = enable_if_t<dec::split_complex_eq2_v<is_rscalar_operation_v<split_complex<T1>, T2>, T1, T2>>>
	inline constexpr bool operator==(const split_complex<T1>& lhs, T2& rhs) {
		return (lhs[0] == rhs) && is_absorbing_element(lhs[1]);
	}
	template <class T1, class T2, class Re, class Im, class Param, class = enable_if_t<dec::split_complex_eq2_v<is_rscalar_operation_v<split_complex<T1>, T2>, T1, T2>>>
	inline auto operator==(split_complex_parameter<T1, Re, Im>, type_parameter<T2, Param> rhs) {
		return (Re() == rhs) && int_parameter<bool, is_absorbing_element(Im::value)>();
	}
	template <class T1, class T2, class = enable_if_t<dec::split_complex_eq3_v<is_rscalar_operation_v<T1, split_complex<T2>>, T1, T2>>>
	inline constexpr bool operator==(const T1& lhs, const split_complex<T2>& rhs) {
		return (lhs == rhs[0]) && is_absorbing_element(rhs[1]);
	}
	template <class T1, class T2, class Param, class Re, class Im, class = enable_if_t<dec::split_complex_eq3_v<is_lscalar_operation_v<T1, split_complex<T2>>, T1, T2>>>
	inline auto operator==(type_parameter<T1, Param> lhs, split_complex_parameter<T2, Re, Im>) {
		return (lhs == Re()) && int_parameter<bool, is_absorbing_element(Im::value)>();
	}
	template <class T1, class T2, class = enable_if_t<dec::split_complex_eq1_v<!is_scalar_operation_v<split_complex<T1>, split_complex<T2>>, T1, T2>>>
	inline constexpr bool operator!=(const split_complex<T1>& lhs, const split_complex<T2>& rhs) { return !(lhs == rhs); }
	template <class T1, class T2, class Re1, class Im1, class Re2, class Im2, class = enable_if_t<dec::split_complex_eq1_v<!is_scalar_operation_v<split_complex<T1>, split_complex<T2>>, T1, T2>>>
	inline auto operator!=(split_complex_parameter<T1, Re1, Im1> lhs, split_complex_parameter<T2, Re2, Im2> rhs) { return !(lhs == rhs); }
	template <class T1, class T2, class = enable_if_t<dec::split_complex_eq2_v<is_rscalar_operation_v<split_complex<T1>, T2>, T1, T2>>>
	inline constexpr bool operator!=(const split_complex<T1>& lhs, T2& rhs) { return !(lhs == rhs); }
	template <class T1, class T2, class Re, class Im, class Param, class = enable_if_t<dec::split_complex_eq2_v<is_rscalar_operation_v<split_complex<T1>, T2>, T1, T2>>>
	inline auto operator!=(split_complex_parameter<T1, Re, Im> lhs, type_parameter<T2, Param> rhs) { return !(lhs == rhs); }
	template <class T1, class T2, class = enable_if_t<dec::split_complex_eq3_v<is_rscalar_operation_v<T1, split_complex<T2>>, T1, T2>>>
	inline constexpr bool operator!=(const T1& lhs, const split_complex<T2>& rhs) { return !(lhs == rhs); }
	template <class T1, class T2, class Param, class Re, class Im, class = enable_if_t<dec::split_complex_eq3_v<is_rscalar_operation_v<T1, split_complex<T2>>, T1, T2>>>
	inline auto operator!=(type_parameter<T1, Param> lhs, split_complex_parameter<T2, Re, Im> rhs) { return !(lhs == rhs); }


	//分解型複素数型の生成
	template <class T1, class T2>
	inline constexpr split_complex<reference_unwrapper_t<T1>> make_split_complex(T1&& re, T2&& im) {
		return split_complex<reference_unwrapper_t<T1>>(forward<T1>(re), forward<T2>(im));
	}


	//分解型複素数の判定
	template <class T>
	struct is_split_complex_impl : false_type {};
	template <class T>
	struct is_split_complex_impl<split_complex<T>> : true_type {};
	template <class T>
	struct is_split_complex : is_split_complex_impl<remove_cv_t<T>> {};

	//分解型複素数の除去
	template <class T>
	struct remove_split_complex {
		using type = T;
	};
	template <class T>
	struct remove_split_complex<split_complex<T>> {
		using type = T;
	};

	//全ての分解型複素数の除去
	template <class T>
	struct remove_all_split_complex {
		using type = T;
	};
	template <class T>
	struct remove_all_split_complex<split_complex<T>> : remove_all_split_complex<T> {};


	template <class From, class To>
	struct is_inclusion_split_complex : is_inclusion<From, typename To::base_type> {};
	template <class From, class To>
	struct is_inclusion_split_complex<split_complex<From>, To> : cat_bool<
		//split_complex<From>がToの基底となる場合も含めて判定
		is_inclusion_v<split_complex<From>, typename To::base_type> || is_inclusion_v<From, typename To::base_type>
	> {};
	//Fromの型によりis_inclusion_split_complexで分岐
	template <class From, class To>
	struct is_inclusion<From, split_complex<To>> : is_inclusion_split_complex<From, split_complex<To>> {};


	//加法パラメータ取得
	template <class T>
	struct addition_traits<split_complex<T>> {
		//単位元
		template <class = enable_if_t<is_exist_additive_identity_v<T>>>
		static constexpr T identity_element() { return T(); }
		//結合律
		static constexpr bool associative_value = addition_traits<T>::associative_value;
		//消約律
		static constexpr bool cancellative_value = addition_traits<T>::cancellative_value;
		//可換律
		static constexpr bool commutative_value = addition_traits<T>::commutative_value;
	};
	//乗法パラメータ取得
	template <class T>
	struct multiplication_traits<split_complex<T>> {
		//単位元
		template <class = enable_if_t<is_exist_multiplicative_identity_v<T>>>
		static constexpr auto identity_element() { return addition_traits<T>::identity_element(); }
		//吸収元
		template <class = enable_if_t<is_exist_absorbing_element_v<T>>>
		static constexpr T absorbing_element() { return T(); }
		//結合律
		static constexpr bool associative_value = multiplication_traits<T>::associative_value;
		//消約律
		static constexpr bool cancellative_value = multiplication_traits<T>::cancellative_value;
		//可換律
		static constexpr bool commutative_value = multiplication_traits<T>::commutative_value;
		//分配律
		static constexpr bool distributive_value = multiplication_traits<T>::distributive_value;
	};


	//逆元が存在するならば逆元の取得(存在しない場合は例外を出す)
	template <class T>
	struct Inverse_element<split_complex<T>> {
		static constexpr split_complex<T> _additive_inverse_(const split_complex<T>& c) {
			return split_complex<T>(additive_inverse(c[0]), additive_inverse(c[1]));
		}

		//Tに乗法逆元が存在する場合の乗法逆元
		static constexpr split_complex<T> _multiplicative_inverse_impl_(const split_complex<T>& c, true_type) {
			//共役を絶対値の2乗で割る
			T temp = c[0] * c[0] - c[1] * c[1];
			return complex<T>(c[0] / temp, -c[1] / temp);
		}
		//Tの乗法逆元が存在しない場合の乗法逆元
		static constexpr split_complex<T> _multiplicative_inverse_impl_(const split_complex<T>& c, false_type) {
			T temp = multiplicative_inverse(c[0] * c[0] - c[1] * c[1]);
			return complex<T>(c[0] * temp, -c[1] * temp);
		}
		static constexpr split_complex<T> _multiplicative_inverse_(const split_complex<T>& c) {
			return _multiplicative_inverse_impl_(c, bool_constant<is_exist_multiplicative_inverse_v<T>>());
		}
	};


	//誤差評価
	template <class T>
	struct Error_evaluation<split_complex<T>> {
		static constexpr split_complex<T> epsilon() { return split_complex<T>(Error_evaluation<T>::epsilon(), Error_evaluation<T>::epsilon()); }
		static constexpr bool _error_evaluation_(const split_complex<T>& x1, const split_complex<T>& x2) {
			return (error_evaluation(x1[0], x2[0])) && (error_evaluation(x1[1], x2[1]));
		}
	};
}


#endif