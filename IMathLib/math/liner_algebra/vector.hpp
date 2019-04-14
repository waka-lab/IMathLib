#ifndef IMATH_MATH_LINER_ALGEBRA_VECTOR_HPP
#define IMATH_MATH_LINER_ALGEBRA_VECTOR_HPP

#include "IMathLib/utility/utility/tuple_type.hpp"
#include "IMathLib/math/math/math_traits.hpp"
#include "IMathLib/math/math/type_parameter.hpp"
#include "IMathLib/math/math/conj.hpp"
#include "IMathLib/container/array.hpp"


namespace iml {

	//ベクトル型の代入演算の定義
#define	IMATH_VECTOR_ASSIGNMENT_OPERATION \
	template <class = enable_if_t<is_inclusion_v<T, Base>>>\
	vector_base& operator=(const vector<T, N>& v) {\
		/*同じインスタンスでなければ代入*/\
		if (this != addressof(v)) for (size_t i = 0; i < N; ++i) this->x[i] = v.x[i];\
		return *this;\
	}\
	template <class = enable_if_t<is_operation<Base, T, Base>::add_value>>\
	vector_base& operator+=(const vector<T, N>& v) {\
		for (size_t i = 0; i < N; ++i) this->x[i] += v.x[i];\
		return *this;\
	}\
	template <class = enable_if_t<is_operation<Base, T, Base>::sub_value>>\
	vector_base& operator-=(const vector<T, N>& v) {\
		for (size_t i = 0; i < N; ++i) this->x[i] -= v.x[i];\
		return *this;\
	}\
	template <class = enable_if_t<is_operation<Base, T, Base>::mul_value>>\
	vector_base& operator*=(const T& k) {\
		for (size_t i = 0; i < N; ++i) this->x[i] *= k;\
		return *this;\
	}\
	template <class = enable_if_t<is_operation<Base, T, Base>::div_value>>\
	vector_base& operator/=(const T& k) {\
		for (size_t i = 0; i < N; ++i) this->x[i] /= k;\
		return *this;\
	}
	//Base == T の場合のベクトル型の2項演算の定義
#define IMATH_VECTOR_BINARY_OPERATION1 \
	template <class = enable_if_t<is_calcable<Base, T>::add_value>>\
	friend constexpr vector<add_result_t<Base, T>, N> operator+(const vector<Base, N>& lhs, const vector<T, N>& rhs) {\
		return vector<add_result_t<Base, T>, N>((lhs[Indices] + rhs[Indices])...);\
	}\
	template <class = enable_if_t<is_calcable<Base, T>::sub_value>>\
	friend constexpr vector<sub_result_t<Base, T>, N> operator-(const vector<Base, N>& lhs, const vector<T, N>& rhs) {\
		return vector<sub_result_t<Base, T>, N>((lhs[Indices] - rhs[Indices])...);\
	}\
	template <class = enable_if_t<is_calcable<Base, T>::mul_value>>\
	friend constexpr vector<mul_result_t<Base, T>, N> operator*(const vector<Base, N>& lhs, const T& rhs) {\
		return vector<mul_result_t<Base, T>, N>((lhs[Indices] * rhs)...);\
	}\
	template <class = enable_if_t<is_calcable<Base, T>::mul_value>>\
	friend constexpr vector<mul_result_t<T, Base>, N> operator*(const T& lhs, const vector<Base, N>& rhs) {\
		return vector<mul_result_t<T, Base>, N>((lhs * rhs[Indices])...);\
	}\
	template <class = enable_if_t<is_calcable<Base, T>::div_value>>\
	friend constexpr vector<div_result_t<Base, T>, N> operator/(const vector<Base, N>& lhs, const T& rhs) {\
		return vector<div_result_t<Base, T>, N>((lhs[Indices] / rhs)...);\
	}\
	/*内積(積が加法についてマグマ)*/\
	template <class = enable_if_t<is_magma<mul_result_t<Base, T>>::add_value>>\
	friend constexpr mul_result_t<Base, T> operator*(const vector<Base, N>& lhs, const vector<T, N>& rhs) {\
		mul_result_t<Base, T> result = lhs[0] * conj(rhs[0]);\
		for (size_t i = 1; i < N; ++i) result += lhs[i] * conj(rhs[i]);\
		return result;\
	}\
	/*比較演算*/\
	template <class = enable_if_t<is_calcable<Base, T>::eq_value>>\
	friend constexpr bool operator==(const vector<Base, N>& lhs, const vector<T, N>& rhs) {\
		for (size_t i = 0; i < N; ++i) if (lhs[i] != rhs[i]) return false;\
		return true;\
	}\
	template <class = enable_if_t<is_calcable<Base, T>::eq_value>>\
	friend constexpr bool operator!=(const vector<Base, N>& lhs, const vector<T, N>& rhs) { return !(lhs == rhs); }
	//Base != T の場合の順序を考慮したベクトル型の2項演算の定義
#define IMATH_VECTOR_BINARY_OPERATION2 \
	IMATH_VECTOR_BINARY_OPERATION1;\
	template <class = enable_if_t<is_calcable<T, Base>::add_value>>\
	friend constexpr vector<add_result_t<T, Base>, N> operator+(const vector<T, N>& lhs, const vector<Base, N>& rhs) {\
		return vector<add_result_t<T, Base>, N>((lhs[Indices] + rhs[Indices])...);\
	}\
	template <class = enable_if_t<is_calcable<T, Base>::sub_value>>\
	friend constexpr vector<sub_result_t<T, Base>, N> operator-(const vector<T, N>& lhs, const vector<Base, N>& rhs) {\
		return vector<sub_result_t<T, Base>, N>((lhs[Indices] - rhs[Indices])...);\
	}\
	/*内積(積が加法についてマグマ)*/\
	template <class = enable_if_t<is_magma<mul_result_t<T, Base>>::add_value>>\
	friend constexpr mul_result_t<T, Base> operator*(const vector<T, N>& lhs, const vector<Base, N>& rhs) {\
		mul_result_t<T, Base> result = lhs[0] * conj(rhs[0]);\
		for (size_t i = 1; i < N; ++i) result += lhs[i] * conj(rhs[i]);\
		return result;\
	}\
	/*比較演算*/\
	template <class = enable_if_t<is_calcable<T, Base>::eq_value>>\
	friend constexpr bool operator==(const vector<T, N>& lhs, const vector<Base, N>& rhs) {\
		for (size_t i = 0; i < N; ++i) if (lhs[i] != rhs[i]) return false;\
		return true;\
	}\
	template <class = enable_if_t<is_calcable<T, Base>::eq_value>>\
	friend constexpr bool operator!=(const vector<T, N>& lhs, const vector<Base, N>& rhs) { return !(lhs == rhs); }
	//Base == T の場合のベクトル型のパラメータの2項演算の定義
#define IMATH_VECTOR_PARAMETER_BINARY_OPERATION1 \
	template <class... Types1, class... Types2, class = enable_if_t<is_calcable<Base, T>::add_value>>\
	friend auto operator+(vector_parameter<Base, N, Types1...>, vector_parameter<T, N, Types2...>) {\
		return vector_parameter<add_result_t<Base, T>, N, decltype(Types1() + Types2())...>();\
	}\
	template <class... Types1, class... Types2, class = enable_if_t<is_calcable<Base, T>::sub_value>>\
	friend auto operator-(vector_parameter<Base, N, Types1...>, vector_parameter<T, N, Types2...>) {\
		return vector_parameter<sub_result_t<Base, T>, N, decltype(Types1() - Types2())...>();\
	}\
	template <class... Types, class Param, class = enable_if_t<is_calcable<Base, T>::mul_value>>\
	friend auto operator*(vector_parameter<Base, N, Types...>, type_parameter<T, Param> rhs) {\
		return vector_parameter<mul_result_t<Base, T>, N, decltype(Types() * rhs)...>();\
	}\
	template <class Param, class... Types, class = enable_if_t<is_calcable<Base, T>::mul_value>>\
	friend auto operator*(type_parameter<T, Param> lhs, vector_parameter<Base, N, Types...>) {\
		return vector_parameter<mul_result_t<T, Base>, N, decltype(lhs * Types())...>();\
	}\
	template <class... Types, class Param, class = enable_if_t<is_calcable<Base, T>::div_value>>\
	friend auto operator/(vector_parameter<Base, N, Types...>, type_parameter<T, Param> rhs) {\
		return vector_parameter<div_result_t<Base, T>, N, decltype(Types() / rhs)...>();\
	}\
	/*内積(積が加法についてマグマ)*/\
	template <class... Types1, class... Types2, class = enable_if_t<is_magma<mul_result_t<Base, T>>::add_value>>\
	friend auto operator*(vector_parameter<Base, N, Types1...>, vector_parameter<T, N, Types2...>) {\
		return tp::sum((Types1() * Types2())...);\
	}\
	/*比較演算*/\
	template <class... Types1, class... Types2, class = enable_if_t<is_calcable<Base, T>::eq_value>>\
	friend auto operator==(vector_parameter<Base, N, Types1...>, vector_parameter<T, N, Types2...>) {\
		constexpr bool temp = tp::sum((Types1() == Types2())...)::value == N;\
		return int_parameter<bool, temp>();\
	}\
	template <class... Types1, class... Types2, class = enable_if_t<is_calcable<Base, T>::eq_value>>\
	friend auto operator!=(vector_parameter<Base, N, Types1...>, vector_parameter<T, N, Types2...>) { return !(lhs == rhs); }
	//Base != T の場合の順序を考慮したベクトル型のパラメータの2項演算の定義
#define IMATH_VECTOR_PARAMETER_BINARY_OPERATION2 \
	IMATH_VECTOR_PARAMETER_BINARY_OPERATION1;\
	template <class... Types1, class... Types2, class = enable_if_t<is_calcable<T, Base>::add_value>>\
	friend auto operator+(vector_parameter<T, N, Types1...>, vector_parameter<Base, N, Types2...>) {\
		return vector_parameter<add_result_t<T, Base>, N, decay_t<decltype(Types1() + Types2())>...>();\
	}\
	template <class... Types1, class... Types2, class = enable_if_t<is_calcable<T, Base>::sub_value>>\
	friend auto operator-(vector_parameter<T, N, Types1...>, vector_parameter<Base, N, Types2...>) {\
		return vector_parameter<sub_result_t<T, Base>, N, decay_t<decltype(Types1() + Types2())>...>();\
	}\
	/*内積(積が加法についてマグマ)*/\
	template <class... Types1, class... Types2, class = enable_if_t<is_magma<mul_result_t<T, Base>>::add_value>>\
	friend auto operator*(vector_parameter<T, N, Types1...>, vector_parameter<Base, N, Types2...>) {\
		return tp::sum((Types1() * Types2())...);\
	}\
	/*比較演算*/\
	template <class... Types1, class... Types2, class = enable_if_t<is_calcable<T, Base>::eq_value>>\
	friend auto operator==(vector_parameter<T, N, Types1...>, vector_parameter<Base, N, Types2...>) {\
		constexpr bool temp = tp::sum((Types1() == Types2())...)::value == N;\
		return int_parameter<bool, temp>();\
	}\
	template <class... Types1, class... Types2, class = enable_if_t<is_calcable<T, Base>::eq_value>>\
	friend auto operator!=(vector_parameter<T, N, Types1...>, vector_parameter<Base, N, Types2...>) { return !(lhs == rhs); }


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


