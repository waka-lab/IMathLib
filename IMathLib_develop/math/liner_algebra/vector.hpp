#ifndef IMATH_MATH_LINER_ALGEBRA_VECTOR_HPP
#define IMATH_MATH_LINER_ALGEBRA_VECTOR_HPP

#include "IMathLib/utility/utility/tuple_type.hpp"
#include "IMathLib/math/math/math_traits.hpp"
#include "IMathLib/math/math/type_parameter.hpp"
#include "IMathLib/math/math/conj.hpp"
#include "IMathLib/container/array.hpp"


namespace iml {

	template<class, size_t>
	class vector;


	//ベクトル型のパラメータ
	template <class T, size_t N, class... Types>
	using vector_parameter = type_parameter<vector<T, N>, type_tuple<Types...>>;
	template <class T, size_t N, class... Params>
	struct type_parameter<vector<T, N>, type_tuple<type_parameter<T, Params>...>> {
		static_assert(N == type_tuple<type_parameter<T, Params>...>::value, "There must be N parameters.");

		using type = vector<T, N>;
		static constexpr type value = type(type_parameter<T, Params>::value...);

		template <class = enable_if_t<is_exist_additive_inverse_v<T>>>
		auto operator-() const { return vector_parameter<T, N, decltype(-type_parameter<T, Params>())...>(); }
		auto operator+() const { return *this; }

		template <class T, T Val, class = enable_if_t<(Val >= 0) && (Val < N)>>
		auto operator[](int_parameter<T, Val>) const { return at_type_t<Val, type_parameter<T, Params>...>(); }
	};


	//2項演算の十分条件を補完するメタ関数の定義
	namespace dec {
		//[name]1 : M(T1)×M(T2)			//上位の数学型の場合をも含む
		//[name]2 : M(T1)×T2
		//[name]3 : T1×M(T2)

#define VECTOR_DECISION(NAME, TERMS)\
		template <bool, class T1, class T2> struct vector_##NAME : bool_constant<TERMS> {};\
		template <class T1, class T2> struct vector_##NAME##<false, T1, T2> : false_type {};\
		template <bool F, class T1, class T2> constexpr bool vector_##NAME##_v = vector_##NAME##<F, T1, T2>::value;
		VECTOR_DECISION(mul1, (is_magma<mul_result_t<T1, T2>>::add_value));
		VECTOR_DECISION(mul2, (is_calcable<T1, T2>::mul_value));
		VECTOR_DECISION(mul3, (is_calcable<T1, T2>::mul_value));
#undef VECTOR_DECISION
	}


	template <class, size_t N, class = index_range_t<size_t, 0, N>>
	class vector_base;
	template <class T, size_t N, size_t... Indices>
	class vector_base<T, N, index_tuple<size_t, Indices...>> {
		template <class, size_t> friend class vector;
		template <class, size_t, class> friend class vector_base;
	protected:
		T x_m[N];
	public:
		constexpr vector_base() : x_m{} {}
		constexpr vector_base(const identity_t<T, Indices>&... args) : x_m{ args... } {}
		template <class U, class = enable_if_t<is_inclusion_v<U, T>>>
		constexpr vector_base(const identity_t<U, Indices>&... args) : x_m{ args... } {}
		constexpr vector_base(const vector<T, N>& v) : x_m{ v.x_m[Indices]... } {}
		template <class U, class = enable_if_t<is_inclusion_v<U, T>>>
		constexpr vector_base(const vector<U, N>& v) : x_m{ v.x_m[Indices]... } {}

		//単項演算
		template <class = enable_if_t<is_exist_additive_inverse_v<T>>>
		constexpr vector<T, N> operator-() const { return vector<T, N>(-this->x_m[Indices]...); }
		constexpr vector<T, N> operator+() const { return vector<T, N>(*this); }

