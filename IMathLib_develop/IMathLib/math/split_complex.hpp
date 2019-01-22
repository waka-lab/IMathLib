﻿#ifndef _IMATH_MATH_SPLIT_COMPLEX_HPP
#define _IMATH_MATH_SPLIT_COMPLEX_HPP

#include "IMathLib/utility/utility.hpp"
#include "IMathLib/utility/tuple.hpp"
#include "IMathLib/math/math.hpp"
#include "IMathLib/container/array.hpp"


namespace iml {

	template <class>
	class split_complex;


	//多重構造の演算に対応させるための補助型
	//継承コンストラクタと継承オペレータオーバーロードにより多数定義可能となる
	template <class, class, bool, bool>
	class _Split_complex_base;

	//簡略にかくためのエイリアス
	template <class Base, class T>
	using _Split_complex_base_base_type = _Split_complex_base<Base, typename T::algebraic_type
		, is_algebraic_structure<typename T::algebraic_type>::value, is_same<Base, T>::value>;
	template <class Base, class T>
	using _Split_complex_base_type = _Split_complex_base<Base, T
		, is_algebraic_structure<T>::value, is_same<Base, T>::value>;

	//下に階層が存在しないかつBase == T
	template <class Base, class T>
	class _Split_complex_base<Base, T, false, true> {
		template <class> friend class split_complex;
		template <class, class, bool, bool> friend class _Split_complex_base;
	protected:
		Base x[2];
	public:
		constexpr _Split_complex_base() : x{} {}
		template <class _T, class = typename enable_if<is_inclusion<_T, Base>::value>::type>
		constexpr _Split_complex_base(const _T& x1, const _T& x2) : x{ static_cast<Base>(x1),static_cast<Base>(x2) } {}
		template <class _T, class = typename enable_if<is_inclusion<_T, Base>::value>::type>
		constexpr _Split_complex_base(const _T& re) : x{ static_cast<Base>(re) } {}

		template <class = typename enable_if<is_exist_add_inverse_element<T>::value>::type>
		_Split_complex_base operator-() const { return _Split_complex_base(-this->x[0], -this->x[1]); }
		_Split_complex_base operator+() const { return _Split_complex_base(*this); }