	template <class Base, size_t N, class T, class = index_range_t<size_t, 0, N>, bool = is_algebraic_structure_v<T>, bool = is_same_v<Base, T>>
	class vector_base;


	//下に階層が存在しないかつBase == T
	template <class Base, size_t N, class T, size_t... Indices>
	class vector_base<Base, N, T, index_tuple<size_t, Indices...>, false, true> {
		template <class, size_t> friend class vector;
		template <class, size_t, class, class, bool, bool> friend class vector_base;
	protected:
		Base x[N];
	public:
		//Base == TとなるためTypesによるコンストラクタは作成しない
		constexpr vector_base() : x{} {}
		constexpr vector_base(const identity_t<Base, Indices>&... x) : x{ x... } {}
		constexpr vector_base(const vector<Base, N>& v) : x{ v.x[Indices]... } {}

		//単項演算
		template <class = enable_if_t<is_exist_additive_inverse_v<Base>>>
		constexpr vector<Base, N> operator-() const { return vector<Base, N>(-this->x[Indices]...); }
		constexpr vector<Base, N> operator+() const { return vector<Base, N>(*this); }

		//代入演算
		IMATH_VECTOR_ASSIGNMENT_OPERATION;

		//アクセサ
		const constexpr Base& operator[](size_t index) const { return this->x[index]; }
		constexpr Base& operator[](size_t index) { return this->x[index]; }