		//2項演算(多重定義防止にスカラー型でない方をTとして扱い，そうでないならT1 = Tとして扱う)
		template <class T2, class = enable_if_t<is_calcable<T, T2>::add_value>>
		friend constexpr auto operator+(const vector<T, N>& lhs, const vector<T2, N>& rhs) {
			return vector<add_result_t<T, T2>, N>((lhs[Indices] + rhs[Indices])...);
		}
		template <class T2, class... Types1, class... Types2, class = enable_if_t<is_calcable<T, T2>::add_value>>
		friend auto operator+(vector_parameter<T, N, Types1...>, vector_parameter<T2, N, Types2...>) {
			return vector_parameter<add_result_t<T, T2>, N, decltype(Types1() + Types2())...>();
		}
		template <class T2, class = enable_if_t<is_calcable<T, T2>::sub_value>>
		friend constexpr auto operator-(const vector<T, N>& lhs, const vector<T2, N>& rhs) {
			return vector<sub_result_t<T, T2>, N>((lhs[Indices] - rhs[Indices])...);
		}
		template <class T2, class... Types1, class... Types2, class = enable_if_t<is_calcable<T, T2>::sub_value>>
		friend auto operator-(vector_parameter<T, N, Types1...>, vector_parameter<T2, N, Types2...>) {
			return vector_parameter<sub_result_t<T, T2>, N, decltype(Types1() - Types2())...>();
		}
		template <class T2, class = enable_if_t<dec::vector_mul2_v<is_rscalar_operation_v<vector<T, N>, T2>, T, T2>>>
		friend constexpr auto operator*(const vector<T, N>& lhs, const T2& rhs) {
			return vector<mul_result_t<T, T2>, N>((lhs[Indices] * rhs)...);
		}
		template <class T2, class... Types, class Param, class = enable_if_t<dec::vector_mul2_v<is_rscalar_operation_v<vector<T, N>, T2>, T, T2>>>
		friend auto operator*(vector_parameter<T, N, Types...>, type_parameter<T2, Param> rhs) {
			return vector_parameter<mul_result_t<T, T2>, N, decltype(Types() * rhs)...>();
		}
		template <class T1, class = enable_if_t<dec::vector_mul3_v<is_lscalar_operation_v<T1, vector<T, N>>, T1, T>>>
		friend constexpr auto operator*(const T1& lhs, const vector<T, N>& rhs) {
			return vector<mul_result_t<T1, T>, N>((lhs * rhs[Indices])...);
		}
		template <class T1, class Param, class... Types, class = enable_if_t<dec::vector_mul3_v<is_lscalar_operation_v<T1, vector<T, N>>, T1, T>>>
		friend auto operator*(type_parameter<T1, Param> lhs, vector_parameter<T, N, Types...>) {
			return vector_parameter<mul_result_t<T1, T>, N, decltype(lhs * Types())...>();
		}
		template <class T2, class = enable_if_t<is_calcable<T, T2>::div_value>>
		friend constexpr auto operator/(const vector<T, N>& lhs, const T2& rhs) {
			return vector<div_result_t<T, T2>, N>((lhs[Indices] / rhs)...);
		}
		template <class T2, class... Types, class Param, class = enable_if_t<is_calcable<T, T2>::div_value>>
		friend auto operator/(vector_parameter<T, N, Types...>, type_parameter<T2, Param> rhs) {
			return vector_parameter<div_result_t<T, T2>, N, decltype(Types() / rhs)...>();
		}
	};

	//ベクトル型
	template <class T, size_t N>
	class vector : public vector_base<T, N>, public value_list_input<array_iterator<T>> {
		//Nは0より大きくなければならない
		static_assert(N > 0, "N must be greater than 0.");

		template <class, size_t> friend class vector;
		template <class, size_t, class> friend class vector_base;
	public:
		using vector_base<T, N>::vector_base;

		using base_type = T;
		using iterator = array_iterator<T>;
		using const_iterator = array_iterator<const T>;

		template <class Other>
		struct rebind {
			using other = vector<Other, N>;
		};
		template <class Other>
		using rebind_t = vector<Other, N>;

		constexpr iterator begin() noexcept { return iterator(x_m); }
		constexpr const_iterator begin() const noexcept { return const_iterator(x_m); }
		constexpr iterator end() noexcept { return iterator(x_m + N); }
		constexpr const_iterator end() const noexcept { return const_iterator(x_m + N); }

