#ifndef IMATH_MATH_LINER_ALGEBRA_MATRIX_HPP
#define IMATH_MATH_LINER_ALGEBRA_MATRIX_HPP

#include "IMathLib/utility/type_traits.hpp"
#include "IMathLib/utility/utility/tuple_type.hpp"
#include "IMathLib/math/math.hpp"
#include "IMathLib/container/array.hpp"


namespace iml {

	template <class, imsize_t, imsize_t>
	class matrix;


	template <class Base, imsize_t M, imsize_t N, class T, class = typename index_imu_range<0, dimension<index_imu_tuple<M, N>>::value>::type, bool = is_algebraic_structure<T>::value, bool = is_same<Base, T>::value>
	class matrix_base;


	//下に階層が存在しないかつBase == T
	template <class Base, imsize_t M, imsize_t N, class T, imsize_t... Indices>
	class matrix_base<Base, M, N, T, index_imu_tuple<Indices...>, false, true> {
		template <class, imsize_t, imsize_t> friend class matrix;
		template <class, imsize_t, imsize_t, class, class, bool, bool> friend class matrix_base;
	protected:
		typename multi_array<Base, M, N>::type x;
	public:
		//Base == TとなるためTypesによるコンストラクタは作成しない
		constexpr matrix_base() : x{} {}
		constexpr matrix_base(const typename identity_type<Base, Indices>::type&... x) : x{ x... } {}
		template <class U>
		constexpr matrix_base(const matrix_base<Base, M, N, U>& ma) : x{ ma.x[0][Indices]... } {}


		template <class = typename enable_if<is_exist_add_inverse_element<Base>::value>::type>
		matrix<Base, M, N> operator-() const { return matrix<Base, M, N>(-this->x[0][Indices]...); }
		matrix<Base, M, N> operator+() const { return matrix<Base, M, N>(*this); }

