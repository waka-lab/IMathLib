#ifndef IMATH_MATH_LINER_ALGEBRA_MATRIX_HPP
#define IMATH_MATH_LINER_ALGEBRA_MATRIX_HPP

#include "IMathLib/math/liner_algebra/vector.hpp"


namespace iml {

	//行列型の代入演算の定義
#define	IMATH_MATRIX_ASSIGNMENT_OPERATION \
	template <class = enable_if_t<is_inclusion_v<T, Base>>>\
	matrix_base& operator=(const matrix<T, M, N>& ma) {\
		/*同じインスタンスでなければ代入*/\
		if (this != addressof(ma)) for (size_t i = 0; i < M * N; ++i) this->x[0][i] = ma.x[0][i];\
		return *this;\
	}\
	template <class = enable_if_t<is_operation<Base, T, Base>::add_value>>\
	matrix_base& operator+=(const matrix<T, M, N>& ma) {\
		for (size_t i = 0; i < M * N; ++i) this->x[0][i] += ma.x[0][i];\
		return *this;\
	}\
	template <class = enable_if_t<is_operation<Base, T, Base>::sub_value>>\
	matrix_base& operator-=(const matrix<T, M, N>& ma) {\
		for (size_t i = 0; i < M * N; ++i) this->x[0][i] -= ma.x[0][i];\
		return *this;\
	}\
	template <class = enable_if_t<is_operation<Base, T, Base>::mul_value>>\
	matrix_base& operator*=(const T& k) {\
		for (size_t i = 0; i < M * N; ++i) this->x[0][i] *= k;\
		return *this;\
	}\
	/*行列の積(内積の一般化に当たる)*/\
	template <class = enable_if_t<is_operation<Base, T, Base>::mul_value && is_magma<Base>::add_value && (M == N)>>\
	matrix_base& operator*=(const matrix<T, M, N>& ma) {\
		matrix<Base, M, M> temp;\
		for (size_t i = 0; i < M; ++i)\
			for (size_t j = 0; j < M; ++j)\
				for (size_t k = 0; k < M; ++k)\
					temp[i][j] += this->x[i][k] * ma.x[k][j];\
		return *this = temp;\
	}\
	template <class = enable_if_t<is_operation<Base, T, Base>::div_value>>\
	matrix_base& operator/=(const T& k) {\
		for (size_t i = 0; i < M * N; ++i) this->x[0][i] /= k;\
		return *this;\
	}
	//Base == T の場合の行列型の2項演算の定義
#define IMATH_MATRIX_BINARY_OPERATION1 \
	template <class = enable_if_t<is_calcable<Base, T>::add_value>>\
	friend constexpr auto operator+(const matrix<Base, M, N>& lhs, const matrix<T, M, N>& rhs) {\
		return matrix<add_result_t<Base, T>, M, N>((lhs[0][Indices] + rhs[0][Indices])...);\
	}\
	template <class = enable_if_t<is_calcable<Base, T>::sub_value>>\
	friend constexpr auto operator-(const matrix<Base, M, N>& lhs, const matrix<T, M, N>& rhs) {\
		return matrix<sub_result_t<Base, T>, M, N>((lhs[0][Indices] - rhs[0][Indices])...);\
	}\
	template <class = enable_if_t<is_calcable<Base, T>::mul_value>>\
	friend constexpr auto operator*(const matrix<Base, M, N>& lhs, const T& rhs) {\
		return matrix<mul_result_t<Base, T>, M, N>((lhs[0][Indices] * rhs)...);\
	}\
	template <class = enable_if_t<is_calcable<T, Base>::mul_value>>\
	friend constexpr auto operator*(const T& lhs, const matrix<Base, M, N>& rhs) {\
		return matrix<mul_result_t<T, Base>, M, N>((lhs*rhs[0][Indices])...);\
	}\
	template <class = enable_if_t<is_calcable<Base, T>::div_value>>\
	friend constexpr auto operator/(const matrix<Base, M, N>& lhs, const T& rhs) {\
		return matrix<div_result_t<Base, T>, M, N>((lhs[0][Indices] / rhs)...);\
	}\
	/*行列の積(内積の一般化に当たる)*/\
	template <size_t L, class = enable_if_t<is_magma<mul_result_t<Base, T>>::mul_value>>\
	friend constexpr auto operator*(const matrix<Base, M, L>& lhs, const matrix<T, L, N>& rhs) {\
		matrix<mul_result_t<Base, T>, M, N> temp{};\
		for (size_t i = 0; i < M; ++i)\
			for (size_t j = 0; j < N; ++j)\
				for (size_t k = 0; k < L; ++k)\
					temp[i][j] += lhs[i][k] * rhs[k][j];\
		return temp;\
	}\
	/*比較演算*/\
	template <class = enable_if_t<is_calcable<Base, T>::eq_value>>\
	friend constexpr bool operator==(const matrix<Base, M, N>& lhs, const matrix<T, M, N>& rhs) {\
		for (size_t i = 0; i < M * N; ++i) if (lhs[0][i] != rhs[0][i]) return false;\
		return true;\
	}\
	template <class = enable_if_t<is_calcable<Base, T>::eq_value>>\
	friend constexpr bool operator!=(const matrix<Base, M, N>& lhs, const matrix<T, M, N>& rhs) { return !(lhs == rhs); }
	//Base != T の場合の順序を考慮した行列型の2項演算の定義
#define IMATH_MATRIX_BINARY_OPERATION2 \
	IMATH_MATRIX_BINARY_OPERATION1;\
	template <class = enable_if_t<is_calcable<T, Base>::add_value>>\
	friend constexpr auto operator+(const matrix<T, M, N>& lhs, const matrix<Base, M, N>& rhs) {\
		return matrix<add_result_t<T, Base>, M, N>((lhs[0][Indices] + rhs[0][Indices])...);\
	}\
	template <class = enable_if_t<is_calcable<T, Base>::sub_value>>\
	friend constexpr auto operator-(const matrix<T, M, N>& lhs, const matrix<Base, M, N>& rhs) {\
		return matrix<sub_result_t<T, Base>, M, N>((lhs[0][Indices] - rhs[0][Indices])...);\
	}\
	/*行列の積(内積の一般化に当たる)*/\
	template <size_t L, class = enable_if_t<is_magma<mul_result_t<T, Base>>::mul_value>>\
	friend constexpr auto operator*(const matrix<T, M, L>& lhs, const matrix<Base, L, N>& rhs) {\
		matrix<mul_result_t<T, Base>, M, N> temp{};\
		for (size_t i = 0; i < M; ++i)\
			for (size_t j = 0; j < N; ++j)\
				for (size_t k = 0; k < L; ++k)\
					temp[i][j] += lhs[i][k] * rhs[k][j];\
		return temp;\
	}\
	/*比較演算*/\
	template <class = enable_if_t<is_calcable<T, Base>::eq_value>>\
	friend constexpr bool operator==(const matrix<T, M, N>& lhs, const matrix<Base, M, N>& rhs) {\
		for (size_t i = 0; i < M * N; ++i) if (lhs[0][i] != rhs[0][i]) return false;\
		return true;\
	}\
	template <class = enable_if_t<is_calcable<T, Base>::eq_value>>\
	friend constexpr bool operator!=(const matrix<T, M, N>& lhs, const matrix<Base, M, N>& rhs) { return !(lhs == rhs); }
	//Base == T の場合の行列型のパラメータの2項演算の定義
#define IMATH_MATRIX_PARAMETER_BINARY_OPERATION1 \
	template <class... Types1, class... Types2, class = enable_if_t<is_calcable<Base, T>::add_value>>\
	friend auto operator+(matrix_parameter<Base, M, N, Types1...>, matrix_parameter<T, M, N, Types2...>) {\
		return matrix_parameter<add_result_t<Base, T>, M, N, decltype(Types1() + Types2())...>();\
	}\
	template <class... Types1, class... Types2, class = enable_if_t<is_calcable<Base, T>::sub_value>>\
	friend auto operator-(matrix_parameter<Base, M, N, Types1...>, matrix_parameter<T, M, N, Types2...>) {\
		return matrix_parameter<sub_result_t<Base, T>, M, N, decltype(Types1() - Types2())...>();\
	}\
	template <class... Types, class Param, class = enable_if_t<is_calcable<Base, T>::mul_value>>\
	friend auto operator*(matrix_parameter<Base, M, N, Types...>, type_parameter<T, Param> rhs) {\
		return matrix_parameter<mul_result_t<Base, T>, N, decltype(Types() * rhs)...>();\
	}\
	template <class Param, class... Types, class = enable_if_t<is_calcable<T, Base>::mul_value>>\
	friend auto operator*(type_parameter<T, Param> lhs, matrix_parameter<Base, M, N>) {\
		return matrix_parameter<mul_result_t<T, Base>, N, decltype(lhs * Types())...>();\
	}\
	template <class... Types, class Param, class = enable_if_t<is_calcable<Base, T>::div_value>>\
	friend auto operator/(matrix_parameter<Base, M, N, Types...>, type_parameter<T, Param> rhs) {\
		return matrix_parameter<mul_result_t<Base, T>, N, decltype(Types() / rhs)...>();\
	}\
	/*行列の積(内積の一般化に当たる)*/\
	template <class... Types1, class... Types2, size_t L, class = enable_if_t<is_magma<mul_result_t<Base, T>>::mul_value>>\
	friend auto operator*(matrix_parameter<Base, M, L, Types1...>, matrix_parameter<T, L, N, Types2...>) {\
		return typename tp::matrix_parameter_mul<matrix_parameter<Base, M, L, Types1...>, matrix_parameter<T, L, N, Types2...>>::type()\
	}\
	/*比較演算*/\
	template <class... Types1, class... Types2, class = enable_if_t<is_calcable<Base, T>::eq_value>>\
	friend auto operator==(matrix_parameter<Base, M, N, Types1...>, matrix_parameter<T, M, N, Types2...>) {\
		constexpr bool temp = tp::sum((Types1() == Types2())...)::value == M * N;\
		return int_parameter<bool, temp>();\
	}\
	template <class... Types1, class... Types2, class = enable_if_t<is_calcable<Base, T>::eq_value>>\
	friend auto operator!=(matrix_parameter<Base, M, N, Types1...> lhs, matrix_parameter<T, M, N, Types2...> rhs) { return !(lhs == rhs); }
	//Base != T の場合の順序を考慮した行列型のパラメータの2項演算の定義
#define IMATH_MATRIX_PARAMETER_BINARY_OPERATION2 \
	IMATH_MATRIX_PARAMETER_BINARY_OPERATION1;\
	template <class... Types1, class... Types2, class = enable_if_t<is_calcable<T, Base>::add_value>>\
	friend auto operator+(matrix_parameter<T, M, N, Types1...>, matrix_parameter<Base, M, N, Types2...>) {\
		return matrix_parameter<add_result_t<Base, T>, M, N, decltype(Types1() + Types2())...>();\
	}\
	template <class... Types1, class... Types2, class = enable_if_t<is_calcable<T, Base>::sub_value>>\
	friend auto operator-(matrix_parameter<T, M, N, Types1...>, matrix_parameter<Base, M, N, Types2...>) {\
		return matrix_parameter<add_result_t<Base, T>, M, N, decltype(Types1() - Types2())...>();\
	}\
	/*行列の積(内積の一般化に当たる)*/\
	template <class... Types1, class... Types2, size_t L, class = enable_if_t<is_magma<mul_result_t<T, Base>>::mul_value>>\
	friend auto operator*(matrix_parameter<T, M, L, Types1...>, matrix_parameter<Base, L, N, Types2...>) {\
		return typename tp::matrix_parameter_mul<matrix_parameter<T, M, L, Types1...>, matrix_parameter<Base, L, N, Types2...>>::type()\
	}\
	/*比較演算*/\
	template <class... Types1, class... Types2, class = enable_if_t<is_calcable<T, Base>::eq_value>>\
	friend auto operator==(matrix_parameter<T, M, N, Types1...>, matrix_parameter<Base, M, N, Types2...>) {\
		constexpr bool temp = tp::sum((Types1() == Types2())...)::value == M * N;\
		return int_parameter<bool, temp>();\
	}\
	template <class... Types1, class... Types2, class = enable_if_t<is_calcable<T, Base>::eq_value>>\
	friend auto operator!=(matrix_parameter<T, M, N, Types1...> lhs, matrix_parameter<Base, M, N, Types2...> rhs) { return !(lhs == rhs); }