		//単項演算
		using vector_base<T, N>::operator-;
		using vector_base<T, N>::operator+;
		//代入演算
		vector& operator=(const vector& v) {
			if (this != addressof(v)) for (size_t i = 0; i < N; ++i) this->x_m[i] = v.x_m[i];
			return *this;
		}
		template <class U, class = enable_if_t<is_inclusion_v<U, T>>>
		vector& operator=(const vector<U, N>& v) {
			for (size_t i = 0; i < N; ++i) this->x_m[i] = v.x_m[i];
			return *this;
		}
		template <class U, class = enable_if_t<is_operation<T, U, T>::add_value>>
		vector& operator+=(const vector<U, N>& v) {
			for (size_t i = 0; i < N; ++i) this->x_m[i] += v.x_m[i];
			return *this;
		}
		template <class U, class = enable_if_t<is_operation<T, U, T>::sub_value>>
		vector& operator-=(const vector<U, N>& v) {
			for (size_t i = 0; i < N; ++i) this->x_m[i] -= v.x_m[i];
			return *this;
		}
		template <class U, class = enable_if_t<dec::vector_mul2_v<is_operation<T, U, T>::mul_value && is_rscalar_operation_v<vector, U>, T, U>>>
		vector& operator*=(const U& k) {
			for (size_t i = 0; i < N; ++i) this->x_m[i] *= k;
			return *this;
		}
		template <class U, class = enable_if_t<is_operation<T, U, T>::div_value>>
		vector& operator/=(const U& k) {
			for (size_t i = 0; i < N; ++i) this->x_m[i] /= k;
			return *this;
		}
		//2項演算
		using value_list_input<array_iterator<T>>::operator<<;

		//添え字演算
		const constexpr T& operator[](size_t index) const { return this->x_m[index]; }
		constexpr T& operator[](size_t index) { return this->x_m[index]; }

		//ストリーム出力
		friend std::ostream& operator<<(std::ostream& os, const vector& v) {
			os << v.x_m[0];
			for (size_t i = 1; i < N; ++i) os << ',' << v.x_m[i];
			return os;
		}
		friend std::wostream& operator<<(std::wostream& os, const vector& v) {
			os << v.x_m[0];
			for (size_t i = 1; i < N; ++i) os << L',' << v.x_m[i];
			return os;
		}
	};


	//内積
	template <class T1, class T2, size_t N, class = enable_if_t<dec::vector_mul1_v<!is_rscalar_operation_v<vector<T1, N>, vector<T2, N>>, T1, T2>>>
	constexpr mul_result_t<T1, T2> operator*(const vector<T1, N>& lhs, const vector<T2, N>& rhs) {
		mul_result_t<T1, T2> result = lhs[0] * conj(rhs[0]);
		for (size_t i = 1; i < N; ++i) result += lhs[i] * conj(rhs[i]);
		return result;
	}
	template <class T1, class T2, size_t N, class... Types1, class... Types2, class = enable_if_t<dec::vector_mul1_v<!is_rscalar_operation_v<vector<T1, N>, vector<T2, N>>, T1, T2>>>
	auto operator*(vector_parameter<T1, N, Types1...>, vector_parameter<T2, N, Types2...>) {
		return tp::sum((Types1() * Types2())...);
	}
	//外積(T1×T2→Sが加法についてループ)
	template <class T1, class T2
		, class = typename enable_if<is_calcable<T1, T2>::mul_value && is_loop<typename calculation_result<T1, T2>::mul_type>::add_value>::type>
		inline vector<typename calculation_result<T1, T2>::mul_type, 3> operator%(const vector<T1, 3>& lhs, const vector<T2, 3>& rhs) {
		return vector<typename calculation_result<T1, T2>::mul_type, 3>(lhs[1] * rhs[2] - lhs[2] * rhs[1], lhs[2] * rhs[0] - lhs[0] * rhs[2], lhs[0] * rhs[1] - lhs[1] * rhs[0]);
	}


	//比較演算
	template <class T1, class T2, size_t N, class = enable_if_t<is_calcable<T1, T2>::eq_value>>
	constexpr bool operator==(const vector<T1, N>& lhs, const vector<T2, N>& rhs) {
		for (size_t i = 0; i < N; ++i) if (lhs[i] != rhs[i]) return false;
		return true;
	}
	template <class T1, class T2, size_t N, class... Types1, class... Types2, class = enable_if_t<is_calcable<T1, T2>::eq_value>>
	auto operator==(vector_parameter<T1, N, Types1...>, vector_parameter<T2, N, Types2...>) {
		constexpr bool temp = tp::sum((Types1() == Types2())...)::value == N;
		return int_parameter<bool, temp>();
	}
	template <class T1, class T2, size_t N, class = enable_if_t<is_calcable<T1, T2>::eq_value>>
	constexpr bool operator!=(const vector<T1, N>& lhs, const vector<T2, N>& rhs) { return !(lhs == rhs); }
	template <class T1, class T2, size_t N, class... Types1, class... Types2, class = enable_if_t<is_calcable<T1, T2>::eq_value>>
	auto operator!=(vector_parameter<T1, N, Types1...> rhs, vector_parameter<T2, N, Types2...> lhs) { return !(lhs == rhs); }


