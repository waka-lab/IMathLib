#ifndef IMATH_MATH_HYPERCOMPLEX_QUATERNION_HPP
#define IMATH_MATH_HYPERCOMPLEX_QUATERNION_HPP

#include "IMathLib/math/liner_algebra/vector.hpp"
#include "IMathLib/math/hypercomplex/complex.hpp"


namespace iml {

	template <class>
	class quaternion;


	//四元数型のパラメータ
	template <class T, class Re, class Im1, class Im2, class Im3>
	using quaternion_parameter = type_parameter<quaternion<T>, type_tuple<Re, Im1, Im2, Im3>>;
	template <class T, class ReP, class ImP1, class ImP2, class ImP3>
	struct type_parameter<quaternion<T>, type_tuple<type_parameter<T, ReP>, type_parameter<T, ImP1>, type_parameter<T, ImP2>, type_parameter<T, ImP3>>> {
		using type = quaternion<T>;
		static constexpr type value = type(type_parameter<T, ReP>::value, type_parameter<T, ImP1>::value, type_parameter<T, ImP2>::value, type_parameter<T, ImP3>::value);

		template <class = enable_if_t<is_exist_additive_inverse_v<T>>>
		auto operator-() const { return quaternion_parameter<T, decltype(-type_parameter<T, ReP>()), decltype(-type_parameter<T, ImP1>()), decltype(-type_parameter<T, ImP2>()), decltype(-type_parameter<T, ImP3>())>(); }
		auto operator+() const { return *this; }

		template <class T, T Val, class = enable_if_t<(Val >= 0) && (Val < 4)>>
			auto operator[](int_parameter<T, Val>) const { return at_type_tuple_t<Val, type_tuple<type_parameter<T, ReP>, type_parameter<T, ImP1>, type_parameter<T, ImP2>, type_parameter<T, ImP3>>>(); }
	};


	//2項演算の十分条件を補完するメタ関数の定義
	namespace dec {
		//[name]1 : M(T1)×M(T2)			//上位の数学型の場合をも含む
		//[name]2 : M(T1)×T2
		//[name]3 : T1×M(T2)

#define QUATERNION_DECISION(NAME, TERMS)\
		template <bool, class T1, class T2> struct quaternion_##NAME : cat_bool<TERMS> {};\
		template <class T1, class T2> struct quaternion_##NAME##<false, T1, T2> : false_type {};\
		template <bool F, class T1, class T2> constexpr bool quaternion_##NAME##_v = quaternion_##NAME##<F, T1, T2>::value;
		QUATERNION_DECISION(add1, (is_calcable<T1, T2>::add_value && addition_traits<T1>::commutative_value && addition_traits<T2>::commutative_value));
		QUATERNION_DECISION(add2, (is_inclusion_v<T1, add_result_t<T1, T2>> && addition_traits<T1>::commutative_value && addition_traits<T2>::commutative_value));
		QUATERNION_DECISION(add3, (is_inclusion_v<T2, add_result_t<T1, T2>> && addition_traits<T1>::commutative_value && addition_traits<T2>::commutative_value));
		QUATERNION_DECISION(sub1, (is_calcable<T1, T2>::sub_value && addition_traits<T1>::commutative_value && addition_traits<T2>::commutative_value));
		QUATERNION_DECISION(sub2, (is_inclusion_v<T1, sub_result_t<T1, T2>> && addition_traits<T1>::commutative_value && addition_traits<T2>::commutative_value));
		QUATERNION_DECISION(sub3, (is_inclusion_v<T2, sub_result_t<T1, T2>> && addition_traits<T1>::commutative_value && addition_traits<T2>::commutative_value));
		QUATERNION_DECISION(mul1, (is_calcable<T1, T2>::mul_value && is_ring_v<T1> && is_ring_v<T2>));
		QUATERNION_DECISION(mul2, (is_calcable<T1, T2>::mul_value && is_ring_v<T2>));
		QUATERNION_DECISION(mul3, (is_calcable<T1, T2>::mul_value && is_ring_v<T1>));
		QUATERNION_DECISION(div1, (is_calcable<T1, T2>::mul_value && is_commutative_field_v<T2> && is_ring_v<T1>));
		QUATERNION_DECISION(div2, (is_calcable<T1, T2>::div_value && is_skew_field_v<T2> && is_ring_v<T1>));
		QUATERNION_DECISION(div3, (is_calcable<T1, T2>::mul_value && is_commutative_field_v<T2> && is_ring_v<T1>));
		QUATERNION_DECISION(eq1, (is_calcable<T1, T2>::eq_value));
		QUATERNION_DECISION(eq2, (is_calcable<T1, T2>::eq_value));
		QUATERNION_DECISION(eq3, (is_calcable<T1, T2>::eq_value));
#undef QUATERNION_DECISION
	}


	template <class Base, class T, bool = is_math_type_has_base<T>::value, bool = is_same<Base, T>::value>
	class quaternion_base;


	//四元数型
	template <class T>
	class quaternion : public value_list_input<array_iterator<T>> {
		template <class> friend class quaternion;
		T x_m[4];
	public:
		constexpr quaternion() : x_m{} {}
		constexpr quaternion(const T& re) : x_m{ re } {}
		template <class U, class = enable_if_t<is_inclusion_v<U, T>>>
		constexpr quaternion(const U& re) : x_m{ re } {}
		constexpr quaternion(const T& re, const T& im1, const T& im2, const T& im3) : x_m{ re,im1,im2,im3 } {}
		template <class U, class = enable_if_t<is_inclusion_v<U, T>>>
		constexpr quaternion(const U& re, const U& im1, const U& im2, const U& im3) : x_m{ re,im1,im2,im3 } {}
		constexpr quaternion(const complex<T>& c) : x_m{ c[0],c[1] } {}
		template <class U, class = enable_if_t<is_inclusion_v<U, T>>>
		constexpr quaternion(const complex<U>& c) : x_m{ c[0],c[1] } {}
		constexpr quaternion(const T& re, const vector3<T>& v) : x_m{ re,v[0],v[1],v[2] } {}
		template <class U, class = enable_if_t<is_inclusion_v<U, T>>>
		constexpr quaternion(const U& re, const vector3<U>& v) : x_m{ re,v[0],v[1],v[2] } {}
		constexpr quaternion(const quaternion& q) : x_m{ q.x_m[0],q.x_m[1],q.x_m[2],q.x_m[3] } {}
		template <class U, class = enable_if_t<is_inclusion_v<U, T>>>
		constexpr quaternion(const quaternion<U>& q) : x_m{ q.x_m[0],q.x_m[1],q.x_m[2],q.x_m[3] } {}