	template <class, size_t, size_t>
	class matrix;


	//行列型のパラメータ
	template <class T, size_t M, size_t N, class... Types>
	using matrix_parameter = type_parameter<matrix<T, M, N>, type_tuple<Types...>>;
	template <class T, size_t M, size_t N, class... Params>
	struct type_parameter<matrix<T, M, N>, type_tuple<type_parameter<T, Params>...>> {
		static_assert(M * N == type_tuple<type_parameter<T, Params>...>::value, "There must be M*N parameters.");

		using type = matrix<T, M, N>;
		static constexpr type value = type(type_parameter<T, Params>::value...);

		template <class = enable_if_t<is_exist_additive_inverse_v<T>>>
		auto operator-() const { return matrix_parameter<T, M, N, decltype(-type_parameter<T, Params>())...>(); }
		auto operator+() const { return *this; }

		template <class T, T Val, class = enable_if_t<(Val >= 0) && (Val < M)>>
		auto operator[](int_parameter<T, Val>) const {
			using array_type = array_parameter<T, index_tuple<size_t, M, N>, type_parameter<T, Params>...>;
			return array_type()[int_parameter<T, Val>];
		}
	};
	namespace tp {

		//行列型のパラメータの乗算の計算
		template <size_t, class, class, class>
		struct matrix_parameter_mul_impl2;
		template <size_t RC, class T1, class T2, size_t M, size_t L, size_t N, class... Types1, class... Types2, size_t... Indices>
		struct matrix_parameter_mul_impl2<RC, matrix_parameter<T1, M, L, Types1...>, matrix_parameter<T2, L, N, Types2...>, index_tuple<size_t, Indices...>> {
			//メモリの配置が temp[0][0], temp[0][1], ・・・, temp[1][0], temp[1][1], ・・・となることから算出
			static constexpr size_t row = RC % M;		//行
			static constexpr size_t col = RC / M;		//列
			//内積をとる
			using type = decltype(tp::sum((matrix_parameter<T1, M, L, Types1...>()[int_parameter<size_t, row>()][int_parameter<size_t, Indices>()]
				* matrix_parameter<T2, L, N, Types2...>()[int_parameter<size_t, Indices>()][int_parameter<size_t, col>()])...));
		};
		template <class, class, class>
		struct matrix_parameter_mul_impl;
		template <class T1, class T2, size_t M, size_t L, size_t N, class... Types1, class... Types2, size_t... Indices>
		struct matrix_parameter_mul_impl<matrix_parameter<T1, M, L, Types1...>, matrix_parameter<T2, L, N, Types2...>, index_tuple<size_t, Indices...>> {
			//各成分づつ計算
			using type = matrix_parameter<mul_result_t<T1, T2>, M, N
				, typename matrix_parameter_mul_impl2<Indices, matrix_parameter<T1, M, L, Types1...>, matrix_parameter<T2, L, N, Types2...>, index_range_t<size_t, 0, L>>::type...>;
		};
		template <class, class>
		struct matrix_parameter_mul;
		template <class T1, class T2, size_t M, size_t L, size_t N, class... Types1, class... Types2>
		struct matrix_parameter_mul<matrix_parameter<T1, M, L, Types1...>, matrix_parameter<T2, L, N, Types2...>>
			//M*N行列の全要素を走査するためのインデックスの生成
			: matrix_parameter_mul_impl<matrix_parameter<T1, M, L, Types1...>, matrix_parameter<T2, L, N, Types2...>, index_range_t<size_t, 0, M * N>> {};
	}