	//ベクトル型の生成
	template <class First, class... Types>
	inline constexpr vector<reference_unwrapper_t<First>, sizeof...(Types) + 1> make_vector(First&& first, Types&&... args) {
		return vector<reference_unwrapper_t<First>, sizeof...(Types) + 1>(forward<First>(first), forward<Types>(args)...);
	}



	//ベクトルの判定
	template <class T>
	struct is_vector_impl : false_type {};
	template <class T, size_t N>
	struct is_vector_impl<vector<T, N>> : true_type {};
	template <class T>
	struct is_vector : is_vector_impl<remove_cv_t<T>> {};
	template <class T>
	constexpr bool is_vector_v = is_vector<T>::value;

	//ベクトルの除去
	template <class T>
	struct remove_vector {
		using type = T;
	};
	template <class T, size_t N>
	struct remove_vector<vector<T, N>> {
		using type = T;
	};
	template <class T>
	using remove_vector_t = typename remove_vector<T>::type;


	//各種次元のベクトルの定義
	template <class T>
	using vector2 = vector<T, 2>;
	template <class T>
	using vector3 = vector<T, 3>;
	template <class T>
	using vector4 = vector<T, 4>;


	template <class From, class To, size_t N>
	struct is_inclusion<vector<From, N>, vector<To, N>> : is_inclusion<From, To> {};


	//加法の特性
	template <class T, size_t N>
	struct addition_traits<vector<T, N>> {
		//単位元
		template <class = enable_if_t<is_exist_additive_identity_v<T>>>
		static constexpr vector<T, N> identity_element() { return vector<T, N>(); }
		//結合律
		static constexpr bool associative_value = addition_traits<T>::associative_value;
		//消約律
		static constexpr bool cancellative_value = addition_traits<T>::cancellative_value;
		//可換律
		static constexpr bool commutative_value = addition_traits<T>::commutative_value;
	};
	//乗法の特性
	template <class T, size_t N>
	struct multiplication_traits<vector<T, N>> {
		//吸収元
		template <class = enable_if_t<is_calcable<T, T>::mul_value && is_exist_absorbing_element_v<T>>>
		static constexpr vector<T, N> absorbing_element() { return vector<T, N>(); }
		//結合律
		static constexpr bool associative_value = false;
		//消約律
		static constexpr bool cancellative_value = false;
		//可換律
		static constexpr bool commutative_value = multiplication_traits<T>::cancellative_value;
		//分配律
		static constexpr bool distributive_value = multiplication_traits<T>::distributive_value;
	};

	//逆元が存在するならば逆元の取得(存在しない場合は例外を出す)
	template <class T, size_t N>
	struct Inverse_element<vector<T, N>> {
		template <size_t... Indices>
		static constexpr vector<T, N> _additive_inverse_impl_(const vector<T, N>& v, index_tuple<size_t, Indices...>) {
			return vector<T, N>(additive_inverse(v[Indices])...);
		}
		static constexpr vector<T, N> _additive_inverse_(const vector<T, N>& v) {
			return _additive_inverse_impl_(v, index_range_t<size_t, 0, N>());
		}
	};


	//誤差評価
	template <class T, size_t N>
	struct Error_evaluation<vector<T, N>> {
		template <size_t... Indices>
		static constexpr vector<T, N> epsilon_impl(index_tuple<size_t, Indices...>) { return vector<T, N>(Error_evaluation<identity_t<T, Indices>>::epsilon()...); }
		static constexpr vector<T, N> epsilon() { return epsilon_impl(index_range_t<size_t, 0, N>()); }
		static constexpr bool _error_evaluation_(const vector<T, N>& x1, const vector<T, N>& x2) {
			for (size_t i = 0; i < N; ++i) if (!error_evaluation(x1[i], x2[i])) return false;
			return true;
		}
	};
}

#endif