		using base_type = T;
		using iterator = array_iterator<T>;
		using const_iterator = array_iterator<const T>;

		template<class Other>
		struct rebind {
			using other = quaternion<Other>;
		};
		template <class Other>
		using rebind_t = quaternion<Other>;

		constexpr iterator begin() noexcept { return iterator(x_m); }
		constexpr const_iterator begin() const noexcept { return const_iterator(x_m); }
		constexpr iterator end() noexcept { return iterator(x_m + 4); }
		constexpr const_iterator end() const noexcept { return const_iterator(x_m + 4); }

		//単項演算
		template <class = enable_if_t<is_exist_additive_inverse_v<T>>>
		constexpr quaternion operator-() const { return quaternion(-this->x_m[0], -this->x_m[1], -this->x_m[2], -this->x_m[3]); }
		constexpr quaternion operator+() const { return quaternion(*this); }
		//代入演算
		quaternion& operator=(const quaternion& q) {
			if (this != addressof(q)) {
				this->x_m[0] = q.x_m[0]; this->x_m[1] = q.x_m[1];
				this->x_m[2] = q.x_m[2]; this->x_m[3] = q.x_m[3];
			}
			return *this;
		}
		template <class U, class = enable_if_t<!is_rscalar_operation_v<quaternion, quaternion<U>> && is_inclusion_v<U, T>>>
		quaternion& operator=(const quaternion<U>& q) {
			this->x_m[0] = q.x_m[0]; this->x_m[1] = q.x_m[1];
			this->x_m[2] = q.x_m[2]; this->x_m[3] = q.x_m[3];
			return *this;
		}
		template <class U, class = enable_if_t<is_inclusion_v<U, T>>>
		quaternion& operator=(const U& n) {
			this->x_m[0] = n; this->x_m[1] = addition_traits<T>::identity_element();
			this->x_m[2] = addition_traits<T>::identity_element(); this->x_m[3] = addition_traits<T>::identity_element();
			return *this;
		}
		template <class U, class = enable_if_t<is_inclusion_v<U, T>>>
		quaternion& operator=(const complex<U>& c) {
			this->x_m[0] = c[0]; this->x_m[1] = c[1];
			this->x_m[2] = addition_traits<T>::identity_element(); this->x_m[3] = addition_traits<T>::identity_element();
			return *this;
		}
		template <class U, class = enable_if_t<dec::quaternion_add1_v<is_operation<T, U, T>::add_value && !is_rscalar_operation_v<quaternion, quaternion<U>>, T, U>>>
		quaternion& operator+=(const quaternion<U>& q) {
			this->x_m[0] += q.x_m[0]; this->x_m[1] += q.x_m[1];
			this->x_m[2] += q.x_m[2]; this->x_m[3] += q.x_m[3];
			return *this;
		}
		template <class U, class = enable_if_t<dec::quaternion_add2_v<is_operation<T, U, T>::add_value && is_rscalar_operation_v<quaternion, U>, T, U>>>
		quaternion& operator+=(const U& n) {
			this->x_m[0] += n;
			return *this;
		}
		template <class U, class = enable_if_t<dec::quaternion_add1_v<is_operation<T, U, T>::add_value && !is_rscalar_operation_v<quaternion, complex<U>>, T, U>>>
		quaternion& operator+=(const complex<U>& c) {
			this->x_m[0] += c[0]; this->x_m[1] += c[1];
			return *this;
		}
		template <class U, class = enable_if_t<dec::quaternion_sub1_v<is_operation<T, U, T>::sub_value && !is_rscalar_operation_v<quaternion, quaternion<U>>, T, U>>>
		quaternion& operator-=(const quaternion<U>& q) {
			this->x_m[0] -= q.x_m[0]; this->x_m[1] -= q.x_m[1];
			this->x_m[2] -= q.x_m[2]; this->x_m[3] -= q.x_m[3];
			return *this;
		}
		template <class U, class = enable_if_t<dec::quaternion_sub2_v<is_operation<T, U, T>::sub_value && is_rscalar_operation_v<quaternion, U>, T, U>>>
		quaternion& operator-=(const U& n) {
			this->x_m[0] -= n;
			return *this;
		}
		template <class U, class = enable_if_t<dec::quaternion_sub1_v<is_operation<T, U, T>::sub_value && !is_rscalar_operation_v<quaternion, complex<U>>, T, U>>>
		quaternion& operator-=(const complex<U>& c) {
			this->x_m[0] -= c[0]; this->x_m[1] -= c[1];
			return *this;
		}
		template <class U, class = enable_if_t<dec::quaternion_mul1_v<is_operation<T, U, T>::mul_value && !is_rscalar_operation_v<quaternion, quaternion<U>>, T, U>>>
		quaternion& operator*=(const quaternion<U>& q) {
			//()は3次元の内積と外積
			T temp[4] = {
				this->x_m[0] * q.x_m[0] - (this->x_m[1] * q.x_m[1] + this->x_m[2] * q.x_m[2] + this->x_m[3] * q.x_m[3])
				,this->x_m[0] * q.x_m[1] + this->x_m[1] * q.x_m[0] + (this->x_m[2] * q.x_m[3] - this->x_m[3] * q.x_m[2])
				,this->x_m[0] * q.x_m[2] + this->x_m[2] * q.x_m[0] + (this->x_m[3] * q.x_m[1] - this->x_m[1] * q.x_m[3])
				,this->x_m[0] * q.x_m[3] + this->x_m[3] * q.x_m[0] + (this->x_m[1] * q.x_m[2] - this->x_m[2] * q.x_m[1])
			};
			this->x_m[0] = temp[0]; this->x_m[1] = temp[1];
			this->x_m[2] = temp[2]; this->x_m[3] = temp[3];
			return *this;
		}
		template <class U, class = enable_if_t<dec::quaternion_mul2_v<is_operation<T, U, T>::mul_value && is_rscalar_operation_v<quaternion, U>, T, U>>>
		quaternion& operator*=(const U& k) {
			this->x_m[0] *= k; this->x_m[1] *= k;
			this->x_m[2] *= k; this->x_m[3] *= k;
			return *this;
		}
		template <class U, class = enable_if_t<dec::quaternion_mul1_v<is_operation<T, U, T>::mul_value && !is_rscalar_operation_v<quaternion, complex<U>>, T, U>>>
		quaternion& operator*=(const complex<U>& c) {
			T temp[4] = {
				this->x_m[0] * c[0] - (this->x_m[1] * c[1])
				,this->x_m[0] * c[1] + this->x_m[1] * c[0]
				,this->x_m[2] * c[0] + (this->x_m[3] * c[1])
				,this->x_m[3] * c[0] + (-this->x_m[2] * c[1])
			};
			this->x_m[0] = temp[0]; this->x_m[1] = temp[1];
			this->x_m[2] = temp[2]; this->x_m[3] = temp[3];
			return *this;
		}
		//Uは体でTは環を仮定しているため，乗算がT×U→Tで十分
		template <class U, class = enable_if_t<dec::quaternion_div1_v<is_operation<T, U, T>::mul_value && !is_rscalar_operation_v<quaternion, quaternion<U>>, T, U>>>
		quaternion& operator/=(const quaternion<U>& q) {
			//逆元は共役を絶対値の二乗で割る
			T temp = q.x_m[0] * q.x_m[0] + q.x_m[1] * q.x_m[1] + q.x_m[2] * q.x_m[2] + q.x_m[3] * q.x_m[3];
			return *this *= quaternion<U>(q.x_m[0] / temp, -q.x_m[1] / temp, -q.x_m[2] / temp, -q.x_m[3] / temp);
		}
		template <class U, class = enable_if_t<dec::quaternion_div2_v<is_operation<T, U, T>::div_value && is_rscalar_operation_v<quaternion, U>, T, U>>>
		quaternion& operator/=(const U& k) {
			this->x_m[0] /= k; this->x_m[1] /= k;
			this->x_m[2] /= k; this->x_m[3] /= k;
			return *this;
		}
		template <class U, class = enable_if_t<dec::quaternion_div1_v<is_operation<T, U, T>::mul_value && !is_rscalar_operation_v<quaternion, complex<U>>, T, U>>>
		quaternion& operator/=(const complex<U>& c) {
			return *this *= multiplicative_inverse(c);
		}
		//2項演算
		using value_list_input<array_iterator<T>>::operator<<;