	template <class Base, size_t M, size_t N, class T, class = index_range_t<size_t, 0, (M * N)>, bool = is_algebraic_structure_v<T>, bool = is_same_v<Base, T>>
	class matrix_base;


	//下に階層が存在しないかつBase == T
	template <class Base, size_t M, size_t N, class T, size_t... Indices>
	class matrix_base<Base, M, N, T, index_tuple<size_t, Indices...>, false, true> {
		template <class, size_t, size_t> friend class matrix;
		template <class, size_t, size_t, class, class, bool, bool> friend class matrix_base;
	protected:
		Base x[M][N];
	public:
		//Base == TとなるためTypesによるコンストラクタは作成しない
		constexpr matrix_base() : x{} {}
		constexpr matrix_base(const identity_t<Base, Indices>&... x) : x{ x... } {}
		constexpr matrix_base(const matrix<Base, M, N>& ma) : x{ ma.x[0][Indices]... } {}


		template <class = enable_if_t<is_exist_additive_inverse_v<Base>>>
		constexpr matrix<Base, M, N> operator-() const { return matrix<Base, M, N>(-this->x[0][Indices]...); }
		constexpr matrix<Base, M, N> operator+() const { return matrix<Base, M, N>(*this); }

		//代入演算
		IMATH_MATRIX_ASSIGNMENT_OPERATION;

