#ifndef _IMATH_MATH_OCTONION_HPP
#define _IMATH_MATH_OCTONION_HPP

#include "IMathLib/math/quaternion.hpp"


namespace iml {

	template <class>
	class octonion;


	//多重構造の演算に対応させるための補助型
	//継承コンストラクタと継承オペレータオーバーロードにより多数定義可能となる
	template <class, class, bool, bool>
	class _Octonion_base;

	//簡略にかくためのエイリアス
	template <class Base, class T>
	using _Octonion_base_base_type = _Octonion_base<Base, typename T::algebraic_type
		, is_algebraic_structure<typename T::algebraic_type>::value, is_same<Base, T>::value>;
	template <class Base, class T>
	using _Octonion_base_type = _Octonion_base<Base, T
		, is_algebraic_structure<T>::value, is_same<Base, T>::value>;

	//下に階層が存在しないかつBase == T
	template <class Base, class T>
	class _Octonion_base<Base, T, false, true> {
		template <class> friend class octonion;
		template <class, class, bool, bool> friend class _Octonion_base;
	protected:
		Base x[8];
	public:
		constexpr _Octonion_base() : x{} {}
		template <class _T, class = typename enable_if<is_inclusion<_T, Base>::value>::type>
		constexpr _Octonion_base(const _T& x1, const _T& x2, const _T& x3, const _T& x4, const _T& x5, const _T& x6, const _T& x7, const _T& x8)
			: x{ static_cast<Base>(x1),static_cast<Base>(x2)
			,static_cast<Base>(x3),static_cast<Base>(x4)
			,static_cast<Base>(x5),static_cast<Base>(x6)
			,static_cast<Base>(x7),static_cast<Base>(x8) } {}
		template <class _T, class = typename enable_if<is_inclusion<_T, Base>::value>::type>
		constexpr _Octonion_base(const _T& re) : x{ static_cast<Base>(re) } {}

		template <class = typename enable_if<is_exist_add_inverse_element<T>::value>::type>
		_Octonion_base operator-() const {
			return _Octonion_base(-this->x[0], -this->x[1], -this->x[2], -this->x[3]
				, -this->x[4], -this->x[5], -this->x[6], -this->x[7]);
		}
		_Octonion_base operator+() const { return _Octonion_base(*this); }