		//添え字演算
		const constexpr T& operator[](size_t index) const { return this->x_m[index]; }
		constexpr T& operator[](size_t index) { return this->x_m[index]; }

		//ストリーム出力
		friend std::ostream& operator<<(std::ostream& os, const quaternion& n) {
			os << '(' << n.x_m[0] << ',' << n.x_m[1] << ',' << n.x_m[2] << ',' << n.x_m[3] << ')';
			return os;
		}
		friend std::wostream& operator<<(std::wostream& os, const quaternion& n) {
			os << '(' << n.x_m[0] << ',' << n.x_m[1] << ',' << n.x_m[2] << ',' << n.x_m[3] << ')';
			return os;
		}
	};


	//四元数の2項演算
	template <class T1, class T2, class = enable_if_t<dec::quaternion_add1_v<!is_scalar_operation_v<quaternion<T1>, quaternion<T2>>, T1, T2>>>
	constexpr auto operator+(const quaternion<T1>& lhs, const quaternion<T2>& rhs) {
		return quaternion<add_result_t<T1, T2>>(lhs[0] + rhs[0], lhs[1] + rhs[1], lhs[2] + rhs[2], lhs[3] + rhs[3]);
	}
	template <class T1, class T2, class Re1, class Im1_1, class Im2_1, class Im3_1, class Re2, class Im1_2, class Im2_2, class Im3_2, class = enable_if_t<dec::quaternion_add1_v<!is_scalar_operation_v<quaternion<T1>, quaternion<T2>>, T1, T2>>>
	auto operator+(quaternion_parameter<T1, Re1, Im1_1, Im2_1, Im3_1>, quaternion_parameter<T2, Re2, Im1_2, Im2_2, Im3_2>) {
		return quaternion_parameter<add_result_t<T1, T2>, decltype(Re1() + Re2())
			, decltype(Im1_1() + Im1_2()), decltype(Im2_1() + Im2_2()), decltype(Im3_1() + Im3_2())>();
	}
	template <class T1, class T2, class = enable_if_t<dec::quaternion_add2_v<is_rscalar_operation_v<quaternion<T1>, T2>, T1, T2>>>
	constexpr auto operator+(const quaternion<T1>& lhs, const T2& rhs) {
		return quaternion<add_result_t<T1, T2>>(lhs[0] + rhs, lhs[1], lhs[2], lhs[3]);
	}
	template <class T1, class T2, class Re, class Im1, class Im2, class Im3, class Param, class = enable_if_t<dec::quaternion_add2_v<is_rscalar_operation_v<quaternion<T1>, T2>, T1, T2>>>
	auto operator+(quaternion_parameter<T1, Re, Im1, Im2, Im3>, type_parameter<T2, Param> rhs) {
		return quaternion_parameter<add_result_t<T1, T2>, decltype(Re() + rhs), Im1, Im2, Im3>();
	}
	template <class T1, class T2, class = enable_if_t<dec::quaternion_add3_v<is_lscalar_operation_v<T1, quaternion<T2>>, T1, T2>>>
	constexpr auto operator+(const T1& lhs, const quaternion<T2>& rhs) {
		return quaternion<add_result_t<T1, T2>>(lhs + rhs[0], rhs[1], rhs[2], rhs[3]);
	}
	template <class T1, class T2, class Param, class Re, class Im1, class Im2, class Im3, class = enable_if_t<dec::quaternion_add3_v<is_lscalar_operation_v<T1, quaternion<T2>>, T1, T2>>>
	auto operator+(type_parameter<T1, Param> lhs, quaternion_parameter<T2, Re, Im1, Im2, Im3>) {
		return quaternion_parameter<add_result_t<T1, T2>, decltype(lhs + Re()), Im1, Im2, Im3>();
	}
	template <class T1, class T2, class = enable_if_t<dec::quaternion_add1_v<!is_scalar_operation_v<quaternion<T1>, complex<T2>>, T1, T2>>>
	constexpr auto operator+(const quaternion<T1>& lhs, const complex<T2>& rhs) {
		return quaternion<add_result_t<T1, T2>>(lhs[0] + rhs[0], lhs[1]+ rhs[1], lhs[2], lhs[3]);
	}
	template <class T1, class T2, class Re1, class Im1_1, class Im2_1, class Im3_1, class Re2, class Im_2, class = enable_if_t<dec::quaternion_add1_v<!is_scalar_operation_v<quaternion<T1>, complex<T2>>, T1, T2>>>
	auto operator+(quaternion_parameter<T1, Re1, Im1_1, Im2_1, Im3_1>, complex_parameter<T2, Re2, Im_2>) {
		return quaternion_parameter<add_result_t<T1, T2>, decltype(Re1() + Re2()), decltype(Im1_1() + Im_2()), Im2_1, Im3_1>();
	}
	template <class T1, class T2, class = enable_if_t<dec::quaternion_add1_v<!is_scalar_operation_v<complex<T1>, quaternion<T2>>, T1, T2>>>
	constexpr auto operator+(const complex<T1>& lhs, const quaternion<T2>& rhs) {
		return quaternion<add_result_t<T1, T2>>(lhs[0] + rhs[0], lhs[1] + rhs[1], rhs[2], rhs[3]);
	}
	template <class T1, class T2, class Re1, class Im_1, class Re2, class Im1_2, class Im2_2, class Im3_2, class = enable_if_t<dec::quaternion_add1_v<!is_scalar_operation_v<complex<T1>, quaternion<T2>>, T1, T2>>>
	auto operator+(complex_parameter<T1, Re1, Im_1>, quaternion_parameter<T2, Re2, Im1_2, Im2_2, Im3_2>) {
		return quaternion_parameter<add_result_t<T1, T2>, decltype(Re1() + Re2()), decltype(Im_1() + Im1_2()), Im2_2, Im3_2>();
	}