		//アクセサ
		const constexpr auto& operator[](size_t index) const { return this->x[index]; }
		constexpr auto& operator[](size_t index) { return this->x[index]; }


		//2項演算
		IMATH_MATRIX_BINARY_OPERATION1;
		IMATH_MATRIX_PARAMETER_BINARY_OPERATION1;
	};
	//下に階層が存在しないかつBase != T
	template <class Base, size_t M, size_t N, class T, size_t... Indices>
	class matrix_base<Base, M, N, T, index_tuple<size_t, Indices...>, false, false> {
		template <class, size_t, size_t> friend class matrix;
		template <class, size_t, size_t, class, class, bool, bool> friend class matrix_base;
	protected:
		Base x[M][N];
	public:
		constexpr matrix_base() : x{} {}
		constexpr matrix_base(const identity_t<Base, Indices>&... x) : x{ x... } {}
		template <class = enable_if_t<is_inclusion_v<T, Base>>>
		constexpr matrix_base(const identity_t<T, Indices>&... x) : x{ static_cast<Base>(x)... } {}
		constexpr matrix_base(const matrix<Base, M, N>& ma) : x{ ma.x[0][Indices]... } {}
		template <class = enable_if_t<is_inclusion_v<T, Base>>>
		constexpr matrix_base(const matrix<T, M, N>& ma) : x{ static_cast<Base>(ma.x[0][Indices])... } {}

		//単項演算
		template <class = enable_if_t<is_exist_additive_inverse_v<Base>>>
		constexpr matrix<Base, M, N> operator-() const { return matrix<Base, M, N>(-this->x[0][Indices]...); }
		constexpr matrix<Base, M, N> operator+() const { return matrix<Base, M, N>(*this); }