		template <class U, class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		matrix_base& operator+=(const matrix_base<T, M, N, U>& ma) {
			for (imsize_t i = 0; i < dimension<index_imu_tuple<M, N>>::value; ++i) this->x[0][i] += ma.x[0][i];
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		matrix_base& operator-=(const matrix_base<T, M, N, U>& ma) {
			for (imsize_t i = 0; i < dimension<index_imu_tuple<M, N>>::value; ++i) this->x[0][i] -= ma.x[0][i];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		matrix_base& operator*=(const T& k) {
			for (imsize_t i = 0; i < dimension<index_imu_tuple<M, N>>::value; ++i) this->x[0][i] *= k;
			return *this;
		}
		//行列の積
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::mul_value && (M == N)>::type>
		matrix_base& operator*=(const matrix_base<T, M, N, U>& ma) {
			matrix<Base, M, M> temp;
			for (imsize_t i = 0; i < M; ++i)
				for (imsize_t j = 0; j < M; ++j)
					for (imsize_t k = 0; k < M; ++k)
						temp[i][j] += this->x[i][k] * ma.x[k][j];
			return *this = temp;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		matrix_base& operator/=(const T& k) {
			for (imsize_t i = 0; i < dimension<index_imu_tuple<M, N>>::value; ++i) this->x[0][i] /= k;
			return *this;
		}

		//アクセサ
		const constexpr auto& operator[](imsize_t index) const { return this->x[index]; }
		constexpr auto& operator[](imsize_t index) { return this->x[index]; }


		//二項演算
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend matrix<Base, M, N> operator+(const matrix<Base, M, N>& lhs, const matrix<T, M, N>& rhs) {
			return matrix<Base, M, N>((lhs[0][Indices] + rhs[0][Indices])...);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend matrix<Base, M, N> operator-(const matrix<Base, M, N>& lhs, const matrix<T, M, N>& rhs) {
			return matrix<Base, M, N>((lhs[0][Indices] - rhs[0][Indices])...);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend matrix<Base, M, N> operator*(const matrix<Base, M, N>& lhs, const T& rhs) {
			return matrix<Base, M, N>((lhs[0][Indices] * rhs)...);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::mul_value>::type>
		friend matrix<Base, M, N> operator*(const T& lhs, const matrix<Base, M, N>& rhs) {
			return matrix<Base, M, N>((lhs*rhs[0][Indices])...);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		friend matrix<Base, M, N> operator/(const matrix<Base, M, N>& lhs, const T& rhs) {
			return matrix<Base, M, N>((lhs[0][Indices] / rhs)...);
		}
	};
	//下に階層が存在しないかつBase != T
	template <class Base, imsize_t M, imsize_t N, class T, imsize_t... Indices>
	class matrix_base<Base, M, N, T, index_imu_tuple<Indices...>, false, false> {
		template <class, imsize_t, imsize_t> friend class matrix;
		template <class, imsize_t, imsize_t, class, class, bool, bool> friend class matrix_base;
	protected:
		typename multi_array<Base, M, N>::type x;
	public:
		constexpr matrix_base() : x{} {}
		constexpr matrix_base(const typename identity_type<Base, Indices>::type&... x) : x{ x... } {}
		template <class = typename enable_if<is_inclusion<T, Base>::value>::type>
		constexpr matrix_base(const typename identity_type<T, Indices>::type&... x) : x{ static_cast<Base>(x)... } {}
		template <class U>
		constexpr matrix_base(const matrix_base<Base, M, N, U>& ma) : x{ ma.x[0][Indices]... } {}
		template <class U, class = typename enable_if<is_inclusion<T, Base>::value>::type>
		constexpr matrix_base(const matrix_base<T, M, N, U>& ma) : x{ static_cast<Base>(ma.x[0][Indices])... } {}


		template <class = typename enable_if<is_exist_add_inverse_element<Base>::value>::type>
		matrix<Base, M, N> operator-() const { return matrix<Base, M, N>(-this->x[0][Indices]...); }
		matrix<Base, M, N> operator+() const { return matrix<Base, M, N>(*this); }

		template <class U, class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		matrix_base& operator+=(const matrix_base<T, M, N, U>& ma) {
			for (imsize_t i = 0; i < dimension<index_imu_tuple<M, N>>::value; ++i) this->x[0][i] += ma.x[0][i];
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		matrix_base& operator-=(const matrix_base<T, M, N, U>& ma) {
			for (imsize_t i = 0; i < dimension<index_imu_tuple<M, N>>::value; ++i) this->x[0][i] -= ma.x[0][i];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		matrix_base& operator*=(const T& k) {
			for (imsize_t i = 0; i < dimension<index_imu_tuple<M, N>>::value; ++i) this->x[0][i] *= k;
			return *this;
		}
		//行列の積
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::mul_value && (M == N)>::type>
		matrix_base& operator*=(const matrix_base<T, M, N, U>& ma) {
			matrix<Base, M, M> temp;
			for (imsize_t i = 0; i < M; ++i)
				for (imsize_t j = 0; j < M; ++j)
					for (imsize_t k = 0; k < M; ++k)
						temp[i][j] += this->x[i][k] * ma.x[k][j];
			return *this = temp;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		matrix_base& operator/=(const T& k) {
			for (imsize_t i = 0; i < dimension<index_imu_tuple<M, N>>::value; ++i) this->x[0][i] /= k;
			return *this;
		}

		//アクセサ
		const constexpr auto& operator[](imsize_t index) const { return this->x[index]; }
		constexpr auto& operator[](imsize_t index) { return this->x[index]; }


		//二項演算
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend matrix<Base, M, N> operator+(const matrix<Base, M, N>& lhs, const matrix<T, M, N>& rhs) {
			return matrix<Base, M, N>((lhs[0][Indices] + rhs[0][Indices])...);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::add_value>::type>
		friend matrix<Base, M, N> operator+(const matrix<T, M, N>& lhs, const matrix<Base, M, N>& rhs) {
			return matrix<Base, M, N>((lhs[0][Indices] + rhs[0][Indices])...);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend matrix<Base, M, N> operator-(const matrix<Base, M, N>& lhs, const matrix<T, M, N>& rhs) {
			return matrix<Base, M, N>((lhs[0][Indices] - rhs[0][Indices])...);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::sub_value>::type>
		friend matrix<Base, M, N> operator-(const matrix<T, M, N>& lhs, const matrix<Base, M, N>& rhs) {
			return matrix<Base, M, N>((lhs[0][Indices] - rhs[0][Indices])...);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend matrix<Base, M, N> operator*(const matrix<Base, M, N>& lhs, const T& rhs) {
			return matrix<Base, M, N>((lhs[0][Indices] * rhs)...);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::mul_value>::type>
		friend matrix<Base, M, N> operator*(const T& lhs, const matrix<Base, M, N>& rhs) {
			return matrix<T, M, N>((lhs*rhs[0][Indices])...);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		friend matrix<Base, M, N> operator/(const matrix<Base, M, N>& lhs, const T& rhs) {
			return matrix<Base, M, N>((lhs[0][Indices] / rhs)...);
		}
	};
	//下に階層が存在するかつBase == T
	template <class Base, imsize_t M, imsize_t N, class T, imsize_t... Indices>
	class matrix_base<Base, M, N, T, index_imu_tuple<Indices...>, true, true> : public matrix_base<Base, M, N, typename T::algebraic_type> {
		template <class, imsize_t, imsize_t> friend class matrix;
		template <class, imsize_t, imsize_t, class, class, bool, bool> friend class matrix_base;
	public:
		//コンストラクタの継承
		using matrix_base<Base, M, N, typename T::algebraic_type>::matrix_base;

		//Base == TとなるためTypesによるコンストラクタは作成しない


		//単項演算の継承
		using matrix_base<Base, M, N, typename T::algebraic_type>::operator+;
		using matrix_base<Base, M, N, typename T::algebraic_type>::operator-;

		//代入演算の継承
		using matrix_base<Base, M, N, typename T::algebraic_type>::operator+=;
		using matrix_base<Base, M, N, typename T::algebraic_type>::operator-=;
		using matrix_base<Base, M, N, typename T::algebraic_type>::operator*=;
		using matrix_base<Base, M, N, typename T::algebraic_type>::operator/=;

		template <class U, class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		matrix_base& operator+=(const matrix_base<T, M, N, U>& ma) {
			for (imsize_t i = 0; i < dimension<index_imu_tuple<M, N>>::value; ++i) this->x[0][i] += ma.x[0][i];
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		matrix_base& operator-=(const matrix_base<T, M, N, U>& ma) {
			for (imsize_t i = 0; i < dimension<index_imu_tuple<M, N>>::value; ++i) this->x[0][i] -= ma.x[0][i];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		matrix_base& operator*=(const T& k) {
			for (imsize_t i = 0; i < dimension<index_imu_tuple<M, N>>::value; ++i) this->x[0][i] *= k;
			return *this;
		}
		//行列の積
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::mul_value && (M == N)>::type>
		matrix_base& operator*=(const matrix_base<T, M, N, U>& ma) {
			matrix<Base, M, M> temp;
			for (imsize_t i = 0; i < M; ++i)
				for (imsize_t j = 0; j < M; ++j)
					for (imsize_t k = 0; k < M; ++k)
						temp[i][j] += this->x[i][k] * ma.x[k][j];
			return *this = temp;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		matrix_base& operator/=(const T& k) {
			for (imsize_t i = 0; i < dimension<index_imu_tuple<M, N>>::value; ++i) this->x[0][i] /= k;
			return *this;
		}

		//添え字演算の継承
		using matrix_base<Base, M, N, typename T::algebraic_type>::operator[];


		//二項演算
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend matrix<Base, M, N> operator+(const matrix<Base, M, N>& lhs, const matrix<T, M, N>& rhs) {
			return matrix<Base, M, N>((lhs[0][Indices] + rhs[0][Indices])...);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend matrix<Base, M, N> operator-(const matrix<Base, M, N>& lhs, const matrix<T, M, N>& rhs) {
			return matrix<Base, M, N>((lhs[0][Indices] - rhs[0][Indices])...);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend matrix<Base, M, N> operator*(const matrix<Base, M, N>& lhs, const T& rhs) {
			return matrix<Base, M, N>((lhs[0][Indices] * rhs)...);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::mul_value>::type>
		friend matrix<Base, M, N> operator*(const T& lhs, const matrix<Base, M, N>& rhs) {
			return matrix<Base, M, N>((lhs*rhs[0][Indices])...);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		friend matrix<Base, M, N> operator/(const matrix<Base, M, N>& lhs, const T& rhs) {
			return matrix<Base, M, N>((lhs[0][Indices] / rhs)...);
		}
	};
	//下に階層が存在するかつBase != T
	template <class Base, imsize_t M, imsize_t N, class T, imsize_t... Indices>
	class matrix_base<Base, M, N, T, index_imu_tuple<Indices...>, true, false> : public matrix_base<Base, M, N, typename T::algebraic_type> {
		template <class, imsize_t, imsize_t> friend class matrix;
		template <class, imsize_t, imsize_t, class, class, bool, bool> friend class matrix_base;
	public:
		//コンストラクタの継承
		using matrix_base<Base, M, N, typename T::algebraic_type>::matrix_base;

		template <class = typename enable_if<is_inclusion<T, Base>::value>::type>
		constexpr matrix_base(const typename identity_type<T, Indices>::type&... x) : matrix_base<Base, M, N, typename T::algebraic_type>(static_cast<Base>(x)...) {}
		template <class U, class = typename enable_if<is_inclusion<T, Base>::value>::type>
		constexpr matrix_base(const matrix_base<T, M, N, U>& ma) : x{ static_cast<Base>(ma.x[0][Indices])... } {}


		//単項演算の継承
		using matrix_base<Base, M, N, typename T::algebraic_type>::operator+;
		using matrix_base<Base, M, N, typename T::algebraic_type>::operator-;

		//代入演算の継承
		using matrix_base<Base, M, N, typename T::algebraic_type>::operator+=;
		using matrix_base<Base, M, N, typename T::algebraic_type>::operator-=;
		using matrix_base<Base, M, N, typename T::algebraic_type>::operator*=;
		using matrix_base<Base, M, N, typename T::algebraic_type>::operator/=;

		template <class U, class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		matrix_base& operator+=(const matrix_base<T, M, N, U>& ma) {
			for (imsize_t i = 0; i < dimension<index_imu_tuple<M, N>>::value; ++i) this->x[0][i] += ma.x[0][i];
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		matrix_base& operator-=(const matrix_base<T, M, N, U>& ma) {
			for (imsize_t i = 0; i < dimension<index_imu_tuple<M, N>>::value; ++i) this->x[0][i] -= ma.x[0][i];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		matrix_base& operator*=(const T& k) {
			for (imsize_t i = 0; i < dimension<index_imu_tuple<M, N>>::value; ++i) this->x[0][i] *= k;
			return *this;
		}
		//行列の積
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::mul_value && (M == N)>::type>
		matrix_base& operator*=(const matrix_base<T, M, N, U>& ma) {
			matrix<Base, M, M> temp;
			for (imsize_t i = 0; i < M; ++i)
				for (imsize_t j = 0; j < M; ++j)
					for (imsize_t k = 0; k < M; ++k)
						temp[i][j] += this->x[i][k] * ma.x[k][j];
			return *this = temp;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		matrix_base& operator/=(const T& k) {
			for (imsize_t i = 0; i < dimension<index_imu_tuple<M, N>>::value; ++i) this->x[0][i] /= k;
			return *this;
		}

		//添え字演算の継承
		using matrix_base<Base, M, N, typename T::algebraic_type>::operator[];


		//二項演算
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend matrix<Base, M, N> operator+(const matrix<Base, M, N>& lhs, const matrix<T, M, N>& rhs) {
			return matrix<Base, M, N>((lhs[0][Indices] + rhs[0][Indices])...);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::add_value>::type>
		friend matrix<Base, M, N> operator+(const matrix<T, M, N>& lhs, const matrix<Base, M, N>& rhs) {
			return matrix<Base, M, N>((lhs[0][Indices] + rhs[0][Indices])...);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend matrix<Base, M, N> operator-(const matrix<Base, M, N>& lhs, const matrix<T, M, N>& rhs) {
			return matrix<Base, M, N>((lhs[0][Indices] - rhs[0][Indices])...);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::sub_value>::type>
		friend matrix<Base, M, N> operator-(const matrix<T, M, N>& lhs, const matrix<Base, M, N>& rhs) {
			return matrix<Base, M, N>((lhs[0][Indices] - rhs[0][Indices])...);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend matrix<Base, M, N> operator*(const matrix<Base, M, N>& lhs, const T& rhs) {
			return matrix<Base, M, N>((lhs[0][Indices] * rhs)...);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::mul_value>::type>
		friend matrix<Base, M, N> operator*(const T& lhs, const matrix<Base, M, N>& rhs) {
			return matrix<T, M, N>((lhs*rhs[0][Indices])...);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		friend matrix<Base, M, N> operator/(const matrix<Base, M, N>& lhs, const T& rhs) {
			return matrix<Base, M, N>((lhs[0][Indices] / rhs)...);
		}
	};

	//行列型
	template <class T, imsize_t M, imsize_t N>
	class matrix : public matrix_base<T, M, N, T>, public value_list_input<array_iterator<T>> {
		//MとNは0より大きくなければならない
		static_assert(M > 0 && N > 0, "M and N must be greater than 0.");

		template <class, imsize_t, imsize_t> friend class matrix;

		//代入演算の補助
		template <class _T>
		static matrix* matrix_copy(matrix* const lhs, const matrix<_T, M, N>& rhs) {
			//同じインスタンスでなければ代入
			if (static_cast<void*>(&lhs->x[0][0]) != static_cast<void*>(const_cast<_T*>(&rhs.x[0][0])))
				for (imsize_t i = 0; i < dimension<index_imu_tuple<M, N>>::value; ++i) lhs->x[0][i] = static_cast<T>(rhs.x[0][i]);
			return lhs;
		}
	public:
		//コンストラクタの継承
		using matrix_base<T, M, N, T>::matrix_base;

		constexpr matrix() : matrix_base<T, M, N, T>() {}
		matrix(const T(&ma)[M][N]) {
			for (imsize_t i = 0; i < M; ++i)
				for (imsize_t j = 0; j < N; ++j)
					this->x[i][j] = ma[i][j];
		}

		using algebraic_type = T;
		using iterator = array_iterator<T>;
		using const_iterator = array_iterator<const T>;

		template<class Other>
		struct rebind {
			using other = matrix<Other, M, N>;
		};

		iterator begin() noexcept { return iterator(&x[0][0]); }
		const_iterator begin() const noexcept { return const_iterator(&x[0][0]); }
		iterator end() noexcept { return iterator(&x[0][dimension<index_imu_tuple<M, N>>::value - 1] + 1); }
		const_iterator end() const noexcept { return const_iterator(&x[0][dimension<index_imu_tuple<M, N>>::value - 1] + 1); }

		//単項演算の継承
		using matrix_base<T, M, N, T>::operator-;
		using matrix_base<T, M, N, T>::operator+;
		//代入演算の継承
		using matrix_base<T, M, N, T>::operator+=;
		using matrix_base<T, M, N, T>::operator-=;
		using matrix_base<T, M, N, T>::operator*=;
		using matrix_base<T, M, N, T>::operator/=;
		//二項演算の継承
		using value_list_input<array_iterator<T>>::operator<<;

		//代入演算
		matrix& operator=(const matrix& ma) { return *matrix_copy(this, ma); }
		template <class _T, class = typename enable_if<is_inclusion<_T, T>::value>::type>
		matrix& operator=(const matrix<_T, M, N>& ma) { return *matrix_copy(this, ma); }
		matrix& operator=(const matrix_base<T, M, N, T>& ma) { return *matrix_copy(this, ma); }
		template <class U>
		matrix& operator=(const matrix_base<T, M, N, U>& ma) { return *matrix_copy(this, ma); }
		template <class U, class _T, class = typename enable_if<is_inclusion<_T, T>::value>::type>
		matrix& operator=(const matrix_base<_T, M, N, U>& ma) { return *matrix_copy(this, ma); }

		//添え字演算の継承
		using matrix_base<T, M, N, T>::operator[];

		//ストリーム出力
		friend std::ostream& operator<<(std::ostream& os, const matrix& ma) {
			//-1は改行対策
			for (imsize_t i = 0; i < M - 1; ++i) {
				for (imsize_t j = 0; j < N; ++j)
					os << ma.x[i][j] << ' ';
				os << std::endl;
			}
			for (imsize_t j = 0; j < N; ++j)
				os << ma.x[M - 1][j] << ' ';
			return os;
		}
		friend std::wostream& operator<<(std::wostream& os, const matrix& ma) {
			//-1は改行対策
			for (imsize_t i = 0; i < M - 1; ++i) {
				for (imsize_t j = 0; j < N; ++j)
					os << ma.x[i][j] << L' ';
				os << std::endl;
			}
			for (imsize_t j = 0; j < N; ++j)
				os << ma.x[M - 1][j] << L' ';
			return os;
		}
	};


	//行列の判定
	template <class T>
	struct _Is_matrix : false_type {};
	template <class T, imsize_t M, imsize_t N>
	struct _Is_matrix<matrix<T, M, N>> : true_type {};
	template <class T>
	struct is_matrix : _Is_matrix<typename remove_cv<T>::type> {};

	//行列の除去
	template <class T>
	struct remove_matrix {
		using type = T;
	};
	template <class T, imsize_t M, imsize_t N>
	struct remove_matrix<matrix<T, M, N>> {
		using type = T;
	};

	//全ての行列の除去
	template <class T>
	struct remove_all_matrix {
		using type = T;
	};
	template <class T, imsize_t M, imsize_t N>
	struct remove_all_matrix<matrix<T, M, N>> : remove_all_matrix<T> {};


	//行列の積(T1×T2→Sが加法についてマグマ)
	template <class T1, class T2, imsize_t M, imsize_t N, imsize_t L
		, class = typename enable_if<is_calcable<T1, T2>::mul_value && is_magma<typename calculation_result<T1, T2>::mul_type>::add_value>::type>
	inline matrix<typename calculation_result<T1, T2>::mul_type, M, L> operator*(const matrix<T1, M, N>& lhs, const matrix<T2, N, L>& rhs) {
		matrix<typename calculation_result<T1, T2>::mul_type, M, L> temp;
		for (imsize_t i = 0; i < M; ++i)
			for (imsize_t j = 0; j < L; ++j)
				for (imsize_t k = 0; k < N; ++k)
					temp[i][j] += lhs[i][k] * rhs[k][j];
		return temp;
	}

	//比較演算
	template <class T1, class T2, imsize_t M, imsize_t N, class = typename enable_if<is_calcable<T1, T2>::eq_value>::type>
	inline bool operator==(const matrix<T1, M, N>& lhs, const matrix<T2, M, N>& rhs) {
		for (imsize_t i = 0; i < dimension<index_imu_tuple<M, N>>::value; ++i) if (lhs[0][i] != rhs[0][i]) return false;
		return true;
	}
	template <class T1, class T2, imsize_t M, imsize_t N, class = typename enable_if<is_calcable<T1, T2>::eq_value>::type>
	inline bool operator!=(const matrix<T1, M, N>& lhs, const matrix<T2, M, N>& rhs) {
		return !(lhs == rhs);
	}


	//各種行列の定義
	template <class T>
	using matrix2 = matrix<T, 2, 2>;
	template <class T>
	using matrix3 = matrix<T, 3, 3>;
	template <class T>
	using matrix4 = matrix<T, 4, 4>;
}


namespace iml {

	//加法パラメータ取得
	template <class T, imsize_t M, imsize_t N>
	struct addition<matrix<T, M, N>> {
		//単位元(零行列)の取得
		static constexpr matrix<T, M, N> identity_element() {
			return matrix<T, M, N>();
		}
		//逆元の取得
		static constexpr matrix<T, M, M> inverse_element(const matrix<T, M, N>& x) {
			return -x;
		}
	};
	//乗法パラメータ取得
	template <class T, imsize_t M, imsize_t N>
	struct multiplicative<matrix<T, M, N>> {};
	template <class T, imsize_t N>
	struct multiplicative<matrix<T, N, N>> {
		//単位元(単位行列)の取得
		static constexpr matrix<T, N, N> identity_element() {
			matrix<T, N, N> temp;
			for (imsize_t i = 0; i < N; ++i) temp[i][i] = multiplicative<T>::identity_element();
			return temp;
		}
		//吸収元(零行列)
		static constexpr matrix<T, N, N> absorbing_element() {
			return matrix<T, N, N>();
		}
	};

}

namespace iml {

	//誤差評価
	template <class T, imsize_t M, imsize_t N>
	struct Error_evaluation<matrix<T, M, N>> {
		static constexpr bool _error_evaluation_(const matrix<T, M, N>& x1, const matrix<T, M, N>& x2) {
			for (imsize_t i = 0; i < dimension<index_imu_tuple<M, N>>::value; ++i) if (!error_evaluation(x1[0][i], x2[0][i])) return false;
			return true;
		}
	};
}

#endif