	template <class T1, class T2, class = enable_if_t<dec::quaternion_sub1_v<!is_scalar_operation_v<quaternion<T1>, quaternion<T2>>, T1, T2>>>
	constexpr auto operator-(const quaternion<T1>& lhs, const quaternion<T2>& rhs) {
		return quaternion<sub_result_t<T1, T2>>(lhs[0] - rhs[0], lhs[1] - rhs[1], lhs[2] - rhs[2], lhs[3] - rhs[3]);
	}
	template <class T1, class T2, class Re1, class Im1_1, class Im2_1, class Im3_1, class Re2, class Im1_2, class Im2_2, class Im3_2, class = enable_if_t<dec::quaternion_sub1_v<!is_scalar_operation_v<quaternion<T1>, quaternion<T2>>, T1, T2>>>
	auto operator-(quaternion_parameter<T1, Re1, Im1_1, Im2_1, Im3_1>, quaternion_parameter<T1, Re2, Im1_2, Im2_2, Im3_2>) {
		return quaternion_parameter<add_result_t<T1, T2>, decltype(Re1() - Re2())
			, decltype(Im1_1() - Im1_2()), decltype(Im2_1() - Im2_2()), decltype(Im3_1() - Im3_2())>();
	}
	template <class T1, class T2, class = enable_if_t<dec::quaternion_sub2_v<is_rscalar_operation_v<quaternion<T1>, T2>, T1, T2>>>
	constexpr auto operator-(const quaternion<T1>& lhs, const T2& rhs) {
		return quaternion<sub_result_t<T1, T2>>(lhs[0] - rhs, lhs[1], lhs[2], lhs[3]);
	}
	template <class T1, class T2, class Re, class Im1, class Im2, class Im3, class Param, class = enable_if_t<dec::quaternion_sub2_v<is_rscalar_operation_v<quaternion<T1>, T2>, T1, T2>>>
	auto operator-(quaternion_parameter<T1, Re, Im1, Im2, Im3>, type_parameter<T2, Param> rhs) {
		return quaternion_parameter<sub_result_t<T1, T2>, decltype(Re() - rhs), Im1, Im2, Im3>();
	}
	template <class T1, class T2, class = enable_if_t<dec::quaternion_sub3_v<is_lscalar_operation_v<T1, quaternion<T2>>, T1, T2>>>
	constexpr auto operator-(const T1& lhs, const quaternion<T2>& rhs) {
		return quaternion<sub_result_t<T1, T2>>(lhs - rhs[0], -rhs[1], -rhs[2], -rhs[3]);
	}
	template <class T1, class T2, class Param, class Re, class Im1, class Im2, class Im3, class = enable_if_t<dec::quaternion_sub3_v<is_lscalar_operation_v<T1, quaternion<T2>>, T1, T2>>>
	auto operator-(type_parameter<T1, Param> lhs, quaternion_parameter<T2, Re, Im1, Im2, Im3>) {
		return quaternion_parameter<sub_result_t<T1, T2>, decltype(lhs - Re()), decltype(-Im1()), decltype(-Im2()), decltype(-Im3())>();
	}
	template <class T1, class T2, class = enable_if_t<dec::quaternion_sub1_v<!is_scalar_operation_v<quaternion<T1>, complex<T2>>, T1, T2>>>
	constexpr auto operator-(const quaternion<T1>& lhs, const complex<T2>& rhs) {
		return quaternion<sub_result_t<T1, T2>>(lhs[0] - rhs[0], lhs[1] - rhs[1], lhs[2], lhs[3]);
	}
	template <class T1, class T2, class Re1, class Im1_1, class Im2_1, class Im3_1, class Re2, class Im_2, class = enable_if_t<dec::quaternion_sub1_v<!is_scalar_operation_v<quaternion<T1>, complex<T2>>, T1, T2>>>
	auto operator-(quaternion_parameter<T1, Re1, Im1_1, Im2_1, Im3_1>, complex_parameter<T2, Re2, Im_2>) {
		return quaternion_parameter<sub_result_t<T1, T2>, decltype(Re1() - Re2()), decltype(Im1_1() - Im_2()), Im2_1, Im3_1>();
	}
	template <class T1, class T2, class = enable_if_t<dec::quaternion_sub1_v<!is_scalar_operation_v<complex<T1>, quaternion<T2>>, T1, T2>>>
	constexpr auto operator-(const complex<T1>& lhs, const quaternion<T2>& rhs) {
		return quaternion<sub_result_t<T1, T2>>(lhs[0] - rhs[0], lhs[1] - rhs[1], -rhs[2], -rhs[3]);
	}
	template <class T1, class T2, class Re1, class Im_1, class Re2, class Im1_2, class Im2_2, class Im3_2, class = enable_if_t<dec::quaternion_sub1_v<!is_scalar_operation_v<complex<T1>, quaternion<T2>>, T1, T2>>>
	auto operator-(complex_parameter<T1, Re1, Im_1>, quaternion_parameter<T2, Re2, Im1_2, Im2_2, Im3_2>) {
		return quaternion_parameter<sub_result_t<T1, T2>, decltype(Re1() - Re2()), decltype(Im_1() - Im1_2()), decltype(-Im2_2()), decltype(-Im3_2())>();
	}