		//代入演算
		IMATH_MATRIX_ASSIGNMENT_OPERATION;

		//アクセサ
		const constexpr auto& operator[](size_t index) const { return this->x[index]; }
		constexpr auto& operator[](size_t index) { return this->x[index]; }

		//2項演算
		IMATH_MATRIX_BINARY_OPERATION2;
		IMATH_MATRIX_PARAMETER_BINARY_OPERATION2;
	};
	//下に階層が存在するかつBase == T
	template <class Base, size_t M, size_t N, class T, size_t... Indices>
	class matrix_base<Base, M, N, T, index_tuple<size_t, Indices...>, true, true> : public matrix_base<Base, M, N, typename T::algebraic_type> {
		template <class, size_t, size_t> friend class matrix;
		template <class, size_t, size_t, class, class, bool, bool> friend class matrix_base;
	public:
		//コンストラクタ
		using matrix_base<Base, M, N, typename T::algebraic_type>::matrix_base;

		//Base == TとなるためTypesによるコンストラクタは作成しない

		//単項演算
		using matrix_base<Base, M, N, typename T::algebraic_type>::operator+;
		using matrix_base<Base, M, N, typename T::algebraic_type>::operator-;

		//代入演算の継承
		using matrix_base<Base, M, N, typename T::algebraic_type>::operator=;
		using matrix_base<Base, M, N, typename T::algebraic_type>::operator+=;
		using matrix_base<Base, M, N, typename T::algebraic_type>::operator-=;
		using matrix_base<Base, M, N, typename T::algebraic_type>::operator*=;
		using matrix_base<Base, M, N, typename T::algebraic_type>::operator/=;
		IMATH_MATRIX_ASSIGNMENT_OPERATION;

		//添え字演算
		using matrix_base<Base, M, N, typename T::algebraic_type>::operator[];

		//2項演算
		IMATH_MATRIX_BINARY_OPERATION1;
		IMATH_MATRIX_PARAMETER_BINARY_OPERATION1;
	};
	//下に階層が存在するかつBase != T
	template <class Base, size_t M, size_t N, class T, size_t... Indices>
	class matrix_base<Base, M, N, T, index_tuple<size_t, Indices...>, true, false> : public matrix_base<Base, M, N, typename T::algebraic_type> {
		template <class, size_t, size_t> friend class matrix;
		template <class, size_t, size_t, class, class, bool, bool> friend class matrix_base;
	public:
		//コンストラクタ
		using matrix_base<Base, M, N, typename T::algebraic_type>::matrix_base;
		template <class = enable_if_t<is_inclusion_v<T, Base>>::type>
		constexpr matrix_base(const identity_t<T, Indices>&... x) : matrix_base<Base, M, N, typename T::algebraic_type>(static_cast<Base>(x)...) {}
		template <class = enable_if_t<is_inclusion_v<T, Base>>>
		constexpr matrix_base(const matrix<T, M, N>& ma) : x{ static_cast<Base>(ma.x[0][Indices])... } {}

		//単項演算
		using matrix_base<Base, M, N, typename T::algebraic_type>::operator+;
		using matrix_base<Base, M, N, typename T::algebraic_type>::operator-;

		//代入演算
		using matrix_base<Base, M, N, typename T::algebraic_type>::operator=;
		using matrix_base<Base, M, N, typename T::algebraic_type>::operator+=;
		using matrix_base<Base, M, N, typename T::algebraic_type>::operator-=;
		using matrix_base<Base, M, N, typename T::algebraic_type>::operator*=;
		using matrix_base<Base, M, N, typename T::algebraic_type>::operator/=;
		IMATH_MATRIX_ASSIGNMENT_OPERATION;

		//添え字演算
		using matrix_base<Base, M, N, typename T::algebraic_type>::operator[];

		//2項演算
		IMATH_MATRIX_BINARY_OPERATION2;
		IMATH_MATRIX_PARAMETER_BINARY_OPERATION2;
	};

	//行列型
	template <class T, size_t M, size_t N>
	class matrix : public matrix_base<T, M, N, T>, public value_list_input<array_iterator<T>> {
		//MとNは0より大きくなければならない
		static_assert(M > 0 && N > 0, "M and N must be greater than 0.");

		template <class, size_t, size_t> friend class matrix;
		template <class, size_t, size_t, class, class, bool, bool> friend class matrix_base;
	public:
		using matrix_base<T, M, N, T>::matrix_base;

		using algebraic_type = T;
		using iterator = array_iterator<T>;
		using const_iterator = array_iterator<const T>;

		template<class Other>
		struct rebind {
			using other = matrix<Other, M, N>;
		};

		constexpr iterator begin() noexcept { return iterator(&x[0][0]); }
		constexpr const_iterator begin() const noexcept { return const_iterator(&x[0][0]); }
		constexpr iterator end() noexcept { return iterator(&x[0][M * N - 1] + 1); }
		constexpr const_iterator end() const noexcept { return const_iterator(&x[0][M * N - 1] + 1); }

