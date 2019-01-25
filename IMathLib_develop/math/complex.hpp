#ifndef _IMATH_COMPLEX_HPP
#define _IMATH_COMPLEX_HPP

#include "IMathLib/utility/utility.hpp"
#include "IMathLib/utility/tuple.hpp"
#include "IMathLib/math/math.hpp"
#include "IMathLib/container/array.hpp"


namespace iml {

	template <class>
	class complex;


	//多重構造の演算に対応させるための補助型
	//継承コンストラクタと継承オペレータオーバーロードにより多数定義可能となる
	template <class, class, bool, bool>
	class _Complex_base;

	//簡略にかくためのエイリアス
	template <class Base, class T>
	using _Complex_base_base_type = _Complex_base<Base, typename T::algebraic_type
		, is_algebraic_structure<typename T::algebraic_type>::value, is_same<Base, typename T::algebraic_type>::value>;
	template <class Base, class T>
	using _Complex_base_type = _Complex_base<Base, T
		, is_algebraic_structure<T>::value, is_same<Base, T>::value>;

	//下に階層が存在しないかつBase == T
	template <class Base, class T>
	class _Complex_base<Base, T, false, true> {
		template <class> friend class complex;
		template <class, class, bool, bool> friend class _Complex_base;
	protected:
		Base x[2];
	public:
		constexpr _Complex_base() : x{} {}
		constexpr _Complex_base(const Base& re, const Base& im) : x{ re,im } {}
		constexpr _Complex_base(const Base& re) : x{ re } {}

		template <class = typename enable_if<is_exist_add_inverse_element<T>::value>::type>
		_Complex_base operator-() const { return _Complex_base(-this->x[0], -this->x[1]); }
		_Complex_base operator+() const { return _Complex_base(*this); }

