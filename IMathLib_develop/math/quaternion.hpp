#ifndef IMATH_MATH_QUATERNION_HPP
#define IMATH_MATH_QUATERNION_HPP

#include "IMathLib/math/liner_algebra/vector.hpp"


namespace iml {

	template <class>
	class quaternion;


	template <class Base, class T, bool = is_algebraic_structure<T>::value, bool = is_same<Base, T>::value>
	class quaternion_base;


	//下に階層が存在しないかつBase == T
	template <class Base, class T>
	class quaternion_base<Base, T, false, true> {
		template <class> friend class quaternion;
		template <class, class, bool, bool> friend class quaternion_base;
	protected:
		Base x[4];
	public:
		constexpr quaternion_base() : x{} {}
		constexpr quaternion_base(const Base& re, const Base& im1, const Base& im2, const Base& im3)
			: x{ re,im1,im2,im3 } {}
		constexpr quaternion_base(const Base& re) : x{ re } {}

		template <class = typename enable_if<is_exist_add_inverse_element<Base>::value>::type>
		quaternion<Base> operator-() const { return quaternion<Base>(-this->x[0], -this->x[1], -this->x[2], -this->x[3]); }
		quaternion<Base> operator+() const { return quaternion<Base>(*this); }

		template <class U, class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		quaternion_base& operator+=(const quaternion_base<T, U>& n) {
			this->x[0] += n.x[0]; this->x[1] += n.x[1];
			this->x[2] += n.x[2]; this->x[3] += n.x[3];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		quaternion_base& operator+=(const T& n) {
			this->x[0] += n;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		quaternion_base& operator-=(const quaternion_base<T, U>& n) {
			this->x[0] -= n.x[0]; this->x[1] -= n.x[1];
			this->x[2] -= n.x[2]; this->x[3] -= n.x[3];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		quaternion_base& operator-=(const T& n) {
			this->x[0] -= n;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		quaternion_base& operator*=(const quaternion_base<T, U>& n) {
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
		quaternion_base& operator*=(const T& k) {
			this->x[0] *= k; this->x[1] *= k;
			this->x[2] *= k; this->x[3] *= k;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		quaternion_base& operator/=(const quaternion_base<T, U>& n) {
			//逆元は共役を絶対値の二乗で割る
			T temp = n.x[0] * n.x[0] + n.x[1] * n.x[1] + n.x[2] * n.x[2] + n.x[3] * n.x[3];
			return *this *= quaternion_base(n.x[0] / temp, -n.x[1] / temp, -n.x[2] / temp, -n.x[3] / temp);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		quaternion_base& operator/=(const T& k) {
			this->x[0] /= k; this->x[1] /= k;
			this->x[2] /= k; this->x[3] /= k;
			return *this;
		}

		//アクセサ
		const constexpr Base& operator[](imsize_t index) const { return this->x[index]; }
		constexpr Base& operator[](imsize_t index) { return this->x[index]; }


		//2項演算
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend quaternion<Base> operator+(const quaternion<Base>& lhs, const quaternion<T>& rhs) {
			return quaternion<Base>(lhs[0] + rhs[0], lhs[1] + rhs[1], lhs[2] + rhs[2], lhs[3] + rhs[3]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend quaternion<Base> operator+(const quaternion<Base>& lhs, const T& rhs) {
			return quaternion<Base>(lhs[0] + rhs, lhs[1], lhs[2], lhs[3]);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::add_value>::type>
		friend quaternion<Base> operator+(const T& lhs, const quaternion<Base>& rhs) {
			return quaternion<Base>(lhs + rhs[0], rhs[1], rhs[2], rhs[3]);
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend quaternion<Base> operator-(const quaternion<Base>& lhs, const quaternion<T>& rhs) {
			return quaternion<Base>(lhs[0] - rhs[0], lhs[1] - rhs[1], lhs[2] - rhs[2], lhs[3] - rhs[3]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend quaternion<Base> operator-(const quaternion<Base>& lhs, const T& rhs) {
			return quaternion<Base>(lhs[0] - rhs, lhs[1], lhs[2], lhs[3]);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::sub_value>::type>
		friend quaternion<Base> operator-(const T& lhs, const quaternion<Base>& rhs) {
			return quaternion<Base>(lhs - rhs[0], -rhs[1], -rhs[2], -rhs[3]);
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend quaternion<Base> operator*(const quaternion<Base>& lhs, const quaternion<T>& rhs) {
			return quaternion<Base>(lhs[0] * rhs[0] - (lhs[1] * rhs[1] + lhs[2] * rhs[2] + lhs[3] * rhs[3])
				, lhs[0] * rhs[1] + rhs[0] * lhs[1] + (lhs[2] * rhs[3] - lhs[3] * rhs[2])
				, lhs[0] * rhs[2] + rhs[0] * lhs[2] + (lhs[3] * rhs[1] - lhs[1] * rhs[3])
				, lhs[0] * rhs[3] + rhs[0] * lhs[3] + (lhs[1] * rhs[2] - lhs[2] * rhs[1]));
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend quaternion<Base> operator*(const quaternion<Base>& lhs, const T& rhs) {
			return quaternion<Base>(lhs[0] * rhs, lhs[1] * rhs, lhs[2] * rhs, lhs[3] * rhs);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::mul_value>::type>
		friend quaternion<Base> operator*(const T& lhs, const quaternion<Base>& rhs) {
			return quaternion<Base>(lhs * rhs[0], lhs * rhs[1], lhs * rhs[2], lhs * rhs[3]);
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		friend quaternion<Base> operator/(const quaternion<Base>& lhs, const quaternion<T>& rhs) {
			T temp = rhs[0] * rhs[0] + rhs[1] * rhs[1] + rhs[2] * rhs[2] + rhs[3] * rhs[3];
			return lhs * quaternion<T>(rhs[0] / temp, -rhs[1] / temp, -rhs[2] / temp, -rhs[3] / temp);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		friend quaternion<Base> operator/(const quaternion<Base>& lhs, const T& rhs) {
			return quaternion<Base>(lhs[0] / rhs, lhs[1] / rhs, lhs[2] / rhs, lhs[3] / rhs);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::div_value>::type>
		friend quaternion<Base> operator/(const T& lhs, const quaternion<Base>& rhs) {
			Base temp = rhs[0] * rhs[0] + rhs[1] * rhs[1] + rhs[2] * rhs[2] + rhs[3] * rhs[3];
			return lhs * quaternion<Base>(rhs[0] / temp, -rhs[1] / temp, -rhs[2] / temp, -rhs[3] / temp);
		}
	};
	//下に階層が存在しないかつBase != T
	template <class Base, class T>
	class quaternion_base<Base, T, false, false> {
		template <class> friend class quaternion;
		template <class, class, bool, bool> friend class quaternion_base;
	protected:
		Base x[4];
	public:
		constexpr quaternion_base() : x{} {}
		constexpr quaternion_base(const Base& re, const Base& im1, const Base& im2, const Base& im3)
			: x{ re,im1,im2,im3 } {}
		constexpr quaternion_base(const Base& re) : x{ re } {}
		template <class = typename enable_if<is_inclusion<T, Base>::value>::type>
		constexpr quaternion_base(const T& re, const T& im1, const T& im2, const T& im3)
			: x{ static_cast<Base>(re),static_cast<Base>(im1),
			static_cast<Base>(im2),static_cast<Base>(im3) } {}
		template <class = typename enable_if<is_inclusion<T, Base>::value>::type>
		constexpr quaternion_base(const T& re) : x{ static_cast<Base>(re) } {}

		template <class = typename enable_if<is_exist_add_inverse_element<Base>::value>::type>
		quaternion<Base> operator-() const { return quaternion<Base>(-this->x[0], -this->x[1], -this->x[2], -this->x[3]); }
		quaternion<Base> operator+() const { return quaternion<Base>(*this); }

		template <class U, class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		quaternion_base& operator+=(const quaternion_base<T, U>& n) {
			this->x[0] += n.x[0]; this->x[1] += n.x[1];
			this->x[2] += n.x[2]; this->x[3] += n.x[3];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		quaternion_base& operator+=(const T& n) {
			this->x[0] += n;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		quaternion_base& operator-=(const quaternion_base<T, U>& n) {
			this->x[0] -= n.x[0]; this->x[1] -= n.x[1];
			this->x[2] -= n.x[2]; this->x[3] -= n.x[3];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		quaternion_base& operator-=(const T& n) {
			this->x[0] -= n;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		quaternion_base& operator*=(const quaternion_base<T, U>& n) {
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
		quaternion_base& operator*=(const T& k) {
			this->x[0] *= k; this->x[1] *= k;
			this->x[2] *= k; this->x[3] *= k;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		quaternion_base& operator/=(const quaternion_base<T, U>& n) {
			//逆元は共役を絶対値の二乗で割る
			T temp = n.x[0] * n.x[0] + n.x[1] * n.x[1] + n.x[2] * n.x[2] + n.x[3] * n.x[3];
			return *this *= quaternion_base(n.x[0] / temp, -n.x[1] / temp, -n.x[2] / temp, -n.x[3] / temp);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		quaternion_base& operator/=(const T& k) {
			this->x[0] /= k; this->x[1] /= k;
			this->x[2] /= k; this->x[3] /= k;
			return *this;
		}

		//アクセサ
		const constexpr Base& operator[](imsize_t index) const { return this->x[index]; }
		constexpr Base& operator[](imsize_t index) { return this->x[index]; }


		//2項演算
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend quaternion<Base> operator+(const quaternion<Base>& lhs, const quaternion<T>& rhs) {
			return quaternion<Base>(lhs[0] + rhs[0], lhs[1] + rhs[1], lhs[2] + rhs[2], lhs[3] + rhs[3]);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::add_value>::type>
		friend quaternion<Base> operator+(const quaternion<T>& lhs, const quaternion<Base>& rhs) {
			return quaternion<Base>(lhs[0] + rhs[0], lhs[1] + rhs[1], lhs[2] + rhs[2], lhs[3] + rhs[3]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend quaternion<Base> operator+(const quaternion<Base>& lhs, const T& rhs) {
			return quaternion<Base>(lhs[0] + rhs, lhs[1], lhs[2], lhs[3]);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::add_value>::type>
		friend quaternion<Base> operator+(const T& lhs, const quaternion<Base>& rhs) {
			return quaternion<Base>(lhs + rhs[0], rhs[1], rhs[2], rhs[3]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend quaternion<Base> operator-(const quaternion<Base>& lhs, const quaternion<T>& rhs) {
			return quaternion<Base>(lhs[0] - rhs[0], lhs[1] - rhs[1], lhs[2] - rhs[2], lhs[3] - rhs[3]);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::sub_value>::type>
		friend quaternion<Base> operator-(const quaternion<T>& lhs, const quaternion<Base>& rhs) {
			return quaternion<Base>(lhs[0] - rhs[0], lhs[1] - rhs[1], lhs[2] - rhs[2], lhs[3] - rhs[3]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend quaternion<Base> operator-(const quaternion<Base>& lhs, const T& rhs) {
			return quaternion<Base>(lhs[0] - rhs, lhs[1], lhs[2], lhs[3]);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::sub_value>::type>
		friend quaternion<Base> operator-(const T& lhs, const quaternion<Base>& rhs) {
			return quaternion<Base>(lhs - rhs[0], -rhs[1], -rhs[2], -rhs[3]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend quaternion<Base> operator*(const quaternion<Base>& lhs, const quaternion<T>& rhs) {
			return quaternion<Base>(lhs[0] * rhs[0] - (lhs[1] * rhs[1] + lhs[2] * rhs[2] + lhs[3] * rhs[3])
				, lhs[0] * rhs[1] + rhs[0] * lhs[1] + (lhs[2] * rhs[3] - lhs[3] * rhs[2])
				, lhs[0] * rhs[2] + rhs[0] * lhs[2] + (lhs[3] * rhs[1] - lhs[1] * rhs[3])
				, lhs[0] * rhs[3] + rhs[0] * lhs[3] + (lhs[1] * rhs[2] - lhs[2] * rhs[1]));
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::mul_value>::type>
		friend quaternion<Base> operator*(const quaternion<T>& lhs, const quaternion<Base>& rhs) {
			return quaternion<Base>(lhs[0] * rhs[0] - (lhs[1] * rhs[1] + lhs[2] * rhs[2] + lhs[3] * rhs[3])
				, lhs[0] * rhs[1] + rhs[0] * lhs[1] + (lhs[2] * rhs[3] - lhs[3] * rhs[2])
				, lhs[0] * rhs[2] + rhs[0] * lhs[2] + (lhs[3] * rhs[1] - lhs[1] * rhs[3])
				, lhs[0] * rhs[3] + rhs[0] * lhs[3] + (lhs[1] * rhs[2] - lhs[2] * rhs[1]));
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend quaternion<Base> operator*(const quaternion<Base>& lhs, const T& rhs) {
			return quaternion<Base>(lhs[0] * rhs, lhs[1] * rhs, lhs[2] * rhs, lhs[3] * rhs);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::mul_value>::type>
		friend quaternion<Base> operator*(const T& lhs, const quaternion<Base>& rhs) {
			return quaternion<Base>(lhs * rhs[0], lhs * rhs[1], lhs * rhs[2], lhs * rhs[3]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		friend quaternion<Base> operator/(const quaternion<Base>& lhs, const quaternion<T>& rhs) {
			T temp = rhs[0] * rhs[0] + rhs[1] * rhs[1] + rhs[2] * rhs[2] + rhs[3] * rhs[3];
			return lhs * quaternion<T>(rhs[0] / temp, -rhs[1] / temp, -rhs[2] / temp, -rhs[3] / temp);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::div_value>::type>
		friend quaternion<Base> operator/(const quaternion<T>& lhs, const quaternion<Base>& rhs) {
			Base temp = rhs[0] * rhs[0] + rhs[1] * rhs[1] + rhs[2] * rhs[2] + rhs[3] * rhs[3];
			return lhs * quaternion<Base>(rhs[0] / temp, -rhs[1] / temp, -rhs[2] / temp, -rhs[3] / temp);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		friend quaternion<Base> operator/(const quaternion<Base>& lhs, const T& rhs) {
			return quaternion<Base>(lhs[0] / rhs, lhs[1] / rhs, lhs[2] / rhs, lhs[3] / rhs);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::div_value>::type>
		friend quaternion<Base> operator/(const T& lhs, const quaternion<Base>& rhs) {
			Base temp = rhs[0] * rhs[0] + rhs[1] * rhs[1] + rhs[2] * rhs[2] + rhs[3] * rhs[3];
			return lhs * quaternion<Base>(rhs[0] / temp, -rhs[1] / temp, -rhs[2] / temp, -rhs[3] / temp);
		}
	};
	//下に階層が存在するかつBase == T
	template <class Base, class T>
	class quaternion_base<Base, T, true, true> : public quaternion_base<Base, typename T::algebraic_type> {
		template <class> friend class quaternion;
		template <class, class, bool, bool> friend class quaternion_base;
	public:
		//コンストラクタの継承
		using quaternion_base<Base, typename T::algebraic_type>::quaternion_base;

		//単項演算の継承
		using quaternion_base<Base, typename T::algebraic_type>::operator+;
		using quaternion_base<Base, typename T::algebraic_type>::operator-;

		//代入演算の継承
		using quaternion_base<Base, typename T::algebraic_type>::operator+=;
		using quaternion_base<Base, typename T::algebraic_type>::operator-=;
		using quaternion_base<Base, typename T::algebraic_type>::operator*=;
		using quaternion_base<Base, typename T::algebraic_type>::operator/=;

		template <class U, class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		quaternion_base& operator+=(const quaternion_base<T, U>& n) {
			this->x[0] += n.x[0]; this->x[1] += n.x[1];
			this->x[2] += n.x[2]; this->x[3] += n.x[3];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		quaternion_base& operator+=(const T& n) {
			this->x[0] += n;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		quaternion_base& operator-=(const quaternion_base<T, U>& n) {
			this->x[0] -= n.x[0]; this->x[1] -= n.x[1];
			this->x[2] -= n.x[2]; this->x[3] -= n.x[3];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		quaternion_base& operator-=(const T& n) {
			this->x[0] -= n;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		quaternion_base& operator*=(const quaternion_base<T, U>& n) {
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
		quaternion_base& operator*=(const T& k) {
			this->x[0] *= k; this->x[1] *= k;
			this->x[2] *= k; this->x[3] *= k;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		quaternion_base& operator/=(const quaternion_base<T, U>& n) {
			//逆元は共役を絶対値の二乗で割る
			T temp = n.x[0] * n.x[0] + n.x[1] * n.x[1] + n.x[2] * n.x[2] + n.x[3] * n.x[3];
			return *this *= quaternion_base(n.x[0] / temp, -n.x[1] / temp, -n.x[2] / temp, -n.x[3] / temp);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		quaternion_base& operator/=(const T& k) {
			this->x[0] /= k; this->x[1] /= k;
			this->x[2] /= k; this->x[3] /= k;
			return *this;
		}

		//添え字演算の継承
		using quaternion_base<Base, typename T::algebraic_type>::operator[];


		//2項演算
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend quaternion<Base> operator+(const quaternion<Base>& lhs, const quaternion<T>& rhs) {
			return quaternion<Base>(lhs[0] + rhs[0], lhs[1] + rhs[1], lhs[2] + rhs[2], lhs[3] + rhs[3]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend quaternion<Base> operator+(const quaternion<Base>& lhs, const T& rhs) {
			return quaternion<Base>(lhs[0] + rhs, lhs[1], lhs[2], lhs[3]);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::add_value>::type>
		friend quaternion<Base> operator+(const T& lhs, const quaternion<Base>& rhs) {
			return quaternion<Base>(lhs + rhs[0], rhs[1], rhs[2], rhs[3]);
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend quaternion<Base> operator-(const quaternion<Base>& lhs, const quaternion<T>& rhs) {
			return quaternion<Base>(lhs[0] - rhs[0], lhs[1] - rhs[1], lhs[2] - rhs[2], lhs[3] - rhs[3]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend quaternion<Base> operator-(const quaternion<Base>& lhs, const T& rhs) {
			return quaternion<Base>(lhs[0] - rhs, lhs[1], lhs[2], lhs[3]);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::sub_value>::type>
		friend quaternion<Base> operator-(const T& lhs, const quaternion<Base>& rhs) {
			return quaternion<Base>(lhs - rhs[0], -rhs[1], -rhs[2], -rhs[3]);
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend quaternion<Base> operator*(const quaternion<Base>& lhs, const quaternion<T>& rhs) {
			return quaternion<Base>(lhs[0] * rhs[0] - (lhs[1] * rhs[1] + lhs[2] * rhs[2] + lhs[3] * rhs[3])
				, lhs[0] * rhs[1] + rhs[0] * lhs[1] + (lhs[2] * rhs[3] - lhs[3] * rhs[2])
				, lhs[0] * rhs[2] + rhs[0] * lhs[2] + (lhs[3] * rhs[1] - lhs[1] * rhs[3])
				, lhs[0] * rhs[3] + rhs[0] * lhs[3] + (lhs[1] * rhs[2] - lhs[2] * rhs[1]));
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend quaternion<Base> operator*(const quaternion<Base>& lhs, const T& rhs) {
			return quaternion<Base>(lhs[0] * rhs, lhs[1] * rhs, lhs[2] * rhs, lhs[3] * rhs);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::mul_value>::type>
		friend quaternion<Base> operator*(const T& lhs, const quaternion<Base>& rhs) {
			return quaternion<Base>(lhs * rhs[0], lhs * rhs[1], lhs * rhs[2], lhs * rhs[3]);
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		friend quaternion<Base> operator/(const quaternion<Base>& lhs, const quaternion<T>& rhs) {
			T temp = rhs[0] * rhs[0] + rhs[1] * rhs[1] + rhs[2] * rhs[2] + rhs[3] * rhs[3];
			return lhs * quaternion<T>(rhs[0] / temp, -rhs[1] / temp, -rhs[2] / temp, -rhs[3] / temp);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		friend quaternion<Base> operator/(const quaternion<Base>& lhs, const T& rhs) {
			return quaternion<Base>(lhs[0] / rhs, lhs[1] / rhs, lhs[2] / rhs, lhs[3] / rhs);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::div_value>::type>
		friend quaternion<Base> operator/(const T& lhs, const quaternion<Base>& rhs) {
			Base temp = rhs[0] * rhs[0] + rhs[1] * rhs[1] + rhs[2] * rhs[2] + rhs[3] * rhs[3];
			return lhs * quaternion<Base>(rhs[0] / temp, -rhs[1] / temp, -rhs[2] / temp, -rhs[3] / temp);
		}
	};
	//下に階層が存在するかつBase != T
	template <class Base, class T>
	class quaternion_base<Base, T, true, false> : public quaternion_base<Base, typename T::algebraic_type> {
		template <class> friend class quaternion;
		template <class, class, bool, bool> friend class quaternion_base;
	public:
		//コンストラクタの継承
		using quaternion_base<Base, typename T::algebraic_type>::quaternion_base;

		constexpr quaternion_base() : quaternion_base<Base, typename T::algebraic_type>() {}
		template <class = typename enable_if<is_inclusion<T, Base>::value>::type>
		constexpr quaternion_base(const T& re, const T& im1, const T& im2, const T& im3)
			: x{ static_cast<Base>(re),static_cast<Base>(im1),
			static_cast<Base>(im2),static_cast<Base>(im3) } {}
		template <class = typename enable_if<is_inclusion<T, Base>::value>::type>
		constexpr quaternion_base(const T& re) : x{ static_cast<Base>(re) } {}

		//単項演算の継承
		using quaternion_base<Base, typename T::algebraic_type>::operator+;
		using quaternion_base<Base, typename T::algebraic_type>::operator-;

		//代入演算の継承
		using quaternion_base<Base, typename T::algebraic_type>::operator+=;
		using quaternion_base<Base, typename T::algebraic_type>::operator-=;
		using quaternion_base<Base, typename T::algebraic_type>::operator*=;
		using quaternion_base<Base, typename T::algebraic_type>::operator/=;

		template <class U, class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		quaternion_base& operator+=(const quaternion_base<T, U>& n) {
			this->x[0] += n.x[0]; this->x[1] += n.x[1];
			this->x[2] += n.x[2]; this->x[3] += n.x[3];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		quaternion_base& operator+=(const T& n) {
			this->x[0] += n;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		quaternion_base& operator-=(const quaternion_base<T, U>& n) {
			this->x[0] -= n.x[0]; this->x[1] -= n.x[1];
			this->x[2] -= n.x[2]; this->x[3] -= n.x[3];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		quaternion_base& operator-=(const T& n) {
			this->x[0] -= n;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		quaternion_base& operator*=(const quaternion_base<T, U>& n) {
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
		quaternion_base& operator*=(const T& k) {
			this->x[0] *= k; this->x[1] *= k;
			this->x[2] *= k; this->x[3] *= k;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		quaternion_base& operator/=(const quaternion_base<T, U>& n) {
			//逆元は共役を絶対値の二乗で割る
			T temp = n.x[0] * n.x[0] + n.x[1] * n.x[1] + n.x[2] * n.x[2] + n.x[3] * n.x[3];
			return *this *= quaternion_base(n.x[0] / temp, -n.x[1] / temp, -n.x[2] / temp, -n.x[3] / temp);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		quaternion_base& operator/=(const T& k) {
			this->x[0] /= k; this->x[1] /= k;
			this->x[2] /= k; this->x[3] /= k;
			return *this;
		}

		//添え字演算の継承
		using quaternion_base<Base, typename T::algebraic_type>::operator[];


		//2項演算
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend quaternion<Base> operator+(const quaternion<Base>& lhs, const quaternion<T>& rhs) {
			return quaternion<Base>(lhs[0] + rhs[0], lhs[1] + rhs[1], lhs[2] + rhs[2], lhs[3] + rhs[3]);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::add_value>::type>
		friend quaternion<Base> operator+(const quaternion<T>& lhs, const quaternion<Base>& rhs) {
			return quaternion<Base>(lhs[0] + rhs[0], lhs[1] + rhs[1], lhs[2] + rhs[2], lhs[3] + rhs[3]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend quaternion<Base> operator+(const quaternion<Base>& lhs, const T& rhs) {
			return quaternion<Base>(lhs[0] + rhs, lhs[1], lhs[2], lhs[3]);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::add_value>::type>
		friend quaternion<Base> operator+(const T& lhs, const quaternion<Base>& rhs) {
			return quaternion<Base>(lhs + rhs[0], rhs[1], rhs[2], rhs[3]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend quaternion<Base> operator-(const quaternion<Base>& lhs, const quaternion<T>& rhs) {
			return quaternion<Base>(lhs[0] - rhs[0], lhs[1] - rhs[1], lhs[2] - rhs[2], lhs[3] - rhs[3]);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::sub_value>::type>
		friend quaternion<Base> operator-(const quaternion<T>& lhs, const quaternion<Base>& rhs) {
			return quaternion<Base>(lhs[0] - rhs[0], lhs[1] - rhs[1], lhs[2] - rhs[2], lhs[3] - rhs[3]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend quaternion<Base> operator-(const quaternion<Base>& lhs, const T& rhs) {
			return quaternion<Base>(lhs[0] - rhs, lhs[1], lhs[2], lhs[3]);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::sub_value>::type>
		friend quaternion<Base> operator-(const T& lhs, const quaternion<Base>& rhs) {
			return quaternion<Base>(lhs - rhs[0], -rhs[1], -rhs[2], -rhs[3]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend quaternion<Base> operator*(const quaternion<Base>& lhs, const quaternion<T>& rhs) {
			return quaternion<Base>(lhs[0] * rhs[0] - (lhs[1] * rhs[1] + lhs[2] * rhs[2] + lhs[3] * rhs[3])
				, lhs[0] * rhs[1] + rhs[0] * lhs[1] + (lhs[2] * rhs[3] - lhs[3] * rhs[2])
				, lhs[0] * rhs[2] + rhs[0] * lhs[2] + (lhs[3] * rhs[1] - lhs[1] * rhs[3])
				, lhs[0] * rhs[3] + rhs[0] * lhs[3] + (lhs[1] * rhs[2] - lhs[2] * rhs[1]));
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::mul_value>::type>
		friend quaternion<Base> operator*(const quaternion<T>& lhs, const quaternion<Base>& rhs) {
			return quaternion<Base>(lhs[0] * rhs[0] - (lhs[1] * rhs[1] + lhs[2] * rhs[2] + lhs[3] * rhs[3])
				, lhs[0] * rhs[1] + rhs[0] * lhs[1] + (lhs[2] * rhs[3] - lhs[3] * rhs[2])
				, lhs[0] * rhs[2] + rhs[0] * lhs[2] + (lhs[3] * rhs[1] - lhs[1] * rhs[3])
				, lhs[0] * rhs[3] + rhs[0] * lhs[3] + (lhs[1] * rhs[2] - lhs[2] * rhs[1]));
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend quaternion<Base> operator*(const quaternion<Base>& lhs, const T& rhs) {
			return quaternion<Base>(lhs[0] * rhs, lhs[1] * rhs, lhs[2] * rhs, lhs[3] * rhs);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::mul_value>::type>
		friend quaternion<Base> operator*(const T& lhs, const quaternion<Base>& rhs) {
			return quaternion<Base>(lhs * rhs[0], lhs * rhs[1], lhs * rhs[2], lhs * rhs[3]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		friend quaternion<Base> operator/(const quaternion<Base>& lhs, const quaternion<T>& rhs) {
			T temp = rhs[0] * rhs[0] + rhs[1] * rhs[1] + rhs[2] * rhs[2] + rhs[3] * rhs[3];
			return lhs * quaternion<T>(rhs[0] / temp, -rhs[1] / temp, -rhs[2] / temp, -rhs[3] / temp);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::div_value>::type>
		friend quaternion<Base> operator/(const quaternion<T>& lhs, const quaternion<Base>& rhs) {
			Base temp = rhs[0] * rhs[0] + rhs[1] * rhs[1] + rhs[2] * rhs[2] + rhs[3] * rhs[3];
			return lhs * quaternion<Base>(rhs[0] / temp, -rhs[1] / temp, -rhs[2] / temp, -rhs[3] / temp);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		friend quaternion<Base> operator/(const quaternion<Base>& lhs, const T& rhs) {
			return quaternion<Base>(lhs[0] / rhs, lhs[1] / rhs, lhs[2] / rhs, lhs[3] / rhs);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::div_value>::type>
		friend quaternion<Base> operator/(const T& lhs, const quaternion<Base>& rhs) {
			Base temp = rhs[0] * rhs[0] + rhs[1] * rhs[1] + rhs[2] * rhs[2] + rhs[3] * rhs[3];
			return lhs * quaternion<Base>(rhs[0] / temp, -rhs[1] / temp, -rhs[2] / temp, -rhs[3] / temp);
		}
	};

	//四元数型
	template <class T>
	class quaternion : public quaternion_base<T, T>, public value_list_input<array_iterator<T>> {
		template <class> friend class quaternion;

		//代入演算の補助
		template <class _T>
		static quaternion* quaternion_copy(quaternion* const lhs, const quaternion<_T>& rhs) {
			//同じインスタンスでなければ代入
			if (static_cast<void*>(&lhs->x[0]) != static_cast<void*>(const_cast<_T*>(&rhs.x[0]))) {
				lhs->x[0] = static_cast<T>(rhs.x[0]); lhs->x[1] = static_cast<T>(rhs.x[1]);
				lhs->x[2] = static_cast<T>(rhs.x[2]); lhs->x[3] = static_cast<T>(rhs.x[3]);
			}
			return lhs;
		}
	public:
		//コンストラクタの継承
		using quaternion_base<T, T>::quaternion_base;

		constexpr quaternion(const quaternion& n)
			: quaternion_base<T, T>(n.x[0], n.x[1], n.x[2], n.x[3]) {}
		template <class _T, class = typename enable_if<is_inclusion<_T, T>::value>::type>
		constexpr quaternion(const quaternion<_T>& n)
			: quaternion_base<T, T>(n.x[0], n.x[1], n.x[2], n.x[3]) {}
		constexpr quaternion() : quaternion_base<T, T>() {}
		template <class U>
		constexpr quaternion(const quaternion_base<T, U>& n)
			: quaternion_base<T, T>(n.x[0], n.x[1], n.x[2], n.x[3]) {}
		template <class U, class _T, class = typename enable_if<is_inclusion<_T, T>::value>::type>
		constexpr quaternion(const quaternion_base<_T, U>& n)
			: quaternion_base<T, T>(n.x[0], n.x[1], n.x[2], n.x[3]) {}

		constexpr quaternion(const T& re, const vector3<T>& im)
			: quaternion_base<T, T>(re, im[0], im[1], im[2]) {}

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
		using quaternion_base<T, T>::operator-;
		using quaternion_base<T, T>::operator+;
		//代入演算の継承
		using quaternion_base<T, T>::operator+=;
		using quaternion_base<T, T>::operator-=;
		using quaternion_base<T, T>::operator*=;
		using quaternion_base<T, T>::operator/=;
		//二項演算の継承
		using value_list_input<array_iterator<T>>::operator<<;

		//代入演算
		template <class U>
		quaternion& operator=(const quaternion_base<T, U>& n) { return *quaternion_copy(this, n); }
		template <class U, class _T, class = typename enable_if<is_inclusion<_T, T>::value>::type>
		quaternion& operator=(const quaternion_base<_T, U>& n) { return *quaternion_copy(this, n); }

		//添え字演算の継承
		using quaternion_base<T, T>::operator[];

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
	template <class T1, class T2, imsize_t N, class = typename enable_if<is_calcable<T1, T2>::eq_value>::type>
	inline bool operator==(const quaternion<T1>& lhs, const quaternion<T2>& rhs) {
		return (lhs[0] == rhs[0]) && (lhs[1] == rhs[1])
			&& (lhs[2] == rhs[2]) && (lhs[3] == rhs[3]);
	}
	template <class T1, class T2, imsize_t N, class = typename enable_if<is_calcable<T1, T2>::eq_value>::type>
	inline bool operator!=(const quaternion<T1>& lhs, const quaternion<T2>& rhs) {
		return !(lhs == rhs);
	}

}

namespace iml {
	//加法パラメータ取得
	template <class T>
	struct addition<quaternion<T>> {
		//単位元の取得
		static constexpr quaternion<T> identity_element() {
			return quaternion<T>();
		}
		//逆元の取得
		template <class = typename enable_if<is_exist_add_inverse_element<T>::value>::type>
		static constexpr quaternion<T> inverse_element(const quaternion<T>& x) {
			return -x;
		}
	};
	//乗法パラメータ取得
	template <class T>
	struct multiplicative<quaternion<T>> {
		//単位元の取得
		static constexpr quaternion<T> identity_element() {
			return quaternion<T>(multiplicative<T>::identity_element());
		}
		//逆元の取得
		static constexpr quaternion<T> inverse_element(const quaternion<T>& x) {
			//共役を絶対値の2乗で割る
			return quaternion<T>(x[0], -x[1], -x[2], -x[3]) / (x[0] * x[0] + x[1] * x[1] + x[2] * x[2] + x[3] * x[3]);
		}
		//吸収元
		static constexpr quaternion<T> absorbing_element() {
			return quaternion<T>();
		}
	};
}


#endif