		//単項演算
		using matrix_base<T, M, N, T>::operator-;
		using matrix_base<T, M, N, T>::operator+;
		//代入演算
		using matrix_base<T, M, N, T>::operator=;
		using matrix_base<T, M, N, T>::operator+=;
		using matrix_base<T, M, N, T>::operator-=;
		using matrix_base<T, M, N, T>::operator*=;
		using matrix_base<T, M, N, T>::operator/=;
		//2項演算
		using value_list_input<array_iterator<T>>::operator<<;

		//添え字演算
		using matrix_base<T, M, N, T>::operator[];

		//ストリーム出力
		friend std::ostream& operator<<(std::ostream& os, const matrix& ma) {
			//-1は改行対策
			for (size_t i = 0; i < M - 1; ++i) {
				for (size_t j = 0; j < N; ++j)
					os << ma.x[i][j] << ' ';
				os << std::endl;
			}
			for (size_t j = 0; j < N; ++j)
				os << ma.x[M - 1][j] << ' ';
			return os;
		}
		friend std::wostream& operator<<(std::wostream& os, const matrix& ma) {
			//-1は改行対策
			for (size_t i = 0; i < M - 1; ++i) {
				for (size_t j = 0; j < N; ++j)
					os << ma.x[i][j] << L' ';
				os << std::endl;
			}
			for (size_t j = 0; j < N; ++j)
				os << ma.x[M - 1][j] << L' ';
			return os;
		}
	};
#undef IMATH_MATRIX_ASSIGNMENT_OPERATION
#undef IMATH_MATRIX_BINARY_OPERATION1
#undef IMATH_MATRIX_BINARY_OPERATION2
#undef IMATH_MATRIX_PARAMETER_BINARY_OPERATION1
#undef IMATH_MATRIX_PARAMETER_BINARY_OPERATION2


	//線形写像
	template <class T1, class T2, size_t M, size_t N>
	inline constexpr auto operator*(const matrix<T1, M, N>& lhs, const vector<T2, N>& rhs) {
		vector<mul_result_t<T1, T2>, M> temp{};
		for (size_t i = 0; i < M; ++i) for (size_t j = 0; j < N; ++j) temp[i] += lhs[i][j] * rhs[j];
		return temp;
	}
	namespace tp {
		//内積の補助(Indices1 : ベクトルの各要素に対応するシーケンス, Indices2 : 内積をとるためのシーケンス)
		template <class, class, class, class>
		struct matrix_vector_parameter_mul_impl;
		template <class T1, class... Types1, class T2, class... Types2, size_t M, size_t N, size_t... Indices1, size_t... Indices2>
		struct matrix_vector_parameter_mul_impl<matrix_parameter<T1, M, N, Types1...>, vector_parameter<T2, N, Types2...>, index_tuple<size_t, Indices1...>, index_tuple<size_t, Indices2...>> {
			using type = vector_parameter<mul_result_t<T1, T2>, M
				, decltype(tp::sum(matrix_parameter<T1, M, N, Types1...>()[int_parameter<size_t, Indices1>()][int_parameter<size_t, Indices2>()]
					* vector_parameter<T2, N, Types2...>()[int_parameter<size_t, Indices2>()]...))...>;
		};
	}
	template <class T1, class... Types1, class T2, class... Types2, size_t M, size_t N>
	inline auto operator*(matrix_parameter<T1, M, N, Types1...> lhs, vector_parameter<T2, N, Types2...> rhs) {
		return typename tp::matrix_vector_parameter_mul_impl<matrix_parameter<T1, M, N, Types1...>, vector_parameter<T2, N, Types2...>
			, index_range_t<size_t, 0, M>, index_range_t<size_t, 0, N>>::type();
	}


	//行列の判定
	template <class T>
	struct is_matrix_impl : false_type {};
	template <class T, size_t M, size_t N>
	struct is_matrix_impl<matrix<T, M, N>> : true_type {};
	template <class T>
	struct is_matrix : is_matrix_impl<decay_t<T>> {};
	template <class T>
	using is_matrix_t = typename is_matrix<T>::type;


	//行列の除去
	template <class T>
	struct remove_matrix {
		using type = T;
	};
	template <class T, size_t M, size_t N>
	struct remove_matrix<matrix<T, M, N>> {
		using type = T;
	};
	template <class T>
	using remove_matrix_t = typename remove_matrix<T>::type;


	//各種行列の定義
	template <class T>
	using matrix2 = matrix<T, 2, 2>;
	template <class T>
	using matrix3 = matrix<T, 3, 3>;
	template <class T>
	using matrix4 = matrix<T, 4, 4>;
}


namespace iml {