		template <class U, class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		_Octonion_base& operator+=(const _Octonion_base_type<T, U>& n) {
			this->x[0] += n.x[0]; this->x[1] += n.x[1];
			this->x[2] += n.x[2]; this->x[3] += n.x[3];
			this->x[4] += n.x[4]; this->x[5] += n.x[5];
			this->x[6] += n.x[6]; this->x[7] += n.x[7];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		_Octonion_base& operator+=(const T& n) {
			this->x[0] += n;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		_Octonion_base& operator-=(const _Octonion_base_type<T, U>& n) {
			this->x[0] -= n.x[0]; this->x[1] -= n.x[1];
			this->x[2] -= n.x[2]; this->x[3] -= n.x[3];
			this->x[4] -= n.x[4]; this->x[5] -= n.x[5];
			this->x[6] -= n.x[6]; this->x[7] -= n.x[7];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		_Octonion_base& operator-=(const T& n) {
			this->x[0] -= n;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		_Octonion_base& operator*=(const _Octonion_base_type<T, U>& n) {
			//()は7次元の内積と外積
			Base temp[8] = {
				this->x[0] * n.x[0] - (this->x[1] * n.x[1] + this->x[2] * n.x[2] + this->x[3] * n.x[3] + this->x[4] * n.x[4] + this->x[5] * n.x[5] + this->x[6] * n.x[6] + this->x[7] * n.x[7])
				,this->x[0] * n.x[1] + n.x[0] * this->x[1] + (this->x[2] * n.x[3] - this->x[3] * n.x[2] - this->x[4] * n.x[5] + this->x[5] * n.x[4] - this->x[6] * n.x[7] + this->x[7] * n.x[6])
				,this->x[0] * n.x[2] + n.x[0] * this->x[2] + (-this->x[1] * n.x[3] + this->x[3] * n.x[1] - this->x[4] * n.x[6] + this->x[5] * n.x[7] + this->x[6] * n.x[4] - this->x[7] * n.x[5])
				,this->x[0] * n.x[3] + n.x[0] * this->x[3] + (this->x[1] * n.x[2] - this->x[2] * n.x[1] - this->x[4] * n.x[7] - this->x[5] * n.x[6] + this->x[6] * n.x[5] + this->x[7] * n.x[4])
				,this->x[0] * n.x[4] + n.x[0] * this->x[4] + (this->x[1] * n.x[5] + this->x[2] * n.x[6] + this->x[3] * n.x[7] - this->x[5] * n.x[1] - this->x[6] * n.x[2] - this->x[7] * n.x[3])
				,this->x[0] * n.x[5] + n.x[0] * this->x[5] + (-this->x[1] * n.x[4] - this->x[2] * n.x[7] + this->x[3] * n.x[6] + this->x[4] * n.x[1] - this->x[6] * n.x[3] + this->x[7] * n.x[2])
				,this->x[0] * n.x[6] + n.x[0] * this->x[6] + (this->x[1] * n.x[7] - this->x[2] * n.x[4] - this->x[3] * n.x[5] + this->x[4] * n.x[2] + this->x[5] * n.x[3] - this->x[7] * n.x[1])
				,this->x[0] * n.x[7] + n.x[0] * this->x[7] + (-this->x[1] * n.x[6] + this->x[2] * n.x[5] - this->x[3] * n.x[4] + this->x[4] * n.x[3] - this->x[5] * n.x[2] + this->x[6] * n.x[1])
			};
			this->x[0] = temp[0]; this->x[1] = temp[1];
			this->x[2] = temp[2]; this->x[3] = temp[3];
			this->x[4] = temp[4]; this->x[5] = temp[5];
			this->x[6] = temp[6]; this->x[7] = temp[7];
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		_Octonion_base& operator*=(const T& k) {
			this->x[0] *= k; this->x[1] *= k;
			this->x[2] *= k; this->x[3] *= k;
			this->x[4] *= k; this->x[5] *= k;
			this->x[6] *= k; this->x[7] *= k;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		_Octonion_base& operator/=(const _Octonion_base_type<T, U>& n) {
			//逆元は共役を絶対値の二乗で割る
			T temp = n.x[0] * n.x[0] + n.x[1] * n.x[1] + n.x[2] * n.x[2] + n.x[3] * n.x[3]
				+ n.x[4] * n.x[4] + n.x[5] * n.x[5] + n.x[6] * n.x[6] + n.x[7] * n.x[7];
			return *this *= _Octonion_base(n.x[0] / temp, -n.x[1] / temp, -n.x[2] / temp, -n.x[3] / temp
				, -n.x[4] / temp, -n.x[5] / temp, -n.x[6] / temp, -n.x[7] / temp);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		_Octonion_base& operator/=(const T& k) {
			this->x[0] /= k; this->x[1] /= k;
			this->x[2] /= k; this->x[3] /= k;
			this->x[4] /= k; this->x[5] /= k;
			this->x[6] /= k; this->x[7] /= k;
			return *this;
		}

		//アクセサ
		const Base& operator[](imsize_t index) const { return this->x[index]; }
		Base& operator[](imsize_t index) { return this->x[index]; }


		//二項演算
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend octonion<Base> operator+(const _Octonion_base& c1, const _Octonion_base_type<T, U>& c2) {
			return octonion<Base>(c1[0] + c2[0], c1[1] + c2[1], c1[2] + c2[2], c1[3] + c2[3]
				, c1[4] + c2[4], c1[5] + c2[5], c1[6] + c2[6], c1[7] + c2[7]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend octonion<Base> operator+(const _Octonion_base& c, const T& n) {
			return octonion<Base>(c[0] + n, c[1], c[2], c[3], c[4], c[5], c[6], c[7]);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::add_value>::type>
		friend octonion<Base> operator+(const T& n, const _Octonion_base& c) {
			return octonion<Base>(n + c[0], c[1], c[2], c[3], c[4], c[5], c[6], c[7]);
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend octonion<Base> operator-(const _Octonion_base& c1, const _Octonion_base_type<T, U>& c2) {
			return octonion<Base>(c1[0] - c2[0], c1[1] - c2[1], c1[2] - c2[2], c1[3] - c2[3]
				, c1[4] - c2[4], c1[5] - c2[5], c1[6] - c2[6], c1[7] - c2[7]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend octonion<Base> operator-(const _Octonion_base& c, const T& n) {
			return octonion<Base>(c[0] - n, c[1], c[2], c[3], c[4], c[5], c[6], c[7]);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::sub_value>::type>
		friend octonion<Base> operator-(const T& n, const _Octonion_base& c) {
			return octonion<Base>(n - c[0], -c[1], -c[2], -c[3], -c[4], -c[5], -c[6], -c[7]);
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend octonion<Base> operator*(const _Octonion_base& c1, const _Octonion_base_type<T, U>& c2) {
			return octonion<Base>(c1[0] * c2[0] - (c1[1] * c2[1] + c1[2] * c2[2] + c1[3] * c2[3] + c1[4] * c2[4] + c1[5] * c2[5] + c1[6] * c2[6] + c1[7] * c2[7])
				, c1[0] * c2[1] + c2[0] * c1[1] + (c1[2] * c2[3] - c1[3] * c2[2] - c1[4] * c2[5] + c1[5] * c2[4] - c1[6] * c2[7] + c1[7] * c2[6])
				, c1[0] * c2[2] + c2[0] * c1[2] + (-c1[1] * c2[3] + c1[3] * c2[1] - c1[4] * c2[6] + c1[5] * c2[7] + c1[6] * c2[4] - c1[7] * c2[5])
				, c1[0] * c2[3] + c2[0] * c1[3] + (c1[1] * c2[2] - c1[2] * c2[1] - c1[4] * c2[7] - c1[5] * c2[6] + c1[6] * c2[5] + c1[7] * c2[4])
				, c1[0] * c2[4] + c2[0] * c1[4] + (c1[1] * c2[5] + c1[2] * c2[6] + c1[3] * c2[7] - c1[5] * c2[1] - c1[6] * c2[2] - c1[7] * c2[3])
				, c1[0] * c2[5] + c2[0] * c1[5] + (-c1[1] * c2[4] - c1[2] * c2[7] + c1[3] * c2[6] + c1[4] * c2[1] - c1[6] * c2[3] + c1[7] * c2[2])
				, c1[0] * c2[6] + c2[0] * c1[6] + (c1[1] * c2[7] - c1[2] * c2[4] - c1[3] * c2[5] + c1[4] * c2[2] + c1[5] * c2[3] - c1[7] * c2[1])
				, c1[0] * c2[7] + c2[0] * c1[7] + (-c1[1] * c2[6] + c1[2] * c2[5] - c1[3] * c2[4] + c1[4] * c2[3] - c1[5] * c2[2] + c1[6] * c2[1]));
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend octonion<Base> operator*(const _Octonion_base& c, const T& k) {
			return octonion<Base>(c[0] * k, c[1] * k, c[2] * k, c[3] * k, c[4] * k, c[5] * k, c[6] * k, c[7] * k);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::mul_value>::type>
		friend octonion<Base> operator*(const T& k, const _Octonion_base& c) {
			return octonion<Base>(k * c[0], k * c[1], k * c[2], k * c[3], k * c[4], k * c[5], k * c[6], k * c[7]);
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		friend octonion<Base> operator/(const _Octonion_base& c1, const _Octonion_base_type<T, U>& c2) {
			T temp = c2[0] * c2[0] + c2[1] * c2[1] + c2[2] * c2[2] + c2[3] * c2[3]
				+ c2[4] * c2[4] + c2[5] * c2[5] + c2[6] * c2[6] + c2[7] * c2[7];
			return c1 * _Octonion_base_type<T, U>(c2[0] / temp, -c2[1] / temp, -c2[2] / temp, -c2[3] / temp
				, -c2[4] / temp, -c2[5] / temp, -c2[6] / temp, -c2[7] / temp);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		friend octonion<Base> operator/(const _Octonion_base& c, const T& k) {
			return octonion<Base>(c[0] / k, c[1] / k, c[2] / k, c[3] / k, c[4] / k, c[5] / k, c[6] / k, c[7] / k);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::div_value>::type>
		friend octonion<Base> operator/(const T& k, const _Octonion_base& c) {
			Base temp = c[0] * c[0] + c[1] * c[1] + c[2] * c[2] + c[3] * c[3]
				+ c[4] * c[4] + c[5] * c[5] + c[6] * c[6] + c[7] * c[7];
			return k * _Octonion_base(c[0] / temp, -c[1] / temp, -c[2] / temp, -c[3] / temp
				, -c[4] / temp, -c[5] / temp, -c[6] / temp, -c[7] / temp);
		}
	};
	//下に階層が存在しないかつBase != T
	template <class Base, class T>
	class _Octonion_base<Base, T, false, false> {
		template <class> friend class octonion;
		template <class, class, bool, bool> friend class _Octonion_base;
	protected:
		Base x[8];
	public:
		constexpr _Octonion_base() : x{} {}
		template <class _T, class = typename enable_if<is_inclusion<_T, Base>::value>::type>
		constexpr _Octonion_base(const _T& x1, const _T& x2, const _T& x3, const _T& x4, const _T& x5, const _T& x6, const _T& x7, const _T& x8)
			: x{ static_cast<Base>(x1),static_cast<Base>(x2)
			,static_cast<Base>(x3),static_cast<Base>(x4)
			,static_cast<Base>(x5),static_cast<Base>(x6)
			,static_cast<Base>(x7),static_cast<Base>(x8) } {}
		template <class _T, class = typename enable_if<is_inclusion<_T, Base>::value>::type>
		constexpr _Octonion_base(const _T& re) : x{ static_cast<Base>(re) } {}

		template <class = typename enable_if<is_exist_add_inverse_element<T>::value>::type>
		_Octonion_base operator-() const {
			return _Octonion_base(-this->x[0], -this->x[1], -this->x[2], -this->x[3]
				, -this->x[4], -this->x[5], -this->x[6], -this->x[7]);
		}
		_Octonion_base operator+() const { return _Octonion_base(*this); }

		template <class U, class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		_Octonion_base& operator+=(const _Octonion_base_type<T, U>& n) {
			this->x[0] += n.x[0]; this->x[1] += n.x[1];
			this->x[2] += n.x[2]; this->x[3] += n.x[3];
			this->x[4] += n.x[4]; this->x[5] += n.x[5];
			this->x[6] += n.x[6]; this->x[7] += n.x[7];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		_Octonion_base& operator+=(const T& n) {
			this->x[0] += n;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		_Octonion_base& operator-=(const _Octonion_base_type<T, U>& n) {
			this->x[0] -= n.x[0]; this->x[1] -= n.x[1];
			this->x[2] -= n.x[2]; this->x[3] -= n.x[3];
			this->x[4] -= n.x[4]; this->x[5] -= n.x[5];
			this->x[6] -= n.x[6]; this->x[7] -= n.x[7];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		_Octonion_base& operator-=(const T& n) {
			this->x[0] -= n;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		_Octonion_base& operator*=(const _Octonion_base_type<T, U>& n) {
			//()は7次元の内積と外積
			Base temp[8] = {
				this->x[0] * n.x[0] - (this->x[1] * n.x[1] + this->x[2] * n.x[2] + this->x[3] * n.x[3] + this->x[4] * n.x[4] + this->x[5] * n.x[5] + this->x[6] * n.x[6] + this->x[7] * n.x[7])
				,this->x[0] * n.x[1] + n.x[0] * this->x[1] + (this->x[2] * n.x[3] - this->x[3] * n.x[2] - this->x[4] * n.x[5] + this->x[5] * n.x[4] - this->x[6] * n.x[7] + this->x[7] * n.x[6])
				,this->x[0] * n.x[2] + n.x[0] * this->x[2] + (-this->x[1] * n.x[3] + this->x[3] * n.x[1] - this->x[4] * n.x[6] + this->x[5] * n.x[7] + this->x[6] * n.x[4] - this->x[7] * n.x[5])
				,this->x[0] * n.x[3] + n.x[0] * this->x[3] + (this->x[1] * n.x[2] - this->x[2] * n.x[1] - this->x[4] * n.x[7] - this->x[5] * n.x[6] + this->x[6] * n.x[5] + this->x[7] * n.x[4])
				,this->x[0] * n.x[4] + n.x[0] * this->x[4] + (this->x[1] * n.x[5] + this->x[2] * n.x[6] + this->x[3] * n.x[7] - this->x[5] * n.x[1] - this->x[6] * n.x[2] - this->x[7] * n.x[3])
				,this->x[0] * n.x[5] + n.x[0] * this->x[5] + (-this->x[1] * n.x[4] - this->x[2] * n.x[7] + this->x[3] * n.x[6] + this->x[4] * n.x[1] - this->x[6] * n.x[3] + this->x[7] * n.x[2])
				,this->x[0] * n.x[6] + n.x[0] * this->x[6] + (this->x[1] * n.x[7] - this->x[2] * n.x[4] - this->x[3] * n.x[5] + this->x[4] * n.x[2] + this->x[5] * n.x[3] - this->x[7] * n.x[1])
				,this->x[0] * n.x[7] + n.x[0] * this->x[7] + (-this->x[1] * n.x[6] + this->x[2] * n.x[5] - this->x[3] * n.x[4] + this->x[4] * n.x[3] - this->x[5] * n.x[2] + this->x[6] * n.x[1])
			};
			this->x[0] = temp[0]; this->x[1] = temp[1];
			this->x[2] = temp[2]; this->x[3] = temp[3];
			this->x[4] = temp[4]; this->x[5] = temp[5];
			this->x[6] = temp[6]; this->x[7] = temp[7];
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		_Octonion_base& operator*=(const T& k) {
			this->x[0] *= k; this->x[1] *= k;
			this->x[2] *= k; this->x[3] *= k;
			this->x[4] *= k; this->x[5] *= k;
			this->x[6] *= k; this->x[7] *= k;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		_Octonion_base& operator/=(const _Octonion_base_type<T, U>& n) {
			//逆元は共役を絶対値の二乗で割る
			T temp = n.x[0] * n.x[0] + n.x[1] * n.x[1] + n.x[2] * n.x[2] + n.x[3] * n.x[3]
				+ n.x[4] * n.x[4] + n.x[5] * n.x[5] + n.x[6] * n.x[6] + n.x[7] * n.x[7];
			return *this *= _Octonion_base(n.x[0] / temp, -n.x[1] / temp, -n.x[2] / temp, -n.x[3] / temp
				, -n.x[4] / temp, -n.x[5] / temp, -n.x[6] / temp, -n.x[7] / temp);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		_Octonion_base& operator/=(const T& k) {
			this->x[0] /= k; this->x[1] /= k;
			this->x[2] /= k; this->x[3] /= k;
			this->x[4] /= k; this->x[5] /= k;
			this->x[6] /= k; this->x[7] /= k;
			return *this;
		}

		//アクセサ
		const Base& operator[](imsize_t index) const { return this->x[index]; }
		Base& operator[](imsize_t index) { return this->x[index]; }


		//二項演算
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend octonion<Base> operator+(const _Octonion_base& c1, const _Octonion_base_type<T, U>& c2) {
			return octonion<Base>(c1[0] + c2[0], c1[1] + c2[1], c1[2] + c2[2], c1[3] + c2[3]
				, c1[4] + c2[4], c1[5] + c2[5], c1[6] + c2[6], c1[7] + c2[7]);
		}
		template <class U, class = typename enable_if<is_operation<T, Base, Base>::add_value>::type>
		friend octonion<Base> operator+(const _Octonion_base_type<T, U>& c1, const _Octonion_base& c2) {
			return octonion<Base>(c1[0] + c2[0], c1[1] + c2[1], c1[2] + c2[2], c1[3] + c2[3]
				, c1[4] + c2[4], c1[5] + c2[5], c1[6] + c2[6], c1[7] + c2[7]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend octonion<Base> operator+(const _Octonion_base& c, const T& n) {
			return octonion<Base>(c[0] + n, c[1], c[2], c[3], c[4], c[5], c[6], c[7]);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::add_value>::type>
		friend octonion<Base> operator+(const T& n, const _Octonion_base& c) {
			return octonion<Base>(n + c[0], c[1], c[2], c[3], c[4], c[5], c[6], c[7]);
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend octonion<Base> operator-(const _Octonion_base& c1, const _Octonion_base_type<T, U>& c2) {
			return octonion<Base>(c1[0] - c2[0], c1[1] - c2[1], c1[2] - c2[2], c1[3] - c2[3]
				, c1[4] - c2[4], c1[5] - c2[5], c1[6] - c2[6], c1[7] - c2[7]);
		}
		template <class U, class = typename enable_if<is_operation<T, Base, Base>::sub_value>::type>
		friend octonion<Base> operator-(const _Octonion_base_type<T, U>& c1, const _Octonion_base& c2) {
			return octonion<Base>(c1[0] - c2[0], c1[1] - c2[1], c1[2] - c2[2], c1[3] - c2[3]
				, c1[4] - c2[4], c1[5] - c2[5], c1[6] - c2[6], c1[7] - c2[7]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend octonion<Base> operator-(const _Octonion_base& c, const T& n) {
			return octonion<Base>(c[0] - n, c[1], c[2], c[3], c[4], c[5], c[6], c[7]);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::sub_value>::type>
		friend octonion<Base> operator-(const T& n, const _Octonion_base& c) {
			return octonion<Base>(n - c[0], -c[1], -c[2], -c[3], -c[4], -c[5], -c[6], -c[7]);
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend octonion<Base> operator*(const _Octonion_base& c1, const _Octonion_base_type<T, U>& c2) {
			return octonion<Base>(c1[0] * c2[0] - (c1[1] * c2[1] + c1[2] * c2[2] + c1[3] * c2[3] + c1[4] * c2[4] + c1[5] * c2[5] + c1[6] * c2[6] + c1[7] * c2[7])
				, c1[0] * c2[1] + c2[0] * c1[1] + (c1[2] * c2[3] - c1[3] * c2[2] - c1[4] * c2[5] + c1[5] * c2[4] - c1[6] * c2[7] + c1[7] * c2[6])
				, c1[0] * c2[2] + c2[0] * c1[2] + (-c1[1] * c2[3] + c1[3] * c2[1] - c1[4] * c2[6] + c1[5] * c2[7] + c1[6] * c2[4] - c1[7] * c2[5])
				, c1[0] * c2[3] + c2[0] * c1[3] + (c1[1] * c2[2] - c1[2] * c2[1] - c1[4] * c2[7] - c1[5] * c2[6] + c1[6] * c2[5] + c1[7] * c2[4])
				, c1[0] * c2[4] + c2[0] * c1[4] + (c1[1] * c2[5] + c1[2] * c2[6] + c1[3] * c2[7] - c1[5] * c2[1] - c1[6] * c2[2] - c1[7] * c2[3])
				, c1[0] * c2[5] + c2[0] * c1[5] + (-c1[1] * c2[4] - c1[2] * c2[7] + c1[3] * c2[6] + c1[4] * c2[1] - c1[6] * c2[3] + c1[7] * c2[2])
				, c1[0] * c2[6] + c2[0] * c1[6] + (c1[1] * c2[7] - c1[2] * c2[4] - c1[3] * c2[5] + c1[4] * c2[2] + c1[5] * c2[3] - c1[7] * c2[1])
				, c1[0] * c2[7] + c2[0] * c1[7] + (-c1[1] * c2[6] + c1[2] * c2[5] - c1[3] * c2[4] + c1[4] * c2[3] - c1[5] * c2[2] + c1[6] * c2[1]));
		}
		template <class U, class = typename enable_if<is_operation<T, Base, Base>::mul_value>::type>
		friend octonion<Base> operator*(const _Octonion_base_type<T, U>& c1, const _Octonion_base& c2) {
			return octonion<Base>(c1[0] * c2[0] - (c1[1] * c2[1] + c1[2] * c2[2] + c1[3] * c2[3] + c1[4] * c2[4] + c1[5] * c2[5] + c1[6] * c2[6] + c1[7] * c2[7])
				, c1[0] * c2[1] + c2[0] * c1[1] + (c1[2] * c2[3] - c1[3] * c2[2] - c1[4] * c2[5] + c1[5] * c2[4] - c1[6] * c2[7] + c1[7] * c2[6])
				, c1[0] * c2[2] + c2[0] * c1[2] + (-c1[1] * c2[3] + c1[3] * c2[1] - c1[4] * c2[6] + c1[5] * c2[7] + c1[6] * c2[4] - c1[7] * c2[5])
				, c1[0] * c2[3] + c2[0] * c1[3] + (c1[1] * c2[2] - c1[2] * c2[1] - c1[4] * c2[7] - c1[5] * c2[6] + c1[6] * c2[5] + c1[7] * c2[4])
				, c1[0] * c2[4] + c2[0] * c1[4] + (c1[1] * c2[5] + c1[2] * c2[6] + c1[3] * c2[7] - c1[5] * c2[1] - c1[6] * c2[2] - c1[7] * c2[3])
				, c1[0] * c2[5] + c2[0] * c1[5] + (-c1[1] * c2[4] - c1[2] * c2[7] + c1[3] * c2[6] + c1[4] * c2[1] - c1[6] * c2[3] + c1[7] * c2[2])
				, c1[0] * c2[6] + c2[0] * c1[6] + (c1[1] * c2[7] - c1[2] * c2[4] - c1[3] * c2[5] + c1[4] * c2[2] + c1[5] * c2[3] - c1[7] * c2[1])
				, c1[0] * c2[7] + c2[0] * c1[7] + (-c1[1] * c2[6] + c1[2] * c2[5] - c1[3] * c2[4] + c1[4] * c2[3] - c1[5] * c2[2] + c1[6] * c2[1]));
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend octonion<Base> operator*(const _Octonion_base& c, const T& k) {
			return octonion<Base>(c[0] * k, c[1] * k, c[2] * k, c[3] * k, c[4] * k, c[5] * k, c[6] * k, c[7] * k);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::mul_value>::type>
		friend octonion<Base> operator*(const T& k, const _Octonion_base& c) {
			return octonion<Base>(k * c[0], k * c[1], k * c[2], k * c[3], k * c[4], k * c[5], k * c[6], k * c[7]);
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		friend octonion<Base> operator/(const _Octonion_base& c1, const _Octonion_base_type<T, U>& c2) {
			T temp = c2[0] * c2[0] + c2[1] * c2[1] + c2[2] * c2[2] + c2[3] * c2[3]
				+ c2[4] * c2[4] + c2[5] * c2[5] + c2[6] * c2[6] + c2[7] * c2[7];
			return c1 * _Octonion_base_type<T, U>(c2[0] / temp, -c2[1] / temp, -c2[2] / temp, -c2[3] / temp
				, -c2[4] / temp, -c2[5] / temp, -c2[6] / temp, -c2[7] / temp);
		}
		template <class U, class = typename enable_if<is_operation<T, Base, Base>::div_value>::type>
		friend octonion<Base> operator/(const _Octonion_base_type<T, U>& c1, const _Octonion_base& c2) {
			Base temp = c2[0] * c2[0] + c2[1] * c2[1] + c2[2] * c2[2] + c2[3] * c2[3]
				+ c2[4] * c2[4] + c2[5] * c2[5] + c2[6] * c2[6] + c2[7] * c2[7];
			return c1 * _Octonion_base(c2[0] / temp, -c2[1] / temp, -c2[2] / temp, -c2[3] / temp
				, -c2[4] / temp, -c2[5] / temp, -c2[6] / temp, -c2[7] / temp);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		friend octonion<Base> operator/(const _Octonion_base& c, const T& k) {
			return octonion<Base>(c[0] / k, c[1] / k, c[2] / k, c[3] / k, c[4] / k, c[5] / k, c[6] / k, c[7] / k);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::div_value>::type>
		friend octonion<Base> operator/(const T& k, const _Octonion_base& c) {
			Base temp = c[0] * c[0] + c[1] * c[1] + c[2] * c[2] + c[3] * c[3]
				+ c[4] * c[4] + c[5] * c[5] + c[6] * c[6] + c[7] * c[7];
			return k * _Octonion_base(c[0] / temp, -c[1] / temp, -c[2] / temp, -c[3] / temp
				, -c[4] / temp, -c[5] / temp, -c[6] / temp, -c[7] / temp);
		}
	};
	//下に階層が存在するかつBase == T
	template <class Base, class T>
	class _Octonion_base<Base, T, true, true> : public _Octonion_base_base_type<Base, T> {
		template <class> friend class octonion;
		template <class, class, bool, bool> friend class _Octonion_base;
	public:
		//コンストラクタの継承
		using _Octonion_base_base_type<Base, T>::_Octonion_base;

		//単項演算の継承
		using _Octonion_base_base_type<Base, T>::operator+;
		using _Octonion_base_base_type<Base, T>::operator-;

		//代入演算の継承
		using _Octonion_base_base_type<Base, T>::operator+=;
		using _Octonion_base_base_type<Base, T>::operator-=;
		using _Octonion_base_base_type<Base, T>::operator*=;
		using _Octonion_base_base_type<Base, T>::operator/=;

		template <class U, class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		_Octonion_base& operator+=(const _Octonion_base_type<T, U>& n) {
			this->x[0] += n.x[0]; this->x[1] += n.x[1];
			this->x[2] += n.x[2]; this->x[3] += n.x[3];
			this->x[4] += n.x[4]; this->x[5] += n.x[5];
			this->x[6] += n.x[6]; this->x[7] += n.x[7];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		_Octonion_base& operator+=(const T& n) {
			this->x[0] += n;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		_Octonion_base& operator-=(const _Octonion_base_type<T, U>& n) {
			this->x[0] -= n.x[0]; this->x[1] -= n.x[1];
			this->x[2] -= n.x[2]; this->x[3] -= n.x[3];
			this->x[4] -= n.x[4]; this->x[5] -= n.x[5];
			this->x[6] -= n.x[6]; this->x[7] -= n.x[7];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		_Octonion_base& operator-=(const T& n) {
			this->x[0] -= n;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		_Octonion_base& operator*=(const _Octonion_base_type<T, U>& n) {
			//()は7次元の内積と外積
			Base temp[8] = {
				this->x[0] * n.x[0] - (this->x[1] * n.x[1] + this->x[2] * n.x[2] + this->x[3] * n.x[3] + this->x[4] * n.x[4] + this->x[5] * n.x[5] + this->x[6] * n.x[6] + this->x[7] * n.x[7])
				,this->x[0] * n.x[1] + n.x[0] * this->x[1] + (this->x[2] * n.x[3] - this->x[3] * n.x[2] - this->x[4] * n.x[5] + this->x[5] * n.x[4] - this->x[6] * n.x[7] + this->x[7] * n.x[6])
				,this->x[0] * n.x[2] + n.x[0] * this->x[2] + (-this->x[1] * n.x[3] + this->x[3] * n.x[1] - this->x[4] * n.x[6] + this->x[5] * n.x[7] + this->x[6] * n.x[4] - this->x[7] * n.x[5])
				,this->x[0] * n.x[3] + n.x[0] * this->x[3] + (this->x[1] * n.x[2] - this->x[2] * n.x[1] - this->x[4] * n.x[7] - this->x[5] * n.x[6] + this->x[6] * n.x[5] + this->x[7] * n.x[4])
				,this->x[0] * n.x[4] + n.x[0] * this->x[4] + (this->x[1] * n.x[5] + this->x[2] * n.x[6] + this->x[3] * n.x[7] - this->x[5] * n.x[1] - this->x[6] * n.x[2] - this->x[7] * n.x[3])
				,this->x[0] * n.x[5] + n.x[0] * this->x[5] + (-this->x[1] * n.x[4] - this->x[2] * n.x[7] + this->x[3] * n.x[6] + this->x[4] * n.x[1] - this->x[6] * n.x[3] + this->x[7] * n.x[2])
				,this->x[0] * n.x[6] + n.x[0] * this->x[6] + (this->x[1] * n.x[7] - this->x[2] * n.x[4] - this->x[3] * n.x[5] + this->x[4] * n.x[2] + this->x[5] * n.x[3] - this->x[7] * n.x[1])
				,this->x[0] * n.x[7] + n.x[0] * this->x[7] + (-this->x[1] * n.x[6] + this->x[2] * n.x[5] - this->x[3] * n.x[4] + this->x[4] * n.x[3] - this->x[5] * n.x[2] + this->x[6] * n.x[1])
			};
			this->x[0] = temp[0]; this->x[1] = temp[1];
			this->x[2] = temp[2]; this->x[3] = temp[3];
			this->x[4] = temp[4]; this->x[5] = temp[5];
			this->x[6] = temp[6]; this->x[7] = temp[7];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		_Octonion_base& operator*=(const T& k) {
			this->x[0] *= k; this->x[1] *= k;
			this->x[2] *= k; this->x[3] *= k;
			this->x[4] *= k; this->x[5] *= k;
			this->x[6] *= k; this->x[7] *= k;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		_Octonion_base& operator/=(const _Octonion_base_type<T, U>& n) {
			//逆元は共役を絶対値の二乗で割る
			T temp = n.x[0] * n.x[0] + n.x[1] * n.x[1] + n.x[2] * n.x[2] + n.x[3] * n.x[3]
				+ n.x[4] * n.x[4] + n.x[5] * n.x[5] + n.x[6] * n.x[6] + n.x[7] * n.x[7];
			return *this *= _Octonion_base(n.x[0] / temp, -n.x[1] / temp, -n.x[2] / temp, -n.x[3] / temp
				, -n.x[4] / temp, -n.x[5] / temp, -n.x[6] / temp, -n.x[7] / temp);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		_Octonion_base& operator/=(const T& k) {
			this->x[0] /= k; this->x[1] /= k;
			this->x[2] /= k; this->x[3] /= k;
			this->x[4] /= k; this->x[5] /= k;
			this->x[6] /= k; this->x[7] /= k;
			return *this;
		}

		//添え字演算の継承
		using _Octonion_base_base_type<Base, T>::operator[];


		//二項演算
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend octonion<Base> operator+(const _Octonion_base& c1, const _Octonion_base_type<T, U>& c2) {
			return octonion<Base>(c1[0] + c2[0], c1[1] + c2[1], c1[2] + c2[2], c1[3] + c2[3]
				, c1[4] + c2[4], c1[5] + c2[5], c1[6] + c2[6], c1[7] + c2[7]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend octonion<Base> operator+(const _Octonion_base& c, const T& n) {
			return octonion<Base>(c[0] + n, c[1], c[2], c[3], c[4], c[5], c[6], c[7]);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::add_value>::type>
		friend octonion<Base> operator+(const T& n, const _Octonion_base& c) {
			return octonion<Base>(n + c[0], c[1], c[2], c[3], c[4], c[5], c[6], c[7]);
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend octonion<Base> operator-(const _Octonion_base& c1, const _Octonion_base_type<T, U>& c2) {
			return octonion<Base>(c1[0] - c2[0], c1[1] - c2[1], c1[2] - c2[2], c1[3] - c2[3]
				, c1[4] - c2[4], c1[5] - c2[5], c1[6] - c2[6], c1[7] - c2[7]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend octonion<Base> operator-(const _Octonion_base& c, const T& n) {
			return octonion<Base>(c[0] - n, c[1], c[2], c[3], c[4], c[5], c[6], c[7]);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::sub_value>::type>
		friend octonion<Base> operator-(const T& n, const _Octonion_base& c) {
			return octonion<Base>(n - c[0], -c[1], -c[2], -c[3], -c[4], -c[5], -c[6], -c[7]);
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend octonion<Base> operator*(const _Octonion_base& c1, const _Octonion_base_type<T, U>& c2) {
			return octonion<Base>(c1[0] * c2[0] - (c1[1] * c2[1] + c1[2] * c2[2] + c1[3] * c2[3] + c1[4] * c2[4] + c1[5] * c2[5] + c1[6] * c2[6] + c1[7] * c2[7])
				, c1[0] * c2[1] + c2[0] * c1[1] + (c1[2] * c2[3] - c1[3] * c2[2] - c1[4] * c2[5] + c1[5] * c2[4] - c1[6] * c2[7] + c1[7] * c2[6])
				, c1[0] * c2[2] + c2[0] * c1[2] + (-c1[1] * c2[3] + c1[3] * c2[1] - c1[4] * c2[6] + c1[5] * c2[7] + c1[6] * c2[4] - c1[7] * c2[5])
				, c1[0] * c2[3] + c2[0] * c1[3] + (c1[1] * c2[2] - c1[2] * c2[1] - c1[4] * c2[7] - c1[5] * c2[6] + c1[6] * c2[5] + c1[7] * c2[4])
				, c1[0] * c2[4] + c2[0] * c1[4] + (c1[1] * c2[5] + c1[2] * c2[6] + c1[3] * c2[7] - c1[5] * c2[1] - c1[6] * c2[2] - c1[7] * c2[3])
				, c1[0] * c2[5] + c2[0] * c1[5] + (-c1[1] * c2[4] - c1[2] * c2[7] + c1[3] * c2[6] + c1[4] * c2[1] - c1[6] * c2[3] + c1[7] * c2[2])
				, c1[0] * c2[6] + c2[0] * c1[6] + (c1[1] * c2[7] - c1[2] * c2[4] - c1[3] * c2[5] + c1[4] * c2[2] + c1[5] * c2[3] - c1[7] * c2[1])
				, c1[0] * c2[7] + c2[0] * c1[7] + (-c1[1] * c2[6] + c1[2] * c2[5] - c1[3] * c2[4] + c1[4] * c2[3] - c1[5] * c2[2] + c1[6] * c2[1]));
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend octonion<Base> operator*(const _Octonion_base& c, const T& k) {
			return octonion<Base>(c[0] * k, c[1] * k, c[2] * k, c[3] * k, c[4] * k, c[5] * k, c[6] * k, c[7] * k);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::mul_value>::type>
		friend octonion<Base> operator*(const T& k, const _Octonion_base& c) {
			return octonion<Base>(k * c[0], k * c[1], k * c[2], k * c[3], k * c[4], k * c[5], k * c[6], k * c[7]);
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		friend octonion<Base> operator/(const _Octonion_base& c1, const _Octonion_base_type<T, U>& c2) {
			T temp = c2[0] * c2[0] + c2[1] * c2[1] + c2[2] * c2[2] + c2[3] * c2[3]
				+ c2[4] * c2[4] + c2[5] * c2[5] + c2[6] * c2[6] + c2[7] * c2[7];
			return c1 * _Octonion_base_type<T, U>(c2[0] / temp, -c2[1] / temp, -c2[2] / temp, -c2[3] / temp
				, -c2[4] / temp, -c2[5] / temp, -c2[6] / temp, -c2[7] / temp);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		friend octonion<Base> operator/(const _Octonion_base& c, const T& k) {
			return octonion<Base>(c[0] / k, c[1] / k, c[2] / k, c[3] / k, c[4] / k, c[5] / k, c[6] / k, c[7] / k);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::div_value>::type>
		friend octonion<Base> operator/(const T& k, const _Octonion_base& c) {
			Base temp = c[0] * c[0] + c[1] * c[1] + c[2] * c[2] + c[3] * c[3]
				+ c[4] * c[4] + c[5] * c[5] + c[6] * c[6] + c[7] * c[7];
			return k * _Octonion_base(c[0] / temp, -c[1] / temp, -c[2] / temp, -c[3] / temp
				, -c[4] / temp, -c[5] / temp, -c[6] / temp, -c[7] / temp);
		}
	};
	//下に階層が存在するかつBase != T
	template <class Base, class T>
	class _Octonion_base<Base, T, true, false> : public _Octonion_base_base_type<Base, T> {
		template <class> friend class octonion;
		template <class, class, bool, bool> friend class _Octonion_base;
	public:
		//コンストラクタの継承
		using _Octonion_base_base_type<Base, T>::_Octonion_base;

		//単項演算の継承
		using _Octonion_base_base_type<Base, T>::operator+;
		using _Octonion_base_base_type<Base, T>::operator-;

		//代入演算の継承
		using _Octonion_base_base_type<Base, T>::operator+=;
		using _Octonion_base_base_type<Base, T>::operator-=;
		using _Octonion_base_base_type<Base, T>::operator*=;
		using _Octonion_base_base_type<Base, T>::operator/=;

		template <class U, class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		_Octonion_base& operator+=(const _Octonion_base_type<T, U>& n) {
			this->x[0] += n.x[0]; this->x[1] += n.x[1];
			this->x[2] += n.x[2]; this->x[3] += n.x[3];
			this->x[4] += n.x[4]; this->x[5] += n.x[5];
			this->x[6] += n.x[6]; this->x[7] += n.x[7];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		_Octonion_base& operator+=(const T& n) {
			this->x[0] += n;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		_Octonion_base& operator-=(const _Octonion_base_type<T, U>& n) {
			this->x[0] -= n.x[0]; this->x[1] -= n.x[1];
			this->x[2] -= n.x[2]; this->x[3] -= n.x[3];
			this->x[4] -= n.x[4]; this->x[5] -= n.x[5];
			this->x[6] -= n.x[6]; this->x[7] -= n.x[7];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		_Octonion_base& operator-=(const T& n) {
			this->x[0] -= n;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		_Octonion_base& operator*=(const _Octonion_base_type<T, U>& n) {
			//()は7次元の内積と外積
			Base temp[8] = {
				this->x[0] * n.x[0] - (this->x[1] * n.x[1] + this->x[2] * n.x[2] + this->x[3] * n.x[3] + this->x[4] * n.x[4] + this->x[5] * n.x[5] + this->x[6] * n.x[6] + this->x[7] * n.x[7])
				,this->x[0] * n.x[1] + n.x[0] * this->x[1] + (this->x[2] * n.x[3] - this->x[3] * n.x[2] - this->x[4] * n.x[5] + this->x[5] * n.x[4] - this->x[6] * n.x[7] + this->x[7] * n.x[6])
				,this->x[0] * n.x[2] + n.x[0] * this->x[2] + (-this->x[1] * n.x[3] + this->x[3] * n.x[1] - this->x[4] * n.x[6] + this->x[5] * n.x[7] + this->x[6] * n.x[4] - this->x[7] * n.x[5])
				,this->x[0] * n.x[3] + n.x[0] * this->x[3] + (this->x[1] * n.x[2] - this->x[2] * n.x[1] - this->x[4] * n.x[7] - this->x[5] * n.x[6] + this->x[6] * n.x[5] + this->x[7] * n.x[4])
				,this->x[0] * n.x[4] + n.x[0] * this->x[4] + (this->x[1] * n.x[5] + this->x[2] * n.x[6] + this->x[3] * n.x[7] - this->x[5] * n.x[1] - this->x[6] * n.x[2] - this->x[7] * n.x[3])
				,this->x[0] * n.x[5] + n.x[0] * this->x[5] + (-this->x[1] * n.x[4] - this->x[2] * n.x[7] + this->x[3] * n.x[6] + this->x[4] * n.x[1] - this->x[6] * n.x[3] + this->x[7] * n.x[2])
				,this->x[0] * n.x[6] + n.x[0] * this->x[6] + (this->x[1] * n.x[7] - this->x[2] * n.x[4] - this->x[3] * n.x[5] + this->x[4] * n.x[2] + this->x[5] * n.x[3] - this->x[7] * n.x[1])
				,this->x[0] * n.x[7] + n.x[0] * this->x[7] + (-this->x[1] * n.x[6] + this->x[2] * n.x[5] - this->x[3] * n.x[4] + this->x[4] * n.x[3] - this->x[5] * n.x[2] + this->x[6] * n.x[1])
			};
			this->x[0] = temp[0]; this->x[1] = temp[1];
			this->x[2] = temp[2]; this->x[3] = temp[3];
			this->x[4] = temp[4]; this->x[5] = temp[5];
			this->x[6] = temp[6]; this->x[7] = temp[7];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		_Octonion_base& operator*=(const T& k) {
			this->x[0] *= k; this->x[1] *= k;
			this->x[2] *= k; this->x[3] *= k;
			this->x[4] *= k; this->x[5] *= k;
			this->x[6] *= k; this->x[7] *= k;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		_Octonion_base& operator/=(const _Octonion_base_type<T, U>& n) {
			//逆元は共役を絶対値の二乗で割る
			T temp = n.x[0] * n.x[0] + n.x[1] * n.x[1] + n.x[2] * n.x[2] + n.x[3] * n.x[3]
				+ n.x[4] * n.x[4] + n.x[5] * n.x[5] + n.x[6] * n.x[6] + n.x[7] * n.x[7];
			return *this *= _Octonion_base(n.x[0] / temp, -n.x[1] / temp, -n.x[2] / temp, -n.x[3] / temp
				, -n.x[4] / temp, -n.x[5] / temp, -n.x[6] / temp, -n.x[7] / temp);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		_Octonion_base& operator/=(const T& k) {
			this->x[0] /= k; this->x[1] /= k;
			this->x[2] /= k; this->x[3] /= k;
			this->x[4] /= k; this->x[5] /= k;
			this->x[6] /= k; this->x[7] /= k;
			return *this;
		}

		//添え字演算の継承
		using _Octonion_base_base_type<Base, T>::operator[];


		//二項演算
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend octonion<Base> operator+(const _Octonion_base& c1, const _Octonion_base_type<T, U>& c2) {
			return octonion<Base>(c1[0] + c2[0], c1[1] + c2[1], c1[2] + c2[2], c1[3] + c2[3]
				, c1[4] + c2[4], c1[5] + c2[5], c1[6] + c2[6], c1[7] + c2[7]);
		}
		template <class U, class = typename enable_if<is_operation<T, Base, Base>::add_value>::type>
		friend octonion<Base> operator+(const _Octonion_base_type<T, U>& c1, const _Octonion_base& c2) {
			return octonion<Base>(c1[0] + c2[0], c1[1] + c2[1], c1[2] + c2[2], c1[3] + c2[3]
				, c1[4] + c2[4], c1[5] + c2[5], c1[6] + c2[6], c1[7] + c2[7]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend octonion<Base> operator+(const _Octonion_base& c, const T& n) {
			return octonion<Base>(c[0] + n, c[1], c[2], c[3], c[4], c[5], c[6], c[7]);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::add_value>::type>
		friend octonion<Base> operator+(const T& n, const _Octonion_base& c) {
			return octonion<Base>(n + c[0], c[1], c[2], c[3], c[4], c[5], c[6], c[7]);
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend octonion<Base> operator-(const _Octonion_base& c1, const _Octonion_base_type<T, U>& c2) {
			return octonion<Base>(c1[0] - c2[0], c1[1] - c2[1], c1[2] - c2[2], c1[3] - c2[3]
				, c1[4] - c2[4], c1[5] - c2[5], c1[6] - c2[6], c1[7] - c2[7]);
		}
		template <class U, class = typename enable_if<is_operation<T, Base, Base>::sub_value>::type>
		friend octonion<Base> operator-(const _Octonion_base_type<T, U>& c1, const _Octonion_base& c2) {
			return octonion<Base>(c1[0] - c2[0], c1[1] - c2[1], c1[2] - c2[2], c1[3] - c2[3]
				, c1[4] - c2[4], c1[5] - c2[5], c1[6] - c2[6], c1[7] - c2[7]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend octonion<Base> operator-(const _Octonion_base& c, const T& n) {
			return octonion<Base>(c[0] - n, c[1], c[2], c[3], c[4], c[5], c[6], c[7]);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::sub_value>::type>
		friend octonion<Base> operator-(const T& n, const _Octonion_base& c) {
			return octonion<Base>(n - c[0], -c[1], -c[2], -c[3], -c[4], -c[5], -c[6], -c[7]);
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend octonion<Base> operator*(const _Octonion_base& c1, const _Octonion_base_type<T, U>& c2) {
			return octonion<Base>(c1[0] * c2[0] - (c1[1] * c2[1] + c1[2] * c2[2] + c1[3] * c2[3] + c1[4] * c2[4] + c1[5] * c2[5] + c1[6] * c2[6] + c1[7] * c2[7])
				, c1[0] * c2[1] + c2[0] * c1[1] + (c1[2] * c2[3] - c1[3] * c2[2] - c1[4] * c2[5] + c1[5] * c2[4] - c1[6] * c2[7] + c1[7] * c2[6])
				, c1[0] * c2[2] + c2[0] * c1[2] + (-c1[1] * c2[3] + c1[3] * c2[1] - c1[4] * c2[6] + c1[5] * c2[7] + c1[6] * c2[4] - c1[7] * c2[5])
				, c1[0] * c2[3] + c2[0] * c1[3] + (c1[1] * c2[2] - c1[2] * c2[1] - c1[4] * c2[7] - c1[5] * c2[6] + c1[6] * c2[5] + c1[7] * c2[4])
				, c1[0] * c2[4] + c2[0] * c1[4] + (c1[1] * c2[5] + c1[2] * c2[6] + c1[3] * c2[7] - c1[5] * c2[1] - c1[6] * c2[2] - c1[7] * c2[3])
				, c1[0] * c2[5] + c2[0] * c1[5] + (-c1[1] * c2[4] - c1[2] * c2[7] + c1[3] * c2[6] + c1[4] * c2[1] - c1[6] * c2[3] + c1[7] * c2[2])
				, c1[0] * c2[6] + c2[0] * c1[6] + (c1[1] * c2[7] - c1[2] * c2[4] - c1[3] * c2[5] + c1[4] * c2[2] + c1[5] * c2[3] - c1[7] * c2[1])
				, c1[0] * c2[7] + c2[0] * c1[7] + (-c1[1] * c2[6] + c1[2] * c2[5] - c1[3] * c2[4] + c1[4] * c2[3] - c1[5] * c2[2] + c1[6] * c2[1]));
		}
		template <class U, class = typename enable_if<is_operation<T, Base, Base>::mul_value>::type>
		friend octonion<Base> operator*(const _Octonion_base_type<T, U>& c1, const _Octonion_base& c2) {
			return octonion<Base>(c1[0] * c2[0] - (c1[1] * c2[1] + c1[2] * c2[2] + c1[3] * c2[3] + c1[4] * c2[4] + c1[5] * c2[5] + c1[6] * c2[6] + c1[7] * c2[7])
				, c1[0] * c2[1] + c2[0] * c1[1] + (c1[2] * c2[3] - c1[3] * c2[2] - c1[4] * c2[5] + c1[5] * c2[4] - c1[6] * c2[7] + c1[7] * c2[6])
				, c1[0] * c2[2] + c2[0] * c1[2] + (-c1[1] * c2[3] + c1[3] * c2[1] - c1[4] * c2[6] + c1[5] * c2[7] + c1[6] * c2[4] - c1[7] * c2[5])
				, c1[0] * c2[3] + c2[0] * c1[3] + (c1[1] * c2[2] - c1[2] * c2[1] - c1[4] * c2[7] - c1[5] * c2[6] + c1[6] * c2[5] + c1[7] * c2[4])
				, c1[0] * c2[4] + c2[0] * c1[4] + (c1[1] * c2[5] + c1[2] * c2[6] + c1[3] * c2[7] - c1[5] * c2[1] - c1[6] * c2[2] - c1[7] * c2[3])
				, c1[0] * c2[5] + c2[0] * c1[5] + (-c1[1] * c2[4] - c1[2] * c2[7] + c1[3] * c2[6] + c1[4] * c2[1] - c1[6] * c2[3] + c1[7] * c2[2])
				, c1[0] * c2[6] + c2[0] * c1[6] + (c1[1] * c2[7] - c1[2] * c2[4] - c1[3] * c2[5] + c1[4] * c2[2] + c1[5] * c2[3] - c1[7] * c2[1])
				, c1[0] * c2[7] + c2[0] * c1[7] + (-c1[1] * c2[6] + c1[2] * c2[5] - c1[3] * c2[4] + c1[4] * c2[3] - c1[5] * c2[2] + c1[6] * c2[1]));
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend octonion<Base> operator*(const _Octonion_base& c, const T& k) {
			return octonion<Base>(c[0] * k, c[1] * k, c[2] * k, c[3] * k, c[4] * k, c[5] * k, c[6] * k, c[7] * k);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::mul_value>::type>
		friend octonion<Base> operator*(const T& k, const _Octonion_base& c) {
			return octonion<Base>(k * c[0], k * c[1], k * c[2], k * c[3], k * c[4], k * c[5], k * c[6], k * c[7]);
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		friend octonion<Base> operator/(const _Octonion_base& c1, const _Octonion_base_type<T, U>& c2) {
			T temp = c2[0] * c2[0] + c2[1] * c2[1] + c2[2] * c2[2] + c2[3] * c2[3]
				+ c2[4] * c2[4] + c2[5] * c2[5] + c2[6] * c2[6] + c2[7] * c2[7];
			return c1 * _Octonion_base_type<T, U>(c2[0] / temp, -c2[1] / temp, -c2[2] / temp, -c2[3] / temp
				, -c2[4] / temp, -c2[5] / temp, -c2[6] / temp, -c2[7] / temp);
		}
		template <class U, class = typename enable_if<is_operation<T, Base, Base>::div_value>::type>
		friend octonion<Base> operator/(const _Octonion_base_type<T, U>& c1, const _Octonion_base& c2) {
			Base temp = c2[0] * c2[0] + c2[1] * c2[1] + c2[2] * c2[2] + c2[3] * c2[3]
				+ c2[4] * c2[4] + c2[5] * c2[5] + c2[6] * c2[6] + c2[7] * c2[7];
			return c1 * _Octonion_base(c2[0] / temp, -c2[1] / temp, -c2[2] / temp, -c2[3] / temp
				, -c2[4] / temp, -c2[5] / temp, -c2[6] / temp, -c2[7] / temp);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		friend octonion<Base> operator/(const _Octonion_base& c, const T& k) {
			return octonion<Base>(c[0] / k, c[1] / k, c[2] / k, c[3] / k, c[4] / k, c[5] / k, c[6] / k, c[7] / k);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::div_value>::type>
		friend octonion<Base> operator/(const T& k, const _Octonion_base& c) {
			Base temp = c[0] * c[0] + c[1] * c[1] + c[2] * c[2] + c[3] * c[3]
				+ c[4] * c[4] + c[5] * c[5] + c[6] * c[6] + c[7] * c[7];
			return k * _Octonion_base(c[0] / temp, -c[1] / temp, -c[2] / temp, -c[3] / temp
				, -c[4] / temp, -c[5] / temp, -c[6] / temp, -c[7] / temp);
		}
	};

	//八元数型
	template <class T>
	class octonion : public _Octonion_base_type<T, T>, public value_list_input<array_iterator<T>> {
		template <class> friend class octonion;

		//代入演算の補助
		template <class _T>
		static octonion* octonion_copy(octonion* const n1, const octonion<_T>& n2) {
			//同じインスタンスでなければ代入
			if (static_cast<void*>(&n1->x[0]) != static_cast<void*>(const_cast<_T*>(&n2.x[0]))) {
				n1->x[0] = static_cast<T>(n2.x[0]); n1->x[1] = static_cast<T>(n2.x[1]);
				n1->x[2] = static_cast<T>(n2.x[2]); n1->x[3] = static_cast<T>(n2.x[3]);
				n1->x[4] = static_cast<T>(n2.x[4]); n1->x[5] = static_cast<T>(n2.x[5]);
				n1->x[6] = static_cast<T>(n2.x[6]); n1->x[7] = static_cast<T>(n2.x[7]);
			}
			return n1;
		}
	public:
		//コンストラクタの継承
		using _Octonion_base_type<T, T>::_Octonion_base_type;

		constexpr octonion() : _Octonion_base_type<T, T>() {}
		constexpr octonion(const octonion& n)
			: _Octonion_base_type<T, T>(n.x[0], n.x[1], n.x[2], n.x[3], n.x[4], n.x[5], n.x[6], n.x[7]) {}
		template <class _T, class = typename enable_if<is_inclusion<_T, T>::value>::type>
		constexpr octonion(const octonion<_T>& n)
			: _Octonion_base_type<T, T>(n.x[0], n.x[1], n.x[2], n.x[3], n.x[4], n.x[5], n.x[6], n.x[7]) {}
		template <class U>
		constexpr octonion(const _Octonion_base_type<T, U>& n)
			: _Octonion_base_type<T, T>(n.x[0], n.x[1], n.x[2], n.x[3], n.x[4], n.x[5], n.x[6], n.x[7]) {}
		template <class U, class _T, class = typename enable_if<is_inclusion<_T, T>::value>::type>
		constexpr octonion(const _Octonion_base_type<_T, U>& n)
			: _Octonion_base_type<T, T>(n.x[0], n.x[1], n.x[2], n.x[3], n.x[4], n.x[5], n.x[6], n.x[7]) {}

		using algebraic_type = T;
		using iterator = array_iterator<T>;
		using const_iterator = array_iterator<const T>;

		template<class Other>
		struct rebind {
			using other = octonion<Other>;
		};

		iterator begin() noexcept { return iterator(&x[0]); }
		const_iterator begin() const noexcept { return const_iterator(&x[0]); }
		iterator end() noexcept { return iterator(&x[7] + 1); }
		const_iterator end() const noexcept { return const_iterator(&x[7] + 1); }

		//単項演算の継承
		using _Octonion_base_type<T, T>::operator-;
		using _Octonion_base_type<T, T>::operator+;
		//代入演算の継承
		using _Octonion_base_type<T, T>::operator+=;
		using _Octonion_base_type<T, T>::operator-=;
		using _Octonion_base_type<T, T>::operator*=;
		using _Octonion_base_type<T, T>::operator/=;
		//二項演算の継承
		using value_list_input<array_iterator<T>>::operator<<;

		//代入演算
		template <class U>
		octonion& operator=(const _Octonion_base_type<T, U>& n) { return *octonion_copy(this, n); }
		template <class U, class _T, class = typename enable_if<is_inclusion<_T, T>::value>::type>
		octonion& operator=(const _Octonion_base_type<_T, U>& n) { return *octonion_copy(this, n); }

		//添え字演算の継承
		using _Octonion_base_type<T, T>::operator[];

		//メンバ取得
		template <class S, class = typename enable_if<is_inclusion<S, T>::value>::type>
		void real(const S& re) { this->x[0] = re; }
		constexpr T real() const { return this->x[0]; }
		template <class S1, class S2, class S3, class S4, class S5, class S6, class S7
			, class = typename enable_if<is_inclusion<S1, T>::value && is_inclusion<S2, T>::value && is_inclusion<S3, T>::value && is_inclusion<S4, T>::value
			&& is_inclusion<S5, T>::value && is_inclusion<S6, T>::value && is_inclusion<S7, T>::value>::type>
		void imag(const S1& im1, const S2& im2, const S3& im3, const S4& im4, const S5& im5, const S6& im6, const S7& im7) {
			this->x[1] = static_cast<T>(im1);
			this->x[2] = static_cast<T>(im2);
			this->x[3] = static_cast<T>(im3);
			this->x[4] = static_cast<T>(im4);
			this->x[5] = static_cast<T>(im5);
			this->x[6] = static_cast<T>(im6);
			this->x[7] = static_cast<T>(im7);
		}
		template <class S, class = typename enable_if<is_inclusion<S, T>::value>::type>
		void imag(const vector<S, 7>& im) {
			this->x[1] = static_cast<T>(im.x[0]);
			this->x[2] = static_cast<T>(im.x[1]);
			this->x[3] = static_cast<T>(im.x[2]);
			this->x[4] = static_cast<T>(im.x[3]);
			this->x[5] = static_cast<T>(im.x[4]);
			this->x[6] = static_cast<T>(im.x[5]);
			this->x[7] = static_cast<T>(im.x[6]);
		}
		constexpr vector<T, 7> imag() const {
			return vector<T, 7>(this->x[1], this->x[2], this->x[3], this->x[4], this->x[5], this->x[6], this->x[7]);
		}

		//ストリーム出力
		friend std::ostream& operator<<(std::ostream& os, const octonion& n) {
			os << '(' << n.x[0] << ',' << n.x[1] << ',' << n.x[2] << ',' << n.x[3]
				<< ',' << n.x[4] << ',' << n.x[5] << ',' << n.x[6] << ',' << n.x[7] << ')';
			return os;
		}
		friend std::wostream& operator<<(std::wostream& os, const octonion& n) {
			os << L'(' << n.x[0] << L',' << n.x[1] << L',' << n.x[2] << L',' << n.x[3]
				<< L',' << n.x[4] << L',' << n.x[5] << L',' << n.x[6] << L',' << n.x[7] << L')';
			return os;
		}
	};


	//四元数の判定
	template <class T>
	struct _Is_octonion : false_type {};
	template <class T>
	struct _Is_octonion<octonion<T>> : true_type {};
	template <class T>
	struct is_octonion : _Is_octonion<typename remove_cv<T>::type> {};

	//四元数の除去
	template <class T>
	struct remove_octonion {
		using type = T;
	};
	template <class T>
	struct remove_octonion<octonion<T>> {
		using type = T;
	};

	//全ての四元数の除去
	template <class T>
	struct remove_all_octonion {
		using type = T;
	};
	template <class T>
	struct remove_all_octonion<octonion<T>> : remove_all_octonion<T> {};


	//比較演算
	template <class U1, class U2, class T1, class T2, imsize_t N, class = typename enable_if<is_inclusion<T1, T2>::value || is_inclusion<T2, T1>::value>::type>
	inline bool operator==(const _Octonion_base_type<T1, U1>& n1, const _Octonion_base_type<T2, U2>& n2) {
		return (n1[0] == n2[0]) && (n1[1] == n2[1])
			&& (n1[2] == n2[2]) && (n1[3] == n2[3])
			&& (n1[4] == n2[4]) && (n1[5] == n2[5])
			&& (n1[6] == n2[6]) && (n1[7] == n2[7]);
	}
	template <class U1, class U2, class T1, class T2, imsize_t N, class = typename enable_if<is_inclusion<T1, T2>::value || is_inclusion<T2, T1>::value>::type>
	inline bool operator!=(const _Octonion_base_type<T1, U1>& n1, const _Octonion_base_type<T2, U2>& n2) {
		return !(n1 == n2);
	}

}

namespace iml {
	//加法パラメータ取得
	template <class T>
	struct addition<octonion<T>> {
		//単位元の取得
		static octonion<T> identity_element() {
			return octonion<T>();
		}
		//逆元の取得
		template <class = typename enable_if<is_exist_add_inverse_element<T>::value>::type>
		static octonion<T> inverse_element(const octonion<T>& x) {
			return -x;
		}
	};
	//乗法パラメータ取得
	template <class T>
	struct multiplicative<octonion<T>> {
		//単位元の取得
		static octonion<T> identity_element() {
			return octonion<T>(multiplicative<T>::identity_element());
		}
		//逆元の取得
		static octonion<T> inverse_element(const octonion<T>& x) {
			T temp = x[0] * x[0] + x[1] * x[1] + x[2] * x[2] + x[3] * x[3]
				+ x[4] * x[4] + x[5] * x[5] + x[6] * x[6] + x[7] * x[7];
			//共役を絶対値で割る
			return octonion<T>(x[0] / temp, -x[1] / temp, -x[2] / temp, -x[3] / temp
				, -x[4] / temp, -x[5] / temp, -x[6] / temp, -x[7] / temp);
		}
		//吸収元
		static octonion<T> absorbing_element() {
			return octonion<T>();
		}
	};
}

#endif