		template <class U, class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		_Complex_base& operator+=(const _Complex_base_type<T, U>& n) {
			this->x[0] += n.x[0]; this->x[1] += n.x[1];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		_Complex_base& operator+=(const T& n) {
			this->x[0] += n;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		_Complex_base& operator-=(const _Complex_base_type<T, U>& n) {
			this->x[0] -= n.x[0]; this->x[1] -= n.x[1];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		_Complex_base& operator-=(const T& n) {
			this->x[0] -= n;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		_Complex_base& operator*=(const _Complex_base_type<T, U>& n) {
			Base temp[2] = { this->x[0] * n.x[0] - this->x[1] * n.x[1], this->x[0] * n.x[1] + this->x[1] * n.x[0] };
			this->x[0] = temp[0]; this->x[1] = temp[1];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		_Complex_base& operator*=(const T& k) {
			this->x[0] *= k; this->x[1] *= k;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		_Complex_base& operator/=(const _Complex_base_type<T, U>& n) {
			Base temp[3] = { this->x[0] * n.x[0] + this->x[1] * n.x[1], this->x[1] * n.x[0] - this->x[0] * n.x[1]
			,n.x[0] * n.x[0] + n.x[1] * n.x[1] };
			this->x[0] = temp[0] / temp[2]; this->x[1] = temp[1] / temp[2];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		_Complex_base& operator/=(const T& k) {
			this->x[0] /= k; this->x[1] /= k;
			return *this;
		}

		//アクセサ
		const Base& operator[](imsize_t index) const { return this->x[index]; }
		Base& operator[](imsize_t index) { return this->x[index]; }


		//二項演算
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend complex<Base> operator+(const _Complex_base& c1, const _Complex_base_type<T, U>& c2) {
			return complex<Base>(c1[0] + c2[0], c1[1] + c2[1]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend complex<Base> operator+(const _Complex_base& c, const T& n) {
			return complex<Base>(c[0] + n, c[1]);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::add_value>::type>
		friend complex<Base> operator+(const T& n, const _Complex_base& c) {
			return complex<Base>(n + c[0], c[1]);
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend complex<Base> operator-(const _Complex_base& c1, const _Complex_base_type<T, U>& c2) {
			return complex<Base>(c1[0] - c2[0], c1[1] - c2[1]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend complex<Base> operator-(const _Complex_base& c, const T& n) {
			return complex<Base>(c[0] - n, c[1]);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::add_value>::type>
		friend complex<Base> operator-(const T& n, const _Complex_base& c) {
			return complex<Base>(n - c[0], -c[1]);
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend complex<Base> operator*(const _Complex_base& c1, const _Complex_base_type<T, U>& c2) {
			return complex<Base>(c1[0] * c2[0] - c1[1] * c2[1], c1[0] * c2[1] + c1[1] * c2[0]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend complex<Base> operator*(const _Complex_base& c, const T& k) {
			return complex<Base>(c[0] * k, c[1] * k);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::mul_value>::type>
		friend complex<Base> operator*(const T& k, const _Complex_base& c) {
			return complex<Base>(k * c[0], k * c[1]);
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		friend complex<Base> operator/(const _Complex_base& c1, const _Complex_base_type<T, U>& c2) {
			T temp = c2[0] * c2[0] + c2[1] * c2[1];
			return complex<Base>((c1[0] * c2[0] + c1[1] * c2[1]) / temp
				, (c1[1] * c2[0] - c1[0] * c2[1]) / temp);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		friend complex<Base> operator/(const _Complex_base& c, const T& k) {
			return complex<Base>(c[0] / k, c[1] / k);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::div_value>::type>
		friend complex<Base> operator/(const T& k, const _Complex_base& c) {
			Base temp = c[0] * c[0] + c[1] * c[1];
			return complex<Base>(k * c[0] / temp, -k * c[1] / temp);
		}
	};
	//下に階層が存在しないかつBase != T
	template <class Base, class T>
	class _Complex_base<Base, T, false, false> {
		template <class> friend class complex;
		template <class, class, bool, bool> friend class _Complex_base;
	protected:
		Base x[2];
	public:
		constexpr _Complex_base() : x{} {}
		constexpr _Complex_base(const Base& re, const Base& im) : x{ re,im } {}
		constexpr _Complex_base(const Base& re) : x{ re } {}
		template <class = typename enable_if<is_inclusion<T, Base>::value>::type>
		constexpr _Complex_base(const T& re, const T& im) : x{ static_cast<Base>(re),static_cast<Base>(im) } {}
		template <class = typename enable_if<is_inclusion<T, Base>::value>::type>
		constexpr _Complex_base(const T& re) : x{ static_cast<Base>(re) } {}

		template <class = typename enable_if<is_exist_add_inverse_element<T>::value>::type>
		_Complex_base operator-() const { return _Complex_base(-this->x[0], -this->x[1]); }
		_Complex_base operator+() const { return _Complex_base(*this); }

		template <class U, class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		_Complex_base& operator+=(const _Complex_base_type<T, U>& n) {
			this->x[0] += n.x[0]; this->x[1] += n.x[1];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		_Complex_base& operator+=(const T& n) {
			this->x[0] += n;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		_Complex_base& operator-=(const _Complex_base_type<T, U>& n) {
			this->x[0] -= n.x[0]; this->x[1] -= n.x[1];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		_Complex_base& operator-=(const T& n) {
			this->x[0] -= n;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		_Complex_base& operator*=(const _Complex_base_type<T, U>& n) {
			Base temp[2] = { this->x[0] * n.x[0] - this->x[1] * n.x[1], this->x[0] * n.x[1] + this->x[1] * n.x[0] };
			this->x[0] = temp[0]; this->x[1] = temp[1];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		_Complex_base& operator*=(const T& k) {
			this->x[0] *= k; this->x[1] *= k;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		_Complex_base& operator/=(const _Complex_base_type<T, U>& n) {
			Base temp[3] = { this->x[0] * n.x[0] + this->x[1] * n.x[1], this->x[1] * n.x[0] - this->x[0] * n.x[1]
			,n.x[0] * n.x[0] + n.x[1] * n.x[1] };
			this->x[0] = temp[0] / temp[2]; this->x[1] = temp[1] / temp[2];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		_Complex_base& operator/=(const T& k) {
			this->x[0] /= k; this->x[1] /= k;
			return *this;
		}

		//アクセサ
		const Base& operator[](imsize_t index) const { return this->x[index]; }
		Base& operator[](imsize_t index) { return this->x[index]; }


		//二項演算
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend complex<Base> operator+(const _Complex_base& c1, const _Complex_base_type<T, U>& c2) {
			return complex<Base>(c1[0] + c2[0], c1[1] + c2[1]);
		}
		template <class U, class = typename enable_if<is_operation<T, Base, Base>::add_value>::type>
		friend complex<Base> operator+(const _Complex_base_type<T, U>& c1, const _Complex_base& c2) {
			return complex<Base>(c1[0] + c2[0], c1[1] + c2[1]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend complex<Base> operator+(const _Complex_base& c, const T& n) {
			return complex<Base>(c[0] + n, c[1]);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::add_value>::type>
		friend complex<Base> operator+(const T& n, const _Complex_base& c) {
			return complex<Base>(n + c[0], c[1]);
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend complex<Base> operator-(const _Complex_base& c1, const _Complex_base_type<T, U>& c2) {
			return complex<Base>(c1[0] - c2[0], c1[1] - c2[1]);
		}
		template <class U, class = typename enable_if<is_operation<T, Base, Base>::sub_value>::type>
		friend complex<Base> operator-(const _Complex_base_type<T, U>& c1, const _Complex_base& c2) {
			return complex<Base>(c1[0] - c2[0], c1[1] - c2[1]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend complex<Base> operator-(const _Complex_base& c, const T& n) {
			return complex<Base>(c[0] - n, c[1]);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::sub_value>::type>
		friend complex<Base> operator-(const T& n, const _Complex_base& c) {
			return complex<Base>(n - c[0], -c[1]);
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend complex<Base> operator*(const _Complex_base& c1, const _Complex_base_type<T, U>& c2) {
			return complex<Base>(c1[0] * c2[0] - c1[1] * c2[1], c1[0] * c2[1] + c1[1] * c2[0]);
		}
		template <class U, class = typename enable_if<is_operation<T, Base, Base>::mul_value>::type>
		friend complex<Base> operator*(const _Complex_base_type<T, U>& c1, const _Complex_base& c2) {
			return complex<Base>(c1[0] * c2[0] - c1[1] * c2[1], c1[0] * c2[1] + c1[1] * c2[0]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend complex<Base> operator*(const _Complex_base& c, const T& k) {
			return complex<Base>(c[0] * k, c[1] * k);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::mul_value>::type>
		friend complex<Base> operator*(const T& k, const _Complex_base& c) {
			return complex<Base>(k * c[0], k * c[1]);
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		friend complex<Base> operator/(const _Complex_base& c1, const _Complex_base_type<T, U>& c2) {
			T temp = c2[0] * c2[0] + c2[1] * c2[1];
			return complex<Base>((c1[0] * c2[0] + c1[1] * c2[1]) / temp
				, (c1[1] * c2[0] - c1[0] * c2[1]) / temp);
		}
		template <class U, class = typename enable_if<is_operation<T, Base, Base>::div_value>::type>
		friend complex<Base> operator/(const _Complex_base_type<T, U>& c1, const _Complex_base& c2) {
			Base temp = c2[0] * c2[0] + c2[1] * c2[1];
			return complex<Base>((c1[0] * c2[0] + c1[1] * c2[1]) / temp
				, (c1[1] * c2[0] - c1[0] * c2[1]) / temp);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		friend complex<Base> operator/(const _Complex_base& c, const T& k) {
			return complex<Base>(c[0] / k, c[1] / k);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::div_value>::type>
		friend complex<Base> operator/(const T& k, const _Complex_base& c) {
			Base temp = c[0] * c[0] + c[1] * c[1];
			return complex<Base>(k * c[0] / temp, -k * c[1] / temp);
		}
	};
	//下に階層が存在するかつBase == T
	template <class Base, class T>
	class _Complex_base<Base, T, true, true> : public _Complex_base_base_type<Base, T> {
		template <class> friend class complex;
		template <class, class, bool, bool> friend class _Complex_base;
	public:
		//コンストラクタの継承
		using _Complex_base_base_type<Base, T>::_Complex_base;

		//単項演算の継承
		using _Complex_base_base_type<Base, T>::operator+;
		using _Complex_base_base_type<Base, T>::operator-;

		//代入演算の継承
		using _Complex_base_base_type<Base, T>::operator+=;
		using _Complex_base_base_type<Base, T>::operator-=;
		using _Complex_base_base_type<Base, T>::operator*=;
		using _Complex_base_base_type<Base, T>::operator/=;

		template <class U, class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		_Complex_base& operator+=(const _Complex_base_type<T, U>& n) {
			this->x[0] += n.x[0]; this->x[1] += n.x[1];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		_Complex_base& operator+=(const T& n) {
			this->x[0] += n;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		_Complex_base& operator-=(const _Complex_base_type<T, U>& n) {
			this->x[0] -= n.x[0]; this->x[1] -= n.x[1];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		_Complex_base& operator-=(const T& n) {
			this->x[0] -= n;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		_Complex_base& operator*=(const _Complex_base_type<T, U>& n) {
			Base temp[2] = { this->x[0] * n.x[0] - this->x[1] * n.x[1], this->x[0] * n.x[1] + this->x[1] * n.x[0] };
			this->x[0] = temp[0]; this->x[1] = temp[1];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		_Complex_base& operator*=(const T& k) {
			this->x[0] *= k; this->x[1] *= k;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		_Complex_base& operator/=(const _Complex_base_type<T, U>& n) {
			Base temp[3] = { this->x[0] * n.x[0] + this->x[1] * n.x[1], this->x[1] * n.x[0] - this->x[0] * n.x[1]
			,n.x[0] * n.x[0] + n.x[1] * n.x[1] };
			this->x[0] = temp[0] / temp[2]; this->x[1] = temp[1] / temp[2];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		_Complex_base& operator/=(const T& k) {
			this->x[0] /= k; this->x[1] /= k;
			return *this;
		}

		//添え字演算の継承
		using _Complex_base_base_type<Base, T>::operator[];


		//二項演算
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend complex<Base> operator+(const _Complex_base& c1, const _Complex_base_type<T, U>& c2) {
			return complex<Base>(c1[0] + c2[0], c1[1] + c2[1]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend complex<Base> operator+(const _Complex_base& c, const T& n) {
			return complex<Base>(c[0] + n, c[1]);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::add_value>::type>
		friend complex<Base> operator+(const T& n, const _Complex_base& c) {
			return complex<Base>(n + c[0], c[1]);
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend complex<Base> operator-(const _Complex_base& c1, const _Complex_base_type<T, U>& c2) {
			return complex<Base>(c1[0] - c2[0], c1[1] - c2[1]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend complex<Base> operator-(const _Complex_base& c, const T& n) {
			return complex<Base>(c[0] - n, c[1]);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::sub_value>::type>
		friend complex<Base> operator-(const T& n, const _Complex_base& c) {
			return complex<Base>(n - c[0], -c[1]);
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend complex<Base> operator*(const _Complex_base& c1, const _Complex_base_type<T, U>& c2) {
			return complex<Base>(c1[0] * c2[0] - c1[1] * c2[1], c1[0] * c2[1] + c1[1] * c2[0]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend complex<Base> operator*(const _Complex_base& c, const T& k) {
			return complex<Base>(c[0] * k, c[1] * k);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::mul_value>::type>
		friend complex<Base> operator*(const T& k, const _Complex_base& c) {
			return complex<Base>(k * c[0], k * c[1]);
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		friend complex<Base> operator/(const _Complex_base& c1, const _Complex_base_type<T, U>& c2) {
			T temp = c2[0] * c2[0] + c2[1] * c2[1];
			return complex<Base>((c1[0] * c2[0] + c1[1] * c2[1]) / temp
				, (c1[1] * c2[0] - c1[0] * c2[1]) / temp);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		friend complex<Base> operator/(const _Complex_base& c, const T& k) {
			return complex<Base>(c[0] / k, c[1] / k);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::div_value>::type>
		friend complex<Base> operator/(const T& k, const _Complex_base& c) {
			Base temp = c[0] * c[0] + c[1] * c[1];
			return complex<Base>(k * c[0] / temp, -k * c[1] / temp);
		}
	};
	//下に階層が存在するかつBase != T
	template <class Base, class T>
	class _Complex_base<Base, T, true, false> : public _Complex_base_base_type<Base, T> {
		template <class> friend class complex;
		template <class, class, bool, bool> friend class _Complex_base;
	public:
		//コンストラクタの継承
		using _Complex_base_base_type<Base, T>::_Complex_base;

		constexpr _Complex_base() : _Complex_base_base_type<Base, T>() {}
		template <class = typename enable_if<is_inclusion<T, Base>::value>::type>
		constexpr _Complex_base(const T& re, const T& im) : x{ static_cast<Base>(re),static_cast<Base>(im) } {}
		template <class = typename enable_if<is_inclusion<T, Base>::value>::type>
		constexpr _Complex_base(const T& re) : x{ static_cast<Base>(re) } {}

		//単項演算の継承
		using _Complex_base_base_type<Base, T>::operator+;
		using _Complex_base_base_type<Base, T>::operator-;

		//代入演算の継承
		using _Complex_base_base_type<Base, T>::operator+=;
		using _Complex_base_base_type<Base, T>::operator-=;
		using _Complex_base_base_type<Base, T>::operator*=;
		using _Complex_base_base_type<Base, T>::operator/=;

		template <class U, class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		_Complex_base& operator+=(const _Complex_base_type<T, U>& n) {
			this->x[0] += n.x[0]; this->x[1] += n.x[1];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		_Complex_base& operator+=(const T& n) {
			this->x[0] += n;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		_Complex_base& operator-=(const _Complex_base_type<T, U>& n) {
			this->x[0] -= n.x[0]; this->x[1] -= n.x[1];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		_Complex_base& operator-=(const T& n) {
			this->x[0] -= n;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		_Complex_base& operator*=(const _Complex_base_type<T, U>& n) {
			Base temp[2] = { this->x[0] * n.x[0] - this->x[1] * n.x[1], this->x[0] * n.x[1] + this->x[1] * n.x[0] };
			this->x[0] = temp[0]; this->x[1] = temp[1];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		_Complex_base& operator*=(const T& k) {
			this->x[0] *= k; this->x[1] *= k;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		_Complex_base& operator/=(const _Complex_base_type<T, U>& n) {
			Base temp[3] = { this->x[0] * n.x[0] + this->x[1] * n.x[1], this->x[1] * n.x[0] - this->x[0] * n.x[1]
			,n.x[0] * n.x[0] + n.x[1] * n.x[1] };
			this->x[0] = temp[0] / temp[2]; this->x[1] = temp[1] / temp[2];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		_Complex_base& operator/=(const T& k) {
			this->x[0] /= k; this->x[1] /= k;
			return *this;
		}

		//添え字演算の継承
		using _Complex_base_base_type<Base, T>::operator[];


		//二項演算
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend complex<Base> operator+(const _Complex_base& c1, const _Complex_base_type<T, U>& c2) {
			return complex<Base>(c1[0] + c2[0], c1[1] + c2[1]);
		}
		template <class U, class = typename enable_if<is_operation<T, Base, Base>::add_value>::type>
		friend complex<Base> operator+(const _Complex_base_type<T, U>& c1, const _Complex_base& c2) {
			return complex<Base>(c1[0] + c2[0], c1[1] + c2[1]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend complex<Base> operator+(const _Complex_base& c, const T& n) {
			return complex<Base>(c[0] + n, c[1]);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::add_value>::type>
		friend complex<Base> operator+(const T& n, const _Complex_base& c) {
			return complex<Base>(n + c[0], c[1]);
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend complex<Base> operator-(const _Complex_base& c1, const _Complex_base_type<T, U>& c2) {
			return complex<Base>(c1[0] - c2[0], c1[1] - c2[1]);
		}
		template <class U, class = typename enable_if<is_operation<T, Base, Base>::sub_value>::type>
		friend complex<Base> operator-(const _Complex_base_type<T, U>& c1, const _Complex_base& c2) {
			return complex<Base>(c1[0] - c2[0], c1[1] - c2[1]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend complex<Base> operator-(const _Complex_base& c, const T& n) {
			return complex<Base>(c[0] - n, c[1]);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::sub_value>::type>
		friend complex<Base> operator-(const T& n, const _Complex_base& c) {
			return complex<Base>(n - c[0], -c[1]);
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend complex<Base> operator*(const _Complex_base& c1, const _Complex_base_type<T, U>& c2) {
			return complex<Base>(c1[0] * c2[0] - c1[1] * c2[1], c1[0] * c2[1] + c1[1] * c2[0]);
		}
		template <class U, class = typename enable_if<is_operation<T, Base, Base>::mul_value>::type>
		friend complex<Base> operator*(const _Complex_base_type<T, U>& c1, const _Complex_base& c2) {
			return complex<Base>(c1[0] * c2[0] - c1[1] * c2[1], c1[0] * c2[1] + c1[1] * c2[0]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend complex<Base> operator*(const _Complex_base& c, const T& k) {
			return complex<Base>(c[0] * k, c[1] * k);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::mul_value>::type>
		friend complex<Base> operator*(const T& k, const _Complex_base& c) {
			return complex<Base>(k * c[0], k * c[1]);
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		friend complex<Base> operator/(const _Complex_base& c1, const _Complex_base_type<T, U>& c2) {
			T temp = c2[0] * c2[0] + c2[1] * c2[1];
			return complex<Base>((c1[0] * c2[0] + c1[1] * c2[1]) / temp
				, (c1[1] * c2[0] - c1[0] * c2[1]) / temp);
		}
		template <class U, class = typename enable_if<is_operation<T, Base, Base>::div_value>::type>
		friend complex<Base> operator/(const _Complex_base_type<T, U>& c1, const _Complex_base& c2) {
			Base temp = c2[0] * c2[0] + c2[1] * c2[1];
			return complex<Base>((c1[0] * c2[0] + c1[1] * c2[1]) / temp
				, (c1[1] * c2[0] - c1[0] * c2[1]) / temp);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		friend complex<Base> operator/(const _Complex_base& c, const T& k) {
			return complex<Base>(c[0] / k, c[1] / k);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::div_value>::type>
		friend complex<Base> operator/(const T& k, const _Complex_base& c) {
			Base temp = c[0] * c[0] + c[1] * c[1];
			return complex<Base>(k * c[0] / temp, -k * c[1] / temp);
		}
	};

	//複素数型
	template <class T>
	class complex : public _Complex_base_type<T, T>, public value_list_input<array_iterator<T>> {
		template <class> friend class complex;

		//代入演算の補助
		template <class _T>
		static complex* complex_copy(complex* const n1, const complex<_T>& n2) {
			//同じインスタンスでなければ代入
			if (static_cast<void*>(&n1->x[0]) != static_cast<void*>(const_cast<_T*>(&n2.x[0]))) {
				n1->x[0] = static_cast<T>(n2.x[0]); n1->x[1] = static_cast<T>(n2.x[1]);
			}
			return n1;
		}
	public:
		//コンストラクタの継承
		using _Complex_base_type<T, T>::_Complex_base;

		constexpr complex() : _Complex_base_type<T, T>() {}
		constexpr complex(const complex& n) : _Complex_base_type<T, T>(n.x[0], n.x[1]) {}
		template <class _T, class = typename enable_if<is_inclusion<_T, T>::value>::type>
		explicit constexpr complex(const complex<_T>& n) : _Complex_base_type<T, T>(n.x[0], n.x[1]) {}
		template <class U>
		constexpr complex(const _Complex_base_type<T, U>& n)
			: _Complex_base_type<T, T>(n.x[0], n.x[1]) {}
		template <class U, class _T, class = typename enable_if<is_inclusion<_T, T>::value>::type>
		explicit constexpr complex(const _Complex_base_type<_T, U>& n)
			: _Complex_base_type<T, T>(n.x[0], n.x[1]) {}

		using algebraic_type = T;
		using iterator = array_iterator<T>;
		using const_iterator = array_iterator<const T>;

		template<class Other>
		struct rebind {
			using other = complex<Other>;
		};

		iterator begin() noexcept { return iterator(&x[0]); }
		const_iterator begin() const noexcept { return const_iterator(&x[0]); }
		iterator end() noexcept { return iterator(&x[1] + 1); }
		const_iterator end() const noexcept { return const_iterator(&x[1] + 1); }

		//単項演算の継承
		using _Complex_base_type<T, T>::operator-;
		using _Complex_base_type<T, T>::operator+;
		//代入演算の継承
		using _Complex_base_type<T, T>::operator+=;
		using _Complex_base_type<T, T>::operator-=;
		using _Complex_base_type<T, T>::operator*=;
		using _Complex_base_type<T, T>::operator/=;
		//二項演算の継承
		using value_list_input<array_iterator<T>>::operator<<;

		//代入演算
		template <class U>
		complex& operator=(const _Complex_base_type<T, U>& n) { return *complex_copy(this, n); }
		template <class U, class _T, class = typename enable_if<is_inclusion<_T, T>::value>::type>
		complex& operator=(const _Complex_base_type<_T, U>& n) { return *complex_copy(this, n); }

		//添え字演算の継承
		using _Complex_base_type<T, T>::operator[];

		//メンバ取得
		template <class S, class = typename enable_if<is_inclusion<S, T>::value>::type>
		void real(const S& re) { this->x[0] = re; }
		constexpr T real() const { return this->x[0]; }
		template <class S, class = typename enable_if<is_inclusion<S, T>::value>::type>
		void imag(const S& im) { this->x[1] = im; }
		constexpr T imag() const { return this->x[1]; }

		//ストリーム出力
		friend std::ostream& operator<<(std::ostream& os, const complex& n) {
			os << '(' << n.x[0] << ',' << n.x[1] << ')';
			return os;
		}
		friend std::wostream& operator<<(std::wostream& os, const complex& n) {
			os << L'(' << n.x[0] << L',' << n.x[1] << L')';
			return os;
		}
	};


	//複素数の判定
	template <class T>
	struct _Is_complex : false_type {};
	template <class T>
	struct _Is_complex<complex<T>> : true_type {};
	template <class T>
	struct is_complex : _Is_complex<typename remove_cv<T>::type> {};

	//複素数の除去
	template <class T>
	struct remove_complex {
		using type = T;
	};
	template <class T>
	struct remove_complex<complex<T>> {
		using type = T;
	};

	//全ての複素数の除去
	template <class T>
	struct remove_all_complex {
		using type = T;
	};
	template <class T>
	struct remove_all_complex<complex<T>> : remove_all_complex<T> {};

	//比較演算
	template <class U1, class U2, class T1, class T2, imsize_t N, class = typename enable_if<is_calcable<T1, T2>::eq_value>::type>
	inline bool operator==(const _Complex_base_type<T1, U1>& n1, const _Complex_base_type<T2, U2>& n2) {
		return (n1[0] == n2[0]) && (n1[1] == n2[1]);
	}
	template <class U1, class U2, class T1, class T2, imsize_t N, class = typename enable_if<is_calcable<T1, T2>::eq_value>::type>
	inline bool operator!=(const _Complex_base_type<T1, U1>& n1, const _Complex_base_type<T2, U2>& n2) {
		return !(n1 == n2);
	}

}

namespace iml {

	//加法パラメータ取得
	template <class T>
	struct addition<complex<T>> {
		//単位元の取得
		static complex<T> identity_element() {
			return complex<T>();
		}
		//逆元の取得
		template <class = typename enable_if<is_exist_add_inverse_element<T>::value>::type>
		static complex<T> inverse_element(const complex<T>& x) {
			return -x;
		}
	};
	//乗法パラメータ取得
	template <class T>
	struct multiplicative<complex<T>> {
		//単位元の取得
		static complex<T> identity_element() {
			return complex<T>(multiplicative<T>::identity_element(), addition<T>::identity_element());
		}
		//逆元の取得
		static complex<T> inverse_element(const complex<T>& x) {
			return complex<T>(x[0], -x[1]) / (x[0] * x[0] + x[1] * x[1]);
		}
		//吸収元
		static complex<T> absorbing_element() {
			return complex<T>(addition<T>::identity_element(), addition<T>::identity_element());
		}
	};
}

#endif