	//加法の特性
	template <class T, size_t M, size_t N>
	struct addition_traits<matrix<T, M, N>> {
		//単位元(零行列)
		template <class = enable_if_t<is_exist_additive_identity_v<T>>>
		static constexpr matrix<T, M, N> identity_element() { return matrix<T, M, N>(); }
		//結合律
		static constexpr bool associative_value = addition_traits<T>::associative_value;
		//消約律
		static constexpr bool cancellative_value = addition_traits<T>::cancellative_value;
		//可換律
		static constexpr bool commutative_value = addition_traits<T>::commutative_value;
	};
	//乗法の特性
	template <class T, size_t M, size_t N>
	struct multiplication_traits<matrix<T, M, N>> {
		//単位元(単位行列)
		template <class = enable_if_t<is_exist_multiplicative_identity_v<T> && (M == N)>>
		static constexpr matrix<T, M, M> identity_element() {
			matrix<T, M, M> temp{};
			for (size_t i = 0; i < M; ++i) temp[i][i] = multiplication_traits<T>::identity_element();
			return temp;
		}
		//吸収元(零行列)
		template <class = enable_if_t<is_exist_absorbing_element_v<T> && (M == N)>>
		static constexpr matrix<T, M, M> absorbing_element() { return matrix<T, M, M>(); }
		//結合律
		static constexpr bool associative_value = multiplication_traits<T>::associative_value;
		//消約律
		static constexpr bool cancellative_value = false;
		//可換律
		static constexpr bool commutative_value = false;
		//分配律
		static constexpr bool distributive_value = multiplication_traits<T>::distributive_value;
	};


	//逆元が存在するならば逆元の取得(存在しない場合は例外を出す)
	template <class T, size_t M, size_t N>
	struct Inverse_element<matrix<T, M, N>> {
		template <size_t... Indices>
		static constexpr matrix<T, M, N> _additive_inverse_impl_(const matrix<T, M, N>& x, index_tuple<size_t, Indices...>) {
			return matrix<T, M, N>(additive_inverse(x[Indices])...);
		}
		static constexpr matrix<T, M, N> _additive_inverse_(const matrix<T, M, N>& x) {
			return _additive_inverse_impl_(x, index_range_t<size_t, 0, M * N>());
		}