	template <class T1, class T2, class = enable_if_t<dec::quaternion_mul1_v<!is_scalar_operation_v<quaternion<T1>, quaternion<T2>>, T1, T2>>>
	constexpr auto operator*(const quaternion<T1>& lhs, const quaternion<T2>& rhs) {
		return quaternion<mul_result_t<T1, T2>>(
			lhs[0] * rhs[0] - (lhs[1] * rhs[1] + lhs[2] * rhs[2] + lhs[3] * rhs[3])
			, lhs[0] * rhs[1] + lhs[1] * rhs[0] + (lhs[2] * rhs[3] - lhs[3] * rhs[2])
			, lhs[0] * rhs[2] + lhs[2] * rhs[0] + (lhs[3] * rhs[1] - lhs[1] * rhs[3])
			, lhs[0] * rhs[3] + lhs[3] * rhs[0] + (lhs[1] * rhs[2] - lhs[2] * rhs[1]));
	}
	template <class T1, class T2, class Re1, class Im1_1, class Im2_1, class Im3_1, class Re2, class Im1_2, class Im2_2, class Im3_2, class = enable_if_t<dec::quaternion_mul1_v<!is_scalar_operation_v<quaternion<T1>, quaternion<T2>>, T1, T2>>>
	auto operator*(quaternion_parameter<T1, Re1, Im1_1, Im2_1, Im3_1>, quaternion_parameter<T1, Re2, Im1_2, Im2_2, Im3_2>) {
		return quaternion_parameter<mul_result_t<T1, T2>
			, decltype(Re1() * Re2() - (Im1_1() * Im1_2()+ Im2_1() * Im2_2()+ Im3_1() * Im3_2()))
			, decltype(Re1() * Im1_2() + Im1_1() * Re2() + (Im2_1() * Im3_2() - Im3_1() * Im2_2()))
			, decltype(Re1() * Im2_2() + Im2_1() * Re2() + (Im3_1() * Im1_2() - Im1_1() * Im3_2()))
			, decltype(Re1() * Im3_2() + Im3_1() * Re2() + (Im1_1() * Im2_2() - Im2_1() * Im1_2()))>();
	}
	template <class T1, class T2, class = enable_if_t<dec::quaternion_mul2_v<is_rscalar_operation_v<quaternion<T1>, T2>, T1, T2>>>
	constexpr auto operator*(const quaternion<T1>& lhs, const T2& rhs) {
		return quaternion<mul_result_t<T1, T2>>(lhs[0] * rhs, lhs[1] * rhs, lhs[2] * rhs, lhs[3] * rhs);
	}
	template <class T1, class T2, class Re, class Im1, class Im2, class Im3, class Param, class = enable_if_t<dec::quaternion_mul2_v<is_rscalar_operation_v<quaternion<T1>, T2>, T1, T2>>>
	auto operator*(quaternion_parameter<T1, Re, Im1, Im2, Im3>, type_parameter<T2, Param> rhs) {
		return quaternion_parameter<mul_result_t<T1, T2>, decltype(Re() * rhs), decltype(Im1() * rhs), decltype(Im2() * rhs), decltype(Im3() * rhs)>();
	}
	template <class T1, class T2, class = enable_if_t<dec::quaternion_mul3_v<is_rscalar_operation_v<T1, quaternion<T2>>, T1, T2>>>
	constexpr auto operator*(const T1& lhs, const quaternion<T2>& rhs) {
		return quaternion<mul_result_t<T1, T2>>(lhs * rhs[0], lhs * rhs[1], lhs * rhs[2], lhs * rhs[3]);
	}
	template <class T1, class T2, class Param, class Re, class Im1, class Im2, class Im3, class = enable_if_t<dec::quaternion_mul3_v<is_lscalar_operation_v<T1, quaternion<T2>>, T1, T2>>>
	auto operator*(type_parameter<T1, Param> lhs, quaternion_parameter<T2, Re, Im1, Im2, Im3>) {
		return quaternion_parameter<mul_result_t<T1, T2>, decltype(lhs * Re()), decltype(lhs * Im1()), decltype(lhs * Im2()), decltype(lhs * Im3())>();
	}
	template <class T1, class T2, class = enable_if_t<dec::quaternion_mul1_v<!is_scalar_operation_v<quaternion<T1>, complex<T2>>, T1, T2>>>
	constexpr auto operator*(const quaternion<T1>& lhs, const complex<T2>& rhs) {
		return quaternion<mul_result_t<T1, T2>>(
			lhs[0] * rhs[0] - (lhs[1] * rhs[1])
			, lhs[0] * rhs[1] + lhs[1] * rhs[0]
			, lhs[2] * rhs[0] + (lhs[3] * rhs[1])
			, lhs[3] * rhs[0] + (-lhs[2] * rhs[1]));
	}
	template <class T1, class T2, class Re1, class Im1_1, class Im2_1, class Im3_1, class Re2, class Im_2, class = enable_if_t<dec::quaternion_mul1_v<!is_scalar_operation_v<quaternion<T1>, complex<T2>>, T1, T2>>>
	auto operator*(quaternion_parameter<T1, Re1, Im1_1, Im2_1, Im3_1>, complex_parameter<T2, Re2, Im_2>) {
		return quaternion_parameter<mul_result_t<T1, T2>
			, decltype(Re1() * Re2() - (Im1_1() * Im_2()))
			, decltype(Re1() * Im_2() + Im1_1() * Re2())
			, decltype(Im2_1() * Re2() + (Im3_1() * Im_2()))
			, decltype(Im3_1() * Re2() + (-Im2_1() * Im_2()))>();
	}
	template <class T1, class T2, class = enable_if_t<dec::quaternion_mul1_v<!is_scalar_operation_v<complex<T1>, quaternion<T2>>, T1, T2>>>
	constexpr auto operator*(const complex<T1>& lhs, const quaternion<T2>& rhs) {
		return quaternion<mul_result_t<T1, T2>>(
			lhs[0] * rhs[0] - (lhs[1] * rhs[1])
			, lhs[0] * rhs[1] + lhs[1] * rhs[0]
			, lhs[0] * rhs[2] + (-lhs[1] * rhs[3])
			, lhs[0] * rhs[3] + (lhs[1] * rhs[2]));
	}
	template <class T1, class T2, class Re1, class Im_1, class Re2, class Im1_2, class Im2_2, class Im3_2, class = enable_if_t<dec::quaternion_mul1_v<!is_scalar_operation_v<complex<T1>, quaternion<T2>>, T1, T2>>>
	auto operator*(complex_parameter<T1, Re1, Im_1>, quaternion_parameter<T2, Re2, Im1_2, Im2_2, Im3_2>) {
		return quaternion_parameter<mul_result_t<T1, T2>
			, decltype(Re1() * Re2() - (Im_1() * Im1_2()))
			, decltype(Re1() * Im1_2() + Im_1() * Re2())
			, decltype(Re1() * Im2_2() + (-Im_1() * Im3_2()))
			, decltype(Re1() * Im3_2() + (Im_1() * Im2_2()))>();
	}

