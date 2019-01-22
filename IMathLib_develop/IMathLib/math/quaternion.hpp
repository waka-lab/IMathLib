#ifndef _IMATH_MATH_QUATERNION_HPP
#define _IMATH_MATH_QUATERNION_HPP

#include "IMathLib/math/liner_algebra/vector.hpp"


namespace iml {

	template <class>
	class quaternion;

	//多重構造の演算に対応させるための補助型
	//継承コンストラクタと継承オペレータオーバーロードにより多数定義可能となる
	template <class, class, bool, bool>
	class _Quaternion_base;

	//簡略にかくためのエイリアス
	template <class Base, class T>
	using _Quaternion_base_base_type = _Quaternion_base<Base, typename T::algebraic_type
		, is_algebraic_structure<typename T::algebraic_type>::value, is_same<Base, T>::value>;
	template <class Base, class T>
	using _Quaternion_base_type = _Quaternion_base<Base, T
		, is_algebraic_structure<T>::value, is_same<Base, T>::value>;

	//下に階層が存在しないかつBase == T
	template <class Base, class T>
	class _Quaternion_base<Base, T, false, true> {
		template <class> friend class quaternion;
		template <class, class, bool, bool> friend class _Quaternion_base;
	protected:
		Base x[4];
	public:
		constexpr _Quaternion_base() : x{} {}
		template <class _T, class = typename enable_if<is_inclusion<_T, Base>::value>::type>
		constexpr _Quaternion_base(const _T& x1, const _T& x2, const _T& x3, const _T& x4)
			: x{ static_cast<Base>(x1),static_cast<Base>(x2),static_cast<Base>(x3),static_cast<Base>(x4) } {}
		template <class _T, class = typename enable_if<is_inclusion<_T, Base>::value>::type>
		constexpr _Quaternion_base(const _T& re) : x{ static_cast<Base>(re) } {}

		template <class = typename enable_if<is_exist_add_inverse_element<T>::value>::type>
		_Quaternion_base operator-() const { return _Quaternion_base(-this->x[0], -this->x[1], -this->x[2], -this->x[3]); }
		_Quaternion_base operator+() const { return _Quaternion_base(*this); }

