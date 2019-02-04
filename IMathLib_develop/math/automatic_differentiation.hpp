#ifndef IMATH_MATH_AUTOMATIC_DIFFERENTIATION_HPP
#define IMATH_MATH_AUTOMATIC_DIFFERENTIATION_HPP

#include "IMathLib/utility/utility.hpp"
#include "IMathLib/utility/tuple.hpp"
#include "IMathLib/math/math.hpp"
#include "IMathLib/container/array.hpp"
//#include "IMathLib/math/liner_algebra/vector.hpp"

//二重数を利用した自動微分(関数値が不定であるときは結果は保証されない)

namespace iml {

	template<class, imsize_t>
	class bottomup_ad;

	//多重構造の演算に対応させるための補助型
	//継承コンストラクタと継承オペレータオーバーロードにより多数定義可能となる
	template <class, imsize_t, class, bool, class, bool>
	class _Bottomup_ad_base;

	//簡略にかくためのエイリアス
	template <class Base, imsize_t N, class T>
	using _Bottomup_ad_base_base_type = _Bottomup_ad_base<Base, N, typename T::algebraic_type
		, is_algebraic_structure<typename T::algebraic_type>::value, typename index_range<1, N>::type, is_same<Base, typename T::algebraic_type>::value>;
	template <class Base, imsize_t N, class T>
	using _Bottomup_ad_base_type = _Bottomup_ad_base<Base, N, T
		, is_algebraic_structure<T>::value, typename index_range<1, N>::type, is_same<Base, T>::value>;

	//下に階層が存在しないかつBase == T
	template <class Base, imsize_t N, class T, imsize_t... Indices>
	class _Bottomup_ad_base<Base, N, T, false, index_tuple<Indices...>, true> {
		template <class, imsize_t> friend class bottomup_ad;
		template <class, imsize_t, class, bool, class, bool> friend class _Bottomup_ad_base;
	protected:
		Base x[N];
	public:
		constexpr _Bottomup_ad_base() : x{} {}
		constexpr _Bottomup_ad_base(const Base& re) : x{ re } {}
		constexpr _Bottomup_ad_base(const Base& re, const typename identity_type<Base, Indices>::type&... x) : x{ re,x... } {}
		template <class U>
		constexpr _Bottomup_ad_base(const _Bottomup_ad_base_type<Base, N, U>& n) : x{ n.x[0], n.x[Indices]... } {}

		template <class = typename enable_if<is_exist_add_inverse_element<T>::value>::type>
		_Bottomup_ad_base operator-() const { return _Bottomup_ad_base(-this->x[0], (-this->x[Indices])...); }
		_Bottomup_ad_base operator+() const { return _Bottomup_ad_base(*this); }

