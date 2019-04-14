#ifndef IMATH_MATH_DUAL_NUMBERS_HPP
#define IMATH_MATH_DUAL_NUMBERS_HPP

#include "IMathLib/utility/utility/tuple_type.hpp"
#include "IMathLib/math/math.hpp"
#include "IMathLib/container/array.hpp"


namespace iml {

	template <class>
	class dual_numbers;


	template <class Base, class T, bool = is_algebraic_structure<T>::value, bool = is_same<Base, T>::value>
	class dual_numbers_base;


	//下に階層が存在しないかつBase == T
	template <class Base, class T>
	class dual_numbers_base<Base, T, false, true> {
		template <class> friend class dual_numbers;
		template <class, class, bool, bool> friend class dual_numbers_base;
	protected:
		Base x[2];
	public:
		constexpr dual_numbers_base() : x{} {}
		constexpr dual_numbers_base(const Base& re, const Base& im) : x{ re,im } {}
		constexpr dual_numbers_base(const Base& re) : x{ re } {}

		template <class = typename enable_if<is_exist_additive_inverse_v<Base>>::type>
		dual_numbers<Base> operator-() const { return dual_numbers<Base>(-this->x[0], -this->x[1]); }
		dual_numbers<Base> operator+() const { return dual_numbers<Base>(*this); }

		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		dual_numbers_base& operator+=(const dual_numbers<T>& n) {
			this->x[0] += n.x[0]; this->x[1] += n.x[1];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		dual_numbers_base& operator+=(const T& n) {
			this->x[0] += n;
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		dual_numbers_base& operator-=(const dual_numbers<T>& n) {
			this->x[0] -= n.x[0]; this->x[1] -= n.x[1];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		dual_numbers_base& operator-=(const T& n) {
			this->x[0] -= n;
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		dual_numbers_base& operator*=(const dual_numbers<T>& n) {
			Base temp[2] = { this->x[0] * n.x[0], this->x[0] * n.x[1] + this->x[1] * n.x[0] };
			this->x[0] = temp[0]; this->x[1] = temp[1];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		dual_numbers_base& operator*=(const T& k) {
			this->x[0] *= k; this->x[1] *= k;
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		dual_numbers_base& operator/=(const T& k) {
			this->x[0] /= k; this->x[1] /= k;
			return *this;
		}

		//アクセサ
		const constexpr Base& operator[](size_t index) const { return this->x[index]; }
		constexpr Base& operator[](size_t index) { return this->x[index]; }


		//2項演算
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend dual_numbers<Base> operator+(const dual_numbers<Base>& lhs, const dual_numbers<T>& rhs) {
			return dual_numbers<Base>(lhs[0] + rhs[0], lhs[1] + rhs[1]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend dual_numbers<Base> operator+(const dual_numbers<Base>& lhs, const T& rhs) {
			return dual_numbers<Base>(lhs[0] + rhs, lhs[1]);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::add_value>::type>
		friend dual_numbers<Base> operator+(const T& lhs, const dual_numbers<Base>& rhs) {
			return dual_numbers<Base>(lhs + rhs[0], rhs[1]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend dual_numbers<Base> operator-(const dual_numbers<Base>& lhs, const dual_numbers<T>& rhs) {
			return dual_numbers<Base>(lhs[0] - rhs[0], lhs[1] - rhs[1]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend dual_numbers<Base> operator-(const dual_numbers<Base>& lhs, const T& rhs) {
			return dual_numbers<Base>(lhs[0] - rhs, lhs[1]);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::sub_value>::type>
		friend dual_numbers<Base> operator-(const T& lhs, const dual_numbers<Base>& rhs) {
			return dual_numbers<Base>(lhs - rhs[0], -rhs[1]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend dual_numbers<Base> operator*(const dual_numbers<Base>& lhs, const dual_numbers<T>& rhs) {
			return dual_numbers<Base>(lhs[0] * rhs[0], lhs[0] * rhs[1] + lhs[1] * rhs[0]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend dual_numbers<Base> operator*(const dual_numbers<Base>& lhs, const T& rhs) {
			return dual_numbers<Base>(lhs[0] * rhs, lhs[1] * rhs);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::mul_value>::type>
		friend dual_numbers<Base> operator*(const T& lhs, const dual_numbers<Base>& rhs) {
			return dual_numbers<Base>(lhs * rhs[0], lhs * rhs[1]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		friend dual_numbers<Base> operator/(const dual_numbers<Base>& lhs, const T& rhs) {
			return dual_numbers<Base>(lhs[0] / rhs, lhs[1] / rhs);
		}
	};
	//下に階層が存在しないかつBase != T
	template <class Base, class T>
	class dual_numbers_base<Base, T, false, false> {
		template <class> friend class dual_numbers;
		template <class, class, bool, bool> friend class dual_numbers_base;
	protected:
		Base x[2];
	public:
		constexpr dual_numbers_base() : x{} {}
		constexpr dual_numbers_base(const Base& re, const Base& im) : x{ re,im } {}
		constexpr dual_numbers_base(const Base& re) : x{ re } {}
		template <class = typename enable_if<is_inclusion<T, Base>::value>::type>
		constexpr dual_numbers_base(const T& re, const T& im) : x{ static_cast<Base>(re),static_cast<Base>(im) } {}
		template <class = typename enable_if<is_inclusion<T, Base>::value>::type>
		constexpr dual_numbers_base(const T& re) : x{ static_cast<Base>(re) } {}

		template <class = typename enable_if<is_exist_additive_inverse_v<Base>>::type>
		dual_numbers<Base> operator-() const { return dual_numbers<Base>(-this->x[0], -this->x[1]); }
		dual_numbers<Base> operator+() const { return dual_numbers<Base>(*this); }

		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		dual_numbers_base& operator+=(const dual_numbers<T>& n) {
			this->x[0] += n.x[0]; this->x[1] += n.x[1];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		dual_numbers_base& operator+=(const T& n) {
			this->x[0] += n;
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		dual_numbers_base& operator-=(const dual_numbers<T>& n) {
			this->x[0] -= n.x[0]; this->x[1] -= n.x[1];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		dual_numbers_base& operator-=(const T& n) {
			this->x[0] -= n;
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		dual_numbers_base& operator*=(const dual_numbers<T>& n) {
			Base temp[2] = { this->x[0] * n.x[0], this->x[0] * n.x[1] + this->x[1] * n.x[0] };
			this->x[0] = temp[0]; this->x[1] = temp[1];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		dual_numbers_base& operator*=(const T& k) {
			this->x[0] *= k; this->x[1] *= k;
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		dual_numbers_base& operator/=(const T& k) {
			this->x[0] /= k; this->x[1] /= k;
			return *this;
		}

		//アクセサ
		const constexpr Base& operator[](size_t index) const { return this->x[index]; }
		constexpr Base& operator[](size_t index) { return this->x[index]; }


		//2項演算
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend dual_numbers<Base> operator+(const dual_numbers<Base>& lhs, const dual_numbers<T>& rhs) {
			return dual_numbers<Base>(lhs[0] + rhs[0], lhs[1] + rhs[1]);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::add_value>::type>
		friend dual_numbers<Base> operator+(const dual_numbers<T>& lhs, const dual_numbers<Base>& rhs) {
			return dual_numbers<Base>(lhs[0] + rhs[0], lhs[1] + rhs[1]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend dual_numbers<Base> operator+(const dual_numbers<Base>& lhs, const T& rhs) {
			return dual_numbers<Base>(lhs[0] + rhs, lhs[1]);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::add_value>::type>
		friend dual_numbers<Base> operator+(const T& lhs, const dual_numbers<Base>& rhs) {
			return dual_numbers<Base>(lhs + rhs[0], rhs[1]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend dual_numbers<Base> operator-(const dual_numbers<Base>& lhs, const dual_numbers<T>& rhs) {
			return dual_numbers<Base>(lhs[0] - rhs[0], lhs[1] - rhs[1]);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::sub_value>::type>
		friend dual_numbers<Base> operator-(const dual_numbers<T>& lhs, const dual_numbers<Base>& rhs) {
			return dual_numbers<Base>(lhs[0] - rhs[0], lhs[1] - rhs[1]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend dual_numbers<Base> operator-(const dual_numbers<Base>& lhs, const T& rhs) {
			return dual_numbers<Base>(lhs[0] - rhs, lhs[1]);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::sub_value>::type>
		friend dual_numbers<Base> operator-(const T& lhs, const dual_numbers<Base>& rhs) {
			return dual_numbers<Base>(lhs - rhs[0], -rhs[1]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend dual_numbers<Base> operator*(const dual_numbers<Base>& lhs, const dual_numbers<T>& rhs) {
			return dual_numbers<Base>(lhs[0] * rhs[0], lhs[0] * rhs[1] + lhs[1] * rhs[0]);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::mul_value>::type>
		friend dual_numbers<Base> operator*(const dual_numbers<T>& lhs, const dual_numbers<Base>& rhs) {
			return dual_numbers<Base>(lhs[0] * rhs[0], lhs[0] * rhs[1] + lhs[1] * rhs[0]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend dual_numbers<Base> operator*(const dual_numbers<Base>& lhs, const T& rhs) {
			return dual_numbers<Base>(lhs[0] * rhs, lhs[1] * rhs);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::mul_value>::type>
		friend dual_numbers<Base> operator*(const T& lhs, const dual_numbers<Base>& rhs) {
			return dual_numbers<Base>(lhs * rhs[0], lhs * rhs[1]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		friend dual_numbers<Base> operator/(const dual_numbers<Base>& lhs, const T& rhs) {
			return dual_numbers<Base>(lhs[0] / rhs, lhs[1] / rhs);
		}
	};
	//下に階層が存在するかつBase == T
	template <class Base, class T>
	class dual_numbers_base<Base, T, true, true> : public dual_numbers_base<Base, typename T::algebraic_type> {
		template <class> friend class dual_numbers;
		template <class, class, bool, bool> friend class dual_numbers_base;
	public:
		//コンストラクタの継承
		using dual_numbers_base<Base, typename T::algebraic_type>::dual_numbers_base;

		//単項演算の継承
		using dual_numbers_base<Base, typename T::algebraic_type>::operator+;
		using dual_numbers_base<Base, typename T::algebraic_type>::operator-;

		//代入演算の継承
		using dual_numbers_base<Base, typename T::algebraic_type>::operator+=;
		using dual_numbers_base<Base, typename T::algebraic_type>::operator-=;
		using dual_numbers_base<Base, typename T::algebraic_type>::operator*=;
		using dual_numbers_base<Base, typename T::algebraic_type>::operator/=;

		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		dual_numbers_base& operator+=(const dual_numbers<T>& n) {
			this->x[0] += n.x[0]; this->x[1] += n.x[1];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		dual_numbers_base& operator+=(const T& n) {
			this->x[0] += n;
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		dual_numbers_base& operator-=(const dual_numbers<T>& n) {
			this->x[0] -= n.x[0]; this->x[1] -= n.x[1];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		dual_numbers_base& operator-=(const T& n) {
			this->x[0] -= n;
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		dual_numbers_base& operator*=(const dual_numbers<T>& n) {
			Base temp[2] = { this->x[0] * n.x[0], this->x[0] * n.x[1] + this->x[1] * n.x[0] };
			this->x[0] = temp[0]; this->x[1] = temp[1];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		dual_numbers_base& operator*=(const T& k) {
			this->x[0] *= k; this->x[1] *= k;
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		dual_numbers_base& operator/=(const T& k) {
			this->x[0] /= k; this->x[1] /= k;
			return *this;
		}

		//添え字演算の継承
		using dual_numbers_base<Base, typename T::algebraic_type>::operator[];


		//2項演算
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend dual_numbers<Base> operator+(const dual_numbers<Base>& lhs, const dual_numbers<T>& rhs) {
			return dual_numbers<Base>(lhs[0] + rhs[0], lhs[1] + rhs[1]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend dual_numbers<Base> operator+(const dual_numbers<Base>& lhs, const T& rhs) {
			return dual_numbers<Base>(lhs[0] + rhs, lhs[1]);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::add_value>::type>
		friend dual_numbers<Base> operator+(const T& lhs, const dual_numbers<Base>& rhs) {
			return dual_numbers<Base>(lhs + rhs[0], rhs[1]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend dual_numbers<Base> operator-(const dual_numbers<Base>& lhs, const dual_numbers<T>& rhs) {
			return dual_numbers<Base>(lhs[0] - rhs[0], lhs[1] - rhs[1]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend dual_numbers<Base> operator-(const dual_numbers<Base>& lhs, const T& rhs) {
			return dual_numbers<Base>(lhs[0] - rhs, lhs[1]);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::sub_value>::type>
		friend dual_numbers<Base> operator-(const T& lhs, const dual_numbers<Base>& rhs) {
			return dual_numbers<Base>(lhs - rhs[0], -rhs[1]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend dual_numbers<Base> operator*(const dual_numbers<Base>& lhs, const dual_numbers<T>& rhs) {
			return dual_numbers<Base>(lhs[0] * rhs[0], lhs[0] * rhs[1] + lhs[1] * rhs[0]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend dual_numbers<Base> operator*(const dual_numbers<Base>& lhs, const T& rhs) {
			return dual_numbers<Base>(lhs[0] * rhs, lhs[1] * rhs);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::mul_value>::type>
		friend dual_numbers<Base> operator*(const T& lhs, const dual_numbers<Base>& rhs) {
			return dual_numbers<Base>(lhs * rhs[0], lhs * rhs[1]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		friend dual_numbers<Base> operator/(const dual_numbers<Base>& lhs, const T& rhs) {
			return dual_numbers<Base>(lhs[0] / rhs, lhs[1] / rhs);
		}
	};
	//下に階層が存在するかつBase != T
	template <class Base, class T>
	class dual_numbers_base<Base, T, true, false> : public dual_numbers_base<Base, typename T::algebraic_type> {
		template <class> friend class dual_numbers;
		template <class, class, bool, bool> friend class dual_numbers_base;
	public:
		//コンストラクタの継承
		using dual_numbers_base<Base, typename T::algebraic_type>::dual_numbers_base;

		constexpr dual_numbers_base() : dual_numbers_base<Base, typename T::algebraic_type>() {}
		template <class = typename enable_if<is_inclusion<T, Base>::value>::type>
		constexpr dual_numbers_base(const T& re, const T& im) : x{ static_cast<Base>(re),static_cast<Base>(im) } {}
		template <class = typename enable_if<is_inclusion<T, Base>::value>::type>
		constexpr dual_numbers_base(const T& re) : x{ static_cast<Base>(re) } {}

		//単項演算の継承
		using dual_numbers_base<Base, typename T::algebraic_type>::operator+;
		using dual_numbers_base<Base, typename T::algebraic_type>::operator-;

		//代入演算の継承
		using dual_numbers_base<Base, typename T::algebraic_type>::operator+=;
		using dual_numbers_base<Base, typename T::algebraic_type>::operator-=;
		using dual_numbers_base<Base, typename T::algebraic_type>::operator*=;
		using dual_numbers_base<Base, typename T::algebraic_type>::operator/=;

		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		dual_numbers_base& operator+=(const dual_numbers<T>& n) {
			this->x[0] += n.x[0]; this->x[1] += n.x[1];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		dual_numbers_base& operator+=(const T& n) {
			this->x[0] += n;
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		dual_numbers_base& operator-=(const dual_numbers<T>& n) {
			this->x[0] -= n.x[0]; this->x[1] -= n.x[1];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		dual_numbers_base& operator-=(const T& n) {
			this->x[0] -= n;
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		dual_numbers_base& operator*=(const dual_numbers<T>& n) {
			Base temp[2] = { this->x[0] * n.x[0], this->x[0] * n.x[1] + this->x[1] * n.x[0] };
			this->x[0] = temp[0]; this->x[1] = temp[1];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		dual_numbers_base& operator*=(const T& k) {
			this->x[0] *= k; this->x[1] *= k;
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		dual_numbers_base& operator/=(const T& k) {
			this->x[0] /= k; this->x[1] /= k;
			return *this;
		}

		//添え字演算の継承
		using dual_numbers_base<Base, typename T::algebraic_type>::operator[];


		//2項演算
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend dual_numbers<Base> operator+(const dual_numbers<Base>& lhs, const dual_numbers<T>& rhs) {
			return dual_numbers<Base>(lhs[0] + rhs[0], lhs[1] + rhs[1]);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::add_value>::type>
		friend dual_numbers<Base> operator+(const dual_numbers<T>& lhs, const dual_numbers<Base>& rhs) {
			return dual_numbers<Base>(lhs[0] + rhs[0], lhs[1] + rhs[1]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend dual_numbers<Base> operator+(const dual_numbers<Base>& lhs, const T& rhs) {
			return dual_numbers<Base>(lhs[0] + rhs, lhs[1]);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::add_value>::type>
		friend dual_numbers<Base> operator+(const T& lhs, const dual_numbers<Base>& rhs) {
			return dual_numbers<Base>(lhs + rhs[0], rhs[1]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend dual_numbers<Base> operator-(const dual_numbers<Base>& lhs, const dual_numbers<T>& rhs) {
			return dual_numbers<Base>(lhs[0] - rhs[0], lhs[1] - rhs[1]);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::sub_value>::type>
		friend dual_numbers<Base> operator-(const dual_numbers<T>& lhs, const dual_numbers<Base>& rhs) {
			return dual_numbers<Base>(lhs[0] - rhs[0], lhs[1] - rhs[1]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend dual_numbers<Base> operator-(const dual_numbers<Base>& lhs, const T& rhs) {
			return dual_numbers<Base>(lhs[0] - rhs, lhs[1]);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::sub_value>::type>
		friend dual_numbers<Base> operator-(const T& lhs, const dual_numbers<Base>& rhs) {
			return dual_numbers<Base>(lhs - rhs[0], -rhs[1]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend dual_numbers<Base> operator*(const dual_numbers<Base>& lhs, const dual_numbers<T>& rhs) {
			return dual_numbers<Base>(lhs[0] * rhs[0], lhs[0] * rhs[1] + lhs[1] * rhs[0]);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::mul_value>::type>
		friend dual_numbers<Base> operator*(const dual_numbers<T>& lhs, const dual_numbers<Base>& rhs) {
			return dual_numbers<Base>(lhs[0] * rhs[0], lhs[0] * rhs[1] + lhs[1] * rhs[0]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend dual_numbers<Base> operator*(const dual_numbers<Base>& lhs, const T& rhs) {
			return dual_numbers<Base>(lhs[0] * rhs, lhs[1] * rhs);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::mul_value>::type>
		friend dual_numbers<Base> operator*(const T& lhs, const dual_numbers<Base>& rhs) {
			return dual_numbers<Base>(lhs * rhs[0], lhs * rhs[1]);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		friend dual_numbers<Base> operator/(const dual_numbers<Base>& lhs, const T& rhs) {
			return dual_numbers<Base>(lhs[0] / rhs, lhs[1] / rhs);
		}
	};

	//二重数型
	template <class T>
	class dual_numbers : public dual_numbers_base<T, T>, public value_list_input<array_iterator<T>> {
		template <class> friend class dual_numbers;
		template <class, class, bool, bool> friend class dual_numbers_base;

		//代入演算の補助
		template <class _T>
		static dual_numbers* dual_numbers_copy(dual_numbers* const lhs, const dual_numbers<_T>& rhs) {
			//同じインスタンスでなければ代入
			if (static_cast<void*>(&lhs->x[0]) != static_cast<void*>(const_cast<_T*>(&rhs.x[0]))) {
				lhs->x[0] = static_cast<T>(rhs.x[0]); lhs->x[1] = static_cast<T>(rhs.x[1]);
			}
			return lhs;
		}
	public:
		constexpr dual_numbers() : dual_numbers_base<T, T>() {}
		template <class U>
		constexpr dual_numbers(const dual_numbers_base<T, U>& n)
			: dual_numbers_base<T, T>(n.x[0], n.x[1]) {}
		template <class U, class _T, class = typename enable_if<is_inclusion<_T, T>::value>::type>
		constexpr dual_numbers(const dual_numbers_base<_T, U>& n)
			: dual_numbers_base<T, T>(n.x[0], n.x[1]) {}

		//コンストラクタの継承
		using dual_numbers_base<T, T>::dual_numbers_base;

		using algebraic_type = T;
		using iterator = array_iterator<T>;
		using const_iterator = array_iterator<const T>;

		template<class Other>
		struct rebind {
			using other = dual_numbers<Other>;
		};

		iterator begin() noexcept { return iterator(&x[0]); }
		const_iterator begin() const noexcept { return const_iterator(&x[0]); }
		iterator end() noexcept { return iterator(&x[1] + 1); }
		const_iterator end() const noexcept { return const_iterator(&x[1] + 1); }

		//単項演算の継承
		using dual_numbers_base<T, T>::operator-;
		using dual_numbers_base<T, T>::operator+;
		//代入演算の継承
		using dual_numbers_base<T, T>::operator+=;
		using dual_numbers_base<T, T>::operator-=;
		using dual_numbers_base<T, T>::operator*=;
		using dual_numbers_base<T, T>::operator/=;
		//二項演算の継承
		using value_list_input<array_iterator<T>>::operator<<;

		//代入演算
		template <class U>
		dual_numbers& operator=(const dual_numbers_base<T, U>& n) { return *dual_numbers_copy(this, n); }
		template <class U, class _T, class = typename enable_if<is_inclusion<_T, T>::value>::type>
		dual_numbers& operator=(const dual_numbers_base<_T, U>& n) { return *dual_numbers_copy(this, n); }

		//添え字演算の継承
		using dual_numbers_base<T, T>::operator[];

		//メンバ取得
		template <class S, class = typename enable_if<is_inclusion<S, T>::value>::type>
		void real(const S& re) { this->x[0] = re; }
		constexpr T real() const { return this->x[0]; }
		template <class S, class = typename enable_if<is_inclusion<S, T>::value>::type>
		void imag(const S& im) { this->x[1] = im; }
		constexpr T imag() const { return this->x[1]; }

		//ストリーム出力
		friend std::ostream& operator<<(std::ostream& os, const dual_numbers& n) {
			os << '(' << n.x[0] << ',' << n.x[1] << ')';
			return os;
		}
		friend std::wostream& operator<<(std::wostream& os, const dual_numbers& n) {
			os << L'(' << n.x[0] << L',' << n.x[1] << L')';
			return os;
		}
	};


	//二重数の判定
	template <class T>
	struct _Is_dual_numbers : false_type {};
	template <class T>
	struct _Is_dual_numbers<dual_numbers<T>> : true_type {};
	template <class T>
	struct is_dual_numbers : _Is_dual_numbers<typename remove_cv<T>::type> {};

	//二重数の除去
	template <class T>
	struct remove_dual_numbers {
		using type = T;
	};
	template <class T>
	struct remove_dual_numbers<dual_numbers<T>> {
		using type = T;
	};

	//全ての二重数の除去
	template <class T>
	struct remove_all_dual_numbers {
		using type = T;
	};
	template <class T>
	struct remove_all_dual_numbers<dual_numbers<T>> : remove_all_dual_numbers<T> {};


	//比較演算
	template <class T1, class T2, size_t N, class = typename enable_if<is_calcable<T1, T2>::eq_value>::type>
	inline bool operator==(const dual_numbers<T1>& lhs, const dual_numbers<T2>& rhs) {
		return (lhs[0] == rhs[0]) && (lhs[1] == rhs[1]);
	}
	template <class T1, class T2, size_t N, class = typename enable_if<is_calcable<T1, T2>::eq_value>::type>
	inline bool operator!=(const dual_numbers<T1>& lhs, const dual_numbers<T2>& rhs) {
		return !(lhs == rhs);
	}

}

namespace iml {
	//加法パラメータ取得
	template <class T>
	struct addition_traits<dual_numbers<T>> {
		//単位元の取得
		static constexpr dual_numbers<T> identity_element() {
			return dual_numbers<T>();
		}
		//逆元の取得
		static constexpr dual_numbers<T> inverse_element(const dual_numbers<T>& x) {
			return -x;
		}
	};
	//乗法パラメータ取得
	template <class T>
	struct multiplication_traits<dual_numbers<T>> {
		//単位元の取得
		static constexpr dual_numbers<T> identity_element() {
			return dual_numbers<T>(multiplication_traits<T>::identity_element());
		}
		//逆元の取得
		static constexpr dual_numbers<T> inverse_element(const dual_numbers<T>& x) {
			//共役を絶対値の二乗で割る
			return dual_numbers<T>(1 / x[0], -x[1] / (x[0] * x[0]));
		}
		//吸収元
		static constexpr dual_numbers<T> absorbing_element() {
			return dual_numbers<T>();
		}
	};
}

namespace iml {

	//誤差評価
	template <class T>
	struct Error_evaluation<dual_numbers<T>> {
		static bool _error_evaluation_(const dual_numbers<T>& lhs, const dual_numbers<T>& rhs) {
			return error_evaluation(lhs[0], rhs[0]) && error_evaluation(lhs[1], rhs[1]);
		}
	};
}


#endif