		//2項演算
		IMATH_VECTOR_BINARY_OPERATION1;
		IMATH_VECTOR_PARAMETER_BINARY_OPERATION1;
	};
	//下に階層が存在しないかつBase != T
	template <class Base, size_t N, class T, size_t... Indices>
	class vector_base<Base, N, T, index_tuple<size_t, Indices...>, false, false> {
		template <class, size_t> friend class vector;
		template <class, size_t, class, class, bool, bool> friend class vector_base;
	protected:
		Base x[N];
	public:
		constexpr vector_base() : x{} {}
		constexpr vector_base(const identity_t<Base, Indices>&... x) : x{ x... } {}
		template <class = enable_if_t<is_inclusion_v<T, Base>>>
		constexpr vector_base(const identity_t<T, Indices>&... x) : x{ static_cast<Base>(x)... } {}
		constexpr vector_base(const vector<Base, N>& v) : x{ v.x[Indices]... } {}
		template <class = enable_if_t<is_inclusion_v<T, Base>>>
		constexpr vector_base(const vector<T, N>& v) : x{ static_cast<Base>(v.x[Indices])... } {}

		//単項演算
		template <class = enable_if_t<is_exist_additive_inverse_v<Base>>>
		constexpr vector<Base, N> operator-() const { return vector<Base, N>(-this->x[Indices]...); }
		constexpr vector<Base, N> operator+() const { return vector<Base, N>(*this); }

