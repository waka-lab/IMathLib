#ifndef IMATH_MATH_LINER_ALGEBRA_VECTOR_HPP
#define IMATH_MATH_LINER_ALGEBRA_VECTOR_HPP

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
		, is_algebraic_structure<typename T::algebraic_type>::value, typename index_range<0, N>::type, is_same<Base, typename T::algebraic_type>::value>;
	template <class Base, imsize_t N, class T>
	using _Vector_base_type = _Vector_base<Base, N, T
		, is_algebraic_structure<T>::value, typename index_range<0, N>::type, is_same<Base, T>::value>;


	//下に階層が存在しないかつBase == T
	template <class Base, imsize_t N, class T, imsize_t... Indices>
	class _Vector_base<Base, N, T, false, index_tuple<Indices...>, true> {
		template <class, imsize_t> friend class vector;
		template <class, imsize_t, class, bool, class, bool> friend class _Vector_base;
	protected:
		Base x[N];
	public:
		//Base == TとなるためTypesによるコンストラクタは作成しない
		constexpr _Vector_base() : x{} {}
		constexpr _Vector_base(const typename identity_type<Base, Indices>::type&... x) : x{ x... } {}
		template <class U>
		constexpr _Vector_base(const _Vector_base_type<Base, N, U>& v) : x{ v.x[Indices]... } {}


		template <class = typename enable_if<is_exist_add_inverse_element<T>::value>::type>
		_Vector_base operator-() const { return _Vector_base(-this->x[Indices]...); }
		_Vector_base operator+() const { return _Vector_base(*this); }

		template <class U, class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		_Vector_base& operator+=(const _Vector_base_type<T, N, U>& v) {
			for (imsize_t i = 0; i < N; ++i) this->x[i] += v.x[i];
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		_Vector_base& operator-=(const _Vector_base_type<T, N, U>& v) {
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
		friend vector<Base, N> operator+(const _Vector_base& v1, const _Vector_base_type<T, N, U>& v2) {
			return vector<Base, N>((v1[Indices] + v2[Indices])...);
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend vector<Base, N> operator-(const _Vector_base& v1, const _Vector_base_type<T, N, U>& v2) {
			return vector<Base, N>((v1[Indices] - v2[Indices])...);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend vector<Base, N> operator*(const _Vector_base& v, const T& k) {
			return vector<Base, N>((v[Indices] * k)...);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::mul_value>::type>
		friend vector<Base, N> operator*(const T& k, const _Vector_base& v) {
			return vector<Base, N>((k*v[Indices])...);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		friend vector<Base, N> operator/(const _Vector_base& v, const T& k) {
			return vector<Base, N>((v[Indices] / k)...);
		}
	};
	//下に階層が存在しないかつBase != T
	template <class Base, imsize_t N, class T, imsize_t... Indices>
	class _Vector_base<Base, N, T, false, index_tuple<Indices...>, false> {
		template <class, imsize_t> friend class vector;
		template <class, imsize_t, class, bool, class, bool> friend class _Vector_base;
	protected:
		Base x[N];
	public:
		constexpr _Vector_base() : x{} {}
		constexpr _Vector_base(const typename identity_type<Base, Indices>::type&... x) : x{ x... } {}
		template <class = typename enable_if<is_inclusion<T, Base>::value>::type>
		constexpr _Vector_base(const typename identity_type<T, Indices>::type&... x) : x{ static_cast<Base>(x)... } {}
		template <class U>
		constexpr _Vector_base(const _Vector_base_type<Base, N, U>& v) : x{ v.x[Indices]... } {}
		template <class U, class = typename enable_if<is_inclusion<T, Base>::value>::type>
		constexpr _Vector_base(const _Vector_base_type<T, N, U>& v) : x{ static_cast<Base>(v.x[Indices])... } {}


		template <class = typename enable_if<is_exist_add_inverse_element<T>::value>::type>
		_Vector_base operator-() const { return _Vector_base(-this->x[Indices]...); }
		_Vector_base operator+() const { return _Vector_base(*this); }

		template <class U, class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		_Vector_base& operator+=(const _Vector_base_type<T, N, U>& v) {
			for (imsize_t i = 0; i < N; ++i) this->x[i] += v.x[i];
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		_Vector_base& operator-=(const _Vector_base_type<T, N, U>& v) {
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
		friend vector<Base, N> operator+(const _Vector_base& v1, const _Vector_base_type<T, N, U>& v2) {
			return vector<Base, N>((v1[Indices] + v2[Indices])...);
		}
		template <class U, class = typename enable_if<is_operation<T, Base, Base>::add_value>::type>
		friend vector<Base, N> operator+(const _Vector_base_type<T, N, U>& v1, const _Vector_base& v2) {
			return vector<Base, N>((v1[Indices] + v2[Indices])...);
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend vector<Base, N> operator-(const _Vector_base& v1, const _Vector_base_type<T, N, U>& v2) {
			return vector<Base, N>((v1[Indices] - v2[Indices])...);
		}
		template <class U, class = typename enable_if<is_operation<T, Base, Base>::sub_value>::type>
		friend vector<Base, N> operator-(const _Vector_base_type<T, N, U>& v1, const _Vector_base& v2) {
			return vector<Base, N>((v1[Indices] - v2[Indices])...);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend vector<Base, N> operator*(const _Vector_base& v, const T& k) {
			return vector<Base, N>((v[Indices] * k)...);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::mul_value>::type>
		friend vector<Base, N> operator*(const T& k, const _Vector_base& v) {
			return vector<Base, N>((k*v[Indices])...);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		friend vector<Base, N> operator/(const _Vector_base& v, const T& k) {
			return vector<Base, N>((v[Indices] / k)...);
		}
	};
	//下に階層が存在するかつBase == T
	template <class Base, imsize_t N, class T, imsize_t... Indices>
	class _Vector_base<Base, N, T, true, index_tuple<Indices...>, true> : public _Vector_base_base_type<Base, N, T> {
		template <class, imsize_t> friend class vector;
		template <class, imsize_t, class, bool, class, bool> friend class _Vector_base;
	public:
		//コンストラクタの継承
		using _Vector_base_base_type<Base, N, T>::_Vector_base;

		//Base == TとなるためTypesによるコンストラクタは作成しない

		//単項演算の継承
		using _Vector_base_base_type<Base, N, T>::operator+;
		using _Vector_base_base_type<Base, N, T>::operator-;

		//代入演算の継承
		using _Vector_base_base_type<Base, N, T>::operator+=;
		using _Vector_base_base_type<Base, N, T>::operator-=;
		using _Vector_base_base_type<Base, N, T>::operator*=;
		using _Vector_base_base_type<Base, N, T>::operator/=;

		template <class U, class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		_Vector_base& operator+=(const _Vector_base_type<T, N, U>& v) {
			for (imsize_t i = 0; i < N; ++i) this->x[i] += v.x[i];
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		_Vector_base& operator-=(const _Vector_base_type<T, N, U>& v) {
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
		friend vector<Base, N> operator+(const _Vector_base& v1, const _Vector_base_type<T, N, U>& v2) {
			return vector<Base, N>((v1[Indices] + v2[Indices])...);
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend vector<Base, N> operator-(const _Vector_base& v1, const _Vector_base_type<T, N, U>& v2) {
			return vector<Base, N>((v1[Indices] - v2[Indices])...);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend vector<Base, N> operator*(const _Vector_base& v, const T& k) {
			return vector<Base, N>((v[Indices] * k)...);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::mul_value>::type>
		friend vector<Base, N> operator*(const T& k, const _Vector_base& v) {
			return vector<Base, N>((k*v[Indices])...);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		friend vector<Base, N> operator/(const _Vector_base& v, const T& k) {
			return vector<Base, N>((v[Indices] / k)...);
		}
	};
	//下に階層が存在するかつBase != T
	template <class Base, imsize_t N, class T, imsize_t... Indices>
	class _Vector_base<Base, N, T, true, index_tuple<Indices...>, false> : public _Vector_base_base_type<Base, N, T> {
		template <class, imsize_t> friend class vector;
		template <class, imsize_t, class, bool, class, bool> friend class _Vector_base;
	public:
		//コンストラクタの継承
		using _Vector_base_base_type<Base, N, T>::_Vector_base;

		constexpr _Vector_base() : _Vector_base_base_type<Base, N, T>() {}
		template <class = typename enable_if<is_inclusion<T, Base>::value>::type>
		constexpr _Vector_base(const typename identity_type<T, Indices>::type&... x) : _Vector_base_base_type<Base, N, T>(static_cast<Base>(x)...) {}
		template <class U, class = typename enable_if<is_inclusion<T, Base>::value>::type>
		constexpr _Vector_base(const _Vector_base_type<T, N, U>& v) : _Vector_base_base_type<Base, N, T>(static_cast<Base>(v.x[Indices])...) {}

		//単項演算の継承
		using _Vector_base_base_type<Base, N, T>::operator+;
		using _Vector_base_base_type<Base, N, T>::operator-;

		//代入演算の継承
		using _Vector_base_base_type<Base, N, T>::operator+=;
		using _Vector_base_base_type<Base, N, T>::operator-=;
		using _Vector_base_base_type<Base, N, T>::operator*=;
		using _Vector_base_base_type<Base, N, T>::operator/=;

		template <class U, class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		_Vector_base& operator+=(const _Vector_base_type<T, N, U>& v) {
			for (imsize_t i = 0; i < N; ++i) this->x[i] += v.x[i];
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		_Vector_base& operator-=(const _Vector_base_type<T, N, U>& v) {
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
		friend vector<Base, N> operator+(const _Vector_base& v1, const _Vector_base_type<T, N, U>& v2) {
			return vector<Base, N>((v1[Indices] + v2[Indices])...);
		}
		template <class U, class = typename enable_if<is_operation<T, Base, Base>::add_value>::type>
		friend vector<Base, N> operator+(const _Vector_base_type<T, N, U>& v1, const _Vector_base& v2) {
			return vector<Base, N>((v1[Indices] + v2[Indices])...);
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend vector<Base, N> operator-(const _Vector_base& v1, const _Vector_base_type<T, N, U>& v2) {
			return vector<Base, N>((v1[Indices] - v2[Indices])...);
		}
		template <class U, class = typename enable_if<is_operation<T, Base, Base>::sub_value>::type>
		friend vector<Base, N> operator-(const _Vector_base_type<T, N, U>& v1, const _Vector_base& v2) {
			return vector<Base, N>((v1[Indices] - v2[Indices])...);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend vector<Base, N> operator*(const _Vector_base& v, const T& k) {
			return vector<Base, N>((v[Indices] * k)...);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::mul_value>::type>
		friend vector<Base, N> operator*(const T& k, const _Vector_base& v) {
			return vector<Base, N>((k*v[Indices])...);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		friend vector<Base, N> operator/(const _Vector_base& v, const T& k) {
			return vector<Base, N>((v[Indices] / k)...);
		}
	};

	//ベクトル型
	template <class T, imsize_t N>
	class vector : public _Vector_base_type<T, N, T>, public value_list_input<array_iterator<T>> {
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
		using _Vector_base_type<T, N, T>::_Vector_base;
		
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
		using _Vector_base_type<T, N, T>::operator-;
		using _Vector_base_type<T, N, T>::operator+;
		//代入演算の継承
		using _Vector_base_type<T, N, T>::operator+=;
		using _Vector_base_type<T, N, T>::operator-=;
		using _Vector_base_type<T, N, T>::operator*=;
		using _Vector_base_type<T, N, T>::operator/=;
		//二項演算の継承
		using value_list_input<array_iterator<T>>::operator<<;

		//代入演算
		vector& operator=(const vector& v) { return *vector_copy(this, v); }
		template <class _T>
		vector& operator=(const vector<_T, N>& v) { return *vector_copy(this, v); }
		vector& operator=(const _Vector_base_type<T, N, T>& v) { return *vector_copy(this, v); }
		template <class U>
		vector& operator=(const _Vector_base_type<T, N, U>& v) { return *vector_copy(this, v); }
		template <class U, class _T, class = typename enable_if<is_inclusion<_T, T>::value>::type>
		vector& operator=(const _Vector_base_type<_T, N, U>& v) { return *vector_copy(this, v); }

		//添え字演算の継承
		using _Vector_base_type<T, N, T>::operator[];

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
	inline typename calculation_result<T1, T2>::mul_type operator*(const _Vector_base_type<T1, N, U1>& v1, const _Vector_base_type<T2, N, U2>& v2) {
		//加法単位元で初期化
		typename calculation_result<T1, T2>::mul_type temp(addition<typename calculation_result<T1, T2>::mul_type>::identity_element());
		for (imsize_t i = 0; i < N; ++i) temp += v1[i] * conj(v2[i]);
		return temp;
	}
	//外積(T1×T2→Sが加法についてループ)
	template <class U1, class U2, class T1, class T2
		, class = typename enable_if<is_calcable<T1, T2>::mul_value && is_loop<typename calculation_result<T1, T2>::mul_type>::add_value>::type>
	inline vector<typename calculation_result<T1, T2>::mul_type, 3> operator%(const _Vector_base_type<T1, 3, U1>& v1, const _Vector_base_type<T2, 3, U2>& v2) {
		return vector<typename calculation_result<T1, T2>::mul_type, 3>(v1[1] * v2[2] - v1[2] * v2[1], v1[2] * v2[0] - v1[0] * v2[2], v1[0] * v2[1] - v1[1] * v2[0]);
	}

	//比較演算
	template <class U1, class U2, class T1, class T2, imsize_t N, class = typename enable_if<is_inclusion<T1, T2>::value || is_inclusion<T2, T1>::value>::type>
	inline bool operator==(const _Vector_base_type<T1, N, U1>& v1, const _Vector_base_type<T2, N, U2>& v2) {
		for (imsize_t i = 0; i < N; ++i) if (v1[i] != v2[i]) return false;
		return true;
	}
	template <class U1, class U2, class T1, class T2, imsize_t N, class = typename enable_if<is_inclusion<T1, T2>::value || is_inclusion<T2, T1>::value>::type>
	inline bool operator!=(const _Vector_base_type<T1, N, U1>& v1, const _Vector_base_type<T2, N, U2>& v2) {
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