		template <class U, class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		_Bottomup_ad_base& operator+=(const _Bottomup_ad_base_type<T, N, U>& n) {
			for (imsize_t i = 0; i < N; ++i) this->x[i] += n.x[i];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		_Bottomup_ad_base& operator+=(const T& n) {
			this->x[0] += n;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		_Bottomup_ad_base& operator-=(const _Bottomup_ad_base_type<T, N, U>& n) {
			for (imsize_t i = 0; i < N; ++i) this->x[i] -= n.x[i];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		_Bottomup_ad_base& operator-=(const T& n) {
			this->x[0] -= n;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		_Bottomup_ad_base& operator*=(const _Bottomup_ad_base_type<T, N, U>& n) {
			Base result[N] = { this->x[0] * n.x[0] };
			for (imsize_t i = 1; i < N; ++i) {
				Base temp = 1;
				result[i] = this->x[0] * n.x[i];
				for (imsize_t j = 1; j <= i; ++j) {
					temp *= Base(i + 1 - j) / j;
					result[i] += temp * this->x[j] * n.x[i - j];
				}
			}
			for (imsize_t i = 0; i < N; ++i) this->x[i] = result[i];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		_Bottomup_ad_base& operator*=(const T& k) {
			for (imsize_t i = 0; i < N; ++i) this->x[i] *= k;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		_Bottomup_ad_base& operator/=(const _Bottomup_ad_base_type<T, N, U>& n) {
			Base result[N] = { this->x[0] / n.x[0] };
			auto temp1 = multiplicative<bottomup_ad<T, N>>::inverse_element(n);
			for (imsize_t i = 1; i < N; ++i) {
				Base temp2 = 1;
				result[i] = this->x[0] * temp1.x[i];
				for (imsize_t j = 1; j <= i; ++j) {
					temp2 *= Base(i + 1 - j) / j;
					result[i] += temp2 * this->x[j] * temp1.x[i - j];
				}
			}
			for (imsize_t i = 0; i < N; ++i) this->x[i] = result[i];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		_Bottomup_ad_base& operator/=(const T& k) {
			for (imsize_t i = 0; i < N; ++i) this->x[i] /= k;
			return *this;
		}

		//アクセサ
		const Base& operator[](imsize_t index) const { return this->x[index]; }
		Base& operator[](imsize_t index) { return this->x[index]; }


		//二項演算
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend bottomup_ad<Base, N> operator+(const _Bottomup_ad_base& n1, const _Bottomup_ad_base_type<T, N, U>& n2) {
			return bottomup_ad<Base, N>(n1[0] + n2[0], (n1[Indices] + n2[Indices])...);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend bottomup_ad<Base, N> operator+(const _Bottomup_ad_base& n, const T& k) {
			return bottomup_ad<Base, N>(n[0] + k, n[Indices]...);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::add_value>::type>
		friend bottomup_ad<Base, N> operator+(const T& k, const _Bottomup_ad_base& n) {
			return bottomup_ad<Base, N>(k + n[0], n[Indices]...);
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend bottomup_ad<Base, N> operator-(const _Bottomup_ad_base& n1, const _Bottomup_ad_base_type<T, N, U>& n2) {
			return bottomup_ad<Base, N>(n1[0] - n2[0], (n1[Indices] - n2[Indices])...);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend bottomup_ad<Base, N> operator-(const _Bottomup_ad_base& n, const T& k) {
			return bottomup_ad<Base, N>(n[0] - k, n[Indices]...);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::sub_value>::type>
		friend bottomup_ad<Base, N> operator-(const T& k, const _Bottomup_ad_base& n) {
			return bottomup_ad<Base, N>(k - n[0], (-n[Indices])...);
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend bottomup_ad<Base, N> operator*(const _Bottomup_ad_base& n1, const _Bottomup_ad_base_type<T, N, U>& n2) {
			bottomup_ad<Base, N> result(n1[0] * n2[0]);
			for (imsize_t i = 1; i < N; ++i) {
				Base temp = 1;
				result[i] = n1[0] * n2[i];
				for (imsize_t j = 1; j <= i; ++j) {
					temp *= Base(i + 1 - j) / j;
					result[i] += temp * n1[j] * n2[i - j];
				}
			}
			return result;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend bottomup_ad<Base, N> operator*(const _Bottomup_ad_base& n, const T& k) {
			return bottomup_ad<Base, N>(n[0] * k, (n[Indices] * k)...);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::mul_value>::type>
		friend bottomup_ad<Base, N> operator*(const T& k, const _Bottomup_ad_base& n) {
			return bottomup_ad<Base, N>(k*n[0], (k*n[Indices])...);
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		friend bottomup_ad<Base, N> operator/(const _Bottomup_ad_base& n1, const _Bottomup_ad_base_type<T, N, U>& n2) {
			bottomup_ad<Base, N> result(n1[0] / n2[0]);
			bottomup_ad<Base, N> temp1 = multiplicative<bottomup_ad<T, N>>::inverse_element(n2);
			for (imsize_t i = 1; i < N; ++i) {
				Base temp2 = 1;
				result[i] = n1[0] * temp1[i];
				for (imsize_t j = 1; j <= i; ++j) {
					temp2 *= Base(i + 1 - j) / j;
					result[i] += temp2 * n1[j] * temp1[i - j];
				}
			}
			return result;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		friend bottomup_ad<Base, N> operator/(const _Bottomup_ad_base& n, const T& k) {
			return bottomup_ad<Base, N>(n[0] / k, (n[Indices] / k)...);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::div_value>::type>
		friend bottomup_ad<Base, N> operator/(const T& k, const _Bottomup_ad_base& n) {
			return multiplicative<bottomup_ad<Base, N>>::inverse_element(k*n);
		}


		//比較演算
		template <class U, class = typename enable_if<is_calcable<Base, T>::eq_value>::type>
		friend bool operator==(const _Bottomup_ad_base& n1, const _Bottomup_ad_base_type<T, N, U>& n2) {
			return n1[0] == n2[0];
		}
		template <class = typename enable_if<is_calcable<Base, T>::eq_value>::type>
		friend bool operator==(const _Bottomup_ad_base& n1, const T& n2) {
			return n1[0] == n2;
		}
		template <class = typename enable_if<is_calcable<T, Base>::eq_value>::type>
		friend bool operator==(const T& n1, const _Bottomup_ad_base& n2) {
			return n1 == n2[0];
		}
		template <class U, class = typename enable_if<is_calcable<Base, T>::eq_value>::type>
		friend bool operator!=(const _Bottomup_ad_base& n1, const _Bottomup_ad_base_type<T, N, U>& n2) {
			return n1[0] != n2[0];
		}
		template <class = typename enable_if<is_calcable<Base, T>::eq_value>::type>
		friend bool operator!=(const _Bottomup_ad_base& n1, const T& n2) {
			return n1[0] != n2;
		}
		template <class = typename enable_if<is_calcable<T, Base>::eq_value>::type>
		friend bool operator!=(const T& n1, const _Bottomup_ad_base& n2) {
			return n1 != n2[0];
		}
		template <class U, class = typename enable_if<is_calcable<Base, T>::lt_value>::type>
		friend bool operator<(const _Bottomup_ad_base& n1, const _Bottomup_ad_base_type<T, N, U>& n2) {
			return n1[0] < n2[0];
		}
		template <class = typename enable_if<is_calcable<Base, T>::lt_value>::type>
		friend bool operator<(const _Bottomup_ad_base& n1, const T& n2) {
			return n1[0] < n2;
		}
		template <class = typename enable_if<is_calcable<T, Base>::lt_value>::type>
		friend bool operator<(const T& n1, const _Bottomup_ad_base& n2) {
			return n1 < n2[0];
		}
		template <class U, class = typename enable_if<is_calcable<Base, T>::lt_eq_value>::type>
		friend bool operator<=(const _Bottomup_ad_base& n1, const _Bottomup_ad_base_type<T, N, U>& n2) {
			return n1[0] <= n2[0];
		}
		template <class = typename enable_if<is_calcable<Base, T>::lt_eq_value>::type>
		friend bool operator<=(const _Bottomup_ad_base& n1, const T& n2) {
			return n1[0] <= n2;
		}
		template <class = typename enable_if<is_calcable<T, Base>::lt_eq_value>::type>
		friend bool operator<=(const T& n1, const _Bottomup_ad_base& n2) {
			return n1 <= n2[0];
		}
		template <class U, class = typename enable_if<is_calcable<Base, T>::gt_value>::type>
		friend bool operator>(const _Bottomup_ad_base& n1, const _Bottomup_ad_base_type<T, N, U>& n2) {
			return n1[0] > n2[0];
		}
		template <class = typename enable_if<is_calcable<Base, T>::gt_value>::type>
		friend bool operator>(const _Bottomup_ad_base& n1, const T& n2) {
			return n1[0] > n2;
		}
		template <class = typename enable_if<is_calcable<T, Base>::gt_value>::type>
		friend bool operator>(const T& n1, const _Bottomup_ad_base& n2) {
			return n1 > n2[0];
		}
		template <class U, class = typename enable_if<is_calcable<Base, T>::gt_eq_value>::type>
		friend bool operator>=(const _Bottomup_ad_base& n1, const _Bottomup_ad_base_type<T, N, U>& n2) {
			return n1[0] >= n2[0];
		}
		template <class = typename enable_if<is_calcable<Base, T>::gt_eq_value>::type>
		friend bool operator>=(const _Bottomup_ad_base& n1, const T& n2) {
			return n1[0] >= n2;
		}
		template <class = typename enable_if<is_calcable<T, Base>::gt_eq_value>::type>
		friend bool operator>=(const T& n1, const _Bottomup_ad_base& n2) {
			return n1 >= n2[0];
		}
	};
	//下に階層が存在しないかつBase != T
	template <class Base, imsize_t N, class T, imsize_t... Indices>
	class _Bottomup_ad_base<Base, N, T, false, index_tuple<Indices...>, false> {
		template <class, imsize_t> friend class bottomup_ad;
		template <class, imsize_t, class, bool, class, bool> friend class _Bottomup_ad_base;
	protected:
		Base x[N];
	public:
		constexpr _Bottomup_ad_base() : x{} {}
		constexpr _Bottomup_ad_base(const Base& re) : x{ re } {}
		constexpr _Bottomup_ad_base(const Base& re, const typename identity_type<Base, Indices>::type&... x) : x{ re,x... } {}
		template <class = typename enable_if<is_inclusion<T, Base>::value>::type>
		constexpr _Bottomup_ad_base(const T& re) : x{ static_cast<Base>(re) } {}
		template <class = typename enable_if<is_inclusion<T, Base>::value>::type>
		constexpr _Bottomup_ad_base(const T& re, const typename identity_type<T, Indices>::type&... x) : x{ static_cast<Base>(re),static_cast<Base>(x)... } {}
		template <class U>
		constexpr _Bottomup_ad_base(const _Bottomup_ad_base_type<Base, N, U>& n) : x{ n.x[0], n.x[Indices]... } {}
		template <class U, class = typename enable_if<is_inclusion<T, Base>::value>::type>
		constexpr _Bottomup_ad_base(const _Bottomup_ad_base_type<T, N, U>& n) : x{ static_cast<Base>(n.x[0]), static_cast<Base>(n.x[Indices])... } {}


		template <class = typename enable_if<is_exist_add_inverse_element<T>::value>::type>
		_Bottomup_ad_base operator-() const { return _Bottomup_ad_base(-this->x[0], (-this->x[Indices])...); }
		_Bottomup_ad_base operator+() const { return _Bottomup_ad_base(*this); }

		template <class U, class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		_Bottomup_ad_base& operator+=(const _Bottomup_ad_base_type<T, N, U>& n) {
			for (imsize_t i = 0; i < N; ++i) this->x[i] += n.x[i];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		_Bottomup_ad_base& operator+=(const T& n) {
			this->x[0] += n;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		_Bottomup_ad_base& operator-=(const _Bottomup_ad_base_type<T, N, U>& n) {
			for (imsize_t i = 0; i < N; ++i) this->x[i] -= n.x[i];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		_Bottomup_ad_base& operator-=(const T& n) {
			this->x[0] -= n;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		_Bottomup_ad_base& operator*=(const _Bottomup_ad_base_type<T, N, U>& n) {
			Base result[N] = { this->x[0] * n.x[0] };
			for (imsize_t i = 1; i < N; ++i) {
				Base temp = 1;
				result[i] = this->x[0] * n.x[i];
				for (imsize_t j = 1; j <= i; ++j) {
					temp *= Base(i + 1 - j) / j;
					result[i] += temp * this->x[j] * n.x[i - j];
				}
			}
			for (imsize_t i = 0; i < N; ++i) this->x[i] = result[i];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		_Bottomup_ad_base& operator*=(const T& k) {
			for (imsize_t i = 0; i < N; ++i) this->x[i] *= k;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		_Bottomup_ad_base& operator/=(const _Bottomup_ad_base_type<T, N, U>& n) {
			Base result[N] = { this->x[0] / n.x[0] };
			auto temp1 = multiplicative<bottomup_ad<T, N>>::inverse_element(n);
			for (imsize_t i = 1; i < N; ++i) {
				Base temp2 = 1;
				result[i] = this->x[0] * temp1.x[i];
				for (imsize_t j = 1; j <= i; ++j) {
					temp2 *= Base(i + 1 - j) / j;
					result[i] += temp2 * this->x[j] * temp1.x[i - j];
				}
			}
			for (imsize_t i = 0; i < N; ++i) this->x[i] = result[i];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		_Bottomup_ad_base& operator/=(const T& k) {
			for (imsize_t i = 0; i < N; ++i) this->x[i] /= k;
			return *this;
		}

		//アクセサ(各要素を直接書き換えるのは不可)
		const Base& operator[](imsize_t index) const { return this->x[index]; }
		Base& operator[](imsize_t index) { return this->x[index]; }


		//二項演算
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend bottomup_ad<Base, N> operator+(const _Bottomup_ad_base& n1, const _Bottomup_ad_base_type<T, N, U>& n2) {
			return bottomup_ad<Base, N>(n1[0] + n2[0], (n1[Indices] + n2[Indices])...);
		}
		template <class U, class = typename enable_if<is_operation<T, Base, Base>::add_value>::type>
		friend bottomup_ad<Base, N> operator+(const _Bottomup_ad_base_type<T, N, U>& n1, const _Bottomup_ad_base& n2) {
			return bottomup_ad<Base, N>(n1[0] + n2[0], (n1[Indices] + n2[Indices])...);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend bottomup_ad<Base, N> operator+(const _Bottomup_ad_base& n, const T& k) {
			return bottomup_ad<Base, N>(n[0] + k, n[Indices]...);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::add_value>::type>
		friend bottomup_ad<Base, N> operator+(const T& k, const _Bottomup_ad_base& n) {
			return bottomup_ad<Base, N>(k + n[0], n[Indices]...);
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend bottomup_ad<Base, N> operator-(const _Bottomup_ad_base& n1, const _Bottomup_ad_base_type<T, N, U>& n2) {
			return bottomup_ad<Base, N>(n1[0] - n2[0], (n1[Indices] - n2[Indices])...);
		}
		template <class U, class = typename enable_if<is_operation<T, Base, Base>::sub_value>::type>
		friend bottomup_ad<Base, N> operator-(const _Bottomup_ad_base_type<T, N, U>& n1, const _Bottomup_ad_base& n2) {
			return bottomup_ad<Base, N>(n1[0] - n2[0], (n1[Indices] - n2[Indices])...);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend bottomup_ad<Base, N> operator-(const _Bottomup_ad_base& n, const T& k) {
			return bottomup_ad<Base, N>(n[0] - k, n[Indices]...);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::sub_value>::type>
		friend bottomup_ad<Base, N> operator-(const T& k, const _Bottomup_ad_base& n) {
			return bottomup_ad<Base, N>(k - n[0], (-n[Indices])...);
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend bottomup_ad<Base, N> operator*(const _Bottomup_ad_base& n1, const _Bottomup_ad_base_type<T, N, U>& n2) {
			bottomup_ad<Base, N> result(n1[0] * n2[0]);
			for (imsize_t i = 1; i < N; ++i) {
				Base temp = 1;
				result[i] = n1[0] * n2[i];
				for (imsize_t j = 1; j <= i; ++j) {
					temp *= Base(i + 1 - j) / j;
					result[i] += temp * n1[j] * n2[i - j];
				}
			}
			return result;
		}
		template <class U, class = typename enable_if<is_operation<T, Base, Base>::mul_value>::type>
		friend bottomup_ad<Base, N> operator*(const _Bottomup_ad_base_type<T, N, U>& n1, const _Bottomup_ad_base& n2) {
			bottomup_ad<Base, N> result(n1[0] * n2[0]);
			for (imsize_t i = 1; i < N; ++i) {
				Base temp = 1;
				result[i] = n1[0] * n2[i];
				for (imsize_t j = 1; j <= i; ++j) {
					temp *= Base(i + 1 - j) / j;
					result[i] += temp * n1[j] * n2[i - j];
				}
			}
			return result;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend bottomup_ad<Base, N> operator*(const _Bottomup_ad_base& n, const T& k) {
			return bottomup_ad<Base, N>(n[0] * k, (n[Indices] * k)...);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::mul_value>::type>
		friend bottomup_ad<Base, N> operator*(const T& k, const _Bottomup_ad_base& n) {
			return bottomup_ad<Base, N>(k*n[0], (k*n[Indices])...);
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		friend bottomup_ad<Base, N> operator/(const _Bottomup_ad_base& n1, const _Bottomup_ad_base_type<T, N, U>& n2) {
			bottomup_ad<Base, N> result(n1[0] / n2[0]);
			bottomup_ad<Base, N> temp1 = multiplicative<bottomup_ad<T, N>>::inverse_element(n2);
			for (imsize_t i = 1; i < N; ++i) {
				Base temp2 = 1;
				result[i] = n1[0] * temp1[i];
				for (imsize_t j = 1; j <= i; ++j) {
					temp2 *= Base(i + 1 - j) / j;
					result[i] += temp2 * n1[j] * temp1[i - j];
				}
			}
			return result;
		}
		template <class U, class = typename enable_if<is_operation<T, Base, Base>::div_value>::type>
		friend bottomup_ad<Base, N> operator/(const _Bottomup_ad_base_type<T, N, U>& n1, const _Bottomup_ad_base& n2) {
			bottomup_ad<Base, N> result(n1[0] / n2[0]);
			bottomup_ad<Base, N> temp1 = multiplicative<bottomup_ad<T, N>>::inverse_element(n2);
			for (imsize_t i = 1; i < N; ++i) {
				Base temp2 = 1;
				result[i] = n1[0] * temp1[i];
				for (imsize_t j = 1; j <= i; ++j) {
					temp2 *= Base(i + 1 - j) / j;
					result[i] += temp2 * n1[j] * temp1[i - j];
				}
			}
			return result;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		friend bottomup_ad<Base, N> operator/(const _Bottomup_ad_base& n, const T& k) {
			return bottomup_ad<Base, N>(n[0] / k, (n[Indices] / k)...);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::div_value>::type>
		friend bottomup_ad<Base, N> operator/(const T& k, const _Bottomup_ad_base& n) {
			return multiplicative<bottomup_ad<Base, N>>::inverse_element(k*n);
		}


		//比較演算
		template <class U, class = typename enable_if<is_calcable<Base, T>::eq_value>::type>
		friend bool operator==(const _Bottomup_ad_base& n1, const _Bottomup_ad_base_type<T, N, U>& n2) {
			return n1[0] == n2[0];
		}
		template <class U, class = typename enable_if<is_calcable<T, Base>::eq_value>::type>
		friend bool operator==(const _Bottomup_ad_base_type<T, N, U>& n1, const _Bottomup_ad_base& n2) {
			return n1[0] == n2[0];
		}
		template <class = typename enable_if<is_calcable<Base, T>::eq_value>::type>
		friend bool operator==(const _Bottomup_ad_base& n1, const T& n2) {
			return n1[0] == n2;
		}
		template <class = typename enable_if<is_calcable<T, Base>::eq_value>::type>
		friend bool operator==(const T& n1, const _Bottomup_ad_base& n2) {
			return n1 == n2[0];
		}
		template <class U, class = typename enable_if<is_calcable<Base, T>::eq_value>::type>
		friend bool operator!=(const _Bottomup_ad_base& n1, const _Bottomup_ad_base_type<T, N, U>& n2) {
			return n1[0] != n2[0];
		}
		template <class U, class = typename enable_if<is_calcable<T, Base>::eq_value>::type>
		friend bool operator!=(const _Bottomup_ad_base_type<T, N, U>& n1, const _Bottomup_ad_base& n2) {
			return n1[0] != n2[0];
		}
		template <class = typename enable_if<is_calcable<Base, T>::eq_value>::type>
		friend bool operator!=(const _Bottomup_ad_base& n1, const T& n2) {
			return n1[0] != n2;
		}
		template <class = typename enable_if<is_calcable<T, Base>::eq_value>::type>
		friend bool operator!=(const T& n1, const _Bottomup_ad_base& n2) {
			return n1 != n2[0];
		}
		template <class U, class = typename enable_if<is_calcable<Base, T>::lt_value>::type>
		friend bool operator<(const _Bottomup_ad_base& n1, const _Bottomup_ad_base_type<T, N, U>& n2) {
			return n1[0] < n2[0];
		}
		template <class U, class = typename enable_if<is_calcable<T, Base>::lt_value>::type>
		friend bool operator<(const _Bottomup_ad_base_type<T, N, U>& n1, const _Bottomup_ad_base& n2) {
			return n1[0] < n2[0];
		}
		template <class = typename enable_if<is_calcable<Base, T>::lt_value>::type>
		friend bool operator<(const _Bottomup_ad_base& n1, const T& n2) {
			return n1[0] < n2;
		}
		template <class = typename enable_if<is_calcable<T, Base>::lt_value>::type>
		friend bool operator<(const T& n1, const _Bottomup_ad_base& n2) {
			return n1 < n2[0];
		}
		template <class U, class = typename enable_if<is_calcable<Base, T>::lt_eq_value>::type>
		friend bool operator<=(const _Bottomup_ad_base& n1, const _Bottomup_ad_base_type<T, N, U>& n2) {
			return n1[0] <= n2[0];
		}
		template <class U, class = typename enable_if<is_calcable<T, Base>::lt_eq_value>::type>
		friend bool operator<=(const _Bottomup_ad_base_type<T, N, U>& n1, const _Bottomup_ad_base& n2) {
			return n1[0] <= n2[0];
		}
		template <class = typename enable_if<is_calcable<Base, T>::lt_eq_value>::type>
		friend bool operator<=(const _Bottomup_ad_base& n1, const T& n2) {
			return n1[0] <= n2;
		}
		template <class = typename enable_if<is_calcable<T, Base>::lt_eq_value>::type>
		friend bool operator<=(const T& n1, const _Bottomup_ad_base& n2) {
			return n1 <= n2[0];
		}
		template <class U, class = typename enable_if<is_calcable<Base, T>::gt_value>::type>
		friend bool operator>(const _Bottomup_ad_base& n1, const _Bottomup_ad_base_type<T, N, U>& n2) {
			return n1[0] > n2[0];
		}
		template <class U, class = typename enable_if<is_calcable<T, Base>::gt_value>::type>
		friend bool operator>(const _Bottomup_ad_base_type<T, N, U>& n1, const _Bottomup_ad_base& n2) {
			return n1[0] > n2[0];
		}
		template <class = typename enable_if<is_calcable<Base, T>::gt_value>::type>
		friend bool operator>(const _Bottomup_ad_base& n1, const T& n2) {
			return n1[0] > n2;
		}
		template <class = typename enable_if<is_calcable<T, Base>::gt_value>::type>
		friend bool operator>(const T& n1, const _Bottomup_ad_base& n2) {
			return n1 > n2[0];
		}
		template <class U, class = typename enable_if<is_calcable<Base, T>::gt_eq_value>::type>
		friend bool operator>=(const _Bottomup_ad_base& n1, const _Bottomup_ad_base_type<T, N, U>& n2) {
			return n1[0] >= n2[0];
		}
		template <class U, class = typename enable_if<is_calcable<T, Base>::gt_eq_value>::type>
		friend bool operator>=(const _Bottomup_ad_base_type<T, N, U>& n1, const _Bottomup_ad_base& n2) {
			return n1[0] >= n2[0];
		}
		template <class = typename enable_if<is_calcable<Base, T>::gt_eq_value>::type>
		friend bool operator>=(const _Bottomup_ad_base& n1, const T& n2) {
			return n1[0] >= n2;
		}
		template <class = typename enable_if<is_calcable<T, Base>::gt_eq_value>::type>
		friend bool operator>=(const T& n1, const _Bottomup_ad_base& n2) {
			return n1 >= n2[0];
		}
	};
	//下に階層が存在するかつBase == T
	template <class Base, imsize_t N, class T, imsize_t... Indices>
	class _Bottomup_ad_base<Base, N, T, true, index_tuple<Indices...>, true> : public _Bottomup_ad_base_base_type<Base, N, T> {
		template <class, imsize_t> friend class bottomup_ad;
		template <class, imsize_t, class, bool, class, bool> friend class _Bottomup_ad_base;
	public:
		//コンストラクタの継承
		using _Bottomup_ad_base_base_type<Base, N, T>::_Bottomup_ad_base;

		//単項演算の継承
		using _Bottomup_ad_base_base_type<Base, N, T>::operator+;
		using _Bottomup_ad_base_base_type<Base, N, T>::operator-;

		//代入演算の継承
		using _Bottomup_ad_base_base_type<Base, N, T>::operator+=;
		using _Bottomup_ad_base_base_type<Base, N, T>::operator-=;
		using _Bottomup_ad_base_base_type<Base, N, T>::operator*=;
		using _Bottomup_ad_base_base_type<Base, N, T>::operator/=;

		template <class U, class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		_Bottomup_ad_base& operator+=(const _Bottomup_ad_base_type<T, N, U>& n) {
			for (imsize_t i = 0; i < N; ++i) this->x[i] += n.x[i];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		_Bottomup_ad_base& operator+=(const T& n) {
			this->x[0] += n;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		_Bottomup_ad_base& operator-=(const _Bottomup_ad_base_type<T, N, U>& n) {
			for (imsize_t i = 0; i < N; ++i) this->x[i] -= n.x[i];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		_Bottomup_ad_base& operator-=(const T& n) {
			this->x[0] -= n;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		_Bottomup_ad_base& operator*=(const _Bottomup_ad_base_type<T, N, U>& n) {
			Base result[N] = { this->x[0] * n.x[0] };
			for (imsize_t i = 1; i < N; ++i) {
				Base temp = 1;
				result[i] = this->x[0] * n.x[i];
				for (imsize_t j = 1; j <= i; ++j) {
					temp *= Base(i + 1 - j) / j;
					result[i] += temp * this->x[j] * n.x[i - j];
				}
			}
			for (imsize_t i = 0; i < N; ++i) this->x[i] = result[i];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		_Bottomup_ad_base& operator*=(const T& k) {
			for (imsize_t i = 0; i < N; ++i) this->x[i] *= k;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		_Bottomup_ad_base& operator/=(const _Bottomup_ad_base_type<T, N, U>& n) {
			Base result[N] = { this->x[0] / n.x[0] };
			auto temp1 = multiplicative<bottomup_ad<T, N>>::inverse_element(n);
			for (imsize_t i = 1; i < N; ++i) {
				Base temp2 = 1;
				result[i] = this->x[0] * temp1.x[i];
				for (imsize_t j = 1; j <= i; ++j) {
					temp2 *= Base(i + 1 - j) / j;
					result[i] += temp2 * this->x[j] * temp1.x[i - j];
				}
			}
			for (imsize_t i = 0; i < N; ++i) this->x[i] = result[i];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		_Bottomup_ad_base& operator/=(const T& k) {
			for (imsize_t i = 0; i < N; ++i) this->x[i] /= k;
			return *this;
		}

		//添え字演算の継承
		using _Bottomup_ad_base_base_type<Base, N, T>::operator[];


		//二項演算
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend bottomup_ad<Base, N> operator+(const _Bottomup_ad_base& n1, const _Bottomup_ad_base_type<T, N, U>& n2) {
			return bottomup_ad<Base, N>(n1[0] + n2[0], (n1[Indices] + n2[Indices])...);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend bottomup_ad<Base, N> operator+(const _Bottomup_ad_base& n, const T& k) {
			return bottomup_ad<Base, N>(n[0] + k, n[Indices]...);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::add_value>::type>
		friend bottomup_ad<Base, N> operator+(const T& k, const _Bottomup_ad_base& n) {
			return bottomup_ad<Base, N>(k + n[0], n[Indices]...);
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend bottomup_ad<Base, N> operator-(const _Bottomup_ad_base& n1, const _Bottomup_ad_base_type<T, N, U>& n2) {
			return bottomup_ad<Base, N>(n1[0] - n2[0], (n1[Indices] - n2[Indices])...);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend bottomup_ad<Base, N> operator-(const _Bottomup_ad_base& n, const T& k) {
			return bottomup_ad<Base, N>(n[0] - k, n[Indices]...);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::sub_value>::type>
		friend bottomup_ad<Base, N> operator-(const T& k, const _Bottomup_ad_base& n) {
			return bottomup_ad<Base, N>(k - n[0], (-n[Indices])...);
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend bottomup_ad<Base, N> operator*(const _Bottomup_ad_base& n1, const _Bottomup_ad_base_type<T, N, U>& n2) {
			bottomup_ad<Base, N> result(n1[0] * n2[0]);
			for (imsize_t i = 1; i < N; ++i) {
				Base temp = 1;
				result[i] = n1[0] * n2[i];
				for (imsize_t j = 1; j <= i; ++j) {
					temp *= Base(i + 1 - j) / j;
					result[i] += temp * n1[j] * n2[i - j];
				}
			}
			return result;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend bottomup_ad<Base, N> operator*(const _Bottomup_ad_base& n, const T& k) {
			return bottomup_ad<Base, N>(n[0] * k, (n[Indices] * k)...);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::mul_value>::type>
		friend bottomup_ad<Base, N> operator*(const T& k, const _Bottomup_ad_base& n) {
			return bottomup_ad<Base, N>(k*n[0], (k*n[Indices])...);
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		friend bottomup_ad<Base, N> operator/(const _Bottomup_ad_base& n1, const _Bottomup_ad_base_type<T, N, U>& n2) {
			bottomup_ad<Base, N> result(n1[0] / n2[0]);
			bottomup_ad<Base, N> temp1 = multiplicative<bottomup_ad<T, N>>::inverse_element(n2);
			for (imsize_t i = 1; i < N; ++i) {
				Base temp2 = 1;
				result[i] = n1[0] * temp1[i];
				for (imsize_t j = 1; j <= i; ++j) {
					temp2 *= Base(i + 1 - j) / j;
					result[i] += temp2 * n1[j] * temp1[i - j];
				}
			}
			return result;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		friend bottomup_ad<Base, N> operator/(const _Bottomup_ad_base& n, const T& k) {
			return bottomup_ad<Base, N>(n[0] / k, (n[Indices] / k)...);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::div_value>::type>
		friend bottomup_ad<Base, N> operator/(const T& k, const _Bottomup_ad_base& n) {
			return multiplicative<bottomup_ad<Base, N>>::inverse_element(k*n);
		}


		//比較演算
		template <class U, class = typename enable_if<is_calcable<Base, T>::eq_value>::type>
		friend bool operator==(const _Bottomup_ad_base& n1, const _Bottomup_ad_base_type<T, N, U>& n2) {
			return n1[0] == n2[0];
		}
		template <class = typename enable_if<is_calcable<Base, T>::eq_value>::type>
		friend bool operator==(const _Bottomup_ad_base& n1, const T& n2) {
			return n1[0] == n2;
		}
		template <class = typename enable_if<is_calcable<T, Base>::eq_value>::type>
		friend bool operator==(const T& n1, const _Bottomup_ad_base& n2) {
			return n1 == n2[0];
		}
		template <class U, class = typename enable_if<is_calcable<Base, T>::eq_value>::type>
		friend bool operator!=(const _Bottomup_ad_base& n1, const _Bottomup_ad_base_type<T, N, U>& n2) {
			return n1[0] != n2[0];
		}
		template <class = typename enable_if<is_calcable<Base, T>::eq_value>::type>
		friend bool operator!=(const _Bottomup_ad_base& n1, const T& n2) {
			return n1[0] != n2;
		}
		template <class = typename enable_if<is_calcable<T, Base>::eq_value>::type>
		friend bool operator!=(const T& n1, const _Bottomup_ad_base& n2) {
			return n1 != n2[0];
		}
		template <class U, class = typename enable_if<is_calcable<Base, T>::lt_value>::type>
		friend bool operator<(const _Bottomup_ad_base& n1, const _Bottomup_ad_base_type<T, N, U>& n2) {
			return n1[0] < n2[0];
		}
		template <class = typename enable_if<is_calcable<Base, T>::lt_value>::type>
		friend bool operator<(const _Bottomup_ad_base& n1, const T& n2) {
			return n1[0] < n2;
		}
		template <class = typename enable_if<is_calcable<T, Base>::lt_value>::type>
		friend bool operator<(const T& n1, const _Bottomup_ad_base& n2) {
			return n1 < n2[0];
		}
		template <class U, class = typename enable_if<is_calcable<Base, T>::lt_eq_value>::type>
		friend bool operator<=(const _Bottomup_ad_base& n1, const _Bottomup_ad_base_type<T, N, U>& n2) {
			return n1[0] <= n2[0];
		}
		template <class = typename enable_if<is_calcable<Base, T>::lt_eq_value>::type>
		friend bool operator<=(const _Bottomup_ad_base& n1, const T& n2) {
			return n1[0] <= n2;
		}
		template <class = typename enable_if<is_calcable<T, Base>::lt_eq_value>::type>
		friend bool operator<=(const T& n1, const _Bottomup_ad_base& n2) {
			return n1 <= n2[0];
		}
		template <class U, class = typename enable_if<is_calcable<Base, T>::gt_value>::type>
		friend bool operator>(const _Bottomup_ad_base& n1, const _Bottomup_ad_base_type<T, N, U>& n2) {
			return n1[0] > n2[0];
		}
		template <class = typename enable_if<is_calcable<Base, T>::gt_value>::type>
		friend bool operator>(const _Bottomup_ad_base& n1, const T& n2) {
			return n1[0] > n2;
		}
		template <class = typename enable_if<is_calcable<T, Base>::gt_value>::type>
		friend bool operator>(const T& n1, const _Bottomup_ad_base& n2) {
			return n1 > n2[0];
		}
		template <class U, class = typename enable_if<is_calcable<Base, T>::gt_eq_value>::type>
		friend bool operator>=(const _Bottomup_ad_base& n1, const _Bottomup_ad_base_type<T, N, U>& n2) {
			return n1[0] >= n2[0];
		}
		template <class = typename enable_if<is_calcable<Base, T>::gt_eq_value>::type>
		friend bool operator>=(const _Bottomup_ad_base& n1, const T& n2) {
			return n1[0] >= n2;
		}
		template <class = typename enable_if<is_calcable<T, Base>::gt_eq_value>::type>
		friend bool operator>=(const T& n1, const _Bottomup_ad_base& n2) {
			return n1 >= n2[0];
		}
	};
	//下に階層が存在するかつBase != T
	template <class Base, imsize_t N, class T, imsize_t... Indices>
	class _Bottomup_ad_base<Base, N, T, true, index_tuple<Indices...>, false> : public _Bottomup_ad_base_base_type<Base, N, T> {
		template <class, imsize_t> friend class bottomup_ad;
		template <class, imsize_t, class, bool, class, bool> friend class _Bottomup_ad_base;
	public:
		//コンストラクタの継承
		using _Bottomup_ad_base_base_type<Base, N, T>::_Bottomup_ad_base;

		constexpr _Bottomup_ad_base() : _Bottomup_ad_base_base_type<Base, N, T>() {}
		template <class = typename enable_if<is_inclusion<T, Base>::value>::type>
		constexpr _Bottomup_ad_base(const T& re) : _Bottomup_ad_base_base_type<Base, N, T>(static_cast<Base>(re)) {}
		template <class = typename enable_if<is_inclusion<T, Base>::value>::type>
		constexpr _Bottomup_ad_base(const T& re, const typename identity_type<T, Indices>::type&... x) : _Bottomup_ad_base_base_type<Base, N, T>(static_cast<Base>(re), static_cast<Base>(x)...) {}
		template <class U, class = typename enable_if<is_inclusion<T, Base>::value>::type>
		constexpr _Bottomup_ad_base(const _Bottomup_ad_base_type<T, N, U>& n) : _Bottomup_ad_base_base_type<Base, N, T>(static_cast<Base>(n.x[0]), static_cast<Base>(n.x[Indices])...) {}

		//単項演算の継承
		using _Bottomup_ad_base_base_type<Base, N, T>::operator+;
		using _Bottomup_ad_base_base_type<Base, N, T>::operator-;

		//代入演算の継承
		using _Bottomup_ad_base_base_type<Base, N, T>::operator+=;
		using _Bottomup_ad_base_base_type<Base, N, T>::operator-=;
		using _Bottomup_ad_base_base_type<Base, N, T>::operator*=;
		using _Bottomup_ad_base_base_type<Base, N, T>::operator/=;

		template <class U, class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		_Bottomup_ad_base& operator+=(const _Bottomup_ad_base_type<T, N, U>& n) {
			for (imsize_t i = 0; i < N; ++i) this->x[i] += n.x[i];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		_Bottomup_ad_base& operator+=(const T& n) {
			this->x[0] += n;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		_Bottomup_ad_base& operator-=(const _Bottomup_ad_base_type<T, N, U>& n) {
			for (imsize_t i = 0; i < N; ++i) this->x[i] -= n.x[i];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		_Bottomup_ad_base& operator-=(const T& n) {
			this->x[0] -= n;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		_Bottomup_ad_base& operator*=(const _Bottomup_ad_base_type<T, N, U>& n) {
			Base result[N] = { this->x[0] * n.x[0] };
			for (imsize_t i = 1; i < N; ++i) {
				Base temp = 1;
				result[i] = this->x[0] * n.x[i];
				for (imsize_t j = 1; j <= i; ++j) {
					temp *= Base(i + 1 - j) / j;
					result[i] += temp * this->x[j] * n.x[i - j];
				}
			}
			for (imsize_t i = 0; i < N; ++i) this->x[i] = result[i];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		_Bottomup_ad_base& operator*=(const T& k) {
			for (imsize_t i = 0; i < N; ++i) this->x[i] *= k;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		_Bottomup_ad_base& operator/=(const _Bottomup_ad_base_type<T, N, U>& n) {
			Base result[N] = { this->x[0] / n.x[0] };
			auto temp1 = multiplicative<bottomup_ad<T, N>>::inverse_element(n);
			for (imsize_t i = 1; i < N; ++i) {
				Base temp2 = 1;
				result[i] = this->x[0] * temp1.x[i];
				for (imsize_t j = 1; j <= i; ++j) {
					temp2 *= Base(i + 1 - j) / j;
					result[i] += temp2 * this->x[j] * temp1.x[i - j];
				}
			}
			for (imsize_t i = 0; i < N; ++i) this->x[i] = result[i];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		_Bottomup_ad_base& operator/=(const T& k) {
			for (imsize_t i = 0; i < N; ++i) this->x[i] /= k;
			return *this;
		}

		//添え字演算の継承
		using _Bottomup_ad_base_base_type<Base, N, T>::operator[];


		//二項演算
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend bottomup_ad<Base, N> operator+(const _Bottomup_ad_base& n1, const _Bottomup_ad_base_type<T, N, U>& n2) {
			return bottomup_ad<Base, N>(n1[0] + n2[0], (n1[Indices] + n2[Indices])...);
		}
		template <class U, class = typename enable_if<is_operation<T, Base, Base>::add_value>::type>
		friend bottomup_ad<Base, N> operator+(const _Bottomup_ad_base_type<T, N, U>& n1, const _Bottomup_ad_base& n2) {
			return bottomup_ad<Base, N>(n1[0] + n2[0], (n1[Indices] + n2[Indices])...);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend bottomup_ad<Base, N> operator+(const _Bottomup_ad_base& n, const T& k) {
			return bottomup_ad<Base, N>(n[0] + k, n[Indices]...);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::add_value>::type>
		friend bottomup_ad<Base, N> operator+(const T& k, const _Bottomup_ad_base& n) {
			return bottomup_ad<Base, N>(k + n[0], n[Indices]...);
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend bottomup_ad<Base, N> operator-(const _Bottomup_ad_base& n1, const _Bottomup_ad_base_type<T, N, U>& n2) {
			return bottomup_ad<Base, N>(n1[0] - n2[0], (n1[Indices] - n2[Indices])...);
		}
		template <class U, class = typename enable_if<is_operation<T, Base, Base>::sub_value>::type>
		friend bottomup_ad<Base, N> operator-(const _Bottomup_ad_base_type<T, N, U>& n1, const _Bottomup_ad_base& n2) {
			return bottomup_ad<Base, N>(n1[0] - n2[0], (n1[Indices] - n2[Indices])...);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend bottomup_ad<Base, N> operator-(const _Bottomup_ad_base& n, const T& k) {
			return bottomup_ad<Base, N>(n[0] - k, n[Indices]...);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::sub_value>::type>
		friend bottomup_ad<Base, N> operator-(const T& k, const _Bottomup_ad_base& n) {
			return bottomup_ad<Base, N>(k - n[0], (-n[Indices])...);
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend bottomup_ad<Base, N> operator*(const _Bottomup_ad_base& n1, const _Bottomup_ad_base_type<T, N, U>& n2) {
			bottomup_ad<Base, N> result(n1[0] * n2[0]);
			for (imsize_t i = 1; i < N; ++i) {
				Base temp = 1;
				result[i] = n1[0] * n2[i];
				for (imsize_t j = 1; j <= i; ++j) {
					temp *= Base(i + 1 - j) / j;
					result[i] += temp * n1[j] * n2[i - j];
				}
			}
			return result;
		}
		template <class U, class = typename enable_if<is_operation<T, Base, Base>::mul_value>::type>
		friend bottomup_ad<Base, N> operator*(const _Bottomup_ad_base_type<T, N, U>& n1, const _Bottomup_ad_base& n2) {
			bottomup_ad<Base, N> result(n1[0] * n2[0]);
			for (imsize_t i = 1; i < N; ++i) {
				Base temp = 1;
				result[i] = n1[0] * n2[i];
				for (imsize_t j = 1; j <= i; ++j) {
					temp *= Base(i + 1 - j) / j;
					result[i] += temp * n1[j] * n2[i - j];
				}
			}
			return result;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend bottomup_ad<Base, N> operator*(const _Bottomup_ad_base& n, const T& k) {
			return bottomup_ad<Base, N>(n[0] * k, (n[Indices] * k)...);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::mul_value>::type>
		friend bottomup_ad<Base, N> operator*(const T& k, const _Bottomup_ad_base& n) {
			return bottomup_ad<Base, N>(k*n[0], (k*n[Indices])...);
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		friend bottomup_ad<Base, N> operator/(const _Bottomup_ad_base& n1, const _Bottomup_ad_base_type<T, N, U>& n2) {
			bottomup_ad<Base, N> result(n1[0] / n2[0]);
			bottomup_ad<Base, N> temp1 = multiplicative<bottomup_ad<T, N>>::inverse_element(n2);
			for (imsize_t i = 1; i < N; ++i) {
				Base temp2 = 1;
				result[i] = n1[0] * temp1[i];
				for (imsize_t j = 1; j <= i; ++j) {
					temp2 *= Base(i + 1 - j) / j;
					result[i] += temp2 * n1[j] * temp1[i - j];
				}
			}
			return result;
		}
		template <class U, class = typename enable_if<is_operation<T, Base, Base>::div_value>::type>
		friend bottomup_ad<Base, N> operator/(const _Bottomup_ad_base_type<T, N, U>& n1, const _Bottomup_ad_base& n2) {
			bottomup_ad<Base, N> result(n1[0] / n2[0]);
			bottomup_ad<Base, N> temp1 = multiplicative<bottomup_ad<T, N>>::inverse_element(n2);
			for (imsize_t i = 1; i < N; ++i) {
				Base temp2 = 1;
				result[i] = n1[0] * temp1[i];
				for (imsize_t j = 1; j <= i; ++j) {
					temp2 *= Base(i + 1 - j) / j;
					result[i] += temp2 * n1[j] * temp1[i - j];
				}
			}
			return result;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		friend bottomup_ad<Base, N> operator/(const _Bottomup_ad_base& n, const T& k) {
			return bottomup_ad<Base, N>(n[0] / k, (n[Indices] / k)...);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::div_value>::type>
		friend bottomup_ad<Base, N> operator/(const T& k, const _Bottomup_ad_base& n) {
			return multiplicative<bottomup_ad<Base, N>>::inverse_element(k*n);
		}


		//比較演算
		template <class U, class = typename enable_if<is_calcable<Base, T>::eq_value>::type>
		friend bool operator==(const _Bottomup_ad_base& n1, const _Bottomup_ad_base_type<T, N, U>& n2) {
			return n1[0] == n2[0];
		}
		template <class U, class = typename enable_if<is_calcable<T, Base>::eq_value>::type>
		friend bool operator==(const _Bottomup_ad_base_type<T, N, U>& n1, const _Bottomup_ad_base& n2) {
			return n1[0] == n2[0];
		}
		template <class = typename enable_if<is_calcable<Base, T>::eq_value>::type>
		friend bool operator==(const _Bottomup_ad_base& n1, const T& n2) {
			return n1[0] == n2;
		}
		template <class = typename enable_if<is_calcable<T, Base>::eq_value>::type>
		friend bool operator==(const T& n1, const _Bottomup_ad_base& n2) {
			return n1 == n2[0];
		}
		template <class U, class = typename enable_if<is_calcable<Base, T>::eq_value>::type>
		friend bool operator!=(const _Bottomup_ad_base& n1, const _Bottomup_ad_base_type<T, N, U>& n2) {
			return n1[0] != n2[0];
		}
		template <class U, class = typename enable_if<is_calcable<T, Base>::eq_value>::type>
		friend bool operator!=(const _Bottomup_ad_base_type<T, N, U>& n1, const _Bottomup_ad_base& n2) {
			return n1[0] != n2[0];
		}
		template <class = typename enable_if<is_calcable<Base, T>::eq_value>::type>
		friend bool operator!=(const _Bottomup_ad_base& n1, const T& n2) {
			return n1[0] != n2;
		}
		template <class = typename enable_if<is_calcable<T, Base>::eq_value>::type>
		friend bool operator!=(const T& n1, const _Bottomup_ad_base& n2) {
			return n1 != n2[0];
		}
		template <class U, class = typename enable_if<is_calcable<Base, T>::lt_value>::type>
		friend bool operator<(const _Bottomup_ad_base& n1, const _Bottomup_ad_base_type<T, N, U>& n2) {
			return n1[0] < n2[0];
		}
		template <class U, class = typename enable_if<is_calcable<T, Base>::lt_value>::type>
		friend bool operator<(const _Bottomup_ad_base_type<T, N, U>& n1, const _Bottomup_ad_base& n2) {
			return n1[0] < n2[0];
		}
		template <class = typename enable_if<is_calcable<Base, T>::lt_value>::type>
		friend bool operator<(const _Bottomup_ad_base& n1, const T& n2) {
			return n1[0] < n2;
		}
		template <class = typename enable_if<is_calcable<T, Base>::lt_value>::type>
		friend bool operator<(const T& n1, const _Bottomup_ad_base& n2) {
			return n1 < n2[0];
		}
		template <class U, class = typename enable_if<is_calcable<Base, T>::lt_eq_value>::type>
		friend bool operator<=(const _Bottomup_ad_base& n1, const _Bottomup_ad_base_type<T, N, U>& n2) {
			return n1[0] <= n2[0];
		}
		template <class U, class = typename enable_if<is_calcable<T, Base>::lt_eq_value>::type>
		friend bool operator<=(const _Bottomup_ad_base_type<T, N, U>& n1, const _Bottomup_ad_base& n2) {
			return n1[0] <= n2[0];
		}
		template <class = typename enable_if<is_calcable<Base, T>::lt_eq_value>::type>
		friend bool operator<=(const _Bottomup_ad_base& n1, const T& n2) {
			return n1[0] <= n2;
		}
		template <class = typename enable_if<is_calcable<T, Base>::lt_eq_value>::type>
		friend bool operator<=(const T& n1, const _Bottomup_ad_base& n2) {
			return n1 <= n2[0];
		}
		template <class U, class = typename enable_if<is_calcable<Base, T>::gt_value>::type>
		friend bool operator>(const _Bottomup_ad_base& n1, const _Bottomup_ad_base_type<T, N, U>& n2) {
			return n1[0] > n2[0];
		}
		template <class U, class = typename enable_if<is_calcable<T, Base>::gt_value>::type>
		friend bool operator>(const _Bottomup_ad_base_type<T, N, U>& n1, const _Bottomup_ad_base& n2) {
			return n1[0] > n2[0];
		}
		template <class = typename enable_if<is_calcable<Base, T>::gt_value>::type>
		friend bool operator>(const _Bottomup_ad_base& n1, const T& n2) {
			return n1[0] > n2;
		}
		template <class = typename enable_if<is_calcable<T, Base>::gt_value>::type>
		friend bool operator>(const T& n1, const _Bottomup_ad_base& n2) {
			return n1 > n2[0];
		}
		template <class U, class = typename enable_if<is_calcable<Base, T>::gt_eq_value>::type>
		friend bool operator>=(const _Bottomup_ad_base& n1, const _Bottomup_ad_base_type<T, N, U>& n2) {
			return n1[0] >= n2[0];
		}
		template <class U, class = typename enable_if<is_calcable<T, Base>::gt_eq_value>::type>
		friend bool operator>=(const _Bottomup_ad_base_type<T, N, U>& n1, const _Bottomup_ad_base& n2) {
			return n1[0] >= n2[0];
		}
		template <class = typename enable_if<is_calcable<Base, T>::gt_eq_value>::type>
		friend bool operator>=(const _Bottomup_ad_base& n1, const T& n2) {
			return n1[0] >= n2;
		}
		template <class = typename enable_if<is_calcable<T, Base>::gt_eq_value>::type>
		friend bool operator>=(const T& n1, const _Bottomup_ad_base& n2) {
			return n1 >= n2[0];
		}
	};

	//ボトムアップ型N階自動微分
	template <class T, imsize_t N>
	class bottomup_ad : public _Bottomup_ad_base_type<T, N, T>, public value_list_input<array_iterator<T>> {
		template <class, imsize_t> friend class bottomup_ad;
		template <class, imsize_t, class, bool, class, bool> friend class _Bottomup_ad_base;

		static_assert(N > 0, "number of array elements must greater than 0");

		//代入演算の補助
		template <class _T>
		static bottomup_ad* bottomup_ad_copy(bottomup_ad* const n1, const bottomup_ad<_T, N>& n2) {
			//同じインスタンスでなければ代入
			if (static_cast<void*>(&n1->x[0]) != static_cast<void*>(const_cast<_T*>(&n2[0])))
				for (imsize_t i = 0; i < N; ++i) n1->x[i] = static_cast<T>(n2[i]);
			return n1;
		}
	public:
		//コンストラクタの継承
		using _Bottomup_ad_base_type<T, N, T>::_Bottomup_ad_base;

		using algebraic_type = T;
		using iterator = array_iterator<T>;
		using const_iterator = array_iterator<const T>;

		template<class Other>
		struct rebind {
			using other = bottomup_ad<Other, N>;
		};

		iterator begin() noexcept { return iterator(&x[0]); }
		const_iterator begin() const noexcept { return const_iterator(&x[0]); }
		iterator end() noexcept { return iterator(&x[N - 1] + 1); }
		const_iterator end() const noexcept { return const_iterator(&x[N - 1] + 1); }

		//単項演算の継承
		using _Bottomup_ad_base_type<T, N, T>::operator-;
		using _Bottomup_ad_base_type<T, N, T>::operator+;
		//代入演算の継承
		using _Bottomup_ad_base_type<T, N, T>::operator+=;
		using _Bottomup_ad_base_type<T, N, T>::operator-=;
		using _Bottomup_ad_base_type<T, N, T>::operator*=;
		using _Bottomup_ad_base_type<T, N, T>::operator/=;
		//二項演算の継承
		using value_list_input<array_iterator<T>>::operator<<;

		//代入演算
		bottomup_ad& operator=(const bottomup_ad& n) { return *bottomup_ad_copy(this, n); }
		template <class _T>
		bottomup_ad& operator=(const bottomup_ad<_T, N>& n) { return *bottomup_ad_copy(this, n); }
		bottomup_ad& operator=(const _Bottomup_ad_base_type<T, N, T>& n) { return *bottomup_ad_copy(this, n); }
		template <class U>
		bottomup_ad& operator=(const _Bottomup_ad_base_type<T, N, U>& n) { return *bottomup_ad_copy(this, n); }
		template <class U, class _T, class = typename enable_if<is_inclusion<_T, T>::value>::type>
		bottomup_ad& operator=(const _Bottomup_ad_base_type<_T, N, U>& n) { return *bottomup_ad_copy(this, n); }

		//添え字演算の継承
		using _Bottomup_ad_base_type<T, N, T>::operator[];

		//ストリーム出力
		friend std::ostream& operator<<(std::ostream& os, const bottomup_ad& n) {
			os << n.x[0];
			for (imsize_t i = 1; i < N; ++i) os << ',' << n.x[i];
			return os;
		}
		friend std::wostream& operator<<(std::wostream& os, const bottomup_ad& n) {
			os << n.x[0];
			for (imsize_t i = 1; i < N; ++i) os << L',' << n.x[i];
			return os;
		}
	};


	//ボトムアップ型N階自動微分の判定
	template <class T>
	struct _Is_bottomup_ad : false_type {};
	template <class T, imsize_t N>
	struct _Is_bottomup_ad<bottomup_ad<T, N>> : true_type {};
	template <class T>
	struct is_bottomup_ad : _Is_bottomup_ad<typename remove_cv<T>::type> {};

	//ボトムアップ型N階自動微分の除去
	template <class T>
	struct remove_bottomup_ad {
		using type = T;
	};
	template <class T, imsize_t N>
	struct remove_bottomup_ad<bottomup_ad<T, N>> {
		using type = T;
	};

	//全てのボトムアップ型N階自動微分の除去
	template <class T>
	struct remove_all_bottomup_ad {
		using type = T;
	};
	template <class T, imsize_t N>
	struct remove_all_bottomup_ad<bottomup_ad<T, N>> : remove_all_bottomup_ad<T> {};


	//ボトムアップ型N階自動微分のための変数
	template <class, imsize_t, class>
	struct _Ad_variable;
	template <class T, imsize_t N, imsize_t... Indices>
	struct _Ad_variable<T, N, index_tuple<Indices...>> {
		//任意のテンプレートに対して加法単位元を返す補助
		template <imsize_t>
		static constexpr T _Get() { return addition<T>::identity_element(); }

		static constexpr bottomup_ad<T, N> __ad_variable(const T& x) {
			return bottomup_ad<T, N>(x, multiplicative<T>::identity_element(), _Get<Indices>()...);
		}
	};
	template <imsize_t N, class T>
	bottomup_ad<T, N> ad_variable(const T& x) {
		return _Ad_variable<T, N, typename index_range<2, N>::type>::__ad_variable(x);
	}


	template<class T, imsize_t N>
	struct _Ceil<bottomup_ad<T, N>> {
		static constexpr bottomup_ad<T, N> __ceil(const bottomup_ad<T, N>& x) { return bottomup_ad<T, N>(ceil(x[0])); }
	};
	template<class T, imsize_t N>
	struct _Floor<bottomup_ad<T, N>> {
		static constexpr bottomup_ad<T, N> __floor(const bottomup_ad<T, N>& x) { return bottomup_ad<T, N>(floor(x[0])); }
	};
	template <class T, imsize_t N>
	struct _Pi_<bottomup_ad<T, N>> {
		static constexpr auto pi = _Pi<T>();
	};
}

namespace iml {

	//加法パラメータ取得
	template <class T, imsize_t N>
	struct addition<bottomup_ad<T, N>> {
		//単位元の取得
		static bottomup_ad<T, N> identity_element() {
			return bottomup_ad<T, N>();
		}
		//逆元の取得
		template <class = typename enable_if<is_exist_add_inverse_element<T>::value>::type>
		static constexpr bottomup_ad<T, N> inverse_element(const bottomup_ad<T, N>& x) {
			return -x;
		}
	};
	//乗法パラメータ取得
	template <class T, imsize_t N>
	struct multiplicative<bottomup_ad<T, N>> {
		//単位元の取得
		static bottomup_ad<T, N> identity_element() {
			return bottomup_ad<T, N>(multiplicative<T>::identity_element());
		}
		//逆元の取得
		static bottomup_ad<T, N> inverse_element(const bottomup_ad<T, N>& x) {
			bottomup_ad<T, N> result = { 1 / x[0] };
			bottomup_ad<T, N> temp1[N - 1] = { x };			//xの冪乗のテーブル(x,x^2,...,x^(N-1))

			for (imsize_t i = 1; i < N - 1; ++i) {
				T temp2 = (i + 1) / x[0];
				//result[i] = 0;
				for (imsize_t j = 1; j <= i; ++j) {
					temp2 *= (i + 1 - j) / (-x[0] * j);
					result[i] += temp2 * temp1[j - 1][i] / (1 + j);
				}
				temp1[i] = temp1[i - 1] * x;
			}
			//i == N - 1のとき(テーブルを新しく作成する必要ないため)
			{
				T temp2 = N / x[0];
				//result[N - 1] = 0;
				//for (imsize_t j = 1; j <= N - 1; ++j) {
				for (imsize_t j = 1; j < N; ++j) {
					temp2 *= (N - j) / (-x[0] * j);
					result[N - 1] += temp2 * temp1[j - 1][N - 1] / (1 + j);
				}
			}
			return result;
		}
		//吸収元
		template <class = typename enable_if<is_calcable<T, T>::mul_value>::type>
		static bottomup_ad<T, N> absorbing_element() {
			return bottomup_ad<T, N>();
		}
	};

}

namespace iml {

	//誤差評価
	template <class T, imsize_t N>
	struct _Error_evaluation<bottomup_ad<T, N>> {
		static bool __error_evaluation(const bottomup_ad<T, N>& n1, const bottomup_ad<T, N>& n2) {
			return error_evaluation(n1[0], n2[0]);
		}
	};
}

/*

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