	template <class T1, class T2, class = enable_if_t<dec::quaternion_div1_v<!is_scalar_operation_v<quaternion<T1>, quaternion<T2>>, T1, T2>>>
	constexpr auto operator/(const quaternion<T1>& lhs, const quaternion<T2>& rhs) {
		T2 temp = rhs[0] * rhs[0] + rhs[1] * rhs[1] + rhs[2] * rhs[2] + rhs[3] * rhs[3];
		return lhs * quaternion<T2>(rhs[0] / temp, -rhs[1] / temp, -rhs[2] / temp, -rhs[3] / temp);
	}
	template <class T1, class T2, class Re1, class Im1_1, class Im2_1, class Im3_1, class Re2, class Im1_2, class Im2_2, class Im3_2, class = enable_if_t<dec::quaternion_div1_v<!is_scalar_operation_v<quaternion<T1>, quaternion<T2>>, T1, T2>>>
	auto operator/(quaternion_parameter<T1, Re1, Im1_1, Im2_1, Im3_1> lhs, quaternion_parameter<T1, Re2, Im1_2, Im2_2, Im3_2>) {
		using type = decltype(Re2() * Re2() + Im1_2() * Im1_2() + Im2_2() * Im2_2() + Im3_2() * Im3_2());
		return lhs * quaternion_parameter<T2, decltype(Re2() / type())
			, decltype(Im1_2() / type()), decltype(Im2_2() / type()), decltype(Im3_2() / type())>();
	}
	template <class T1, class T2, class = enable_if_t<dec::quaternion_div2_v<is_rscalar_operation_v<quaternion<T1>, T2>, T1, T2>>>
	constexpr auto operator/(const quaternion<T1>& lhs, const T2& rhs) {
		return quaternion<div_result_t<T1, T2>>(lhs[0] / rhs, lhs[1] / rhs, lhs[2] / rhs, lhs[3] / rhs);
	}
	template <class T1, class T2, class Re, class Im1, class Im2, class Im3, class Param, class = enable_if_t<dec::quaternion_div2_v<is_rscalar_operation_v<quaternion<T1>, T2>, T1, T2>>>
	auto operator/(quaternion_parameter<T1, Re, Im1, Im2, Im3>, type_parameter<T2, Param> rhs) {
		return quaternion_parameter<div_result_t<T1, T2>, decltype(Re() / rhs), decltype(Im1() / rhs), decltype(Im2() / rhs), decltype(Im3() / rhs)>();
	}
	template <class T1, class T2, class = enable_if_t<dec::quaternion_div3_v<is_rscalar_operation_v<T1, quaternion<T2>>, T1, T2>>>
	constexpr auto operator/(const T1& lhs, const quaternion<T2>& rhs) {
		T2 temp = rhs[0] * rhs[0] + rhs[1] * rhs[1] + rhs[2] * rhs[2] + rhs[3] * rhs[3];
		return lhs * quaternion<T2>(rhs[0] / temp, -rhs[1] / temp, -rhs[2] / temp, -rhs[3] / temp);
	}
	template <class T1, class T2, class Param, class Re, class Im1, class Im2, class Im3, class = enable_if_t<dec::quaternion_div3_v<is_lscalar_operation_v<T1, quaternion<T2>>, T1, T2>>>
	auto operator/(type_parameter<T1, Param> lhs, quaternion_parameter<T2, Re, Im1, Im2, Im3>) {
		using type = decltype(Re() * Re() + Im1() * Im1() + Im2() * Im2() + Im3() * Im3());
		return lhs * quaternion_parameter<T2, decltype(Re() / type())
			, decltype(Im1() / type()), decltype(Im2() / type()), decltype(Im3() / type())>();
	}
	template <class T1, class T2, class = enable_if_t<dec::quaternion_div1_v<!is_scalar_operation_v<quaternion<T1>, complex<T2>>, T1, T2>>>
	constexpr auto operator/(const quaternion<T1>& lhs, const complex<T2>& rhs) {
		return lhs * multiplicative_inverse(rhs);
	}
	template <class T1, class T2, class Re1, class Im1_1, class Im2_1, class Im3_1, class Re2, class Im_2, class = enable_if_t<dec::quaternion_div1_v<!is_scalar_operation_v<quaternion<T1>, complex<T2>>, T1, T2>>>
	auto operator/(quaternion_parameter<T1, Re1, Im1_1, Im2_1, Im3_1> lhs, complex_parameter<T2, Re2, Im_2>) {
		using type = decltype(Re2() * Re2() + Im_2() * Im_2());
		return lhs * complex_parameter<T2, decltype(Re2() / type()), decltype(Im_2() / type())>();
	}
	template <class T1, class T2, class = enable_if_t<dec::quaternion_div1_v<!is_scalar_operation_v<complex<T1>, quaternion<T2>>, T1, T2>>>
	constexpr auto operator/(const complex<T1>& lhs, const quaternion<T2>& rhs) {
		T2 temp = rhs[0] * rhs[0] + rhs[1] * rhs[1] + rhs[2] * rhs[2] + rhs[3] * rhs[3];
		return lhs * quaternion<T2>(rhs[0] / temp, -rhs[1] / temp, -rhs[2] / temp, -rhs[3] / temp);
	}
	template <class T1, class T2, class Re1, class Im_1, class Re2, class Im1_2, class Im2_2, class Im3_2, class = enable_if_t<dec::quaternion_div1_v<!is_scalar_operation_v<complex<T1>, quaternion<T2>>, T1, T2>>>
	auto operator/(complex_parameter<T1, Re1, Im_1> lhs, quaternion_parameter<T2, Re2, Im1_2, Im2_2, Im3_2>) {
		using type = decltype(Re2() * Re2() + Im1_2() * Im1_2() + Im2_2() * Im2_2() + Im3_2() * Im3_2());
		return lhs * quaternion_parameter<T2, decltype(Re2() / type())
			, decltype(Im1_2() / type()), decltype(Im2_2() / type()), decltype(Im3_2() / type())>();
	}