		template <class U, class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		_Split_complex_base& operator+=(const _Split_complex_base_type<T, U>& n) {
			this->x[0] += n.x[0]; this->x[1] += n.x[1];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		_Split_complex_base& operator+=(const T& n) {
			this->x[0] += n;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		_Split_complex_base& operator-=(const _Split_complex_base_type<T, U>& n) {
			this->x[0] -= n.x[0]; this->x[1] -= n.x[1];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		_Split_complex_base& operator-=(const T& n) {
			this->x[0] -= n;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		_Split_complex_base& operator*=(const _Split_complex_base_type<T, U>& n) {
			Base temp[2] = { this->x[0] * n.x[0] + this->x[1] * n.x[1], this->x[0] * n.x[1] + this->x[1] * n.x[0] };
			this->x[0] = temp[0]; this->x[1] = temp[1];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		_Split_complex_base& operator*=(const T& k) {
			this->x[0] *= k; this->x[1] *= k;
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		_Split_complex_base& operator/=(const T& k) {
			this->x[0] /= k; this->x[1] /= k;
			return *this;
		}

		//アクセサ
		const Base& operator[](imsize_t index) const { return this->x[index]; }
		Base& operator[](imsize_t index) { return this->x[index]; }


		//二項演算
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend split_complex<Base> operator+(const _Split_complex_base& c1, const _Split_complex_base_type<T, U>& c2) {
			return split_complex<Base>(c1[0] + c2[0], c1[1] + c2[1]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend split_complex<Base> operator+(const _Split_complex_base& c, const T& n) {
			return split_complex<Base>(c[0] + n, c[1]);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::add_value>::type>
		friend split_complex<Base> operator+(const T& n, const _Split_complex_base& c) {
			return split_complex<Base>(n + c[0], c[1]);
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend split_complex<Base> operator-(const _Split_complex_base& c1, const _Split_complex_base_type<T, U>& c2) {
			return split_complex<Base>(c1[0] - c2[0], c1[1] - c2[1]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend split_complex<Base> operator-(const _Split_complex_base& c, const T& n) {
			return split_complex<Base>(c[0] - n, c[1]);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::sub_value>::type>
		friend split_complex<Base> operator-(const T& n, const _Split_complex_base& c) {
			return split_complex<Base>(n - c[0], -c[1]);
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend split_complex<Base> operator*(const _Split_complex_base& c1, const _Split_complex_base_type<T, U>& c2) {
			return split_complex<Base>(c1[0] * c2[0] + c1[1] * c2[1], c1[0] * c2[1] + c1[1] * c2[0]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend split_complex<Base> operator*(const _Split_complex_base& c, const T& k) {
			return split_complex<Base>(c[0] * k, c[1] * k);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::mul_value>::type>
		friend split_complex<Base> operator*(const T& k, const _Split_complex_base& c) {
			return split_complex<Base>(k * c[0], k * c[1]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		friend split_complex<Base> operator/(const _Split_complex_base& c, const T& k) {
			return split_complex<Base>(c[0] / k, c[1] / k);
		}
	};
	//下に階層が存在しないかつBase != T
	template <class Base, class T>
	class _Split_complex_base<Base, T, false, false> {
		template <class> friend class split_complex;
		template <class, class, bool, bool> friend class _Split_complex_base;
	protected:
		Base x[2];
	public:
		constexpr _Split_complex_base() : x{} {}
		template <class _T, class = typename enable_if<is_inclusion<_T, Base>::value>::type>
		constexpr _Split_complex_base(const _T& x1, const _T& x2) : x{ static_cast<Base>(x1),static_cast<Base>(x2) } {}
		template <class _T, class = typename enable_if<is_inclusion<_T, Base>::value>::type>
		constexpr _Split_complex_base(const _T& re) : x{ static_cast<Base>(re) } {}

		template <class = typename enable_if<is_exist_add_inverse_element<T>::value>::type>
		_Split_complex_base operator-() const { return _Split_complex_base(-this->x[0], -this->x[1]); }
		_Split_complex_base operator+() const { return _Split_complex_base(*this); }

		template <class U, class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		_Split_complex_base& operator+=(const _Split_complex_base_type<T, U>& n) {
			this->x[0] += n.x[0]; this->x[1] += n.x[1];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		_Split_complex_base& operator+=(const T& n) {
			this->x[0] += n;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		_Split_complex_base& operator-=(const _Split_complex_base_type<T, U>& n) {
			this->x[0] -= n.x[0]; this->x[1] -= n.x[1];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		_Split_complex_base& operator-=(const T& n) {
			this->x[0] -= n;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		_Split_complex_base& operator*=(const _Split_complex_base_type<T, U>& n) {
			Base temp[2] = { this->x[0] * n.x[0] + this->x[1] * n.x[1], this->x[0] * n.x[1] + this->x[1] * n.x[0] };
			this->x[0] = temp[0]; this->x[1] = temp[1];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		_Split_complex_base& operator*=(const T& k) {
			this->x[0] *= k; this->x[1] *= k;
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		_Split_complex_base& operator/=(const T& k) {
			this->x[0] /= k; this->x[1] /= k;
			return *this;
		}

		//アクセサ
		const Base& operator[](imsize_t index) const { return this->x[index]; }
		Base& operator[](imsize_t index) { return this->x[index]; }


		//二項演算
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend split_complex<Base> operator+(const _Split_complex_base& c1, const _Split_complex_base_type<T, U>& c2) {
			return split_complex<Base>(c1[0] + c2[0], c1[1] + c2[1]);
		}
		template <class U, class = typename enable_if<is_operation<T, Base, Base>::add_value>::type>
		friend split_complex<Base> operator+(const _Split_complex_base_type<T, U>& c1, const _Split_complex_base& c2) {
			return split_complex<Base>(c1[0] + c2[0], c1[1] + c2[1]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend split_complex<Base> operator+(const _Split_complex_base& c, const T& n) {
			return split_complex<Base>(c[0] + n, c[1]);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::add_value>::type>
		friend split_complex<Base> operator+(const T& n, const _Split_complex_base& c) {
			return split_complex<Base>(n + c[0], c[1]);
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend split_complex<Base> operator-(const _Split_complex_base& c1, const _Split_complex_base_type<T, U>& c2) {
			return split_complex<Base>(c1[0] - c2[0], c1[1] - c2[1]);
		}
		template <class U, class = typename enable_if<is_operation<T, Base, Base>::sub_value>::type>
		friend split_complex<Base> operator-(const _Split_complex_base_type<T, U>& c1, const _Split_complex_base& c2) {
			return split_complex<Base>(c1[0] - c2[0], c1[1] - c2[1]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend split_complex<Base> operator-(const _Split_complex_base& c, const T& n) {
			return split_complex<Base>(c[0] - n, c[1]);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::sub_value>::type>
		friend split_complex<Base> operator-(const T& n, const _Split_complex_base& c) {
			return split_complex<Base>(n - c[0], -c[1]);
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend split_complex<Base> operator*(const _Split_complex_base& c1, const _Split_complex_base_type<T, U>& c2) {
			return split_complex<Base>(c1[0] * c2[0] + c1[1] * c2[1], c1[0] * c2[1] + c1[1] * c2[0]);
		}
		template <class U, class = typename enable_if<is_operation<T, Base, Base>::mul_value>::type>
		friend split_complex<Base> operator*(const _Split_complex_base_type<T, U>& c1, const _Split_complex_base& c2) {
			return split_complex<Base>(c1[0] * c2[0] + c1[1] * c2[1], c1[0] * c2[1] + c1[1] * c2[0]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend split_complex<Base> operator*(const _Split_complex_base& c, const T& k) {
			return split_complex<Base>(c[0] * k, c[1] * k);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::mul_value>::type>
		friend split_complex<Base> operator*(const T& k, const _Split_complex_base& c) {
			return split_complex<Base>(k * c[0], k * c[1]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		friend split_complex<Base> operator/(const _Split_complex_base& c, const T& k) {
			return split_complex<Base>(c[0] / k, c[1] / k);
		}
	};
	//下に階層が存在するかつBase == T
	template <class Base, class T>
	class _Split_complex_base<Base, T, true, true> : public _Split_complex_base_base_type<Base, T> {
		template <class> friend class split_complex;
		template <class, class, bool, bool> friend class _Split_complex_base;
	public:
		//コンストラクタの継承
		using _Split_complex_base_base_type<Base, T>::_Split_complex_base;

		//単項演算の継承
		using _Split_complex_base_base_type<Base, T>::operator+;
		using _Split_complex_base_base_type<Base, T>::operator-;

		//代入演算の継承
		using _Split_complex_base_base_type<Base, T>::operator+=;
		using _Split_complex_base_base_type<Base, T>::operator-=;
		using _Split_complex_base_base_type<Base, T>::operator*=;
		using _Split_complex_base_base_type<Base, T>::operator/=;

		template <class U, class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		_Split_complex_base& operator+=(const _Split_complex_base_type<T, U>& n) {
			this->x[0] += n.x[0]; this->x[1] += n.x[1];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		_Split_complex_base& operator+=(const T& n) {
			this->x[0] += n;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		_Split_complex_base& operator-=(const _Split_complex_base_type<T, U>& n) {
			this->x[0] -= n.x[0]; this->x[1] -= n.x[1];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		_Split_complex_base& operator-=(const T& n) {
			this->x[0] -= n;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		_Split_complex_base& operator*=(const _Split_complex_base_type<T, U>& n) {
			Base temp[2] = { this->x[0] * n.x[0] + this->x[1] * n.x[1], this->x[0] * n.x[1] + this->x[1] * n.x[0] };
			this->x[0] = temp[0]; this->x[1] = temp[1];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		_Split_complex_base& operator*=(const T& k) {
			this->x[0] *= k; this->x[1] *= k;
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		_Split_complex_base& operator/=(const T& k) {
			this->x[0] /= k; this->x[1] /= k;
			return *this;
		}

		//添え字演算の継承
		using _Split_complex_base_base_type<Base, T>::operator[];


		//二項演算
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend split_complex<Base> operator+(const _Split_complex_base& c1, const _Split_complex_base_type<T, U>& c2) {
			return split_complex<Base>(c1[0] + c2[0], c1[1] + c2[1]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend split_complex<Base> operator+(const _Split_complex_base& c, const T& n) {
			return split_complex<Base>(c[0] + n, c[1]);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::add_value>::type>
		friend split_complex<Base> operator+(const T& n, const _Split_complex_base& c) {
			return split_complex<Base>(n + c[0], c[1]);
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend split_complex<Base> operator-(const _Split_complex_base& c1, const _Split_complex_base_type<T, U>& c2) {
			return split_complex<Base>(c1[0] - c2[0], c1[1] - c2[1]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend split_complex<Base> operator-(const _Split_complex_base& c, const T& n) {
			return split_complex<Base>(c[0] - n, c[1]);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::sub_value>::type>
		friend split_complex<Base> operator-(const T& n, const _Split_complex_base& c) {
			return split_complex<Base>(n - c[0], -c[1]);
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend split_complex<Base> operator*(const _Split_complex_base& c1, const _Split_complex_base_type<T, U>& c2) {
			return split_complex<Base>(c1[0] * c2[0] + c1[1] * c2[1], c1[0] * c2[1] + c1[1] * c2[0]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend split_complex<Base> operator*(const _Split_complex_base& c, const T& k) {
			return split_complex<Base>(c[0] * k, c[1] * k);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::mul_value>::type>
		friend split_complex<Base> operator*(const T& k, const _Split_complex_base& c) {
			return split_complex<Base>(k * c[0], k * c[1]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		friend split_complex<Base> operator/(const _Split_complex_base& c, const T& k) {
			return split_complex<Base>(c[0] / k, c[1] / k);
		}
	};
	//下に階層が存在するかつBase != T
	template <class Base, class T>
	class _Split_complex_base<Base, T, true, false> : public _Split_complex_base_base_type<Base, T> {
		template <class> friend class split_complex;
		template <class, class, bool, bool> friend class _Split_complex_base;
	public:
		//コンストラクタの継承
		using _Split_complex_base_base_type<Base, T>::_Split_complex_base;

		//単項演算の継承
		using _Split_complex_base_base_type<Base, T>::operator+;
		using _Split_complex_base_base_type<Base, T>::operator-;

		//代入演算の継承
		using _Split_complex_base_base_type<Base, T>::operator+=;
		using _Split_complex_base_base_type<Base, T>::operator-=;
		using _Split_complex_base_base_type<Base, T>::operator*=;
		using _Split_complex_base_base_type<Base, T>::operator/=;

		template <class U, class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		_Split_complex_base& operator+=(const _Split_complex_base_type<T, U>& n) {
			this->x[0] += n.x[0]; this->x[1] += n.x[1];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		_Split_complex_base& operator+=(const T& n) {
			this->x[0] += n;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		_Split_complex_base& operator-=(const _Split_complex_base_type<T, U>& n) {
			this->x[0] -= n.x[0]; this->x[1] -= n.x[1];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		_Split_complex_base& operator-=(const T& n) {
			this->x[0] -= n;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		_Split_complex_base& operator*=(const _Split_complex_base_type<T, U>& n) {
			Base temp[2] = { this->x[0] * n.x[0] + this->x[1] * n.x[1], this->x[0] * n.x[1] + this->x[1] * n.x[0] };
			this->x[0] = temp[0]; this->x[1] = temp[1];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		_Split_complex_base& operator*=(const T& k) {
			this->x[0] *= k; this->x[1] *= k;
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		_Split_complex_base& operator/=(const T& k) {
			this->x[0] /= k; this->x[1] /= k;
			return *this;
		}

		//添え字演算の継承
		using _Split_complex_base_base_type<Base, T>::operator[];


		//二項演算
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend split_complex<Base> operator+(const _Split_complex_base& c1, const _Split_complex_base_type<T, U>& c2) {
			return split_complex<Base>(c1[0] + c2[0], c1[1] + c2[1]);
		}
		template <class U, class = typename enable_if<is_operation<T, Base, Base>::add_value>::type>
		friend split_complex<Base> operator+(const _Split_complex_base_type<T, U>& c1, const _Split_complex_base& c2) {
			return split_complex<Base>(c1[0] + c2[0], c1[1] + c2[1]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend split_complex<Base> operator+(const _Split_complex_base& c, const T& n) {
			return split_complex<Base>(c[0] + n, c[1]);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::add_value>::type>
		friend split_complex<Base> operator+(const T& n, const _Split_complex_base& c) {
			return split_complex<Base>(n + c[0], c[1]);
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend split_complex<Base> operator-(const _Split_complex_base& c1, const _Split_complex_base_type<T, U>& c2) {
			return split_complex<Base>(c1[0] - c2[0], c1[1] - c2[1]);
		}
		template <class U, class = typename enable_if<is_operation<T, Base, Base>::sub_value>::type>
		friend split_complex<Base> operator-(const _Split_complex_base_type<T, U>& c1, const _Split_complex_base& c2) {
			return split_complex<Base>(c1[0] - c2[0], c1[1] - c2[1]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend split_complex<Base> operator-(const _Split_complex_base& c, const T& n) {
			return split_complex<Base>(c[0] - n, c[1]);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::sub_value>::type>
		friend split_complex<Base> operator-(const T& n, const _Split_complex_base& c) {
			return split_complex<Base>(n - c[0], -c[1]);
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend split_complex<Base> operator*(const _Split_complex_base& c1, const _Split_complex_base_type<T, U>& c2) {
			return split_complex<Base>(c1[0] * c2[0] + c1[1] * c2[1], c1[0] * c2[1] + c1[1] * c2[0]);
		}
		template <class U, class = typename enable_if<is_operation<T, Base, Base>::mul_value>::type>
		friend split_complex<Base> operator*(const _Split_complex_base_type<T, U>& c1, const _Split_complex_base& c2) {
			return split_complex<Base>(c1[0] * c2[0] + c1[1] * c2[1], c1[0] * c2[1] + c1[1] * c2[0]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend split_complex<Base> operator*(const _Split_complex_base& c, const T& k) {
			return split_complex<Base>(c[0] * k, c[1] * k);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::mul_value>::type>
		friend split_complex<Base> operator*(const T& k, const _Split_complex_base& c) {
			return split_complex<Base>(k * c[0], k * c[1]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		friend split_complex<Base> operator/(const _Split_complex_base& c, const T& k) {
			return split_complex<Base>(c[0] / k, c[1] / k);
		}
	};

	//分解型複素数型
	template <class T>
	class split_complex : public _Split_complex_base_type<T, T>, public value_list_input<array_iterator<T>> {
		template <class> friend class split_complex;

		//代入演算の補助
		template <class _T>
		static split_complex* complex_copy(split_complex* const n1, const split_complex<_T>& n2) {
			//同じインスタンスでなければ代入
			if (static_cast<void*>(&n1->x[0]) != static_cast<void*>(const_cast<_T*>(&n2.x[0]))) {
				n1->x[0] = static_cast<T>(n2.x[0]); n1->x[1] = static_cast<T>(n2.x[1]);
			}
			return n1;
		}
	public:
		//コンストラクタの継承
		using _Split_complex_base_type<T, T>::_Split_complex_base_type;

		constexpr split_complex(const split_complex& n)
			: _Split_complex_base_type<T, T>(n.x[0], n.x[1]) {}
		template <class _T, class = typename enable_if<is_inclusion<_T, T>::value>::type>
		constexpr split_complex(const split_complex<_T>& n)
			: _Split_complex_base_type<T, T>(n.x[0], n.x[1]) {}
		constexpr split_complex() : _Split_complex_base_type<T, T>() {}
		template <class U>
		constexpr split_complex(const _Split_complex_base_type<T, U>& n)
			: _Split_complex_base_type<T, T>(n.x[0], n.x[1]) {}
		template <class U, class _T, class = typename enable_if<is_inclusion<_T, T>::value>::type>
		constexpr split_complex(const _Split_complex_base_type<_T, U>& n)
			: _Split_complex_base_type<T, T>(n.x[0], n.x[1]) {}

		using algebraic_type = T;
		using iterator = array_iterator<T>;
		using const_iterator = array_iterator<const T>;

		template<class Other>
		struct rebind {
			using other = split_complex<Other>;
		};

		iterator begin() noexcept { return iterator(&x[0]); }
		const_iterator begin() const noexcept { return const_iterator(&x[0]); }
		iterator end() noexcept { return iterator(&x[1] + 1); }
		const_iterator end() const noexcept { return const_iterator(&x[1] + 1); }

		//単項演算の継承
		using _Split_complex_base_type<T, T>::operator-;
		using _Split_complex_base_type<T, T>::operator+;
		//代入演算の継承
		using _Split_complex_base_type<T, T>::operator+=;
		using _Split_complex_base_type<T, T>::operator-=;
		using _Split_complex_base_type<T, T>::operator*=;
		using _Split_complex_base_type<T, T>::operator/=;
		//二項演算の継承
		using value_list_input<array_iterator<T>>::operator<<;

		//代入演算
		template <class U>
		split_complex& operator=(const _Split_complex_base_type<T, U>& n) { return *complex_copy(this, n); }
		template <class U, class _T, class = typename enable_if<is_inclusion<_T, T>::value>::type>
		split_complex& operator=(const _Split_complex_base_type<_T, U>& n) { return *complex_copy(this, n); }

		//添え字演算の継承
		using _Split_complex_base_type<T, T>::operator[];

		//メンバ取得
		template <class S, class = typename enable_if<is_inclusion<S, T>::value>::type>
		void real(const S& re) { this->x[0] = re; }
		constexpr T real() const { return x[0]; }
		template <class S, class = typename enable_if<is_inclusion<S, T>::value>::type>
		void imag(const S& im) { this->x[1] = im; }
		constexpr T imag() const { return this->x[1]; }

		//ストリーム出力
		friend std::ostream& operator<<(std::ostream& os, const split_complex& n) {
			os << '(' << n.x[0] << ',' << n.x[1] << ')';
			return os;
		}
		friend std::wostream& operator<<(std::wostream& os, const split_complex& n) {
			os << L'(' << n.x[0] << L',' << n.x[1] << L')';
			return os;
		}
	};


	//複素数の判定
	template <class T>
	struct _Is_split_complex : false_type {};
	template <class T>
	struct _Is_split_complex<split_complex<T>> : true_type {};
	template <class T>
	struct is_split_complex : _Is_split_complex<typename remove_cv<T>::type> {};

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


	//比較演算
	template <class U1, class U2, class T1, class T2, imsize_t N, class = typename enable_if<is_inclusion<T1, T2>::value || is_inclusion<T2, T1>::value>::type>
	inline bool operator==(const _Split_complex_base_type<T1, U1>& n1, const _Split_complex_base_type<T2, U2>& n2) {
		return (n1[0] == n2[0]) && (n1[1] == n2[1]);
	}
	template <class U1, class U2, class T1, class T2, imsize_t N, class = typename enable_if<is_inclusion<T1, T2>::value || is_inclusion<T2, T1>::value>::type>
	inline bool operator!=(const _Split_complex_base_type<T1, U1>& n1, const _Split_complex_base_type<T2, U2>& n2) {
		return !(n1 == n2);
	}

}

namespace iml {
	//加法パラメータ取得
	template <class T>
	struct addition<split_complex<T>> {
		//単位元の取得
		static split_complex<T> identity_element() {
			return split_complex<T>();
		}
		//逆元の取得
		template <class = typename enable_if<is_exist_add_inverse_element<T>::value>::type>
		static split_complex<T> inverse_element(const split_complex<T>& x) {
			return -x;
		}
	};
	//乗法パラメータ取得
	template <class T>
	struct multiplicative<split_complex<T>> {
		//単位元の取得
		static split_complex<T> identity_element() {
			return split_complex<T>(multiplicative<T>::identity_element(), addition<T>::identity_element());
		}
		//逆元の取得
		/*static split_complex<T> inverse_element(const split_complex<T>& x) {
			//共役を絶対値の二乗で割る
			return split_complex<T>(x.real(), -x.imag()) / (x.real()*x.real() - x.imag()*x.imag());
		}*/
		//吸収元
		static split_complex<T> absorbing_element() {
			return split_complex<T>(addition<T>::identity_element(), addition<T>::identity_element());
		}
	};
}


#endif