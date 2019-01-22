#ifndef _IMATH_LINER_ALGEBRA_VECTOR_HPP
#define _IMATH_LINER_ALGEBRA_VECTOR_HPP

#include "IMathLib/utility/utility.hpp"
#include "IMathLib/utility/tuple.hpp"
#include "IMathLib/math/math.hpp"
#include "IMathLib/container/array.hpp"

namespace iml {

	template<class, imsize_t>
	class vector;

	//多重構造の演算に対応させるための補助型
	//継承コンストラクタと継承オペレータオーバーロードにより多数定義可能となる
	template <class, imsize_t, class, bool, class, bool>
	class _Vector_base;

	//簡略にかくためのエイリアス
	template <class Base, imsize_t N, class T>
	using _Vector_base_base_type = _Vector_base<Base, N, typename T::algebraic_type
		, is_algebraic_structure<typename T::algebraic_type>::value, typename same_arg_tuple<T, N>::type, is_same<Base, T>::value>;
	template <class Base, class T, imsize_t N>
	using _Vector_base_type = _Vector_base<Base, N, T
		, is_algebraic_structure<T>::value, typename same_arg_tuple<T, N>::type, is_same<Base, T>::value>;

	//下に階層が存在しないかつBase == T
	template <class Base, imsize_t N, class T, class... Types>
	class _Vector_base<Base, N, T, false, arg_tuple<Types...>, true> {
		template <class, imsize_t> friend class vector;
		template <class, imsize_t, class> friend class _Vector;
		template <class, imsize_t, class, bool, class, bool> friend class _Vector_base;
	protected:
		Base x[N];

		//ダミー
		template <class... _Types>
		constexpr _Vector_base(arg_tuple<_Types...>, const _Types&... x) : x{ static_cast<Base>(x)... } {}
	public:
		constexpr _Vector_base() : x{} {}
		constexpr _Vector_base(const Types&... x) : x{ static_cast<Base>(x)... } {}