		template <class U, class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		_Quaternion_base& operator+=(const _Quaternion_base_type<T, U>& n) {
			this->x[0] += n.x[0]; this->x[1] += n.x[1];
			this->x[2] += n.x[2]; this->x[3] += n.x[3];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		_Quaternion_base& operator+=(const T& n) {
			this->x[0] += n;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		_Quaternion_base& operator-=(const _Quaternion_base_type<T, U>& n) {
			this->x[0] -= n.x[0]; this->x[1] -= n.x[1];
			this->x[2] -= n.x[2]; this->x[3] -= n.x[3];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		_Quaternion_base& operator-=(const T& n) {
			this->x[0] -= n;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		_Quaternion_base& operator*=(const _Quaternion_base_type<T, U>& n) {
			//return *this = nuaternion<T>(this->x[0] * n.x[0] - this->image()*n.image(), this->x[0] * n.image() + n.x[0] * this->image() + this->image() % n.image());
			//()は3次元の内積と外積
			Base temp[4] = {
				this->x[0] * n.x[0] - (this->x[1] * n.x[1] + this->x[2] * n.x[2] + this->x[3] * n.x[3])
				,this->x[0] * n.x[1] + n.x[0] * this->x[1] + (this->x[2] * n.x[3] - this->x[3] * n.x[2])
				,this->x[0] * n.x[2] + n.x[0] * this->x[2] + (this->x[3] * n.x[1] - this->x[1] * n.x[3])
				,this->x[0] * n.x[3] + n.x[0] * this->x[3] + (this->x[1] * n.x[2] - this->x[2] * n.x[1])
			};
			this->x[0] = temp[0]; this->x[1] = temp[1];
			this->x[2] = temp[2]; this->x[3] = temp[3];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		_Quaternion_base& operator*=(const T& k) {
			this->x[0] *= k; this->x[1] *= k;
			this->x[2] *= k; this->x[3] *= k;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		_Quaternion_base& operator/=(const _Quaternion_base_type<T, U>& n) {
			//逆元は共役を絶対値の二乗で割る
			T temp = n.x[0] * n.x[0] + n.x[1] * n.x[1] + n.x[2] * n.x[2] + n.x[3] * n.x[3];
			return *this *= _Quaternion_base(n.x[0] / temp, -n.x[1] / temp, -n.x[2] / temp, -n.x[3] / temp);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		_Quaternion_base& operator/=(const T& k) {
			this->x[0] /= k; this->x[1] /= k;
			this->x[2] /= k; this->x[3] /= k;
			return *this;
		}

		//アクセサ
		const Base& operator[](imsize_t index) const { return this->x[index]; }
		Base& operator[](imsize_t index) { return this->x[index]; }


		//二項演算
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend quaternion<Base> operator+(const _Quaternion_base& c1, const _Quaternion_base_type<T, U>& c2) {
			return quaternion<Base>(c1[0] + c2[0], c1[1] + c2[1], c1[2] + c2[2], c1[3] + c2[3]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend quaternion<Base> operator+(const _Quaternion_base& c, const T& n) {
			return quaternion<Base>(c[0] + n, c[1], c[2], c[3]);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::add_value>::type>
		friend quaternion<Base> operator+(const T& n, const _Quaternion_base& c) {
			return quaternion<Base>(n + c[0], c[1], c[2], c[3]);
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend quaternion<Base> operator-(const _Quaternion_base& c1, const _Quaternion_base_type<T, U>& c2) {
			return quaternion<Base>(c1[0] - c2[0], c1[1] - c2[1], c1[2] - c2[2], c1[3] - c2[3]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend quaternion<Base> operator-(const _Quaternion_base& c, const T& n) {
			return quaternion<Base>(c[0] - n, c[1], c[2], c[3]);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::sub_value>::type>
		friend quaternion<Base> operator-(const T& n, const _Quaternion_base& c) {
			return quaternion<Base>(n - c[0], -c[1], -c[2], -c[3]);
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend quaternion<Base> operator*(const _Quaternion_base& c1, const _Quaternion_base_type<T, U>& c2) {
			return quaternion<Base>(c1[0] * c2[0] - (c1[1] * c2[1] + c1[2] * c2[2] + c1[3] * c2[3])
				, c1[0] * c2[1] + c2[0] * c1[1] + (c1[2] * c2[3] - c1[3] * c2[2])
				, c1[0] * c2[2] + c2[0] * c1[2] + (c1[3] * c2[1] - c1[1] * c2[3])
				, c1[0] * c2[3] + c2[0] * c1[3] + (c1[1] * c2[2] - c1[2] * c2[1]));
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend quaternion<Base> operator*(const _Quaternion_base& c, const T& k) {
			return quaternion<Base>(c[0] * k, c[1] * k, c[2] * k, c[3] * k);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::mul_value>::type>
		friend quaternion<Base> operator*(const T& k, const _Quaternion_base& c) {
			return quaternion<Base>(k * c[0], k * c[1], k * c[2], k * c[3]);
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		friend quaternion<Base> operator/(const _Quaternion_base& c1, const _Quaternion_base_type<T, U>& c2) {
			T temp = c2[0] * c2[0] + c2[1] * c2[1] + c2[2] * c2[2] + c2[3] * c2[3];
			return c1 * _Quaternion_base_type<T, U>(c2[0] / temp, -c2[1] / temp, -c2[2] / temp, -c2[3] / temp);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		friend quaternion<Base> operator/(const _Quaternion_base& c, const T& k) {
			return quaternion<Base>(c[0] / k, c[1] / k, c[2] / k, c[3] / k);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::div_value>::type>
		friend quaternion<Base> operator/(const T& k, const _Quaternion_base& c) {
			Base temp = c[0] * c[0] + c[1] * c[1] + c[2] * c[2] + c[3] * c[3];
			return k * _Quaternion_base(c[0] / temp, -c[1] / temp, -c[2] / temp, -c[3] / temp);
		}
	};
	//下に階層が存在しないかつBase != T
	template <class Base, class T>
	class _Quaternion_base<Base, T, false, false> {
		template <class> friend class quaternion;
		template <class, class, bool, bool> friend class _Quaternion_base;
	protected:
		Base x[4];
	public:
		constexpr _Quaternion_base() : x{} {}
		template <class _T, class = typename enable_if<is_inclusion<_T, Base>::value>::type>
		constexpr _Quaternion_base(const _T& x1, const _T& x2, const _T& x3, const _T& x4)
			: x{ static_cast<Base>(x1),static_cast<Base>(x2),static_cast<Base>(x3),static_cast<Base>(x4) } {}
		template <class _T, class = typename enable_if<is_inclusion<_T, Base>::value>::type>
		constexpr _Quaternion_base(const _T& re) : x{ static_cast<Base>(re) } {}

		template <class = typename enable_if<is_exist_add_inverse_element<T>::value>::type>
		_Quaternion_base operator-() const { return _Quaternion_base(-this->x[0], -this->x[1], -this->x[2], -this->x[3]); }
		_Quaternion_base operator+() const { return _Quaternion_base(*this); }

		template <class U, class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		_Quaternion_base& operator+=(const _Quaternion_base_type<T, U>& n) {
			this->x[0] += n.x[0]; this->x[1] += n.x[1];
			this->x[2] += n.x[2]; this->x[3] += n.x[3];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		_Quaternion_base& operator+=(const T& n) {
			this->x[0] += n;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		_Quaternion_base& operator-=(const _Quaternion_base_type<T, U>& n) {
			this->x[0] -= n.x[0]; this->x[1] -= n.x[1];
			this->x[2] -= n.x[2]; this->x[3] -= n.x[3];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		_Quaternion_base& operator-=(const T& n) {
			this->x[0] -= n;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		_Quaternion_base& operator*=(const _Quaternion_base_type<T, U>& n) {
			//return *this = nuaternion<T>(this->x[0] * n.x[0] - this->image()*n.image(), this->x[0] * n.image() + n.x[0] * this->image() + this->image() % n.image());
			//()は3次元の内積と外積
			Base temp[4] = {
				this->x[0] * n.x[0] - (this->x[1] * n.x[1] + this->x[2] * n.x[2] + this->x[3] * n.x[3])
				,this->x[0] * n.x[1] + n.x[0] * this->x[1] + (this->x[2] * n.x[3] - this->x[3] * n.x[2])
				,this->x[0] * n.x[2] + n.x[0] * this->x[2] + (this->x[3] * n.x[1] - this->x[1] * n.x[3])
				,this->x[0] * n.x[3] + n.x[0] * this->x[3] + (this->x[1] * n.x[2] - this->x[2] * n.x[1])
			};
			this->x[0] = temp[0]; this->x[1] = temp[1];
			this->x[2] = temp[2]; this->x[3] = temp[3];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		_Quaternion_base& operator*=(const T& k) {
			this->x[0] *= k; this->x[1] *= k;
			this->x[2] *= k; this->x[3] *= k;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		_Quaternion_base& operator/=(const _Quaternion_base_type<T, U>& n) {
			//逆元は共役を絶対値の二乗で割る
			T temp = n.x[0] * n.x[0] + n.x[1] * n.x[1] + n.x[2] * n.x[2] + n.x[3] * n.x[3];
			return *this *= _Quaternion_base(n.x[0] / temp, -n.x[1] / temp, -n.x[2] / temp, -n.x[3] / temp);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		_Quaternion_base& operator/=(const T& k) {
			this->x[0] /= k; this->x[1] /= k;
			this->x[2] /= k; this->x[3] /= k;
			return *this;
		}

		//アクセサ
		const Base& operator[](imsize_t index) const { return this->x[index]; }
		Base& operator[](imsize_t index) { return this->x[index]; }


		//二項演算
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend quaternion<Base> operator+(const _Quaternion_base& c1, const _Quaternion_base_type<T, U>& c2) {
			return quaternion<Base>(c1[0] + c2[0], c1[1] + c2[1], c1[2] + c2[2], c1[3] + c2[3]);
		}
		template <class U, class = typename enable_if<is_operation<T, Base, Base>::add_value>::type>
		friend quaternion<Base> operator+(const _Quaternion_base_type<T, U>& c1, const _Quaternion_base& c2) {
			return quaternion<Base>(c1[0] + c2[0], c1[1] + c2[1], c1[2] + c2[2], c1[3] + c2[3]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend quaternion<Base> operator+(const _Quaternion_base& c, const T& n) {
			return quaternion<Base>(c[0] + n, c[1], c[2], c[3]);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::add_value>::type>
		friend quaternion<Base> operator+(const T& n, const _Quaternion_base& c) {
			return quaternion<Base>(n + c[0], c[1], c[2], c[3]);
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend quaternion<Base> operator-(const _Quaternion_base& c1, const _Quaternion_base_type<T, U>& c2) {
			return quaternion<Base>(c1[0] - c2[0], c1[1] - c2[1], c1[2] - c2[2], c1[3] - c2[3]);
		}
		template <class U, class = typename enable_if<is_operation<T, Base, Base>::sub_value>::type>
		friend quaternion<Base> operator-(const _Quaternion_base_type<T, U>& c1, const _Quaternion_base& c2) {
			return quaternion<Base>(c1[0] - c2[0], c1[1] - c2[1], c1[2] - c2[2], c1[3] - c2[3]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend quaternion<Base> operator-(const _Quaternion_base& c, const T& n) {
			return quaternion<Base>(c[0] - n, c[1], c[2], c[3]);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::sub_value>::type>
		friend quaternion<Base> operator-(const T& n, const _Quaternion_base& c) {
			return quaternion<Base>(n - c[0], -c[1], -c[2], -c[3]);
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend quaternion<Base> operator*(const _Quaternion_base& c1, const _Quaternion_base_type<T, U>& c2) {
			return quaternion<Base>(c1[0] * c2[0] - (c1[1] * c2[1] + c1[2] * c2[2] + c1[3] * c2[3])
				, c1[0] * c2[1] + c2[0] * c1[1] + (c1[2] * c2[3] - c1[3] * c2[2])
				, c1[0] * c2[2] + c2[0] * c1[2] + (c1[3] * c2[1] - c1[1] * c2[3])
				, c1[0] * c2[3] + c2[0] * c1[3] + (c1[1] * c2[2] - c1[2] * c2[1]));
		}
		template <class U, class = typename enable_if<is_operation<T, Base, Base>::mul_value>::type>
		friend quaternion<Base> operator*(const _Quaternion_base_type<T, U>& c1, const _Quaternion_base& c2) {
			return quaternion<Base>(c1[0] * c2[0] - (c1[1] * c2[1] + c1[2] * c2[2] + c1[3] * c2[3])
				, c1[0] * c2[1] + c2[0] * c1[1] + (c1[2] * c2[3] - c1[3] * c2[2])
				, c1[0] * c2[2] + c2[0] * c1[2] + (c1[3] * c2[1] - c1[1] * c2[3])
				, c1[0] * c2[3] + c2[0] * c1[3] + (c1[1] * c2[2] - c1[2] * c2[1]));
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend quaternion<Base> operator*(const _Quaternion_base& c, const T& k) {
			return quaternion<Base>(c[0] * k, c[1] * k, c[2] * k, c[3] * k);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::mul_value>::type>
		friend quaternion<Base> operator*(const T& k, const _Quaternion_base& c) {
			return quaternion<Base>(k * c[0], k * c[1], k * c[2], k * c[3]);
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		friend quaternion<Base> operator/(const _Quaternion_base& c1, const _Quaternion_base_type<T, U>& c2) {
			T temp = c2[0] * c2[0] + c2[1] * c2[1] + c2[2] * c2[2] + c2[3] * c2[3];
			return c1 * _Quaternion_base_type<T, U>(c2[0] / temp, -c2[1] / temp, -c2[2] / temp, -c2[3] / temp);
		}
		template <class U, class = typename enable_if<is_operation<T, Base, Base>::div_value>::type>
		friend quaternion<Base> operator/(const _Quaternion_base_type<T, U>& c1, const _Quaternion_base& c2) {
			Base temp = c2[0] * c2[0] + c2[1] * c2[1] + c2[2] * c2[2] + c2[3] * c2[3];
			return c1 * _Quaternion_base(c2[0] / temp, -c2[1] / temp, -c2[2] / temp, -c2[3] / temp);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		friend quaternion<Base> operator/(const _Quaternion_base& c, const T& k) {
			return quaternion<Base>(c[0] / k, c[1] / k, c[2] / k, c[3] / k);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::div_value>::type>
		friend quaternion<Base> operator/(const T& k, const _Quaternion_base& c) {
			Base temp = c[0] * c[0] + c[1] * c[1] + c[2] * c[2] + c[3] * c[3];
			return k * _Quaternion_base(c[0] / temp, -c[1] / temp, -c[2] / temp, -c[3] / temp);
		}
	};
	//下に階層が存在するかつBase == T
	template <class Base, class T>
	class _Quaternion_base<Base, T, true, true> : public _Quaternion_base_base_type<Base, T> {
		template <class> friend class quaternion;
		template <class, class, bool, bool> friend class _Quaternion_base;
	public:
		//コンストラクタの継承
		using _Quaternion_base_base_type<Base, T>::_Quaternion_base;

		//単項演算の継承
		using _Quaternion_base_base_type<Base, T>::operator+;
		using _Quaternion_base_base_type<Base, T>::operator-;

		//代入演算の継承
		using _Quaternion_base_base_type<Base, T>::operator+=;
		using _Quaternion_base_base_type<Base, T>::operator-=;
		using _Quaternion_base_base_type<Base, T>::operator*=;
		using _Quaternion_base_base_type<Base, T>::operator/=;

		template <class U, class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		_Quaternion_base& operator+=(const _Quaternion_base_type<T, U>& n) {
			this->x[0] += n.x[0]; this->x[1] += n.x[1];
			this->x[2] += n.x[2]; this->x[3] += n.x[3];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		_Quaternion_base& operator+=(const T& n) {
			this->x[0] += n;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		_Quaternion_base& operator-=(const _Quaternion_base_type<T, U>& n) {
			this->x[0] -= n.x[0]; this->x[1] -= n.x[1];
			this->x[2] -= n.x[2]; this->x[3] -= n.x[3];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		_Quaternion_base& operator-=(const T& n) {
			this->x[0] -= n;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		_Quaternion_base& operator*=(const _Quaternion_base_type<T, U>& n) {
			//return *this = nuaternion<T>(this->x[0] * n.x[0] - this->image()*n.image(), this->x[0] * n.image() + n.x[0] * this->image() + this->image() % n.image());
			//()は3次元の内積と外積
			Base temp[4] = {
				this->x[0] * n.x[0] - (this->x[1] * n.x[1] + this->x[2] * n.x[2] + this->x[3] * n.x[3])
				,this->x[0] * n.x[1] + n.x[0] * this->x[1] + (this->x[2] * n.x[3] - this->x[3] * n.x[2])
				,this->x[0] * n.x[2] + n.x[0] * this->x[2] + (this->x[3] * n.x[1] - this->x[1] * n.x[3])
				,this->x[0] * n.x[3] + n.x[0] * this->x[3] + (this->x[1] * n.x[2] - this->x[2] * n.x[1])
			};
			this->x[0] = temp[0]; this->x[1] = temp[1];
			this->x[2] = temp[2]; this->x[3] = temp[3];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		_Quaternion_base& operator*=(const T& k) {
			this->x[0] *= k; this->x[1] *= k;
			this->x[2] *= k; this->x[3] *= k;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		_Quaternion_base& operator/=(const _Quaternion_base_type<T, U>& n) {
			//逆元は共役を絶対値の二乗で割る
			T temp = n.x[0] * n.x[0] + n.x[1] * n.x[1] + n.x[2] * n.x[2] + n.x[3] * n.x[3];
			return *this *= _Quaternion_base(n.x[0] / temp, -n.x[1] / temp, -n.x[2] / temp, -n.x[3] / temp);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		_Quaternion_base& operator/=(const T& k) {
			this->x[0] /= k; this->x[1] /= k;
			this->x[2] /= k; this->x[3] /= k;
			return *this;
		}

		//添え字演算の継承
		using _Quaternion_base_base_type<Base, T>::operator[];


		//二項演算
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend quaternion<Base> operator+(const _Quaternion_base& c1, const _Quaternion_base_type<T, U>& c2) {
			return quaternion<Base>(c1[0] + c2[0], c1[1] + c2[1], c1[2] + c2[2], c1[3] + c2[3]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend quaternion<Base> operator+(const _Quaternion_base& c, const T& n) {
			return quaternion<Base>(c[0] + n, c[1], c[2], c[3]);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::add_value>::type>
		friend quaternion<Base> operator+(const T& n, const _Quaternion_base& c) {
			return quaternion<Base>(n + c[0], c[1], c[2], c[3]);
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend quaternion<Base> operator-(const _Quaternion_base& c1, const _Quaternion_base_type<T, U>& c2) {
			return quaternion<Base>(c1[0] - c2[0], c1[1] - c2[1], c1[2] - c2[2], c1[3] - c2[3]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend quaternion<Base> operator-(const _Quaternion_base& c, const T& n) {
			return quaternion<Base>(c[0] - n, c[1], c[2], c[3]);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::sub_value>::type>
		friend quaternion<Base> operator-(const T& n, const _Quaternion_base& c) {
			return quaternion<Base>(n - c[0], -c[1], -c[2], -c[3]);
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend quaternion<Base> operator*(const _Quaternion_base& c1, const _Quaternion_base_type<T, U>& c2) {
			return quaternion<Base>(c1[0] * c2[0] - (c1[1] * c2[1] + c1[2] * c2[2] + c1[3] * c2[3])
				, c1[0] * c2[1] + c2[0] * c1[1] + (c1[2] * c2[3] - c1[3] * c2[2])
				, c1[0] * c2[2] + c2[0] * c1[2] + (c1[3] * c2[1] - c1[1] * c2[3])
				, c1[0] * c2[3] + c2[0] * c1[3] + (c1[1] * c2[2] - c1[2] * c2[1]));
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend quaternion<Base> operator*(const _Quaternion_base& c, const T& k) {
			return quaternion<Base>(c[0] * k, c[1] * k, c[2] * k, c[3] * k);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::mul_value>::type>
		friend quaternion<Base> operator*(const T& k, const _Quaternion_base& c) {
			return quaternion<Base>(k * c[0], k * c[1], k * c[2], k * c[3]);
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		friend quaternion<Base> operator/(const _Quaternion_base& c1, const _Quaternion_base_type<T, U>& c2) {
			T temp = c2[0] * c2[0] + c2[1] * c2[1] + c2[2] * c2[2] + c2[3] * c2[3];
			return c1 * _Quaternion_base_type<T, U>(c2[0] / temp, -c2[1] / temp, -c2[2] / temp, -c2[3] / temp);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		friend quaternion<Base> operator/(const _Quaternion_base& c, const T& k) {
			return quaternion<Base>(c[0] / k, c[1] / k, c[2] / k, c[3] / k);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::div_value>::type>
		friend quaternion<Base> operator/(const T& k, const _Quaternion_base& c) {
			Base temp = c[0] * c[0] + c[1] * c[1] + c[2] * c[2] + c[3] * c[3];
			return k * _Quaternion_base(c[0] / temp, -c[1] / temp, -c[2] / temp, -c[3] / temp);
		}
	};
	//下に階層が存在するかつBase != T
	template <class Base, class T>
	class _Quaternion_base<Base, T, true, false> : public _Quaternion_base_base_type<Base, T> {
		template <class> friend class quaternion;
		template <class, class, bool, bool> friend class _Quaternion_base;
	public:
		//コンストラクタの継承
		using _Quaternion_base_base_type<Base, T>::_Quaternion_base;

		//単項演算の継承
		using _Quaternion_base_base_type<Base, T>::operator+;
		using _Quaternion_base_base_type<Base, T>::operator-;

		//代入演算の継承
		using _Quaternion_base_base_type<Base, T>::operator+=;
		using _Quaternion_base_base_type<Base, T>::operator-=;
		using _Quaternion_base_base_type<Base, T>::operator*=;
		using _Quaternion_base_base_type<Base, T>::operator/=;

		template <class U, class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		_Quaternion_base& operator+=(const _Quaternion_base_type<T, U>& n) {
			this->x[0] += n.x[0]; this->x[1] += n.x[1];
			this->x[2] += n.x[2]; this->x[3] += n.x[3];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		_Quaternion_base& operator+=(const T& n) {
			this->x[0] += n;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		_Quaternion_base& operator-=(const _Quaternion_base_type<T, U>& n) {
			this->x[0] -= n.x[0]; this->x[1] -= n.x[1];
			this->x[2] -= n.x[2]; this->x[3] -= n.x[3];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		_Quaternion_base& operator-=(const T& n) {
			this->x[0] -= n;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		_Quaternion_base& operator*=(const _Quaternion_base_type<T, U>& n) {
			//return *this = nuaternion<T>(this->x[0] * n.x[0] - this->image()*n.image(), this->x[0] * n.image() + n.x[0] * this->image() + this->image() % n.image());
			//()は3次元の内積と外積
			Base temp[4] = {
				this->x[0] * n.x[0] - (this->x[1] * n.x[1] + this->x[2] * n.x[2] + this->x[3] * n.x[3])
				,this->x[0] * n.x[1] + n.x[0] * this->x[1] + (this->x[2] * n.x[3] - this->x[3] * n.x[2])
				,this->x[0] * n.x[2] + n.x[0] * this->x[2] + (this->x[3] * n.x[1] - this->x[1] * n.x[3])
				,this->x[0] * n.x[3] + n.x[0] * this->x[3] + (this->x[1] * n.x[2] - this->x[2] * n.x[1])
			};
			this->x[0] = temp[0]; this->x[1] = temp[1];
			this->x[2] = temp[2]; this->x[3] = temp[3];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		_Quaternion_base& operator*=(const T& k) {
			this->x[0] *= k; this->x[1] *= k;
			this->x[2] *= k; this->x[3] *= k;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		_Quaternion_base& operator/=(const _Quaternion_base_type<T, U>& n) {
			//逆元は共役を絶対値の二乗で割る
			T temp = n.x[0] * n.x[0] + n.x[1] * n.x[1] + n.x[2] * n.x[2] + n.x[3] * n.x[3];
			return *this *= _Quaternion_base(n.x[0] / temp, -n.x[1] / temp, -n.x[2] / temp, -n.x[3] / temp);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		_Quaternion_base& operator/=(const T& k) {
			this->x[0] /= k; this->x[1] /= k;
			this->x[2] /= k; this->x[3] /= k;
			return *this;
		}

		//添え字演算の継承
		using _Quaternion_base_base_type<Base, T>::operator[];


		//二項演算
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend quaternion<Base> operator+(const _Quaternion_base& c1, const _Quaternion_base_type<T, U>& c2) {
			return quaternion<Base>(c1[0] + c2[0], c1[1] + c2[1], c1[2] + c2[2], c1[3] + c2[3]);
		}
		template <class U, class = typename enable_if<is_operation<T, Base, Base>::add_value>::type>
		friend quaternion<Base> operator+(const _Quaternion_base_type<T, U>& c1, const _Quaternion_base& c2) {
			return quaternion<Base>(c1[0] + c2[0], c1[1] + c2[1], c1[2] + c2[2], c1[3] + c2[3]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend quaternion<Base> operator+(const _Quaternion_base& c, const T& n) {
			return quaternion<Base>(c[0] + n, c[1], c[2], c[3]);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::add_value>::type>
		friend quaternion<Base> operator+(const T& n, const _Quaternion_base& c) {
			return quaternion<Base>(n + c[0], c[1], c[2], c[3]);
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend quaternion<Base> operator-(const _Quaternion_base& c1, const _Quaternion_base_type<T, U>& c2) {
			return quaternion<Base>(c1[0] - c2[0], c1[1] - c2[1], c1[2] - c2[2], c1[3] - c2[3]);
		}
		template <class U, class = typename enable_if<is_operation<T, Base, Base>::sub_value>::type>
		friend quaternion<Base> operator-(const _Quaternion_base_type<T, U>& c1, const _Quaternion_base& c2) {
			return quaternion<Base>(c1[0] - c2[0], c1[1] - c2[1], c1[2] - c2[2], c1[3] - c2[3]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend quaternion<Base> operator-(const _Quaternion_base& c, const T& n) {
			return quaternion<Base>(c[0] - n, c[1], c[2], c[3]);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::sub_value>::type>
		friend quaternion<Base> operator-(const T& n, const _Quaternion_base& c) {
			return quaternion<Base>(n - c[0], -c[1], -c[2], -c[3]);
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend quaternion<Base> operator*(const _Quaternion_base& c1, const _Quaternion_base_type<T, U>& c2) {
			return quaternion<Base>(c1[0] * c2[0] - (c1[1] * c2[1] + c1[2] * c2[2] + c1[3] * c2[3])
				, c1[0] * c2[1] + c2[0] * c1[1] + (c1[2] * c2[3] - c1[3] * c2[2])
				, c1[0] * c2[2] + c2[0] * c1[2] + (c1[3] * c2[1] - c1[1] * c2[3])
				, c1[0] * c2[3] + c2[0] * c1[3] + (c1[1] * c2[2] - c1[2] * c2[1]));
		}
		template <class U, class = typename enable_if<is_operation<T, Base, Base>::mul_value>::type>
		friend quaternion<Base> operator*(const _Quaternion_base_type<T, U>& c1, const _Quaternion_base& c2) {
			return quaternion<Base>(c1[0] * c2[0] - (c1[1] * c2[1] + c1[2] * c2[2] + c1[3] * c2[3])
				, c1[0] * c2[1] + c2[0] * c1[1] + (c1[2] * c2[3] - c1[3] * c2[2])
				, c1[0] * c2[2] + c2[0] * c1[2] + (c1[3] * c2[1] - c1[1] * c2[3])
				, c1[0] * c2[3] + c2[0] * c1[3] + (c1[1] * c2[2] - c1[2] * c2[1]));
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend quaternion<Base> operator*(const _Quaternion_base& c, const T& k) {
			return quaternion<Base>(c[0] * k, c[1] * k, c[2] * k, c[3] * k);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::mul_value>::type>
		friend quaternion<Base> operator*(const T& k, const _Quaternion_base& c) {
			return quaternion<Base>(k * c[0], k * c[1], k * c[2], k * c[3]);
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		friend quaternion<Base> operator/(const _Quaternion_base& c1, const _Quaternion_base_type<T, U>& c2) {
			T temp = c2[0] * c2[0] + c2[1] * c2[1] + c2[2] * c2[2] + c2[3] * c2[3];
			return c1 * _Quaternion_base_type<T, U>(c2[0] / temp, -c2[1] / temp, -c2[2] / temp, -c2[3] / temp);
		}
		template <class U, class = typename enable_if<is_operation<T, Base, Base>::div_value>::type>
		friend quaternion<Base> operator/(const _Quaternion_base_type<T, U>& c1, const _Quaternion_base& c2) {
			Base temp = c2[0] * c2[0] + c2[1] * c2[1] + c2[2] * c2[2] + c2[3] * c2[3];
			return c1 * _Quaternion_base(c2[0] / temp, -c2[1] / temp, -c2[2] / temp, -c2[3] / temp);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		friend quaternion<Base> operator/(const _Quaternion_base& c, const T& k) {
			return quaternion<Base>(c[0] / k, c[1] / k, c[2] / k, c[3] / k);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::div_value>::type>
		friend quaternion<Base> operator/(const T& k, const _Quaternion_base& c) {
			Base temp = c[0] * c[0] + c[1] * c[1] + c[2] * c[2] + c[3] * c[3];
			return k * _Quaternion_base(c[0] / temp, -c[1] / temp, -c[2] / temp, -c[3] / temp);
		}
	};

	//四元数型
	template <class T>
	class quaternion : public _Quaternion_base_type<T, T>, public value_list_input<array_iterator<T>> {
		template <class> friend class quaternion;

		//代入演算の補助
		template <class _T>
		static quaternion* quaternion_copy(quaternion* const n1, const quaternion<_T>& n2) {
			//同じインスタンスでなければ代入
			if (static_cast<void*>(&n1->x[0]) != static_cast<void*>(const_cast<_T*>(&n2.x[0]))) {
				n1->x[0] = static_cast<T>(n2.x[0]); n1->x[1] = static_cast<T>(n2.x[1]);
				n1->x[2] = static_cast<T>(n2.x[2]); n1->x[3] = static_cast<T>(n2.x[3]);
			}
			return n1;
		}
	public:
		//コンストラクタの継承
		using _Quaternion_base_type<T, T>::_Quaternion_base_type;

		constexpr quaternion(const quaternion& n)
			: _Quaternion_base_type<T, T>(n.x[0], n.x[1], n.x[2], n.x[3]) {}
		template <class _T, class = typename enable_if<is_inclusion<_T, T>::value>::type>
		constexpr quaternion(const quaternion<_T>& n)
			: _Quaternion_base_type<T, T>(n.x[0], n.x[1], n.x[2], n.x[3]) {}
		constexpr quaternion() : _Quaternion_base_type<T, T>() {}
		template <class U>
		constexpr quaternion(const _Quaternion_base_type<T, U>& n)
			: _Quaternion_base_type<T, T>(n.x[0], n.x[1], n.x[2], n.x[3]) {}
		template <class U, class _T, class = typename enable_if<is_inclusion<_T, T>::value>::type>
		constexpr quaternion(const _Quaternion_base_type<_T, U>& n)
			: _Quaternion_base_type<T, T>(n.x[0], n.x[1], n.x[2], n.x[3]) {}

		constexpr quaternion(const T& re, const vector3<T>& im)
			: _Quaternion_base_type<T, T>(re, im[0], im[1], im[2]) {}

		using algebraic_type = T;
		using iterator = array_iterator<T>;
		using const_iterator = array_iterator<const T>;

		template<class Other>
		struct rebind {
			using other = quaternion<Other>;
		};

		iterator begin() noexcept { return iterator(&x[0]); }
		const_iterator begin() const noexcept { return const_iterator(&x[0]); }
		iterator end() noexcept { return iterator(&x[3] + 1); }
		const_iterator end() const noexcept { return const_iterator(&x[3] + 1); }

		//単項演算の継承
		using _Quaternion_base_type<T, T>::operator-;
		using _Quaternion_base_type<T, T>::operator+;
		//代入演算の継承
		using _Quaternion_base_type<T, T>::operator+=;
		using _Quaternion_base_type<T, T>::operator-=;
		using _Quaternion_base_type<T, T>::operator*=;
		using _Quaternion_base_type<T, T>::operator/=;
		//二項演算の継承
		using value_list_input<array_iterator<T>>::operator<<;

		//代入演算
		template <class U>
		quaternion& operator=(const _Quaternion_base_type<T, U>& n) { return *quaternion_copy(this, n); }
		template <class U, class _T, class = typename enable_if<is_inclusion<_T, T>::value>::type>
		quaternion& operator=(const _Quaternion_base_type<_T, U>& n) { return *quaternion_copy(this, n); }

		//添え字演算の継承
		using _Quaternion_base_type<T, T>::operator[];

		//メンバ取得
		template <class S, class = typename enable_if<is_inclusion<S, T>::value>::type>
		void real(const S& re) { this->x[0] = re; }
		constexpr T real() const { return this->x[0]; }
		template <class S1, class S2, class S3, class = typename enable_if<is_inclusion<S1, T>::value && is_inclusion<S2, T>::value && is_inclusion<S3, T>::value>::type>
		void imag(const S1& im1, const S2& im2, const S3& im3) {
			this->x[1] = static_cast<T>(im1);
			this->x[2] = static_cast<T>(im2);
			this->x[3] = static_cast<T>(im3);
		}
		template <class S, class = typename enable_if<is_inclusion<S, T>::value>::type>
		void imag(const vector3<S>& im) {
			this->x[1] = static_cast<T>(im.x[0]);
			this->x[2] = static_cast<T>(im.x[1]);
			this->x[3] = static_cast<T>(im.x[2]);
		}
		constexpr vector3<T> imag() const { return vector3<T>(this->x[1], this->x[2], this->x[3]); }

		//ストリーム出力
		friend std::ostream& operator<<(std::ostream& os, const quaternion& n) {
			os << '(' << n.x[0] << ',' << n.x[1] << ',' << n.x[2] << ',' << n.x[3] << ')';
			return os;
		}
		friend std::wostream& operator<<(std::wostream& os, const quaternion& n) {
			os << L'(' << n.x[0] << L',' << n.x[1] << L',' << n.x[2] << L',' << n.x[3] << L')';
			return os;
		}
	};


	//四元数の判定
	template <class T>
	struct _Is_quaternion : false_type {};
	template <class T>
	struct _Is_quaternion<quaternion<T>> : true_type {};
	template <class T>
	struct is_quaternion : _Is_quaternion<typename remove_cv<T>::type> {};

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


	//比較演算
	template <class U1, class U2, class T1, class T2, imsize_t N, class = typename enable_if<is_inclusion<T1, T2>::value || is_inclusion<T2, T1>::value>::type>
	inline bool operator==(const _Quaternion_base_type<T1, U1>& n1, const _Quaternion_base_type<T2, U2>& n2) {
		return (n1[0] == n2[0]) && (n1[1] == n2[1])
			&& (n1[2] == n2[2]) && (n1[3] == n2[3]);
	}
	template <class U1, class U2, class T1, class T2, imsize_t N, class = typename enable_if<is_inclusion<T1, T2>::value || is_inclusion<T2, T1>::value>::type>
	inline bool operator!=(const _Quaternion_base_type<T1, U1>& n1, const _Quaternion_base_type<T2, U2>& n2) {
		return !(n1 == n2);
	}

}

namespace iml {
	//加法パラメータ取得
	template <class T>
	struct addition<quaternion<T>> {
		//単位元の取得
		static quaternion<T> identity_element() {
			return quaternion<T>();
		}
		//逆元の取得
		template <class = typename enable_if<is_exist_add_inverse_element<T>::value>::type>
		static quaternion<T> inverse_element(const quaternion<T>& x) {
			return -x;
		}
	};
	//乗法パラメータ取得
	template <class T>
	struct multiplicative<quaternion<T>> {
		//単位元の取得
		static quaternion<T> identity_element() {
			return quaternion<T>(multiplicative<T>::identity_element());
		}
		//逆元の取得
		static quaternion<T> inverse_element(const quaternion<T>& x) {
			//共役を絶対値の2乗で割る
			return quaternion<T>(x[0], -x[1], -x[2], -x[3]) / (x[0] * x[0] + x[1] * x[1] + x[2] * x[2] + x[3] * x[3]);
		}
		//吸収元
		static quaternion<T> absorbing_element() {
			return quaternion<T>();
		}
	};
}


#endif