	//比較演算
	template <class T1, class T2, class = enable_if_t<dec::quaternion_eq1_v<!is_scalar_operation_v<quaternion<T1>, quaternion<T2>>, T1, T2>>>
	constexpr bool operator==(const quaternion<T1>& lhs, const quaternion<T2>& rhs) {
		return (lhs[0] == rhs[0]) && (lhs[1] == rhs[1]) && (lhs[2] == rhs[2]) && (lhs[3] == rhs[3]);
	}
	template <class T1, class T2, class Re1, class Im1_1, class Im2_1, class Im3_1, class Re2, class Im1_2, class Im2_2, class Im3_2, class = enable_if_t<dec::quaternion_eq1_v<!is_scalar_operation_v<quaternion<T1>, quaternion<T2>>, T1, T2>>>
	auto operator==(quaternion_parameter<T1, Re1, Im1_1, Im2_1, Im3_1>, quaternion_parameter<T1, Re2, Im1_2, Im2_2, Im3_2>) {
		return (Re1() == Re2()) && (Im1_1() == Im1_2()) && (Im2_1() == Im2_2()) && (Im3_1() == Im3_2());
	}
	template <class T1, class T2, class = enable_if_t<dec::quaternion_eq2_v<is_rscalar_operation_v<quaternion<T1>, T2>, T1, T2>>>
	constexpr bool operator==(const quaternion<T1>& lhs, T2& rhs) {
		return (lhs[0] == rhs) && is_absorbing_element(lhs[1]) && is_absorbing_element(lhs[2]) && is_absorbing_element(lhs[3]);
	}
	template <class T1, class T2, class Re, class Im1, class Im2, class Im3, class Param, class = enable_if_t<dec::quaternion_eq2_v<is_rscalar_operation_v<quaternion<T1>, T2>, T1, T2>>>
	auto operator==(quaternion_parameter<T1, Re, Im1, Im2, Im3>, type_parameter<T2, Param> rhs) {
		return (Re() == rhs) && int_parameter<bool, is_absorbing_element(Im1::value)>()
			&& int_parameter<bool, is_absorbing_element(Im2::value)>() && int_parameter<bool, is_absorbing_element(Im3::value)>();
	}
	template <class T1, class T2, class = enable_if_t<dec::quaternion_eq3_v<is_rscalar_operation_v<T1, quaternion<T2>>, T1, T2>>>
	constexpr bool operator==(const T1& lhs, const quaternion<T2>& rhs) {
		return (lhs == rhs[0]) && is_absorbing_element(rhs[1]) && is_absorbing_element(rhs[2]) && is_absorbing_element(rhs[3]);
	}
	template <class T1, class T2, class Param, class Re, class Im1, class Im2, class Im3, class = enable_if_t<dec::quaternion_eq3_v<is_lscalar_operation_v<T1, quaternion<T2>>, T1, T2>>>
	auto operator==(type_parameter<T1, Param> lhs, quaternion_parameter<T2, Re, Im1, Im2, Im3>) {
		return (lhs == Re()) && int_parameter<bool, is_absorbing_element(Im1::value)>()
			&& int_parameter<bool, is_absorbing_element(Im2::value)>() && int_parameter<bool, is_absorbing_element(Im3::value)>();
	}
	template <class T1, class T2, class = enable_if_t<dec::quaternion_eq2_v<!is_scalar_operation_v<quaternion<T1>, complex<T2>>, T1, T2>>>
	constexpr bool operator==(const quaternion<T1>& lhs, complex<T2>& rhs) {
		return (lhs[0] == rhs[0]) && (lhs[1] == rhs[1]) && is_absorbing_element(lhs[2]) && is_absorbing_element(lhs[3]);
	}
	template <class T1, class T2, class Re1, class Im1_1, class Im2_1, class Im3_1, class Re2, class Im_2, class = enable_if_t<dec::quaternion_eq2_v<!is_scalar_operation_v<quaternion<T1>, complex<T2>>, T1, T2>>>
	auto operator==(quaternion_parameter<T1, Re1, Im1_1, Im2_1, Im3_1>, complex_parameter<T2, Re2, Im_2>) {
		return (Re1() == Re2()) && (Im1_1() == Im_2())
			&& int_parameter<bool, is_absorbing_element(Im2_1::value)>() && int_parameter<bool, is_absorbing_element(Im3_1::value)>();
	}
	template <class T1, class T2, class = enable_if_t<dec::quaternion_eq3_v<!is_scalar_operation_v<complex<T1>, quaternion<T2>>, T1, T2>>>
	constexpr bool operator==(const complex<T1>& lhs, const quaternion<T2>& rhs) {
		return (lhs[0] == rhs[0]) && (lhs[1] == rhs[1]) && is_absorbing_element(rhs[2]) && is_absorbing_element(rhs[3]);
	}
	template <class T1, class T2, class Re1, class Im_1, class Re2, class Im1_2, class Im2_2, class Im3_2, class = enable_if_t<dec::quaternion_eq3_v<!is_scalar_operation_v<complex<T1>, quaternion<T2>>, T1, T2>>>
	auto operator==(complex_parameter<T1, Re1, Im_1>, quaternion_parameter<T2, Re2, Im1_2, Im2_2, Im3_2>) {
		return (Re1() == Re2()) && (Im_1() == Im1_2())
			&& int_parameter<bool, is_absorbing_element(Im2_2::value)>() && int_parameter<bool, is_absorbing_element(Im3_2::value)>();
	}
	template <class T1, class T2, class = enable_if_t<dec::quaternion_eq1_v<!is_scalar_operation_v<quaternion<T1>, quaternion<T2>>, T1, T2>>>
	constexpr bool operator!=(const quaternion<T1>& lhs, const quaternion<T2>& rhs) { return !(lhs == rhs); }
	template <class T1, class T2, class Re1, class Im1_1, class Im2_1, class Im3_1, class Re2, class Im1_2, class Im2_2, class Im3_2, class = enable_if_t<dec::quaternion_eq1_v<!is_scalar_operation_v<quaternion<T1>, quaternion<T2>>, T1, T2>>>
	auto operator!=(quaternion_parameter<T1, Re1, Im1_1, Im2_1, Im3_1> lhs, quaternion_parameter<T1, Re2, Im1_2, Im2_2, Im3_2> rhs) { return !(lhs == rhs); }
	template <class T1, class T2, class = enable_if_t<dec::quaternion_eq2_v<is_rscalar_operation_v<quaternion<T1>, T2>, T1, T2>>>
	constexpr bool operator!=(const quaternion<T1>& lhs, T2& rhs) { return !(lhs == rhs); }
	template <class T1, class T2, class Re, class Im1, class Im2, class Im3, class Param, class = enable_if_t<dec::quaternion_eq2_v<is_rscalar_operation_v<quaternion<T1>, T2>, T1, T2>>>
	auto operator!=(quaternion_parameter<T1, Re, Im1, Im2, Im3> lhs, type_parameter<T2, Param> rhs) { return !(lhs == rhs); }
	template <class T1, class T2, class = enable_if_t<dec::quaternion_eq3_v<is_rscalar_operation_v<T1, quaternion<T2>>, T1, T2>>>
	constexpr bool operator!=(const T1& lhs, const quaternion<T2>& rhs) { return !(lhs == rhs); }
	template <class T1, class T2, class Param, class Re, class Im1, class Im2, class Im3, class = enable_if_t<dec::quaternion_eq3_v<is_rscalar_operation_v<T1, quaternion<T2>>, T1, T2>>>
	auto operator!=(type_parameter<T1, Param> lhs, quaternion_parameter<T2, Re, Im1, Im2, Im3> rhs) { return !(lhs == rhs); }
	template <class T1, class T2, class = enable_if_t<dec::quaternion_eq2_v<!is_scalar_operation_v<quaternion<T1>, complex<T2>>, T1, T2>>>
	constexpr bool operator!=(const quaternion<T1>& lhs, complex<T2>& rhs) { return !(lhs == rhs); }
	template <class T1, class T2, class Re1, class Im1_1, class Im2_1, class Im3_1, class Re2, class Im_2, class = enable_if_t<dec::quaternion_eq2_v<!is_scalar_operation_v<quaternion<T1>, complex<T2>>, T1, T2>>>
	auto operator!=(quaternion_parameter<T1, Re1, Im1_1, Im2_1, Im3_1> lhs, complex_parameter<T2, Re2, Im_2> rhs) { return !(lhs == rhs); }
	template <class T1, class T2, class = enable_if_t<dec::quaternion_eq3_v<!is_scalar_operation_v<complex<T1>, quaternion<T2>>, T1, T2>>>
	constexpr bool operator!=(const complex<T1>& lhs, const quaternion<T2>& rhs) { return !(lhs == rhs); }
	template <class T1, class T2, class Re1, class Im_1, class Re2, class Im1_2, class Im2_2, class Im3_2, class = enable_if_t<dec::quaternion_eq3_v<!is_scalar_operation_v<complex<T1>, quaternion<T2>>, T1, T2>>>
	auto operator!=(complex_parameter<T1, Re1, Im_1> lhs, quaternion_parameter<T2, Re2, Im1_2, Im2_2, Im3_2> rhs) { return !(lhs == rhs); }