		template <class U, class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		_Vector_base& operator+=(const _Vector_base_type<T, U, N>& v) {
			for (imsize_t i = 0; i < N; ++i) this->x[i] += v.x[i];
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		_Vector_base& operator-=(const _Vector_base_type<T, U, N>& v) {
			for (imsize_t i = 0; i < N; ++i) this->x[i] -= v.x[i];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		_Vector_base& operator*=(const T& k) {
			for (imsize_t i = 0; i < N; ++i) this->x[i] *= k;
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		_Vector_base& operator/=(const T& k) {
			for (imsize_t i = 0; i < N; ++i) this->x[i] /= k;
			return *this;
		}

		//アクセサ
		const Base& operator[](imsize_t index) const { return this->x[index]; }
		Base& operator[](imsize_t index) { return this->x[index]; }


		//二項演算
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend vector<Base, N> operator+(const _Vector_base& v1, const _Vector_base_type<T, U, N>& v2) {
			vector<Base, N> temp;
			for (imsize_t i = 0; i < N; ++i) temp[i] = v1[i] + v2[i];
			return temp;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend vector<Base, N> operator-(const _Vector_base& v1, const _Vector_base_type<T, U, N>& v2) {
			vector<Base, N> temp;
			for (imsize_t i = 0; i < N; ++i) temp[i] = v1[i] - v2[i];
			return temp;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend vector<Base, N> operator*(const _Vector_base& v, const T& k) {
			vector<Base, N> temp;
			for (imsize_t i = 0; i < N; ++i) temp[i] = v[i] * k;
			return temp;
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::mul_value>::type>
		friend vector<Base, N> operator*(const T& k, const _Vector_base& v) {
			vector<Base, N> temp;
			for (imsize_t i = 0; i < N; ++i) temp[i] = k * v[i];
			return temp;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		friend vector<Base, N> operator/(const _Vector_base& v, const T& k) {
			vector<Base, N> temp;
			for (imsize_t i = 0; i < N; ++i) temp[i] = v[i] / k;
			return temp;
		}
	};
	//下に階層が存在しないかつBase != T
	template <class Base, imsize_t N, class T, class... Types>
	class _Vector_base<Base, N, T, false, arg_tuple<Types...>, false> {
		template <class, imsize_t> friend class vector;
		template <class, imsize_t, class> friend class _Vector;
		template <class, imsize_t, class, bool, class, bool> friend class _Vector_base;
	protected:
		Base x[N];

		//ダミー
		template <class... _Types>
		constexpr _Vector_base(arg_tuple<_Types...>, const _Types&... x) : x{ static_cast<Base>(x)... } {}
	public:
		constexpr _Vector_base() : x{} {}
		constexpr _Vector_base(const Types&... x) : x{ static_cast<Base>(x)... } {}

		template <class U, class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		_Vector_base& operator+=(const _Vector_base_type<T, U, N>& v) {
			for (imsize_t i = 0; i < N; ++i) this->x[i] += v.x[i];
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		_Vector_base& operator-=(const _Vector_base_type<T, U, N>& v) {
			for (imsize_t i = 0; i < N; ++i) this->x[i] -= v.x[i];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		_Vector_base& operator*=(const T& k) {
			for (imsize_t i = 0; i < N; ++i) this->x[i] *= k;
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		_Vector_base& operator/=(const T& k) {
			for (imsize_t i = 0; i < N; ++i) this->x[i] /= k;
			return *this;
		}

		//アクセサ
		const Base& operator[](imsize_t index) const { return this->x[index]; }
		Base& operator[](imsize_t index) { return this->x[index]; }


		//二項演算
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend vector<Base, N> operator+(const _Vector_base& v1, const _Vector_base_type<T, U, N>& v2) {
			vector<Base, N> temp;
			for (imsize_t i = 0; i < N; ++i) temp[i] = v1[i] + v2[i];
			return temp;
		}
		template <class U, class = typename enable_if<is_operation<T, Base, Base>::add_value>::type>
		friend vector<Base, N> operator+(const _Vector_base_type<T, U, N>& v1, const _Vector_base& v2) {
			vector<Base, N> temp;
			for (imsize_t i = 0; i < N; ++i) temp[i] = v1[i] + v2[i];
			return temp;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend vector<Base, N> operator-(const _Vector_base& v1, const _Vector_base_type<T, U, N>& v2) {
			vector<Base, N> temp;
			for (imsize_t i = 0; i < N; ++i) temp[i] = v1[i] - v2[i];
			return temp;
		}
		template <class U, class = typename enable_if<is_operation<T, Base, Base>::sub_value>::type>
		friend vector<Base, N> operator-(const _Vector_base_type<T, U, N>& v1, const _Vector_base& v2) {
			vector<Base, N> temp;
			for (imsize_t i = 0; i < N; ++i) temp[i] = v1[i] - v2[i];
			return temp;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend vector<Base, N> operator*(const _Vector_base& v, const T& k) {
			vector<Base, N> temp;
			for (imsize_t i = 0; i < N; ++i) temp[i] = v[i] * k;
			return temp;
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::mul_value>::type>
		friend vector<Base, N> operator*(const T& k, const _Vector_base& v) {
			vector<Base, N> temp;
			for (imsize_t i = 0; i < N; ++i) temp[i] = k * v[i];
			return temp;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		friend vector<Base, N> operator/(const _Vector_base& v, const T& k) {
			vector<Base, N> temp;
			for (imsize_t i = 0; i < N; ++i) temp[i] = v[i] / k;
			return temp;
		}
	};
	//下に階層が存在するかつBase == T
	template <class Base, imsize_t N, class T, class... Types>
	class _Vector_base<Base, N, T, true, arg_tuple<Types...>, true> : public _Vector_base_base_type<Base, N, T> {
		template <class, imsize_t> friend class vector;
		template <class, imsize_t, class> friend class _Vector;
		template <class, imsize_t, class, bool, class, bool> friend class _Vector_base;
	public:
		//コンストラクタの継承
		using _Vector_base_base_type<Base, N, T>::_Vector_base;

		constexpr _Vector_base() : _Vector_base_base_type<Base, N, T>() {}
		constexpr _Vector_base(const Types&... x)
			: _Vector_base_base_type<Base, N, T>(typename same_arg_tuple<Base, N>::type(), static_cast<Base>(x)...) {}

		//代入演算の継承
		using _Vector_base_base_type<Base, N, T>::operator+=;
		using _Vector_base_base_type<Base, N, T>::operator-=;
		using _Vector_base_base_type<Base, N, T>::operator*=;
		using _Vector_base_base_type<Base, N, T>::operator/=;

		template <class U, class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		_Vector_base& operator+=(const _Vector_base_type<T, U, N>& v) {
			for (imsize_t i = 0; i < N; ++i) this->x[i] += v.x[i];
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		_Vector_base& operator-=(const _Vector_base_type<T, U, N>& v) {
			for (imsize_t i = 0; i < N; ++i) this->x[i] -= v.x[i];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		_Vector_base& operator*=(const T& k) {
			for (imsize_t i = 0; i < N; ++i) this->x[i] *= k;
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		_Vector_base& operator/=(const T& k) {
			for (imsize_t i = 0; i < N; ++i) this->x[i] /= k;
			return *this;
		}

		//添え字演算の継承
		using _Vector_base_base_type<Base, N, T>::operator[];


		//二項演算
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend vector<Base, N> operator+(const _Vector_base& v1, const _Vector_base_type<T, U, N>& v2) {
			vector<Base, N> temp;
			for (imsize_t i = 0; i < N; ++i) temp[i] = v1[i] + v2[i];
			return temp;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend vector<Base, N> operator-(const _Vector_base& v1, const _Vector_base_type<T, U, N>& v2) {
			vector<Base, N> temp;
			for (imsize_t i = 0; i < N; ++i) temp[i] = v1[i] - v2[i];
			return temp;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend vector<Base, N> operator*(const _Vector_base& v, const T& k) {
			vector<Base, N> temp;
			for (imsize_t i = 0; i < N; ++i) temp[i] = v[i] * k;
			return temp;
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::mul_value>::type>
		friend vector<Base, N> operator*(const T& k, const _Vector_base& v) {
			vector<Base, N> temp;
			for (imsize_t i = 0; i < N; ++i) temp[i] = k * v[i];
			return temp;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		friend vector<Base, N> operator/(const _Vector_base& v, const T& k) {
			vector<Base, N> temp;
			for (imsize_t i = 0; i < N; ++i) temp[i] = v[i] / k;
			return temp;
		}
	};
	//下に階層が存在するかつBase != T
	template <class Base, imsize_t N, class T, class... Types>
	class _Vector_base<Base, N, T, true, arg_tuple<Types...>, false> : public _Vector_base_base_type<Base, N, T> {
		template <class, imsize_t> friend class vector;
		template <class, imsize_t, class> friend class _Vector;
		template <class, imsize_t, class, bool, class, bool> friend class _Vector_base;
	public:
		//コンストラクタの継承
		using _Vector_base_base_type<Base, N, T>::_Vector_base;

		constexpr _Vector_base() : _Vector_base_base_type<Base, N, T>() {}
		constexpr _Vector_base(const Types&... x)
			: _Vector_base_base_type<Base, N, T>(typename same_arg_tuple<Base, N>::type(), static_cast<Base>(x)...) {}

		//代入演算の継承
		using _Vector_base_base_type<Base, N, T>::operator+=;
		using _Vector_base_base_type<Base, N, T>::operator-=;
		using _Vector_base_base_type<Base, N, T>::operator*=;
		using _Vector_base_base_type<Base, N, T>::operator/=;

		template <class U, class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		_Vector_base& operator+=(const _Vector_base_type<T, U, N>& v) {
			for (imsize_t i = 0; i < N; ++i) this->x[i] += v.x[i];
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		_Vector_base& operator-=(const _Vector_base_type<T, U, N>& v) {
			for (imsize_t i = 0; i < N; ++i) this->x[i] -= v.x[i];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		_Vector_base& operator*=(const T& k) {
			for (imsize_t i = 0; i < N; ++i) this->x[i] *= k;
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		_Vector_base& operator/=(const T& k) {
			for (imsize_t i = 0; i < N; ++i) this->x[i] /= k;
			return *this;
		}

		//添え字演算の継承
		using _Vector_base_base_type<Base, N, T>::operator[];


		//二項演算
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend vector<Base, N> operator+(const _Vector_base& v1, const _Vector_base_type<T, U, N>& v2) {
			vector<Base, N> temp;
			for (imsize_t i = 0; i < N; ++i) temp[i] = v1[i] + v2[i];
			return temp;
		}
		template <class U, class = typename enable_if<is_operation<T, Base, Base>::add_value>::type>
		friend vector<Base, N> operator+(const _Vector_base_type<T, U, N>& v1, const _Vector_base& v2) {
			vector<Base, N> temp;
			for (imsize_t i = 0; i < N; ++i) temp[i] = v1[i] + v2[i];
			return temp;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend vector<Base, N> operator-(const _Vector_base& v1, const _Vector_base_type<T, U, N>& v2) {
			vector<Base, N> temp;
			for (imsize_t i = 0; i < N; ++i) temp[i] = v1[i] - v2[i];
			return temp;
		}
		template <class U, class = typename enable_if<is_operation<T, Base, Base>::sub_value>::type>
		friend vector<Base, N> operator-(const _Vector_base_type<T, U, N>& v1, const _Vector_base& v2) {
			vector<Base, N> temp;
			for (imsize_t i = 0; i < N; ++i) temp[i] = v1[i] - v2[i];
			return temp;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend vector<Base, N> operator*(const _Vector_base& v, const T& k) {
			vector<Base, N> temp;
			for (imsize_t i = 0; i < N; ++i) temp[i] = v[i] * k;
			return temp;
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::mul_value>::type>
		friend vector<Base, N> operator*(const T& k, const _Vector_base& v) {
			vector<Base, N> temp;
			for (imsize_t i = 0; i < N; ++i) temp[i] = k * v[i];
			return temp;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		friend vector<Base, N> operator/(const _Vector_base& v, const T& k) {
			vector<Base, N> temp;
			for (imsize_t i = 0; i < N; ++i) temp[i] = v[i] / k;
			return temp;
		}
	};

	//連番シーケンスのための補助型
	template <class T, imsize_t N, class Index>
	class _Vector;
	template <class T, imsize_t N, imsize_t... Indices>
	class _Vector<T, N, index_tuple<Indices...>> : public _Vector_base_type<T, T, N> {
		template <class, imsize_t> friend class vector;
		template <class, imsize_t, class> friend class _Vector;
	public:
		constexpr _Vector() : _Vector_base_type<T, T, N>() {}
		//配列的特性を利用した構築
		constexpr _Vector(const _Vector& v) : _Vector_base_type<T, T, N>(v.x[Indices]...) {}
		template <class _T, class = typename enable_if<is_inclusion<_T, T>::value>::type>
		explicit constexpr _Vector(const _Vector<_T, N, index_tuple<Indices...>>& v)
			: _Vector_base_type<T, T, N>(static_cast<const T>(v.x[Indices])...) {}
		template <class U>
		constexpr _Vector(const _Vector_base_type<T, U, N>& v) : _Vector_base_type<T, T, N>(v.x[Indices]...) {}
		template <class U, class _T, class = typename enable_if<is_inclusion<_T, T>::value>::type>
		explicit constexpr _Vector(const _Vector_base_type<_T, U, N>& v) : _Vector_base_type<T, T, N>(static_cast<const T>(v.x[Indices])...) {}
		//イテレータによる構築
		//template <class InputIterator, class = typename enable_if<is_inclusion<_T, T>::value>::type>
		//constexpr _Vector(InputIterator x) : _Vector_base_type<T, T, N>(static_cast<const T>(x[Indices])...) {}

		//コンストラクタの継承
		using _Vector_base_type<T, T, N>::_Vector_base;

		//単項演算の定義
		template <class = typename enable_if<is_exist_add_inverse_element<T>::value>::type>
		_Vector_base_type<T, T, N> operator-() const { return _Vector_base_type<T, T, N>(-this->x[Indices]...); }
		_Vector_base_type<T, T, N> operator+() const { return _Vector_base_type<T, T, N>(*this); }
		//代入演算の継承
		using _Vector_base_type<T, T, N>::operator+=;
		using _Vector_base_type<T, T, N>::operator-=;
		using _Vector_base_type<T, T, N>::operator*=;
		using _Vector_base_type<T, T, N>::operator/=;

		//添え字演算の継承
		using _Vector_base_type<T, T, N>::operator[];
	};

	//簡略にかくためのエイリアス
	template <class T, imsize_t N>
	using _Vector_type = _Vector<T, N, typename index_range<0, N>::type>;

	//ベクトル型
	template <class T, imsize_t N>
	class vector : public _Vector_type<T, N>, public value_list_input<array_iterator<T>> {
		template <class, imsize_t> friend class vector;

		//代入演算の補助
		template <class _T>
		static vector* vector_copy(vector* const v1, const vector<_T, N>& v2) {
			//同じインスタンスでなければ代入
			if (static_cast<void*>(&v1->x[0]) != static_cast<void*>(const_cast<_T*>(&v2.x[0])))
				for (imsize_t i = 0; i < N; ++i) v1->x[i] = static_cast<T>(v2.x[i]);
			return v1;
		}
	public:
		//コンストラクタの継承
		using _Vector_type<T, N>::_Vector;
		
		using algebraic_type = T;
		using iterator = array_iterator<T>;
		using const_iterator = array_iterator<const T>;

		template<class Other>
		struct rebind {
			using other = vector<Other, N>;
		};

		iterator begin() noexcept { return iterator(&x[0]); }
		const_iterator begin() const noexcept { return const_iterator(&x[0]); }
		iterator end() noexcept { return iterator(&x[N - 1] + 1); }
		const_iterator end() const noexcept { return const_iterator(&x[N - 1] + 1); }

		//単項演算の継承
		using _Vector_type<T, N>::operator-;
		using _Vector_type<T, N>::operator+;
		//代入演算の継承
		using _Vector_type<T, N>::operator+=;
		using _Vector_type<T, N>::operator-=;
		using _Vector_type<T, N>::operator*=;
		using _Vector_type<T, N>::operator/=;
		//二項演算の継承
		using value_list_input<array_iterator<T>>::operator<<;

		//代入演算
		vector& operator=(const vector& v) { return *vector_copy(this, v); }
		template <class _T>
		vector& operator=(const vector<_T, N>& v) { return *vector_copy(this, v); }
		vector& operator=(const _Vector_type<T, N>& v) { return *vector_copy(this, v); }
		template <class _T, class = typename enable_if<is_inclusion<_T, T>::value>::type>
		vector& operator=(const _Vector_type<_T, N>& v) { return *vector_copy(this, v); }
		template <class U>
		vector& operator=(const _Vector_base_type<T, U, N>& v) { return *vector_copy(this, v); }
		template <class U, class _T, class = typename enable_if<is_inclusion<_T, T>::value>::type>
		vector& operator=(const _Vector_base_type<_T, U, N>& v) { return *vector_copy(this, v); }

		//添え字演算の継承
		using _Vector_type<T, N>::operator[];

		//ストリーム出力
		friend std::ostream& operator<<(std::ostream& os, const vector& v) {
			os << v.x[0];
			for (imsize_t i = 1; i < N; ++i) os << ',' << v.x[i];
			return os;
		}
		friend std::wostream& operator<<(std::wostream& os, const vector& v) {
			os << v.x[0];
			for (imsize_t i = 1; i < N; ++i) os << L',' << v.x[i];
			return os;
		}
	};


	//ベクトルの判定
	template <class T>
	struct _Is_vector : false_type {};
	template <class T, imsize_t N>
	struct _Is_vector<vector<T, N>> : true_type {};
	template <class T>
	struct is_vector : _Is_vector<typename remove_cv<T>::type> {};

	//ベクトルの除去
	template <class T>
	struct remove_vector {
		using type = T;
	};
	template <class T, imsize_t N>
	struct remove_vector<vector<T, N>> {
		using type = T;
	};

	//全てのベクトルの除去
	template <class T>
	struct remove_all_vector {
		using type = T;
	};
	template <class T, imsize_t N>
	struct remove_all_vector<vector<T, N>> : remove_all_vector<T> {};

	//ベクトルの拡張二項演算
	//内積(T1×T2→Sが加法についてマグマ)
	template <class U1, class U2, class T1, class T2, imsize_t N
		, class = typename enable_if<is_calcable<T1, T2>::mul_value && is_magma<typename calculation_result<T1, T2>::mul_type>::add_value>::type>
	inline typename calculation_result<T1, T2>::mul_type operator*(const _Vector_base_type<T1, U1, N>& v1, const _Vector_base_type<T2, U2, N>& v2) {
		//加法単位元で初期化
		typename calculation_result<T1, T2>::mul_type temp(addition<typename calculation_result<T1, T2>::mul_type>::identity_element());
		for (imsize_t i = 0; i < N; ++i) temp += v1[i] * conj(v2[i]);
		return temp;
	}
	//外積(T1×T2→Sが加法についてループ)
	template <class U1, class U2, class T1, class T2
		, class = typename enable_if<is_calcable<T1, T2>::mul_value && is_loop<typename calculation_result<T1, T2>::mul_type>::add_value>::type>
	inline vector<typename calculation_result<T1, T2>::mul_type, 3> operator%(const _Vector_base_type<T1, U1, 3>& v1, const _Vector_base_type<T2, U2, 3>& v2) {
		return vector<typename calculation_result<T1, T2>::mul_type, 3>(v1[1] * v2[2] - v1[2] * v2[1], v1[2] * v2[0] - v1[0] * v2[2], v1[0] * v2[1] - v1[1] * v2[0]);
	}

	//比較演算
	template <class U1, class U2, class T1, class T2, imsize_t N, class = typename enable_if<is_inclusion<T1, T2>::value || is_inclusion<T2, T1>::value>::type>
	inline bool operator==(const _Vector_base_type<T1, U1, N>& v1, const _Vector_base_type<T2, U2, N>& v2) {
		for (imsize_t i = 0; i < N; ++i) if (v1[i] != v2[i]) return false;
		return true;
	}
	template <class U1, class U2, class T1, class T2, imsize_t N, class = typename enable_if<is_inclusion<T1, T2>::value || is_inclusion<T2, T1>::value>::type>
	inline bool operator!=(const _Vector_base_type<T1, U1, N>& v1, const _Vector_base_type<T2, U2, N>& v2) {
		return !(v1 == v2);
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

	//加法パラメータ取得
	template <class T, imsize_t N>
	struct addition<vector<T, N>> {
		//単位元の取得
		static vector<T, N> identity_element() {
			return vector<T, N>();
		}
		//逆元の取得
		template <class = typename enable_if<is_exist_add_inverse_element<T>::value>::type>
		static constexpr vector<T, N> inverse_element(const vector<T, N>& x) {
			return -x;
		}
	};
	//乗法パラメータ取得
	template <class T, imsize_t N>
	struct multiplicative<vector<T, N>> {
		//吸収元
		template <class = typename enable_if<is_calcable<T, T>::mul_value>::type>
		static vector<T, N> absorbing_element() {
			return vector<T, N>();
		}
	};

}

namespace iml {

	//誤差評価
	template <class T, imsize_t N>
	struct _Error_evaluation<vector<T, N>> {
		static bool __error_evaluation(const vector<T, N>& x1, const vector<T, N>& x2) {
			for (imsize_t i = 0; i < N; ++i) if (!error_evaluation(x1[i], x2[i])) return false;
			return true;
		}
	};
}

#endif