		//Tに乗法逆元が存在する場合の乗法逆元
		static constexpr matrix<T, M, M> _multiplicative_inverse_impl_(matrix<T, M, M> x, true_type) {
			matrix<T, M, M> result(multiplication_traits<matrix<T, M, M>>::identity_element());

			//xを単位行列になるように操作する(ガウスの消去法)
			for (size_t i = 0; i < M; ++i) {
				//対角成分が0であれば現在走査中の行より下段の0でない行と交換する
				if (x[i][i] == 0) {
					size_t j = i + 1;
					for (; j < M; ++j) {
						//0でない項が存在するとき行を交換
						if (x[j][i] != 0) {
							//for (size_t k = i; k < M; ++k) swap(x[i][k], x[j][k]);
							//for (size_t k = 0; k < M; ++k) swap(result[i][k], result[j][k]);
							size_t k = 0;
							for (; k < i; ++k) swap(result[i][k], result[j][k]);
							for (; k < M; ++k) { swap(x[i][k], x[j][k]); swap(result[i][k], result[j][k]); }
							break;
						}
					}
					//交換できなかった時は正則ではない
					if (j == M) return matrix<T, M, M>();
				}
				//i行を対角成で正規化(無駄を省くために対角成分は正規化しない)
				//for (size_t j = i + 1; j < M; ++j) x[i][j] /= x[i][i];
				//for (size_t j = 0; j < M; ++j) result[i][j] /= x[i][i];
				size_t l = 0;
				for (; l < i + 1; ++l) result[i][l] /= x[i][i];
				for (; l < M; ++l) { x[i][l] /= x[i][i]; result[i][l] /= x[i][i]; }
				//第i列の対角成分より上を0にする
				for (size_t j = 0; j < i; ++j) {
					if (x[j][i] != 0) {
						//i列目から順に引いていく(i列より左は全て0であるため計算しない)
						//実際、i列の成分は以後0となるため計算する必要がない
						//for (size_t k = i + 1; k < M; ++k) x[j][k] -= x[i][k] * x[j][i];
						//for (size_t k = 0; k < M; ++k) result[j][k] -= result[i][k] * x[j][i];
						size_t k = 0;
						for (; k < i + 1; ++k) result[j][k] -= result[i][k] * x[j][i];
						for (; k < M; ++k) { x[j][k] -= x[i][k] * x[j][i]; result[j][k] -= result[i][k] * x[j][i]; }
					}
				}
				//第i列の対角成分より下を0にする
				for (size_t j = i + 1; j < M; ++j) {
					if (x[j][i] != 0) {
						//i列目から順に引いていく(i列より左は全て0であるため計算しない)
						//実際、i列の成分は以後0となるため計算する必要がない
						//for (size_t k = i + 1; k < M; ++k) x[j][k] -= x[i][k] * x[j][i];
						//for (size_t k = 0; k < M; ++k) result[j][k] -= result[i][k] * x[j][i];
						size_t k = 0;
						for (; k < i + 1; ++k) result[j][k] -= result[i][k] * x[j][i];
						for (; k < M; ++k) { x[j][k] -= x[i][k] * x[j][i]; result[j][k] -= result[i][k] * x[j][i]; }
					}
				}
			}
			return result;
		}
		//Tの乗法逆元が存在しない場合の乗法逆元
		static constexpr matrix<T, M, M> _multiplicative_inverse_impl_(matrix<T, M, M> x, false_type) {
			matrix<T, M, M> result(multiplication_traits<matrix<T, M, M>>::identity_element());

			//xを単位行列になるように操作する(ガウスの消去法)
			for (size_t i = 0; i < M; ++i) {
				//対角成分が0であれば現在走査中の行より下段の0でない行と交換する
				if (x[i][i] == 0) {
					size_t j = i + 1;
					for (; j < M; ++j) {
						//0でない項が存在するとき行を交換
						if (x[j][i] != 0) {
							//for (size_t k = i; k < M; ++k) swap(x[i][k], x[j][k]);
							//for (size_t k = 0; k < M; ++k) swap(result[i][k], result[j][k]);
							size_t k = 0;
							for (; k < i; ++k) swap(result[i][k], result[j][k]);
							for (; k < M; ++k) { swap(x[i][k], x[j][k]); swap(result[i][k], result[j][k]); }
							break;
						}
					}
					//交換できなかった時は正則ではない
					if (j == M) return matrix<T, M, M>();
				}
				//i行を対角成で正規化(無駄を省くために対角成分は正規化しない)
				T temp = multiplicative_inverse(x[i][i]);
				//for (size_t j = i + 1; j < M; ++j) x[i][j] *= temp;
				//for (size_t j = 0; j < M; ++j) result[i][j] *= temp;
				size_t l = 0;
				for (; l < i + 1; ++l) result[i][l] *= temp;
				for (; l < M; ++l) { x[i][l] *= temp; result[i][l] *= temp; }
				//第i列の対角成分より上を0にする
				for (size_t j = 0; j < i; ++j) {
					if (x[j][i] != 0) {
						//i列目から順に引いていく(i列より左は全て0であるため計算しない)
						//実際、i列の成分は以後0となるため計算する必要がない
						//for (size_t k = i + 1; k < M; ++k) x[j][k] -= x[i][k] * x[j][i];
						//for (size_t k = 0; k < M; ++k) result[j][k] -= result[i][k] * x[j][i];
						size_t k = 0;
						for (; k < i + 1; ++k) result[j][k] -= result[i][k] * x[j][i];
						for (; k < M; ++k) { x[j][k] -= x[i][k] * x[j][i]; result[j][k] -= result[i][k] * x[j][i]; }
					}
				}
				//第i列の対角成分より下を0にする
				for (size_t j = i + 1; j < M; ++j) {
					if (x[j][i] != 0) {
						//i列目から順に引いていく(i列より左は全て0であるため計算しない)
						//実際、i列の成分は以後0となるため計算する必要がない
						//for (size_t k = i + 1; k < M; ++k) x[j][k] -= x[i][k] * x[j][i];
						//for (size_t k = 0; k < M; ++k) result[j][k] -= result[i][k] * x[j][i];
						size_t k = 0;
						for (; k < i + 1; ++k) result[j][k] -= result[i][k] * x[j][i];
						for (; k < M; ++k) { x[j][k] -= x[i][k] * x[j][i]; result[j][k] -= result[i][k] * x[j][i]; }
					}
				}
			}
			return result;
		}
		template <class = enable_if_t<M == N>>
		static constexpr matrix<T, M, M> _multiplicative_inverse_(const matrix<T, M, M>& x) {
			return _multiplicative_inverse_impl_(x, bool_constant<is_exist_multiplicative_inverse_v<T>>());
		}
	};
}

namespace iml {

	//誤差評価
	template <class T, size_t M, size_t N>
	struct Error_evaluation<matrix<T, M, N>> {
		static constexpr matrix<T, M, N> epsilon() {
			matrix<T, M, N> temp{};
			for (size_t i = 0; i < M * N; ++i) temp[0][i] = Error_evaluation<T>::epsilon();
			return temp;
		}
		static constexpr bool _error_evaluation_(const matrix<T, M, N>& x1, const matrix<T, M, N>& x2) {
			for (size_t i = 0; i < M * N; ++i) if (!error_evaluation(x1[0][i], x2[0][i])) return false;
			return true;
		}
	};
}

#endif