	//四元数の判定
	template <class T>
	struct is_quaternion_impl : false_type {};
	template <class T>
	struct is_quaternion_impl<quaternion<T>> : true_type {};
	template <class T>
	struct is_quaternion : is_quaternion_impl<remove_cv_t<T>> {};

	//四元数の除去
	template <class T>
	struct remove_quaternion {
		using type = T;
	};
	template <class T>
	struct remove_quaternion<quaternion<T>> {
		using type = T;
	};

	//全ての四元数の除去
	template <class T>
	struct remove_all_quaternion {
		using type = T;
	};
	template <class T>
	struct remove_all_quaternion<quaternion<T>> : remove_all_quaternion<T> {};


	template <class From, class To>
	struct is_inclusion_quaternion : is_inclusion<From, typename To::base_type> {};
	template <class From, class To>
	struct is_inclusion_quaternion<complex<From>, To> : cat_bool<
		//complex<From>がToの基底となる場合も含めて判定
		is_inclusion_v<complex<From>, typename To::base_type> || is_inclusion_v<From, typename To::base_type>
	> {};
	template <class From, class To>
	struct is_inclusion_quaternion<quaternion<From>, To> : cat_bool<
		//quaternion<From>がToの基底となる場合も含めて判定
		is_inclusion_v<quaternion<From>, typename To::base_type> || is_inclusion_v<From, typename To::base_type>
	> {};
	//Fromの型によりis_inclusion_quaternionで分岐
	template <class From, class To>
	struct is_inclusion<From, quaternion<To>> : is_inclusion_quaternion<From, quaternion<To>> {};


	//加法パラメータ取得
	template <class T>
	struct addition_traits<quaternion<T>> {
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
	struct multiplication_traits<quaternion<T>> {
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
		static constexpr bool commutative_value = false;
		//分配律
		static constexpr bool distributive_value = multiplication_traits<T>::distributive_value;
	};


	//逆元が存在するならば逆元の取得(存在しない場合は例外を出す)
	template <class T>
	struct Inverse_element<quaternion<T>> {
		static constexpr quaternion<T> _additive_inverse_(const quaternion<T>& q) {
			return quaternion<T>(additive_inverse(q[0]), additive_inverse(q[1]), additive_inverse(q[2]), additive_inverse(q[3]));
		}

		//Tに乗法逆元が存在する場合の乗法逆元
		static constexpr quaternion<T> _multiplicative_inverse_impl_(const quaternion<T>& q, true_type) {
			//共役を絶対値の2乗で割る
			T temp = q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3];
			return quaternion<T>(q[0] / temp, -q[1] / temp, -q[2] / temp, -q[3] / temp);
		}
		//Tの乗法逆元が存在しない場合の乗法逆元
		static constexpr quaternion<T> _multiplicative_inverse_impl_(const quaternion<T>& q, false_type) {
			T temp = multiplicative_inverse(q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3]);
			return quaternion<T>(q[0] * temp, -q[1] * temp, -q[2] * temp, -q[3] * temp);
		}
		static constexpr quaternion<T> _multiplicative_inverse_(const quaternion<T>& q) {
			return _multiplicative_inverse_impl_(q, bool_constant<is_exist_multiplicative_inverse_v<T>>());
		}
	};


	//誤差評価
	template <class T>
	struct Error_evaluation<quaternion<T>> {
		static constexpr quaternion<T> epsilon() { return quaternion<T>(Error_evaluation<T>::epsilon(), Error_evaluation<T>::epsilon()); }
		static constexpr bool _error_evaluation_(const quaternion<T>& x1, const quaternion<T>& x2) {
			return error_evaluation(x1[0], x2[0]) && error_evaluation(x1[1], x2[1]) && error_evaluation(x1[2], x2[2]) && error_evaluation(x1[3], x2[3]);
		}
	};
}


#endif