		//代入演算
		IMATH_VECTOR_ASSIGNMENT_OPERATION;

		//アクセサ
		const constexpr Base& operator[](size_t index) const { return this->x[index]; }
		constexpr Base& operator[](size_t index) { return this->x[index]; }

		//2項演算
		IMATH_VECTOR_BINARY_OPERATION2;
		IMATH_VECTOR_PARAMETER_BINARY_OPERATION2;
	};
	//下に階層が存在するかつBase == T
	template <class Base, size_t N, class T, size_t... Indices>
	class vector_base<Base, N, T, index_tuple<size_t, Indices...>, true, true> : public vector_base<Base, N, typename T::algebraic_type> {
		template <class, size_t> friend class vector;
		template <class, size_t, class, class, bool, bool> friend class vector_base;
	public:
		using vector_base<Base, N, typename T::algebraic_type>::vector_base;
		//Base == TとなるためTypesによるコンストラクタは作成しない

		//単項演算
		using vector_base<Base, N, typename T::algebraic_type>::operator+;
		using vector_base<Base, N, typename T::algebraic_type>::operator-;

		//代入演算
		using vector_base<Base, N, typename T::algebraic_type>::operator=;
		using vector_base<Base, N, typename T::algebraic_type>::operator+=;
		using vector_base<Base, N, typename T::algebraic_type>::operator-=;
		using vector_base<Base, N, typename T::algebraic_type>::operator*=;
		using vector_base<Base, N, typename T::algebraic_type>::operator/=;
		IMATH_VECTOR_ASSIGNMENT_OPERATION;

		//添え字演算
		using vector_base<Base, N, typename T::algebraic_type>::operator[];

		//2項演算
		IMATH_VECTOR_BINARY_OPERATION1;
		IMATH_VECTOR_PARAMETER_BINARY_OPERATION1;
	};
	//下に階層が存在するかつBase != T
	template <class Base, size_t N, class T, size_t... Indices>
	class vector_base<Base, N, T, index_tuple<size_t, Indices...>, true, false> : public vector_base<Base, N, typename T::algebraic_type> {
		template <class, size_t> friend class vector;
		template <class, size_t, class, class, bool, bool> friend class vector_base;
	public:
		using vector_base<Base, N, typename T::algebraic_type>::vector_base;
		constexpr vector_base() : vector_base<Base, N, typename T::algebraic_type>() {}
		template <class = enable_if_t<is_inclusion_v<T, Base>>>
		constexpr vector_base(const identity_t<T, Indices>&... x) : vector_base<Base, N, typename T::algebraic_type>(static_cast<Base>(x)...) {}
		template <class = enable_if_t<is_inclusion_v<T, Base>>>
		constexpr vector_base(const vector<T, N>& v) : vector_base<Base, N, typename T::algebraic_type>(static_cast<Base>(v.x[Indices])...) {}

		//単項演算
		using vector_base<Base, N, typename T::algebraic_type>::operator+;
		using vector_base<Base, N, typename T::algebraic_type>::operator-;

		//代入演算
		using vector_base<Base, N, typename T::algebraic_type>::operator=;
		using vector_base<Base, N, typename T::algebraic_type>::operator+=;
		using vector_base<Base, N, typename T::algebraic_type>::operator-=;
		using vector_base<Base, N, typename T::algebraic_type>::operator*=;
		using vector_base<Base, N, typename T::algebraic_type>::operator/=;
		IMATH_VECTOR_ASSIGNMENT_OPERATION;

		//添え字演算の継承
		using vector_base<Base, N, typename T::algebraic_type>::operator[];

		//2項演算
		IMATH_VECTOR_BINARY_OPERATION2;
		IMATH_VECTOR_PARAMETER_BINARY_OPERATION2;
	};

