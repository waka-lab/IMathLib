#ifndef _IMATH_MATH_AUTOMATIC_DIFFERENTIATION_HPP
#define _IMATH_MATH_AUTOMATIC_DIFFERENTIATION_HPP

#include "IMathLib/utility/utility.hpp"
#include "IMathLib/utility/tuple.hpp"
#include "IMathLib/math/math.hpp"
#include "IMathLib/container/array.hpp"
#include "IMathLib/math/liner_algebra/vector.hpp"

//二重数を利用した自動微分(関数値が不定であるときは結果は保証されない)

/*
namespace iml {

	template <class, imsize_t>
	class diff_dual_numbers;


	//多重構造の演算に対応させるための補助型
	//継承コンストラクタと継承オペレータオーバーロードにより多数定義可能となる
	template <class, imsize_t, class, bool>
	class _Diff_dual_numbers_base;

	//簡略にかくためのエイリアス
	template <class Base, imsize_t N, class T>
	using _Diff_dual_numbers_base_base_type = _Diff_dual_numbers_base<Base, N, typename T::algebraic_type
		, is_algebraic_structure<typename T::algebraic_type>::value>;
	template <class Base, imsize_t N, class T>
	using _Diff_dual_numbers_base_type = _Diff_dual_numbers_base<Base, N, T
		, is_algebraic_structure<T>::value>;

	//下に階層が存在しない
	template <class Base, imsize_t N, class T>
	class _Diff_dual_numbers_base<Base, N, T, false> {
		template <class> friend class diff_dual_numbers;
		template <class, class, bool> friend class _Diff_dual_numbers_base;
	protected:
		Base x[N];
	public:
		constexpr _Diff_dual_numbers_base() : x{} {}
		template <class _T, class = typename enable_if<is_inclusion<_T, Base>::value>::type>
		constexpr _Diff_dual_numbers_base(const _T& x1, const _T& x2) : x{ static_cast<Base>(x1), static_cast<Base>(x2) } {}
		template <class _T, class = typename enable_if<is_inclusion<_T, Base>::value>::type>
		constexpr _Diff_dual_numbers_base(const _T& re) : x{ static_cast<Base>(re) } {}

		template <class = typename enable_if<is_exist_add_inverse_element<T>::value>::type>
		_Diff_dual_numbers_base operator-() const { return _Diff_dual_numbers_base(-this->x[0], -this->x[1]); }
		_Diff_dual_numbers_base operator+() const { return _Diff_dual_numbers_base(*this); }

		template <class U, class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		_Diff_dual_numbers_base& operator+=(const _Diff_dual_numbers_base_type<T, U>& n) {
			this->x[0] += n.x[0]; this->x[1] += n.x[1];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		_Diff_dual_numbers_base& operator+=(const T& n) {
			this->x[0] += n;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		_Diff_dual_numbers_base& operator-=(const _Diff_dual_numbers_base_type<T, U>& n) {
			this->x[0] -= n.x[0]; this->x[1] -= n.x[1];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		_Diff_dual_numbers_base& operator-=(const T& n) {
			this->x[0] -= n;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		_Diff_dual_numbers_base& operator*=(const _Diff_dual_numbers_base_type<T, U>& n) {
			Base temp[2] = { this->x[0] * n.x[0], this->x[0] * n.x[1] + this->x[1] * n.x[0] };
			this->x[0] = temp[0]; this->x[1] = temp[1];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		_Diff_dual_numbers_base& operator*=(const T& k) {
			this->x[0] *= k; this->x[1] *= k;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		_Diff_dual_numbers_base& operator/=(const _Diff_dual_numbers_base_type<T, U>& n) {
			Base temp[2] = { this->x[0] / n.x[0], (x[1] * n.x[0] - x[0] * n.x[1]) / (n.x[0] * n.x[0]) };
			this->x[0] = temp[0]; this->x[1] = temp[1];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		_Diff_dual_numbers_base& operator/=(const T& k) {
			this->x[0] /= k; this->x[1] /= k;
			return *this;
		}

		//アクセサ
		const Base& operator[](imsize_t index) const { return this->x[index]; }
		Base& operator[](imsize_t index) { return this->x[index]; }


		//二項演算
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend diff_dual_numbers<Base> operator+(const _Diff_dual_numbers_base& c1, const _Diff_dual_numbers_base_type<T, U>& c2) {
			return diff_dual_numbers<Base>(c1[0] + c2[0], c1[1] + c2[1]);
		}
		template <class U, class = typename enable_if<is_operation<T, Base, Base>::add_value && !is_same<Base, T>::value>::type>
		friend diff_dual_numbers<Base> operator+(const _Diff_dual_numbers_base_type<T, U>& c1, const _Diff_dual_numbers_base& c2) {
			return diff_dual_numbers<Base>(c1[0] + c2[0], c1[1] + c2[1]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend diff_dual_numbers<Base> operator+(const _Diff_dual_numbers_base& c, const T& n) {
			return diff_dual_numbers<Base>(c[0] + n, c[1]);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::add_value && !is_same<Base, T>::value>::type>
		friend diff_dual_numbers<Base> operator+(const T& n, const _Diff_dual_numbers_base& c) {
			return diff_dual_numbers<Base>(n + c[0], c[1]);
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend diff_dual_numbers<Base> operator-(const _Diff_dual_numbers_base& c1, const _Diff_dual_numbers_base_type<T, U>& c2) {
			return diff_dual_numbers<Base>(c1[0] - c2[0], c1[1] - c2[1]);
		}
		template <class U, class = typename enable_if<is_operation<T, Base, Base>::sub_value && !is_same<Base, T>::value>::type>
		friend diff_dual_numbers<Base> operator-(const _Diff_dual_numbers_base_type<T, U>& c1, const _Diff_dual_numbers_base& c2) {
			return diff_dual_numbers<Base>(c1[0] - c2[0], c1[1] - c2[1]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend diff_dual_numbers<Base> operator-(const _Diff_dual_numbers_base& c, const T& n) {
			return diff_dual_numbers<Base>(c[0] - n, c[1]);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::sub_value && !is_same<Base, T>::value>::type>
		friend diff_dual_numbers<Base> operator-(const T& n, const _Diff_dual_numbers_base& c) {
			return diff_dual_numbers<Base>(n - c[0], -c[1]);
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend diff_dual_numbers<Base> operator*(const _Diff_dual_numbers_base& c1, const _Diff_dual_numbers_base_type<T, U>& c2) {
			return diff_dual_numbers<Base>(c1[0] * c2[0], c1[0] * c2[1] + c1[1] * c2[0]);
		}
		template <class U, class = typename enable_if<is_operation<T, Base, Base>::mul_value && !is_same<Base, T>::value>::type>
		friend diff_dual_numbers<Base> operator*(const _Diff_dual_numbers_base_type<T, U>& c1, const _Diff_dual_numbers_base& c2) {
			return diff_dual_numbers<Base>(c1[0] * c2[0], c1[0] * c2[1] + c1[1] * c2[0]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend diff_dual_numbers<Base> operator*(const _Diff_dual_numbers_base& c, const T& k) {
			return diff_dual_numbers<Base>(c[0] * k, c[1] * k);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::mul_value>::type>
		friend diff_dual_numbers<Base> operator*(const T& k, const _Diff_dual_numbers_base& c) {
			return diff_dual_numbers<Base>(k * c[0], k * c[1]);
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		friend diff_dual_numbers<Base> operator/(const _Diff_dual_numbers_base& c1, const _Diff_dual_numbers_base_type<T, U>& c2) {
			Base temp = c2[0] * c2[0] + c2[1] * c2[1];
			return diff_dual_numbers<Base>((c1[0] * c2[0] + c1[1] * c2[1]) / temp
				, (c1[1] * c2[0] - c1[0] * c2[1]) / temp);
		}
		template <class U, class = typename enable_if<is_operation<T, Base, Base>::div_value && !is_same<Base, T>::value>::type>
		friend diff_dual_numbers<Base> operator/(const _Diff_dual_numbers_base_type<T, U>& c1, const _Diff_dual_numbers_base& c2) {
			return diff_dual_numbers<Base>(c1[0] / c2[0], (c1[1] * c2[0] - c1[0] * c2[1]) / (c2[0] * c2[0]));
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		friend diff_dual_numbers<Base> operator/(const _Diff_dual_numbers_base& c, const T& k) {
			return diff_dual_numbers<Base>(c[0] / k, c[1] / k);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::div_value>::type>
		friend diff_dual_numbers<Base> operator/(const T& k, const _Diff_dual_numbers_base& c) {
			return diff_dual_numbers<Base>(k / c[0], -k * c[1] / (c[0] * c[0]));
		}
	};
	//下に階層が存在する
	template <class Base, class T>
	class _Diff_dual_numbers_base<Base, T, true> : public _Diff_dual_numbers_base_base_type<Base, T> {
		template <class> friend class diff_dual_numbers;
		template <class, class, bool> friend class _Diff_dual_numbers_base;
	public:
		//コンストラクタの継承
		using _Diff_dual_numbers_base_base_type<Base, T>::_Diff_dual_numbers_base;

		//単項演算の継承
		using _Diff_dual_numbers_base_base_type<Base, T>::operator+;
		using _Diff_dual_numbers_base_base_type<Base, T>::operator-;

		//代入演算の継承
		using _Diff_dual_numbers_base_base_type<Base, T>::operator+=;
		using _Diff_dual_numbers_base_base_type<Base, T>::operator-=;
		using _Diff_dual_numbers_base_base_type<Base, T>::operator*=;
		using _Diff_dual_numbers_base_base_type<Base, T>::operator/=;

		template <class U, class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		_Diff_dual_numbers_base& operator+=(const _Diff_dual_numbers_base_type<T, U>& n) {
			this->x[0] += n.x[0]; this->x[1] += n.x[1];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		_Diff_dual_numbers_base& operator+=(const T& n) {
			this->x[0] += n;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		_Diff_dual_numbers_base& operator-=(const _Diff_dual_numbers_base_type<T, U>& n) {
			this->x[0] -= n.x[0]; this->x[1] -= n.x[1];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		_Diff_dual_numbers_base& operator-=(const T& n) {
			this->x[0] -= n;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		_Diff_dual_numbers_base& operator*=(const _Diff_dual_numbers_base_type<T, U>& n) {
			Base temp[2] = { this->x[0] * n.x[0], this->x[0] * n.x[1] + this->x[1] * n.x[0] };
			this->x[0] = temp[0]; this->x[1] = temp[1];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		_Diff_dual_numbers_base& operator*=(const T& k) {
			this->x[0] *= k; this->x[1] *= k;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		_Diff_dual_numbers_base& operator/=(const _Diff_dual_numbers_base_type<T, U>& n) {
			Base temp[2] = { this->x[0] / n.x[0], (x[1] * n.x[0] - x[0] * n.x[1]) / (n.x[0] * n.x[0]) };
			this->x[0] = temp[0]; this->x[1] = temp[1];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		_Diff_dual_numbers_base& operator/=(const T& k) {
			this->x[0] /= k; this->x[1] /= k;
			return *this;
		}

		//添え字演算の継承
		using _Diff_dual_numbers_base_base_type<Base, T>::operator[];


		//二項演算
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend diff_dual_numbers<Base> operator+(const _Diff_dual_numbers_base& c1, const _Diff_dual_numbers_base_type<T, U>& c2) {
			return diff_dual_numbers<Base>(c1[0] + c2[0], c1[1] + c2[1]);
		}
		template <class U, class = typename enable_if<is_operation<T, Base, Base>::add_value && !is_same<Base, T>::value>::type>
		friend diff_dual_numbers<Base> operator+(const _Diff_dual_numbers_base_type<T, U>& c1, const _Diff_dual_numbers_base& c2) {
			return diff_dual_numbers<Base>(c1[0] + c2[0], c1[1] + c2[1]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend diff_dual_numbers<Base> operator+(const _Diff_dual_numbers_base& c, const T& n) {
			return diff_dual_numbers<Base>(c[0] + n, c[1]);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::add_value && !is_same<Base, T>::value>::type>
		friend diff_dual_numbers<Base> operator+(const T& n, const _Diff_dual_numbers_base& c) {
			return diff_dual_numbers<Base>(n + c[0], c[1]);
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend diff_dual_numbers<Base> operator-(const _Diff_dual_numbers_base& c1, const _Diff_dual_numbers_base_type<T, U>& c2) {
			return diff_dual_numbers<Base>(c1[0] - c2[0], c1[1] - c2[1]);
		}
		template <class U, class = typename enable_if<is_operation<T, Base, Base>::sub_value && !is_same<Base, T>::value>::type>
		friend diff_dual_numbers<Base> operator-(const _Diff_dual_numbers_base_type<T, U>& c1, const _Diff_dual_numbers_base& c2) {
			return diff_dual_numbers<Base>(c1[0] - c2[0], c1[1] - c2[1]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend diff_dual_numbers<Base> operator-(const _Diff_dual_numbers_base& c, const T& n) {
			return diff_dual_numbers<Base>(c[0] - n, c[1]);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::sub_value && !is_same<Base, T>::value>::type>
		friend diff_dual_numbers<Base> operator-(const T& n, const _Diff_dual_numbers_base& c) {
			return diff_dual_numbers<Base>(n - c[0], -c[1]);
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend diff_dual_numbers<Base> operator*(const _Diff_dual_numbers_base& c1, const _Diff_dual_numbers_base_type<T, U>& c2) {
			return diff_dual_numbers<Base>(c1[0] * c2[0], c1[0] * c2[1] + c1[1] * c2[0]);
		}
		template <class U, class = typename enable_if<is_operation<T, Base, Base>::mul_value && !is_same<Base, T>::value>::type>
		friend diff_dual_numbers<Base> operator*(const _Diff_dual_numbers_base_type<T, U>& c1, const _Diff_dual_numbers_base& c2) {
			return diff_dual_numbers<Base>(c1[0] * c2[0], c1[0] * c2[1] + c1[1] * c2[0]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend diff_dual_numbers<Base> operator*(const _Diff_dual_numbers_base& c, const T& k) {
			return diff_dual_numbers<Base>(c[0] * k, c[1] * k);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::mul_value>::type>
		friend diff_dual_numbers<Base> operator*(const T& k, const _Diff_dual_numbers_base& c) {
			return diff_dual_numbers<Base>(k * c[0], k * c[1]);
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		friend diff_dual_numbers<Base> operator/(const _Diff_dual_numbers_base& c1, const _Diff_dual_numbers_base_type<T, U>& c2) {
			Base temp = c2[0] * c2[0] + c2[1] * c2[1];
			return diff_dual_numbers<Base>((c1[0] * c2[0] + c1[1] * c2[1]) / temp
				, (c1[1] * c2[0] - c1[0] * c2[1]) / temp);
		}
		template <class U, class = typename enable_if<is_operation<T, Base, Base>::div_value && !is_same<Base, T>::value>::type>
		friend diff_dual_numbers<Base> operator/(const _Diff_dual_numbers_base_type<T, U>& c1, const _Diff_dual_numbers_base& c2) {
			return diff_dual_numbers<Base>(c1[0] / c2[0], (c1[1] * c2[0] - c1[0] * c2[1]) / (c2[0] * c2[0]));
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		friend diff_dual_numbers<Base> operator/(const _Diff_dual_numbers_base& c, const T& k) {
			return diff_dual_numbers<Base>(c[0] / k, c[1] / k);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::div_value>::type>
		friend diff_dual_numbers<Base> operator/(const T& k, const _Diff_dual_numbers_base& c) {
			return diff_dual_numbers<Base>(k / c[0], -k * c[1] / (c[0] * c[0]));
		}
	};

	//自動微分のための暗黙的に除算が定義された二重数型
	template <class T>
	class diff_dual_numbers : public _Diff_dual_numbers_base_type<T, T>, public value_list_input<array_iterator<T>> {
		template <class> friend class diff_dual_numbers;

		//代入演算の補助
		template <class _T>
		static diff_dual_numbers* dual_numbers_copy(diff_dual_numbers* const n1, const diff_dual_numbers<_T>& n2) {
			//同じインスタンスでなければ代入
			if (static_cast<void*>(&n1->x[0]) != static_cast<void*>(const_cast<_T*>(&n2.x[0]))) {
				n1->x[0] = static_cast<T>(n2.x[0]); n1->x[1] = static_cast<T>(n2.x[1]);
			}
			return n1;
		}
	public:
		//コンストラクタの継承
		using _Diff_dual_numbers_base_type<T, T>::_Diff_dual_numbers_base_type;

		constexpr diff_dual_numbers() : _Diff_dual_numbers_base_type<T, T>() {}
		constexpr diff_dual_numbers(const diff_dual_numbers& n) : _Diff_dual_numbers_base_type<T, T>(n.x[0], n.x[1]) {}
		template <class _T, class = typename enable_if<is_inclusion<_T, T>::value>::type>
		constexpr diff_dual_numbers(const diff_dual_numbers<_T>& n) : _Diff_dual_numbers_base_type<T, T>(n.x[0], n.x[1]) {}
		template <class U>
		constexpr diff_dual_numbers(const _Diff_dual_numbers_base_type<T, U>& n)
			: _Diff_dual_numbers_base_type<T, T>(n.x[0], n.x[1]) {}
		template <class U, class _T, class = typename enable_if<is_inclusion<_T, T>::value>::type>
		constexpr diff_dual_numbers(const _Diff_dual_numbers_base_type<_T, U>& n)
			: _Diff_dual_numbers_base_type<T, T>(n.x[0], n.x[1]) {}

		using algebraic_type = T;
		using iterator = array_iterator<T>;
		using const_iterator = array_iterator<const T>;

		template<class Other>
		struct rebind {
			using other = diff_dual_numbers<Other>;
		};

		iterator begin() noexcept { return iterator(&x[0]); }
		const_iterator begin() const noexcept { return const_iterator(&x[0]); }
		iterator end() noexcept { return iterator(&x[1] + 1); }
		const_iterator end() const noexcept { return const_iterator(&x[1] + 1); }

		//単項演算の継承
		using _Diff_dual_numbers_base_type<T, T>::operator-;
		using _Diff_dual_numbers_base_type<T, T>::operator+;
		//代入演算の継承
		using _Diff_dual_numbers_base_type<T, T>::operator+=;
		using _Diff_dual_numbers_base_type<T, T>::operator-=;
		using _Diff_dual_numbers_base_type<T, T>::operator*=;
		using _Diff_dual_numbers_base_type<T, T>::operator/=;
		//二項演算の継承
		using value_list_input<array_iterator<T>>::operator<<;

		//代入演算
		template <class U>
		diff_dual_numbers& operator=(const _Diff_dual_numbers_base_type<T, U>& n) { return *dual_numbers_copy(this, n); }
		template <class U, class _T, class = typename enable_if<is_inclusion<_T, T>::value>::type>
		diff_dual_numbers& operator=(const _Diff_dual_numbers_base_type<_T, U>& n) { return *dual_numbers_copy(this, n); }

		//添え字演算の継承
		using _Diff_dual_numbers_base_type<T, T>::operator[];

		//メンバ取得
		template <class S, class = typename enable_if<is_inclusion<S, T>::value>::type>
		void real(const S& re) { this->x[0] = re; }
		constexpr T real() const { return this->x[0]; }
		template <class S, class = typename enable_if<is_inclusion<S, T>::value>::type>
		void imag(const S& im) { this->x[1] = im; }
		constexpr T imag() const { return this->x[1]; }

		//ストリーム出力
		friend std::ostream& operator<<(std::ostream& os, const diff_dual_numbers& n) {
			os << '(' << n.x[0] << ',' << n.x[1] << ')';
			return os;
		}
		friend std::wostream& operator<<(std::wostream& os, const diff_dual_numbers& n) {
			os << L'(' << n.x[0] << L',' << n.x[1] << L')';
			return os;
		}
	};


	//二重数の判定
	template <class T>
	struct _Is_diff_dual_numbers : false_type {};
	template <class T>
	struct _Is_diff_dual_numbers<diff_dual_numbers<T>> : true_type {};
	template <class T>
	struct is_diff_dual_numbers : _Is_diff_dual_numbers<typename remove_cv<T>::type> {};

	//二重数の除去
	template <class T>
	struct remove_diff_dual_numbers {
		using type = T;
	};
	template <class T>
	struct remove_diff_dual_numbers<diff_dual_numbers<T>> {
		using type = T;
	};

	//全ての二重数の除去
	template <class T>
	struct remove_all_diff_dual_numbers {
		using type = T;
	};
	template <class T>
	struct remove_all_diff_dual_numbers<diff_dual_numbers<T>> : remove_all_diff_dual_numbers<T> {};


	//比較演算
	template <class U1, class U2, class T1, class T2, imsize_t N, class = typename enable_if<is_inclusion<T1, T2>::value || is_inclusion<T2, T1>::value>::type>
	inline bool operator==(const _Diff_dual_numbers_base_type<T1, U1>& n1, const _Diff_dual_numbers_base_type<T2, U2>& n2) {
		return (n1[0] == n2[0]) && (n1[1] == n2[1]);
	}
	template <class U1, class U2, class T1, class T2, imsize_t N, class = typename enable_if<is_inclusion<T1, T2>::value || is_inclusion<T2, T1>::value>::type>
	inline bool operator!=(const _Diff_dual_numbers_base_type<T1, U1>& n1, const _Diff_dual_numbers_base_type<T2, U2>& n2) {
		return !(n1 == n2);
	}

}
namespace iml {
	//加法パラメータ取得
	template <class T>
	struct addition<diff_dual_numbers<T>> {
		//単位元の取得
		static diff_dual_numbers<T> identity_element() {
			return diff_dual_numbers<T>();
		}
		//逆元の取得
		static diff_dual_numbers<T> inverse_element(const diff_dual_numbers<T>& x) {
			return -x;
		}
	};
	//乗法パラメータ取得
	template <class T>
	struct multiplicative<diff_dual_numbers<T>> {
		//単位元の取得
		static diff_dual_numbers<T> identity_element() {
			return diff_dual_numbers<T>(multiplicative<T>::identity_element());
		}
		//逆元の取得
		static diff_dual_numbers<T> inverse_element(const diff_dual_numbers<T>& x) {
			//共役を絶対値の二乗で割る
			return diff_dual_numbers<T>(1 / x[0], -x[1] / (x[0] * x[0]));
		}
		//吸収元
		static diff_dual_numbers<T> absorbing_element() {
			return diff_dual_numbers<T>();
		}
	};
}




namespace iml {

	//自動微分のための補助型
	template <class T, imsize_t M, imsize_t N, bool flag = !(M < N)>
	struct _Diff_type {
		using type = diff_dual_numbers<T>;
	};
	template <class T, imsize_t M, imsize_t N>
	struct _Diff_type<T, M, N, false> :_Diff_type<diff_dual_numbers<T>, M + 1, N> {};
	template <class T, imsize_t N>
	struct diff_type :_Diff_type<T, 0, N - 1> {};
	template <class T>
	struct diff_type<T, 0> { using type = T; };

	//拡張二項演算
	template <class T, imsize_t N>
	inline typename diff_type<T, N>::type operator+(const T& k, const typename diff_type<T, N>::type& d) {
		typename diff_type<T, N>::type temp(k*multiplicative<typename diff_type<T, N>::type>::identity_element());
		return temp + d;
	}
	template <class T, imsize_t N>
	inline typename diff_type<T, N>::type operator+(const typename diff_type<T, N>::type& d, const T& k) {
		typename diff_type<T, N>::type temp(k*multiplicative<typename diff_type<T, N>::type>::identity_element());
		return d + temp;
	}
	template <class T, imsize_t N>
	inline typename diff_type<T, N>::type operator-(const T& k, const typename diff_type<T, N>::type& d) {
		typename diff_type<T, N>::type temp(k*multiplicative<typename diff_type<T, N>::type>::identity_element());
		return temp - d;
	}
	template <class T, imsize_t N>
	inline typename diff_type<T, N>::type operator-(const typename diff_type<T, N>::type& d, const T& k) {
		typename diff_type<T, N>::type temp(k*multiplicative<typename diff_type<T, N>::type>::identity_element());
		return d - temp;
	}

	//二重数を多重化した分だけ微分可能な一変数関数の自動微分(D階微分までをサポートする)
	template <class T, imsize_t D>
	class automatic_differentiation {
		function<typename diff_type<T, D>::type(typename diff_type<T, D>::type)> f;

		//引数用に変換する
		template <imsize_t N>
		struct argument_struct {
			static typename diff_type<T, N>::type __argument(const T& x) {
				return typename diff_type<T, N>::type(argument_struct<N - 1>::__argument(x), multiplicative<typename diff_type<T, N-1>::type>::identity_element());
			}
		};
		template <>
		struct argument_struct<0> {
			static typename diff_type<T, 0>::type __argument(const T& x) {
				return x;
			}
		};
		typename diff_type<T, D>::type __argument(const T& x) {
			return argument_struct<D>::__argument(x);
		}
		//N階微分の数値の探索
		template <class S, imsize_t M, imsize_t N, bool flag1 = !(M < N), bool flag2 = !(M < D)>
		struct diff_type_find {
			static S __find(const S& x) {
				return x;
			}
		};
		template <class S, imsize_t M, imsize_t N>
		struct diff_type_find<S, M, N, true, false> {
			static T __find(const S& x) {
				return diff_type_find<typename S::algebraic_type, M + 1, N>::__find(x.real());
			}
		};
		template <class S, imsize_t M, imsize_t N>
		struct diff_type_find<S, M, N, false, false> {
			static T __find(const S& x) {
				return diff_type_find<typename S::algebraic_type, M + 1, N>::__find(x.imag());
			}
		};
		template <imsize_t N>
		static T find(const typename diff_type<T, D>::type& x) {
			return diff_type_find<typename diff_type<T, D>::type, 0, N>::__find(x);
		}
	public:
		constexpr automatic_differentiation() {}
		template <class F>
		constexpr automatic_differentiation(F f) :f(f) {}

		//関数のセット
		template <class F>
		void set(F f) { this->f = f; }

		//微分回数を指定した微分係数の取得
		template <imsize_t N>
		T get(const T& x) {
			static_assert(N <= D, "N is too big.");
			return find<N>(f(__argument(x)));			//目的の位置の探索
		}

		//保持している関数の取得
		auto get_function() { return f; }
	};

	//自動偏微分のための補助型(多変数関数の一変数に対する偏微分)
	template <class T, imsize_t D>
	class _Automatic_partial_differentiation;
	template <class R, class... Args, imsize_t D>
	class _Automatic_partial_differentiation<R(Args...), D> {
		function<typename diff_type<R, D>::type(typename diff_type<Args, D>::type...)> f;

		//引数用に変換する
		template <class T, imsize_t N>
		struct argument_struct {
			static typename diff_type<T, N>::type __argument(const T& x) {
				return typename diff_type<T, N>::type(argument_struct<T, N - 1>::__argument(x), multiplicative<typename diff_type<T, N-1>::type>::identity_element());
			}
		};
		template <class T>
		struct argument_struct<T, 0> {
			static typename diff_type<T, 0>::type __argument(const T& x) {
				return x;
			}
		};
		template <class T>
		typename diff_type<T, D>::type __argument(const T& x) {
			return argument_struct<T, D>::__argument(x);
		}
		//引数を単位元のx倍する
		template <class T>
		static typename diff_type<T, D>::type diff_identity(const T& x) {
			return multiplicative<typename diff_type<T, D>::type>::identity_element()*x;
		}
		//N階微分の数値の探索
		template <class S, imsize_t M, imsize_t N, bool flag1 = !(M < N), bool flag2 = !(M < D)>
		struct diff_type_find {
			static S __find(const S& x) {
				return x;
			}
		};
		template <class S, imsize_t M, imsize_t N>
		struct diff_type_find<S, M, N, true, false> {
			static R __find(const S& x) {
				return diff_type_find<typename S::algebraic_type, M + 1, N>::__find(x.real());
			}
		};
		template <class S, imsize_t M, imsize_t N>
		struct diff_type_find<S, M, N, false, false> {
			static R __find(const S& x) {
				return diff_type_find<typename S::algebraic_type, M + 1, N>::__find(x.imag());
			}
		};
		template <imsize_t N>
		static R find(const typename diff_type<R, D>::type& x) {
			return diff_type_find<typename diff_type<R, D>::type, 0, N>::__find(x);
		}
	public:
		constexpr _Automatic_partial_differentiation() {}
		template <class F>
		constexpr _Automatic_partial_differentiation(F f) :f(f) {}

		//関数のセット
		template <class F>
		void set(F f) { this->f = f; }

		//微分位置Aを指定しでN階微分係数の取得
		template <imsize_t A, imsize_t N>
		R get(Args... args) {
			static_assert(N <= D, "N is too big.");
			tuple<typename diff_type<Args, D>::type...> temp(make_tuple(diff_identity(args)...));
			temp.get<A>() = __argument(get_argument<A>(args...));
			return find<N>(apply(f, temp));
		}

		//保持している関数の取得
		auto get_function() { return f; }
	};

	//自動偏微分のためのクラス
	template <class T, imsize_t D>
	class automatic_partial_differentiation;
	template <class R, class... Args, imsize_t D>
	class automatic_partial_differentiation<R(Args...), D>{
		function<typename diff_type<R, D>::type(typename diff_type<Args, D>::type...)> f;

		//make_differentiation_structのための補助型(テンプレート引数リストの数でエラーがでるため、継承させた構造体を用いる)
		//template <imsize_t N>
		//using make_differentiation_type = _Automatic_partial_differentiation<typename diff_type<R, D - N>::type(typename diff_type<Args, D - N>::type...), N>;
		//Size:微分回数,N:微分の現在の順番
		template <imsize_t Size, imsize_t N>
		struct make_differentiation_type :public _Automatic_partial_differentiation<typename diff_type<R, D - Size + N>::type(typename diff_type<Args, D - Size + N>::type...), 1> {
			using _Automatic_partial_differentiation<typename diff_type<R, D - Size + N>::type(typename diff_type<Args, D - Size + N>::type...), 1>::_Automatic_partial_differentiation;
		};
		//微分順序に合わせた変数の生成
		template <class T, imsize_t... N>
		class make_differentiation_struct;
		template <imsize_t... Indices, imsize_t... Orders>
		class make_differentiation_struct<index_tuple<Indices...>, Orders...> :public make_differentiation_type<sizeof...(Indices), Indices>... {
			
			//最後に目的の型に変換する関数(0階微分)
			_Automatic_partial_differentiation<R(Args...), D - sizeof...(Indices)> f;

			//引数を単位元のx倍する
			template <imsize_t N, class T>
			static typename diff_type<T, D - sizeof...(Indices)+N>::type diff_identity(T x) {
				return multiplicative<typename diff_type<T, D - sizeof...(Indices)+N>::type>::identity_element()*x;
			}
			//長いから短くする(基底クラスとの名前の衝突を回避のため「__」を付ける)
			template <class T, imsize_t N>
			using __type = typename diff_type<T, D - sizeof...(Indices)+N>::type;

			//関数の包容関係の構築
			template <imsize_t N, imsize_t First, imsize_t... Rest>
			struct function_inclusion_struct {
				static void __function_inclusion(make_differentiation_struct<index_tuple<Indices...>, Orders...>& mds) {
					mds.get_diff<N>().set([&](__type<Args, N + 1>... args) {return mds.get_diff<N + 1>().get<First, 1>(iml::forward<__type<Args, N + 1>>(args)...); });
					function_inclusion_struct<N - 1, Rest...>::__function_inclusion(mds);
				}
			};
			template <imsize_t First, imsize_t Second>
			struct function_inclusion_struct<0, First, Second> {
				static void __function_inclusion(make_differentiation_struct<index_tuple<Indices...>, Orders...>& mds) {
					mds.get_diff<0>().set([&](__type<Args, 1>... args) {return mds.get_diff<1>().get<First, 1>(iml::forward<__type<Args, 1>>(args)...); });
					//一番最初についての設定
					mds.f = [&](__type<Args, 0>... args) {return mds.get_diff<0>().get<Second, 1>(iml::forward<__type<Args, 0>>(args)...); };
				}
			};

			//任意の位置の変数の取得
			template <imsize_t N>
			static make_differentiation_type<sizeof...(Indices), N>& get_diff_impl(make_differentiation_type<sizeof...(Indices), N>& t) { return t; }
			template <imsize_t N>
			make_differentiation_type<sizeof...(Indices), N>& get_diff() { return get_diff_impl<N>(*this); }
			
		public:
			make_differentiation_struct(function<typename diff_type<R, D>::type(typename diff_type<Args, D>::type...)>& f) 
				//一番浅い微分のオーダーに対しては関数を指定する
				:make_differentiation_type<sizeof...(Indices), sizeof...(Indices)-1>(f) {
				//関数の包容関係の設定(一番最後のはすでに設定済みのため次)
				function_inclusion_struct<sizeof...(Indices)-2, Orders...>::__function_inclusion(*this);
			}

			//偏微分係数の取得
			R get(Args... args) {
				return f.get<0, 0>(iml::forward<Args>(args)...);
			}
		};
		//一階微分のとき
		template <imsize_t Order>
		class make_differentiation_struct<index_tuple<0>, Order> {
			_Automatic_partial_differentiation<R(Args...), D> f;
		public:
			constexpr make_differentiation_struct(function<typename diff_type<R, D>::type(typename diff_type<Args, D>::type...)>& f) :f(f) {}

			//偏微分係数の取得
			R get(Args... args) {
				return f.get<Order, 1>(iml::forward<Args>(args)...);
			}
		};
		//微分をしないとき
		template <>
		class make_differentiation_struct<index_tuple<>> {
			_Automatic_partial_differentiation<R(Args...), D> f;
		public:
			constexpr make_differentiation_struct(function<typename diff_type<R, D>::type(typename diff_type<Args, D>::type...)>& f) :f(f) {}

			//偏微分係数の取得
			R get(Args... args) {
				return f.get<0, 0>(iml::forward<Args>(args)...);
			}
		};
		//オーダーを逆順にする
		template <class T, imsize_t... Orders>
		struct order_inverse;
		template <imsize_t... Inv, imsize_t FOrder,imsize_t... Orders>
		struct order_inverse<index_tuple<Inv...>, FOrder, Orders...>
			:public order_inverse<index_tuple<FOrder, Inv...>, Orders...> {};
		template <imsize_t... Inv>
		struct order_inverse<index_tuple<Inv...>> {
			using type = index_tuple<Inv...>;
		};
		//index_tupleの要素を逆順にする
		template <class T>
		struct index_tuple_inverse;
		template <imsize_t... Orders>
		struct index_tuple_inverse<index_tuple<Orders...>> {
			using type = typename order_inverse<index_tuple<>, Orders...>::type;
		};
	public:
		constexpr automatic_partial_differentiation() {}
		template <class F>
		constexpr automatic_partial_differentiation(F f) :f(f) {}

		//任意の微分順序の値の取得(Ordersの仕様は逆順を導入することで「1階,2階,...N-1階,N階」となる)
		template <imsize_t... Orders>
		R get(Args... args) {
			make_differentiation_struct<typename index_tuple_inverse<typename index_range<0, sizeof...(Orders)>::type>::type, Orders...> diff_struct(f);
			return diff_struct.get(iml::forward<Args>(args)...);
		}

		//保持している関数の取得
		auto get_function() { return f; }
	};

}

namespace iml {

	//関数型の戻り値型の取得
	template <class R, class... Types, imsize_t D>
	struct function_result<automatic_differentiation<R(Types...), D>> { using type = R; };
	template <class R, class... Types, imsize_t D>
	struct function_result<_Automatic_partial_differentiation<R(Types...), D>> { using type = R; };
	template <class R, class... Types, imsize_t D>
	struct function_result<automatic_partial_differentiation<R(Types...), D>> { using type = R; };
	//関数型の任意の位置の引数型の取得
	template <imsize_t N, class R, class... Types, imsize_t D>
	struct function_args_type<N, automatic_differentiation<R(Types...), D>> { using type = typename at_type<N, Types...>::type; };
	template <imsize_t N, class R, class... Types, imsize_t D>
	struct function_args_type<N, _Automatic_partial_differentiation<R(Types...), D>> { using type = typename at_type<N, Types...>::type; };
	template <imsize_t N, class R, class... Types, imsize_t D>
	struct function_args_type<N, automatic_partial_differentiation<R(Types...), D>> { using type = typename at_type<N, Types...>::type; };

}

namespace iml {

	//勾配ベクトル(任意のスカラー場φ(x1,x2,x3,...)=c)
	template <class T>
	class gradient_vector;
	template <class R, class... Args>
	class gradient_vector<R(Args...)> {
		_Automatic_partial_differentiation<R(Args...), 1> grad;

		//連番リストからgradient_vectorの作成
		template <class T>
		struct make_gradient_vector_struct;
		template <imsize_t... Indices>
		struct make_gradient_vector_struct<index_tuple<Indices...>> {
			static vector<R, sizeof...(Args)> __make_gradient_vector(_Automatic_partial_differentiation<R(Args...), 1>& grad, Args... args) {
				return vector<R, sizeof...(Args)>(grad.get<Indices, 1>(iml::forward<Args>(args)...)...);
			}
		};
	public:
		constexpr gradient_vector() {}
		template <class F>
		constexpr gradient_vector(F f) :grad(f) {}

		template <class F>
		void set(F f) { grad = f; }

		vector<R, sizeof...(Args)> operator()(Args... args) {
			return make_gradient_vector_struct<typename index_range<0, sizeof...(Args)>::type>::__make_gradient_vector(grad, iml::forward<Args>(args)...);
		}
	};

}
*/

#endif