	//ベクトル型
	template <class T, size_t N>
	class vector : public vector_base<T, N, T>, public value_list_input<array_iterator<T>> {
		//Nは0より大きくなければならない
		static_assert(N > 0, "N must be greater than 0.");

		template <class, size_t> friend class vector;
		template <class, size_t, class, class, bool, bool> friend class vector_base;
	public:
		using vector_base<T, N, T>::vector_base;

		using algebraic_type = T;
		using iterator = array_iterator<T>;
		using const_iterator = array_iterator<const T>;

		template <class Other>
		struct rebind {
			using other = vector<Other, N>;
		};

		constexpr iterator begin() noexcept { return iterator(&x[0]); }
		constexpr const_iterator begin() const noexcept { return const_iterator(&x[0]); }
		constexpr iterator end() noexcept { return iterator(&x[N - 1] + 1); }
		constexpr const_iterator end() const noexcept { return const_iterator(&x[N - 1] + 1); }

		//単項演算
		using vector_base<T, N, T>::operator-;
		using vector_base<T, N, T>::operator+;
		//代入演算
		using vector_base<T, N, T>::operator=;
		using vector_base<T, N, T>::operator+=;
		using vector_base<T, N, T>::operator-=;
		using vector_base<T, N, T>::operator*=;
		using vector_base<T, N, T>::operator/=;
		//2項演算
		using value_list_input<array_iterator<T>>::operator<<;

		//添え字演算
		using vector_base<T, N, T>::operator[];

		//ストリーム出力
		friend std::ostream& operator<<(std::ostream& os, const vector& v) {
			os << v.x[0];
			for (size_t i = 1; i < N; ++i) os << ',' << v.x[i];
			return os;
		}
		friend std::wostream& operator<<(std::wostream& os, const vector& v) {
			os << v.x[0];
			for (size_t i = 1; i < N; ++i) os << L',' << v.x[i];
			return os;
		}
	};
#undef IMATH_VECTOR_ASSIGNMENT_OPERATION
#undef IMATH_VECTOR_BINARY_OPERATION1
#undef IMATH_VECTOR_BINARY_OPERATION2
#undef IMATH_VECTOR_PARAMETER_BINARY_OPERATION1
#undef IMATH_VECTOR_PARAMETER_BINARY_OPERATION2


	//ベクトルの判定
	template <class T>
	struct is_vector_impl : false_type {};
	template <class T, size_t N>
	struct is_vector_impl<vector<T, N>> : true_type {};
	template <class T>
	struct is_vector : is_vector_impl<typename remove_cv<T>::type> {};
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


	//外積(T1×T2→Sが加法についてループ)
	template <class T1, class T2
		, class = typename enable_if<is_calcable<T1, T2>::mul_value && is_loop<typename calculation_result<T1, T2>::mul_type>::add_value>::type>
		inline vector<typename calculation_result<T1, T2>::mul_type, 3> operator%(const vector<T1, 3>& lhs, const vector<T2, 3>& rhs) {
		return vector<typename calculation_result<T1, T2>::mul_type, 3>(lhs[1] * rhs[2] - lhs[2] * rhs[1], lhs[2] * rhs[0] - lhs[0] * rhs[2], lhs[0] * rhs[1] - lhs[1] * rhs[0]);
	}


	//各種次元のベクトルの定義
	template <class T>
	using vector2 = vector<T, 2>;
	template <class T>
	using vector3 = vector<T, 3>;
	template <class T>
	using vector4 = vector<T, 4>;
}

namespace iml {

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
		static constexpr vector<T, N> _additive_inverse_impl_(const vector<T, N>& x, index_tuple<size_t, Indices...>) {
			return vector<T, N>(additive_inverse(x[Indices])...);
		}

		static constexpr vector<T, N> _additive_inverse_(const vector<T, N>& x) {
			return _additive_inverse_impl_(x, index_range_t<size_t, 0, N>());
		}
	};
}

namespace iml {

	//誤差評価
	template <class T, size_t N>
	struct Error_evaluation<vector<T, N>> {
		static constexpr vector<T, N> epsilon() {
			vector<T, N> temp{};
			for (size_t i = 0; i < N; ++i) temp[i] = Error_evaluation<T>::epsilon();
			return temp;
		}
		static constexpr bool _error_evaluation_(const vector<T, N>& x1, const vector<T, N>& x2) {
			for (size_t i = 0; i < N; ++i) if (!error_evaluation(x1[i], x2[i])) return false;
			return true;
		}
	};
}

#endif