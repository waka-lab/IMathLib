#ifndef IMATH_MATH_AUTOMATIC_DIFFERENTIATION_HPP
#define IMATH_MATH_AUTOMATIC_DIFFERENTIATION_HPP

#include "IMathLib/math/liner_algebra/vector.hpp"
#include "IMathLib/utility/functional.hpp"


//二重数を利用した自動微分

namespace iml {

	template<class, size_t...>
	class bottomup_ad;


	template <class Base, class Dims, class T, class = typename index_range<size_t, 1, dimension<Dims>::value>::type, bool = is_algebraic_structure<T>::value, bool = is_same<Base, T>::value>
	class bottomup_ad_base;


	//下に階層が存在しないかつBase == T
	template <class Base, size_t... Dims, class T, size_t... Indices>
	class bottomup_ad_base<Base, index_tuple<size_t, Dims...>, T, index_tuple<size_t, Indices...>, false, true> {
		template <class, size_t...> friend class bottomup_ad;
		template <class, class, class, class, bool, bool> friend class bottomup_ad_base;
		template <class> friend struct multiplication_traits;
		template <class> friend struct addition_traits;

		//乗算用の多重ループ(bool:終了条件)
		template <class S, bool = (!is_same<typename remove_extent<S>::type, typename remove_all_extents<S>::type>::value)>
		struct multi_loop_multiplication {};
		template <class S, size_t N>
		struct multi_loop_multiplication<S[N], true> {
			//result:結果, lhs・rhs:演算対象, c:係数
			template <class U, class V>
			static void loop(S* result, const U* lhs, const V* rhs, const T& c) {
				multi_loop_multiplication<S>::loop(result[0], lhs[0], rhs[0], c);
				for (size_t i = 1; i < N; ++i) {
					T temp = c;
					multi_loop_multiplication<S>::loop(result[i], lhs[0], rhs[i], temp);
					for (size_t j = 1; j <= i; ++j) {
						temp *= T(i + 1 - j) / j;
						multi_loop_multiplication<S>::loop(result[i], lhs[j], rhs[i - j], temp);
					}
				}
			}
		};
		template <class S, size_t N>
		struct multi_loop_multiplication<S[N], false> {
			template <class U, class V>
			static void loop(S* result, const U* lhs, const V* rhs, const T& c) {
				result[0] += c * lhs[0] * rhs[0];
				for (size_t i = 1; i < N; ++i) {
					T temp = c;
					result[i] += temp * lhs[0] * rhs[i];
					for (size_t j = 1; j <= i; ++j) {
						temp *= T(i + 1 - j) / j;
						result[i] += temp * lhs[j] * rhs[i - j];
					}
				}
			}
		};
	protected:
		typename multi_array<Base, Dims...>::type x;
	public:
		constexpr bottomup_ad_base() : x{} {}
		constexpr bottomup_ad_base(const Base& re) : x{ re } {}
		constexpr bottomup_ad_base(const Base& re, const identity_t<Base, Indices>&... x) : x{ re,x... } {}
		template <class U>
		constexpr bottomup_ad_base(const bottomup_ad_base<Base, index_tuple<size_t, Dims...>, U>& n) : x{ addressof_multi_array(n.x)[0], addressof_multi_array(n.x)[Indices]... } {}

		template <class = typename enable_if<is_exist_additive_inverse_v<Base>>::type>
		bottomup_ad<Base, Dims...> operator-() const { return bottomup_ad<Base, Dims...>(-addressof_multi_array(this->x)[0], (-addressof_multi_array(this->x)[Indices])...); }
		bottomup_ad<Base, Dims...> operator+() const { return bottomup_ad<Base, Dims...>(*this); }

		template <class U, class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		bottomup_ad_base& operator+=(const bottomup_ad_base<T, index_tuple<size_t, Dims...>, U>& n) {
			for (size_t i = 0; i < dimension<index_tuple<size_t, Dims...>>::value; ++i)
				addressof_multi_array(this->x)[i] += addressof_multi_array(n.x)[i];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		bottomup_ad_base& operator+=(const T& n) {
			addressof_multi_array(this->x)[0] += n;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		bottomup_ad_base& operator-=(const bottomup_ad_base<T, index_tuple<size_t, Dims...>, U>& n) {
			for (size_t i = 0; i < dimension<index_tuple<size_t, Dims...>>::value; ++i)
				addressof_multi_array(this->x)[i] -= addressof_multi_array(n.x)[i];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		bottomup_ad_base& operator-=(const T& n) {
			addressof_multi_array(this->x)[0] -= n;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		bottomup_ad_base& operator*=(const bottomup_ad_base<T, index_tuple<size_t, Dims...>, U>& n) {
			typename multi_array<Base, Dims...>::type result = {};
			multi_loop_multiplication<typename multi_array<Base, Dims...>::type>::loop(result, this->x, n.x, 1);
			for (size_t i = 0; i < dimension<index_tuple<size_t, Dims...>>::value; ++i)
				addressof_multi_array(this->x)[i] = addressof_multi_array(result)[i];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		bottomup_ad_base& operator*=(const T& k) {
			for (size_t i = 0; i < dimension<index_tuple<size_t, Dims...>>::value; ++i)
				addressof_multi_array(this->x)[i] *= k;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		bottomup_ad_base& operator/=(const bottomup_ad_base<T, index_tuple<size_t, Dims...>, U>& n) {
			return *this *= multiplication_traits<bottomup_ad<T, Dims...>>::inverse_element(n);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		bottomup_ad_base& operator/=(const T& k) {
			for (size_t i = 0; i < dimension<index_tuple<size_t, Dims...>>::value; ++i)
				addressof_multi_array(this->x)[i] /= k;
			return *this;
		}

		//アクセサ
		const constexpr auto& operator[](size_t index) const { return this->x[index]; }
		constexpr auto& operator[](size_t index) { return this->x[index]; }


		//2項演算
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend bottomup_ad<Base, Dims...> operator+(const bottomup_ad<Base, Dims...>& lhs, const bottomup_ad<T, Dims...>& rhs) {
			return bottomup_ad<Base, Dims...>(addressof_multi_array(lhs[0])[0] + addressof_multi_array(rhs[0])[0]
				, (addressof_multi_array(lhs[0])[Indices] + addressof_multi_array(rhs[0])[Indices])...);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend bottomup_ad<Base, Dims...> operator+(const bottomup_ad<Base, Dims...>& lhs, const T& rhs) {
			return bottomup_ad<Base, Dims...>(addressof_multi_array(lhs[0])[0] + rhs
				, addressof_multi_array(lhs[0])[Indices]...);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::add_value>::type>
		friend bottomup_ad<Base, Dims...> operator+(const T& lhs, const bottomup_ad<Base, Dims...>& rhs) {
			return bottomup_ad<Base, Dims...>(lhs + addressof_multi_array(rhs[0])[0]
				, addressof_multi_array(rhs[0])[Indices]...);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend bottomup_ad<Base, Dims...> operator-(const bottomup_ad<Base, Dims...>& lhs, const bottomup_ad<T, Dims...>& rhs) {
			return bottomup_ad<Base, Dims...>(addressof_multi_array(lhs[0])[0] - addressof_multi_array(rhs[0])[0]
				, (addressof_multi_array(lhs[0])[Indices] - addressof_multi_array(rhs[0])[Indices])...);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend bottomup_ad<Base, Dims...> operator-(const bottomup_ad<Base, Dims...>& lhs, const T& rhs) {
			return bottomup_ad<Base, Dims...>(addressof_multi_array(lhs[0])[0] - rhs
				, addressof_multi_array(lhs[0])[Indices]...);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::sub_value>::type>
		friend bottomup_ad<Base, Dims...> operator-(const T& lhs, const bottomup_ad<Base, Dims...>& rhs) {
			return bottomup_ad<Base, Dims...>(lhs - addressof_multi_array(rhs[0])[0]
				, (-addressof_multi_array(rhs[0])[Indices])...);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend bottomup_ad<Base, Dims...> operator*(const bottomup_ad<Base, Dims...>& lhs, const bottomup_ad<T, Dims...>& rhs) {
			bottomup_ad<Base, Dims...> result;
			multi_loop_multiplication<typename multi_array<Base, Dims...>::type>::loop(addressof(result[0]), addressof(lhs[0]), addressof(rhs[0]), 1);
			return result;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend bottomup_ad<Base, Dims...> operator*(const bottomup_ad<Base, Dims...>& lhs, const T& rhs) {
			return bottomup_ad<Base, Dims...>(addressof_multi_array(lhs[0])[0] * rhs
				, (addressof_multi_array(lhs[0])[Indices] * rhs)...);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::mul_value>::type>
		friend bottomup_ad<Base, Dims...> operator*(const T& lhs, const bottomup_ad<Base, Dims...>& rhs) {
			return bottomup_ad<Base, Dims...>(lhs * addressof_multi_array(rhs[0])[0]
				, (lhs * addressof_multi_array(rhs[0])[Indices])...);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		friend bottomup_ad<Base, Dims...> operator/(const bottomup_ad<Base, Dims...>& lhs, const bottomup_ad<T, Dims...>& rhs) {
			return lhs * multiplication_traits<bottomup_ad<T, Dims...>>::inverse_element(rhs);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		friend bottomup_ad<Base, Dims...> operator/(const bottomup_ad<Base, Dims...>& lhs, const T& rhs) {
			return bottomup_ad<Base, Dims...>(addressof_multi_array(lhs[0])[0] / rhs
				, (addressof_multi_array(lhs[0])[Indices] / rhs)...);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::div_value>::type>
		friend bottomup_ad<Base, Dims...> operator/(const T& lhs, const bottomup_ad<Base, Dims...>& rhs) {
			return lhs * multiplication_traits<bottomup_ad<Base, Dims...>>::inverse_element(rhs);
		}


		//比較演算
		template <class = typename enable_if<is_calcable<Base, T>::eq_value>::type>
		friend bool operator==(const bottomup_ad<Base, Dims...>& lhs, const bottomup_ad<T, Dims...>& rhs) { return addressof_multi_array(lhs[0])[0] == addressof_multi_array(rhs[0])[0]; }
		template <class = typename enable_if<is_calcable<Base, T>::eq_value>::type>
		friend bool operator==(const bottomup_ad<Base, Dims...>& lhs, const T& rhs) { return addressof_multi_array(lhs[0])[0] == rhs; }
		template <class = typename enable_if<is_calcable<T, Base>::eq_value>::type>
		friend bool operator==(const T& lhs, const bottomup_ad<Base, Dims...>& rhs) { return lhs == addressof_multi_array(rhs[0])[0]; }
		template <class = typename enable_if<is_calcable<Base, T>::eq_value>::type>
		friend bool operator!=(const bottomup_ad<Base, Dims...>& lhs, const bottomup_ad<T, Dims...>& rhs) { return addressof_multi_array(lhs[0])[0] != addressof_multi_array(rhs[0])[0]; }
		template <class = typename enable_if<is_calcable<Base, T>::eq_value>::type>
		friend bool operator!=(const bottomup_ad<Base, Dims...>& lhs, const T& rhs) { return addressof_multi_array(lhs[0])[0] != rhs; }
		template <class = typename enable_if<is_calcable<T, Base>::eq_value>::type>
		friend bool operator!=(const T& lhs, const bottomup_ad<Base, Dims...>& rhs) { return lhs != addressof_multi_array(rhs[0])[0]; }
		template <class = typename enable_if<is_calcable<Base, T>::lt_value>::type>
		friend bool operator<(const bottomup_ad<Base, Dims...>& lhs, const bottomup_ad<T, Dims...>& rhs) { return addressof_multi_array(lhs[0])[0] < addressof_multi_array(rhs[0])[0]; }
		template <class = typename enable_if<is_calcable<Base, T>::lt_value>::type>
		friend bool operator<(const bottomup_ad<Base, Dims...>& lhs, const T& rhs) { return addressof_multi_array(lhs[0])[0] < rhs; }
		template <class = typename enable_if<is_calcable<T, Base>::lt_value>::type>
		friend bool operator<(const T& lhs, const bottomup_ad<Base, Dims...>& rhs) { return lhs < addressof_multi_array(rhs[0])[0]; }
		template <class = typename enable_if<is_calcable<Base, T>::leq_value>::type>
		friend bool operator<=(const bottomup_ad<Base, Dims...>& lhs, const bottomup_ad<T, Dims...>& rhs) { return addressof_multi_array(lhs[0])[0] <= addressof_multi_array(rhs[0])[0]; }
		template <class = typename enable_if<is_calcable<Base, T>::leq_value>::type>
		friend bool operator<=(const bottomup_ad<Base, Dims...>& lhs, const T& rhs) { return addressof_multi_array(lhs[0])[0] <= rhs; }
		template <class = typename enable_if<is_calcable<T, Base>::leq_value>::type>
		friend bool operator<=(const T& lhs, const bottomup_ad<Base, Dims...>& rhs) { return lhs <= addressof_multi_array(rhs[0])[0]; }
		template <class = typename enable_if<is_calcable<Base, T>::gt_value>::type>
		friend bool operator>(const bottomup_ad<Base, Dims...>& lhs, const bottomup_ad<T, Dims...>& rhs) { return addressof_multi_array(lhs[0])[0] > addressof_multi_array(rhs[0])[0]; }
		template <class = typename enable_if<is_calcable<Base, T>::gt_value>::type>
		friend bool operator>(const bottomup_ad<Base, Dims...>& lhs, const T& rhs) { return addressof_multi_array(lhs[0])[0] > rhs; }
		template <class = typename enable_if<is_calcable<T, Base>::gt_value>::type>
		friend bool operator>(const T& lhs, const bottomup_ad<Base, Dims...>& rhs) { return lhs > addressof_multi_array(rhs[0])[0]; }
		template <class = typename enable_if<is_calcable<Base, T>::geq_value>::type>
		friend bool operator>=(const bottomup_ad<Base, Dims...>& lhs, const bottomup_ad<T, Dims...>& rhs) { return addressof_multi_array(lhs[0])[0] >= addressof_multi_array(rhs[0])[0]; }
		template <class = typename enable_if<is_calcable<Base, T>::geq_value>::type>
		friend bool operator>=(const bottomup_ad<Base, Dims...>& lhs, const T& rhs) { return addressof_multi_array(lhs[0])[0] >= rhs; }
		template <class = typename enable_if<is_calcable<T, Base>::geq_value>::type>
		friend bool operator>=(const T& lhs, const bottomup_ad<Base, Dims...>& rhs) { return lhs >= addressof_multi_array(rhs[0])[0]; }
	};
	//下に階層が存在しないかつBase != T
	template <class Base, size_t... Dims, class T, size_t... Indices>
	class bottomup_ad_base<Base, index_tuple<size_t, Dims...>, T, index_tuple<size_t, Indices...>, false, false> {
		template <class, size_t...> friend class bottomup_ad;
		template <class, class, class, class, bool, bool> friend class bottomup_ad_base;
		template <class> friend struct multiplication_traits;
		template <class> friend struct addition_traits;

		//乗算用の多重ループ(bool:終了条件)
		template <class S, bool = (!is_same<typename remove_extent<S>::type, typename remove_all_extents<S>::type>::value)>
		struct multi_loop_multiplication {};
		template <class S, size_t N>
		struct multi_loop_multiplication<S[N], true> {
			//result:結果, lhs・rhs:演算対象, c:係数
			template <class U, class V>
			static void loop(S* result, const U* lhs, const V* rhs, const T& c) {
				multi_loop_multiplication<S>::loop(result[0], lhs[0], rhs[0], c);
				for (size_t i = 1; i < N; ++i) {
					T temp = c;
					multi_loop_multiplication<S>::loop(result[i], lhs[0], rhs[i], temp);
					for (size_t j = 1; j <= i; ++j) {
						temp *= T(i + 1 - j) / j;
						multi_loop_multiplication<S>::loop(result[i], lhs[j], rhs[i - j], temp);
					}
				}
			}
		};
		template <class S, size_t N>
		struct multi_loop_multiplication<S[N], false> {
			template <class U, class V>
			static void loop(S* result, const U* lhs, const V* rhs, const T& c) {
				result[0] += c * lhs[0] * rhs[0];
				for (size_t i = 1; i < N; ++i) {
					T temp = c;
					result[i] += temp * lhs[0] * rhs[i];
					for (size_t j = 1; j <= i; ++j) {
						temp *= T(i + 1 - j) / j;
						result[i] += temp * lhs[j] * rhs[i - j];
					}
				}
			}
		};
	protected:
		typename multi_array<Base, Dims...>::type x;
	public:
		constexpr bottomup_ad_base() : x{} {}
		constexpr bottomup_ad_base(const Base& re) : x{ re } {}
		constexpr bottomup_ad_base(const Base& re, const identity_t<Base, Indices>&... x) : x{ re,x... } {}
		template <class = typename enable_if<is_inclusion<T, Base>::value>::type>
		constexpr bottomup_ad_base(const T& re) : x{ static_cast<Base>(re) } {}
		template <class = typename enable_if<is_inclusion<T, Base>::value>::type>
		constexpr bottomup_ad_base(const T& re, const identity_t<T, Indices>&... x) : x{ static_cast<Base>(re),static_cast<Base>(x)... } {}
		template <class U>
		constexpr bottomup_ad_base(const bottomup_ad_base<Base, index_tuple<size_t, Dims...>, U>& n) : x{ n.x[0], n.x[Indices]... } {}
		template <class U, class = typename enable_if<is_inclusion<T, Base>::value>::type>
		constexpr bottomup_ad_base(const bottomup_ad_base<T, index_tuple<size_t, Dims...>, U>& n) : x{ static_cast<Base>(n.x[0]), static_cast<Base>(n.x[Indices])... } {}


		template <class = typename enable_if<is_exist_additive_inverse_v<Base>>::type>
		bottomup_ad<Base, Dims...> operator-() const { return bottomup_ad<Base, Dims...>(-this->x[0], (-this->x[Indices])...); }
		bottomup_ad<Base, Dims...> operator+() const { return bottomup_ad<Base, Dims...>(*this); }

		template <class U, class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		bottomup_ad_base& operator+=(const bottomup_ad_base<T, index_tuple<size_t, Dims...>, U>& n) {
			for (size_t i = 0; i < dimension<index_tuple<size_t, Dims...>>::value; ++i)
				addressof_multi_array(this->x)[i] += addressof_multi_array(n.x)[i];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		bottomup_ad_base& operator+=(const T& n) {
			addressof_multi_array(this->x)[0] += n;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		bottomup_ad_base& operator-=(const bottomup_ad_base<T, index_tuple<size_t, Dims...>, U>& n) {
			for (size_t i = 0; i < dimension<index_tuple<size_t, Dims...>>::value; ++i)
				addressof_multi_array(this->x)[i] -= addressof_multi_array(n.x)[i];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		bottomup_ad_base& operator-=(const T& n) {
			addressof_multi_array(this->x)[0] -= n;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		bottomup_ad_base& operator*=(const bottomup_ad_base<T, index_tuple<size_t, Dims...>, U>& n) {
			typename multi_array<Base, Dims...>::type result = {};
			multi_loop_multiplication<typename multi_array<Base, Dims...>::type>::loop(result, this->x, n.x, 1);
			for (size_t i = 0; i < dimension<index_tuple<size_t, Dims...>>::value; ++i)
				addressof_multi_array(this->x)[i] = addressof_multi_array(result)[i];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		bottomup_ad_base& operator*=(const T& k) {
			for (size_t i = 0; i < dimension<index_tuple<size_t, Dims...>>::value; ++i)
				addressof_multi_array(this->x)[i] *= k;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		bottomup_ad_base& operator/=(const bottomup_ad_base<T, index_tuple<size_t, Dims...>, U>& n) {
			return *this *= multiplication_traits<bottomup_ad<T, Dims...>>::inverse_element(n);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		bottomup_ad_base& operator/=(const T& k) {
			for (size_t i = 0; i < dimension<index_tuple<size_t, Dims...>>::value; ++i)
				addressof_multi_array(this->x)[i] /= k;
			return *this;
		}

		//アクセサ(各要素を直接書き換えるのは不可)
		const constexpr auto& operator[](size_t index) const { return this->x[index]; }
		constexpr auto& operator[](size_t index) { return this->x[index]; }


		//2項演算
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend bottomup_ad<Base, Dims...> operator+(const bottomup_ad<Base, Dims...>& lhs, const bottomup_ad<T, Dims...>& rhs) {
			return bottomup_ad<Base, Dims...>(addressof_multi_array(lhs[0])[0] + addressof_multi_array(rhs[0])[0]
				, (addressof_multi_array(lhs[0])[Indices] + addressof_multi_array(rhs[0])[Indices])...);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::add_value>::type>
		friend bottomup_ad<Base, Dims...> operator+(const bottomup_ad<T, Dims...>& lhs, const bottomup_ad<Base, Dims...>& rhs) {
			return bottomup_ad<Base, Dims...>(addressof_multi_array(lhs[0])[0] + addressof_multi_array(rhs[0])[0]
				, (addressof_multi_array(lhs[0])[Indices] + addressof_multi_array(rhs[0])[Indices])...);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend bottomup_ad<Base, Dims...> operator+(const bottomup_ad<Base, Dims...>& lhs, const T& rhs) {
			return bottomup_ad<Base, Dims...>(addressof_multi_array(lhs[0])[0] + rhs
				, addressof_multi_array(lhs[0])[Indices]...);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::add_value>::type>
		friend bottomup_ad<Base, Dims...> operator+(const T& lhs, const bottomup_ad<Base, Dims...>& rhs) {
			return bottomup_ad<Base, Dims...>(lhs + addressof_multi_array(rhs[0])[0]
				, addressof_multi_array(rhs[0])[Indices]...);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend bottomup_ad<Base, Dims...> operator-(const bottomup_ad<Base, Dims...>& lhs, const bottomup_ad<T, Dims...>& rhs) {
			return bottomup_ad<Base, Dims...>(addressof_multi_array(lhs[0])[0] - addressof_multi_array(rhs[0])[0]
				, (addressof_multi_array(lhs[0])[Indices] - addressof_multi_array(rhs[0])[Indices])...);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::sub_value>::type>
		friend bottomup_ad<Base, Dims...> operator-(const bottomup_ad<T, Dims...>& lhs, const bottomup_ad<Base, Dims...>& rhs) {
			return bottomup_ad<Base, Dims...>(addressof_multi_array(lhs[0])[0] - addressof_multi_array(rhs[0])[0]
				, (addressof_multi_array(lhs[0])[Indices] - addressof_multi_array(rhs[0])[Indices])...);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend bottomup_ad<Base, Dims...> operator-(const bottomup_ad<Base, Dims...>& lhs, const T& rhs) {
			return bottomup_ad<Base, Dims...>(addressof_multi_array(lhs[0])[0] - rhs
				, addressof_multi_array(lhs[0])[Indices]...);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::sub_value>::type>
		friend bottomup_ad<Base, Dims...> operator-(const T& lhs, const bottomup_ad<Base, Dims...>& rhs) {
			return bottomup_ad<Base, Dims...>(lhs - addressof_multi_array(rhs[0])[0]
				, (-addressof_multi_array(rhs[0])[Indices])...);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend bottomup_ad<Base, Dims...> operator*(const bottomup_ad<Base, Dims...>& lhs, const bottomup_ad<T, Dims...>& rhs) {
			bottomup_ad<Base, Dims...> result;
			multi_loop_multiplication<typename multi_array<Base, Dims...>::type>::loop(addressof(result[0]), addressof(lhs[0]), addressof(rhs[0]), 1);
			return result;
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::mul_value>::type>
		friend bottomup_ad<Base, Dims...> operator*(const bottomup_ad<T, Dims...>& lhs, const bottomup_ad<Base, Dims...>& rhs) {
			bottomup_ad<Base, Dims...> result;
			multi_loop_multiplication<typename multi_array<Base, Dims...>::type>::loop(addressof(result[0]), addressof(lhs[0]), addressof(rhs[0]), 1);
			return result;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend bottomup_ad<Base, Dims...> operator*(const bottomup_ad<Base, Dims...>& lhs, const T& rhs) {
			return bottomup_ad<Base, Dims...>(addressof_multi_array(lhs[0])[0] * rhs
				, (addressof_multi_array(lhs[0])[Indices] * rhs)...);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::mul_value>::type>
		friend bottomup_ad<Base, Dims...> operator*(const T& lhs, const bottomup_ad<Base, Dims...>& rhs) {
			return bottomup_ad<Base, Dims...>(lhs * addressof_multi_array(rhs[0])[0]
				, (lhs * addressof_multi_array(rhs[0])[Indices])...);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		friend bottomup_ad<Base, Dims...> operator/(const bottomup_ad<Base, Dims...>& lhs, const bottomup_ad<T, Dims...>& rhs) {
			return lhs * multiplication_traits<bottomup_ad<T, Dims...>>::inverse_element(rhs);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::div_value>::type>
		friend bottomup_ad<Base, Dims...> operator/(const bottomup_ad<T, Dims...>& lhs, const bottomup_ad<Base, Dims...>& rhs) {
			return lhs * multiplication_traits<bottomup_ad<Base, Dims...>>::inverse_element(rhs);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		friend bottomup_ad<Base, Dims...> operator/(const bottomup_ad<Base, Dims...>& lhs, const T& rhs) {
			return bottomup_ad<Base, Dims...>(addressof_multi_array(lhs[0])[0] / rhs
				, (addressof_multi_array(lhs[0])[Indices] / rhs)...);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::div_value>::type>
		friend bottomup_ad<Base, Dims...> operator/(const T& lhs, const bottomup_ad<Base, Dims...>& rhs) {
			return lhs * multiplication_traits<bottomup_ad<Base, Dims...>>::inverse_element(rhs);
		}


		//比較演算
		template <class = typename enable_if<is_calcable<Base, T>::eq_value>::type>
		friend bool operator==(const bottomup_ad<Base, Dims...>& lhs, const bottomup_ad<T, Dims...>& rhs) { return addressof_multi_array(lhs[0])[0] == addressof_multi_array(rhs[0])[0]; }
		template <class = typename enable_if<is_calcable<T, Base>::eq_value>::type>
		friend bool operator==(const bottomup_ad<T, Dims...>& lhs, const bottomup_ad<Base, Dims...>& rhs) { return addressof_multi_array(lhs[0])[0] == addressof_multi_array(rhs[0])[0]; }
		template <class = typename enable_if<is_calcable<Base, T>::eq_value>::type>
		friend bool operator==(const bottomup_ad<Base, Dims...>& lhs, const T& rhs) { return addressof_multi_array(lhs[0])[0] == rhs; }
		template <class = typename enable_if<is_calcable<T, Base>::eq_value>::type>
		friend bool operator==(const T& lhs, const bottomup_ad<Base, Dims...>& rhs) { return lhs == addressof_multi_array(rhs[0])[0]; }
		template <class = typename enable_if<is_calcable<Base, T>::eq_value>::type>
		friend bool operator!=(const bottomup_ad<Base, Dims...>& lhs, const bottomup_ad<T, Dims...>& rhs) { return addressof_multi_array(lhs[0])[0] != addressof_multi_array(rhs[0])[0]; }
		template <class = typename enable_if<is_calcable<T, Base>::eq_value>::type>
		friend bool operator!=(const bottomup_ad<T, Dims...>& lhs, const bottomup_ad<Base, Dims...>& rhs) { return addressof_multi_array(lhs[0])[0] != addressof_multi_array(rhs[0])[0]; }
		template <class = typename enable_if<is_calcable<Base, T>::eq_value>::type>
		friend bool operator!=(const bottomup_ad<Base, Dims...>& lhs, const T& rhs) { return addressof_multi_array(lhs[0])[0] != rhs; }
		template <class = typename enable_if<is_calcable<T, Base>::eq_value>::type>
		friend bool operator!=(const T& lhs, const bottomup_ad<Base, Dims...>& rhs) { return lhs != addressof_multi_array(rhs[0])[0]; }
		template <class = typename enable_if<is_calcable<Base, T>::lt_value>::type>
		friend bool operator<(const bottomup_ad<Base, Dims...>& lhs, const bottomup_ad<T, Dims...>& rhs) { return addressof_multi_array(lhs[0])[0] < addressof_multi_array(rhs[0])[0]; }
		template <class = typename enable_if<is_calcable<T, Base>::lt_value>::type>
		friend bool operator<(const bottomup_ad<T, Dims...>& lhs, const bottomup_ad<Base, Dims...>& rhs) { return addressof_multi_array(lhs[0])[0] < addressof_multi_array(rhs[0])[0]; }
		template <class = typename enable_if<is_calcable<Base, T>::lt_value>::type>
		friend bool operator<(const bottomup_ad<Base, Dims...>& lhs, const T& rhs) { return addressof_multi_array(lhs[0])[0] < rhs; }
		template <class = typename enable_if<is_calcable<T, Base>::lt_value>::type>
		friend bool operator<(const T& lhs, const bottomup_ad<Base, Dims...>& rhs) { return lhs < addressof_multi_array(rhs[0])[0]; }
		template <class = typename enable_if<is_calcable<Base, T>::leq_value>::type>
		friend bool operator<=(const bottomup_ad<Base, Dims...>& lhs, const bottomup_ad<T, Dims...>& rhs) { return addressof_multi_array(lhs[0])[0] <= addressof_multi_array(rhs[0])[0]; }
		template <class = typename enable_if<is_calcable<T, Base>::leq_value>::type>
		friend bool operator<=(const bottomup_ad<T, Dims...>& lhs, const bottomup_ad<Base, Dims...>& rhs) { return addressof_multi_array(lhs[0])[0] <= addressof_multi_array(rhs[0])[0]; }
		template <class = typename enable_if<is_calcable<Base, T>::leq_value>::type>
		friend bool operator<=(const bottomup_ad<Base, Dims...>& lhs, const T& rhs) { return addressof_multi_array(lhs[0])[0] <= rhs; }
		template <class = typename enable_if<is_calcable<T, Base>::leq_value>::type>
		friend bool operator<=(const T& lhs, const bottomup_ad<Base, Dims...>& rhs) { return lhs <= addressof_multi_array(rhs[0])[0]; }
		template <class = typename enable_if<is_calcable<Base, T>::gt_value>::type>
		friend bool operator>(const bottomup_ad<Base, Dims...>& lhs, const bottomup_ad<T, Dims...>& rhs) { return addressof_multi_array(lhs[0])[0] > addressof_multi_array(rhs[0])[0]; }
		template <class = typename enable_if<is_calcable<T, Base>::gt_value>::type>
		friend bool operator>(const bottomup_ad<T, Dims...>& lhs, const bottomup_ad<Base, Dims...>& rhs) { return addressof_multi_array(lhs[0])[0] > addressof_multi_array(rhs[0])[0]; }
		template <class = typename enable_if<is_calcable<Base, T>::gt_value>::type>
		friend bool operator>(const bottomup_ad<Base, Dims...>& lhs, const T& rhs) { return addressof_multi_array(lhs[0])[0] > rhs; }
		template <class = typename enable_if<is_calcable<T, Base>::gt_value>::type>
		friend bool operator>(const T& lhs, const bottomup_ad<Base, Dims...>& rhs) { return lhs > addressof_multi_array(rhs[0])[0]; }
		template <class = typename enable_if<is_calcable<Base, T>::geq_value>::type>
		friend bool operator>=(const bottomup_ad<Base, Dims...>& lhs, const bottomup_ad<T, Dims...>& rhs) { return addressof_multi_array(lhs[0])[0] >= addressof_multi_array(rhs[0])[0]; }
		template <class = typename enable_if<is_calcable<T, Base>::geq_value>::type>
		friend bool operator>=(const bottomup_ad<T, Dims...>& lhs, const bottomup_ad<Base, Dims...>& rhs) { return addressof_multi_array(lhs[0])[0] >= addressof_multi_array(rhs[0])[0]; }
		template <class = typename enable_if<is_calcable<Base, T>::geq_value>::type>
		friend bool operator>=(const bottomup_ad<Base, Dims...>& lhs, const T& rhs) { return addressof_multi_array(lhs[0])[0] >= rhs; }
		template <class = typename enable_if<is_calcable<T, Base>::geq_value>::type>
		friend bool operator>=(const T& lhs, const bottomup_ad<Base, Dims...>& rhs) { return lhs >= addressof_multi_array(rhs[0])[0]; }
	};
	//下に階層が存在するかつBase == T
	template <class Base, size_t... Dims, class T, size_t... Indices>
	class bottomup_ad_base<Base, index_tuple<size_t, Dims...>, T, index_tuple<size_t, Indices...>, true, true> : public bottomup_ad_base<Base, index_tuple<size_t, Dims...>, typename T::algebraic_type> {
		template <class, size_t...> friend class bottomup_ad;
		template <class, class, class, class, bool, bool> friend class bottomup_ad_base;
		template <class> friend struct multiplication_traits;
		template <class> friend struct addition_traits;
	public:
		//コンストラクタの継承
		using bottomup_ad_base<Base, index_tuple<size_t, Dims...>, typename T::algebraic_type>::bottomup_ad_base;

		//単項演算の継承
		using bottomup_ad_base<Base, index_tuple<size_t, Dims...>, typename T::algebraic_type>::operator+;
		using bottomup_ad_base<Base, index_tuple<size_t, Dims...>, typename T::algebraic_type>::operator-;

		//代入演算の継承
		using bottomup_ad_base<Base, index_tuple<size_t, Dims...>, typename T::algebraic_type>::operator+=;
		using bottomup_ad_base<Base, index_tuple<size_t, Dims...>, typename T::algebraic_type>::operator-=;
		using bottomup_ad_base<Base, index_tuple<size_t, Dims...>, typename T::algebraic_type>::operator*=;
		using bottomup_ad_base<Base, index_tuple<size_t, Dims...>, typename T::algebraic_type>::operator/=;

		template <class U, class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		bottomup_ad_base& operator+=(const bottomup_ad_base<T, index_tuple<size_t, Dims...>, U>& n) {
			for (size_t i = 0; i < dimension<index_tuple<size_t, Dims...>>::value; ++i)
				addressof_multi_array(this->x)[i] += addressof_multi_array(n.x)[i];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		bottomup_ad_base& operator+=(const T& n) {
			addressof_multi_array(this->x)[0] += n;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		bottomup_ad_base& operator-=(const bottomup_ad_base<T, index_tuple<size_t, Dims...>, U>& n) {
			for (size_t i = 0; i < dimension<index_tuple<size_t, Dims...>>::value; ++i)
				addressof_multi_array(this->x)[i] -= addressof_multi_array(n.x)[i];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		bottomup_ad_base& operator-=(const T& n) {
			addressof_multi_array(this->x)[0] -= n;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		bottomup_ad_base& operator*=(const bottomup_ad_base<T, index_tuple<size_t, Dims...>, U>& n) {
			typename multi_array<Base, Dims...>::type result = {};
			multi_loop_multiplication<typename multi_array<Base, Dims...>::type>::loop(result, this->x, n.x, 1);
			for (size_t i = 0; i < dimension<index_tuple<size_t, Dims...>>::value; ++i)
				addressof_multi_array(this->x)[i] = addressof_multi_array(result)[i];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		bottomup_ad_base& operator*=(const T& k) {
			for (size_t i = 0; i < dimension<index_tuple<size_t, Dims...>>::value; ++i)
				addressof_multi_array(this->x)[i] *= k;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		bottomup_ad_base& operator/=(const bottomup_ad_base<T, index_tuple<size_t, Dims...>, U>& n) {
			return *this *= multiplication_traits<bottomup_ad<T, Dims...>>::inverse_element(n);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		bottomup_ad_base& operator/=(const T& k) {
			for (size_t i = 0; i < dimension<index_tuple<size_t, Dims...>>::value; ++i)
				addressof_multi_array(this->x)[i] /= k;
			return *this;
		}

		//添え字演算の継承
		using bottomup_ad_base<Base, index_tuple<size_t, Dims...>, typename T::algebraic_type>::operator[];


		//2項演算
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend bottomup_ad<Base, Dims...> operator+(const bottomup_ad<Base, Dims...>& lhs, const bottomup_ad<T, Dims...>& rhs) {
			return bottomup_ad<Base, Dims...>(addressof_multi_array(lhs[0])[0] + addressof_multi_array(rhs[0])[0]
				, (addressof_multi_array(lhs[0])[Indices] + addressof_multi_array(rhs[0])[Indices])...);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend bottomup_ad<Base, Dims...> operator+(const bottomup_ad<Base, Dims...>& lhs, const T& rhs) {
			return bottomup_ad<Base, Dims...>(addressof_multi_array(lhs[0])[0] + rhs
				, addressof_multi_array(lhs[0])[Indices]...);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::add_value>::type>
		friend bottomup_ad<Base, Dims...> operator+(const T& lhs, const bottomup_ad<Base, Dims...>& rhs) {
			return bottomup_ad<Base, Dims...>(lhs + addressof_multi_array(rhs[0])[0]
				, addressof_multi_array(rhs[0])[Indices]...);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend bottomup_ad<Base, Dims...> operator-(const bottomup_ad<Base, Dims...>& lhs, const bottomup_ad<T, Dims...>& rhs) {
			return bottomup_ad<Base, Dims...>(addressof_multi_array(lhs[0])[0] - addressof_multi_array(rhs[0])[0]
				, (addressof_multi_array(lhs[0])[Indices] - addressof_multi_array(rhs[0])[Indices])...);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend bottomup_ad<Base, Dims...> operator-(const bottomup_ad<Base, Dims...>& lhs, const T& rhs) {
			return bottomup_ad<Base, Dims...>(addressof_multi_array(lhs[0])[0] - rhs
				, addressof_multi_array(lhs[0])[Indices]...);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::sub_value>::type>
		friend bottomup_ad<Base, Dims...> operator-(const T& lhs, const bottomup_ad<Base, Dims...>& rhs) {
			return bottomup_ad<Base, Dims...>(lhs - addressof_multi_array(rhs[0])[0]
				, (-addressof_multi_array(rhs[0])[Indices])...);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend bottomup_ad<Base, Dims...> operator*(const bottomup_ad<Base, Dims...>& lhs, const bottomup_ad<T, Dims...>& rhs) {
			bottomup_ad<Base, Dims...> result;
			multi_loop_multiplication<typename multi_array<Base, Dims...>::type>::loop(addressof(result[0]), addressof(lhs[0]), addressof(rhs[0]), 1);
			return result;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend bottomup_ad<Base, Dims...> operator*(const bottomup_ad<Base, Dims...>& lhs, const T& rhs) {
			return bottomup_ad<Base, Dims...>(addressof_multi_array(lhs[0])[0] * rhs
				, (addressof_multi_array(lhs[0])[Indices] * rhs)...);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::mul_value>::type>
		friend bottomup_ad<Base, Dims...> operator*(const T& lhs, const bottomup_ad<Base, Dims...>& rhs) {
			return bottomup_ad<Base, Dims...>(lhs * addressof_multi_array(rhs[0])[0]
				, (lhs * addressof_multi_array(rhs[0])[Indices])...);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		friend bottomup_ad<Base, Dims...> operator/(const bottomup_ad<Base, Dims...>& lhs, const bottomup_ad<T, Dims...>& rhs) {
			return lhs * multiplication_traits<bottomup_ad<T, Dims...>>::inverse_element(rhs);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		friend bottomup_ad<Base, Dims...> operator/(const bottomup_ad<Base, Dims...>& lhs, const T& rhs) {
			return bottomup_ad<Base, Dims...>(addressof_multi_array(lhs[0])[0] / rhs
				, (addressof_multi_array(lhs[0])[Indices] / rhs)...);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::div_value>::type>
		friend bottomup_ad<Base, Dims...> operator/(const T& lhs, const bottomup_ad<Base, Dims...>& rhs) {
			return lhs * multiplication_traits<bottomup_ad<Base, Dims...>>::inverse_element(rhs);
		}


		//比較演算
		template <class = typename enable_if<is_calcable<Base, T>::eq_value>::type>
		friend bool operator==(const bottomup_ad<Base, Dims...>& lhs, const bottomup_ad<T, Dims...>& rhs) { return addressof_multi_array(lhs[0])[0] == addressof_multi_array(rhs[0])[0]; }
		template <class = typename enable_if<is_calcable<Base, T>::eq_value>::type>
		friend bool operator==(const bottomup_ad<Base, Dims...>& lhs, const T& rhs) { return addressof_multi_array(lhs[0])[0] == rhs; }
		template <class = typename enable_if<is_calcable<T, Base>::eq_value>::type>
		friend bool operator==(const T& lhs, const bottomup_ad<Base, Dims...>& rhs) { return lhs == addressof_multi_array(rhs[0])[0]; }
		template <class = typename enable_if<is_calcable<Base, T>::eq_value>::type>
		friend bool operator!=(const bottomup_ad<Base, Dims...>& lhs, const bottomup_ad<T, Dims...>& rhs) { return addressof_multi_array(lhs[0])[0] != addressof_multi_array(rhs[0])[0]; }
		template <class = typename enable_if<is_calcable<Base, T>::eq_value>::type>
		friend bool operator!=(const bottomup_ad<Base, Dims...>& lhs, const T& rhs) { return addressof_multi_array(lhs[0])[0] != rhs; }
		template <class = typename enable_if<is_calcable<T, Base>::eq_value>::type>
		friend bool operator!=(const T& lhs, const bottomup_ad<Base, Dims...>& rhs) { return lhs != addressof_multi_array(rhs[0])[0]; }
		template <class = typename enable_if<is_calcable<Base, T>::lt_value>::type>
		friend bool operator<(const bottomup_ad<Base, Dims...>& lhs, const bottomup_ad<T, Dims...>& rhs) { return addressof_multi_array(lhs[0])[0] < addressof_multi_array(rhs[0])[0]; }
		template <class = typename enable_if<is_calcable<Base, T>::lt_value>::type>
		friend bool operator<(const bottomup_ad<Base, Dims...>& lhs, const T& rhs) { return addressof_multi_array(lhs[0])[0] < rhs; }
		template <class = typename enable_if<is_calcable<T, Base>::lt_value>::type>
		friend bool operator<(const T& lhs, const bottomup_ad<Base, Dims...>& rhs) { return lhs < addressof_multi_array(rhs[0])[0]; }
		template <class = typename enable_if<is_calcable<Base, T>::leq_value>::type>
		friend bool operator<=(const bottomup_ad<Base, Dims...>& lhs, const bottomup_ad<T, Dims...>& rhs) { return addressof_multi_array(lhs[0])[0] <= addressof_multi_array(rhs[0])[0]; }
		template <class = typename enable_if<is_calcable<Base, T>::leq_value>::type>
		friend bool operator<=(const bottomup_ad<Base, Dims...>& lhs, const T& rhs) { return addressof_multi_array(lhs[0])[0] <= rhs; }
		template <class = typename enable_if<is_calcable<T, Base>::leq_value>::type>
		friend bool operator<=(const T& lhs, const bottomup_ad<Base, Dims...>& rhs) { return lhs <= addressof_multi_array(rhs[0])[0]; }
		template <class = typename enable_if<is_calcable<Base, T>::gt_value>::type>
		friend bool operator>(const bottomup_ad<Base, Dims...>& lhs, const bottomup_ad<T, Dims...>& rhs) { return addressof_multi_array(lhs[0])[0] > addressof_multi_array(rhs[0])[0]; }
		template <class = typename enable_if<is_calcable<Base, T>::gt_value>::type>
		friend bool operator>(const bottomup_ad<Base, Dims...>& lhs, const T& rhs) { return addressof_multi_array(lhs[0])[0] > rhs; }
		template <class = typename enable_if<is_calcable<T, Base>::gt_value>::type>
		friend bool operator>(const T& lhs, const bottomup_ad<Base, Dims...>& rhs) { return lhs > addressof_multi_array(rhs[0])[0]; }
		template <class = typename enable_if<is_calcable<Base, T>::geq_value>::type>
		friend bool operator>=(const bottomup_ad<Base, Dims...>& lhs, const bottomup_ad<T, Dims...>& rhs) { return addressof_multi_array(lhs[0])[0] >= addressof_multi_array(rhs[0])[0]; }
		template <class = typename enable_if<is_calcable<Base, T>::geq_value>::type>
		friend bool operator>=(const bottomup_ad<Base, Dims...>& lhs, const T& rhs) { return addressof_multi_array(lhs[0])[0] >= rhs; }
		template <class = typename enable_if<is_calcable<T, Base>::geq_value>::type>
		friend bool operator>=(const T& lhs, const bottomup_ad<Base, Dims...>& rhs) { return lhs >= addressof_multi_array(rhs[0])[0]; }
	};
	//下に階層が存在するかつBase != T
	template <class Base, size_t... Dims, class T, size_t... Indices>
	class bottomup_ad_base<Base, index_tuple<size_t, Dims...>, T, index_tuple<size_t, Indices...>, true, false> : public bottomup_ad_base<Base, index_tuple<size_t, Dims...>, typename T::algebraic_type> {
		template <class, size_t...> friend class bottomup_ad;
		template <class, class, class, class, bool, bool> friend class bottomup_ad_base;
		template <class> friend struct multiplication_traits;
		template <class> friend struct addition_traits;
	public:
		//コンストラクタの継承
		using bottomup_ad_base<Base, index_tuple<size_t, Dims...>, typename T::algebraic_type>::bottomup_ad_base;

		constexpr bottomup_ad_base() : bottomup_ad_base<Base, index_tuple<size_t, Dims...>, typename T::algebraic_type>() {}
		template <class = typename enable_if<is_inclusion<T, Base>::value>::type>
		constexpr bottomup_ad_base(const T& re) : bottomup_ad_base<Base, index_tuple<size_t, Dims...>, typename T::algebraic_type>(static_cast<Base>(re)) {}
		template <class = typename enable_if<is_inclusion<T, Base>::value>::type>
		constexpr bottomup_ad_base(const T& re, const identity_t<T, Indices>&... x) : bottomup_ad_base<Base, index_tuple<size_t, Dims...>, typename T::algebraic_type>(static_cast<Base>(re), static_cast<Base>(x)...) {}
		template <class U, class = typename enable_if<is_inclusion<T, Base>::value>::type>
		constexpr bottomup_ad_base(const bottomup_ad_base<T, index_tuple<size_t, Dims...>, U>& n) : bottomup_ad_base<Base, index_tuple<size_t, Dims...>, typename T::algebraic_type>(static_cast<Base>(n.x[0]), static_cast<Base>(n.x[Indices])...) {}

		//単項演算の継承
		using bottomup_ad_base<Base, index_tuple<size_t, Dims...>, typename T::algebraic_type>::operator+;
		using bottomup_ad_base<Base, index_tuple<size_t, Dims...>, typename T::algebraic_type>::operator-;

		//代入演算の継承
		using bottomup_ad_base<Base, index_tuple<size_t, Dims...>, typename T::algebraic_type>::operator+=;
		using bottomup_ad_base<Base, index_tuple<size_t, Dims...>, typename T::algebraic_type>::operator-=;
		using bottomup_ad_base<Base, index_tuple<size_t, Dims...>, typename T::algebraic_type>::operator*=;
		using bottomup_ad_base<Base, index_tuple<size_t, Dims...>, typename T::algebraic_type>::operator/=;

		template <class U, class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		bottomup_ad_base& operator+=(const bottomup_ad_base<T, index_tuple<size_t, Dims...>, U>& n) {
			for (size_t i = 0; i < dimension<index_tuple<size_t, Dims...>>::value; ++i)
				addressof_multi_array(this->x)[i] += addressof_multi_array(n.x)[i];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		bottomup_ad_base& operator+=(const T& n) {
			addressof_multi_array(this->x)[0] += n;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		bottomup_ad_base& operator-=(const bottomup_ad_base<T, index_tuple<size_t, Dims...>, U>& n) {
			for (size_t i = 0; i < dimension<index_tuple<size_t, Dims...>>::value; ++i)
				addressof_multi_array(this->x)[i] -= addressof_multi_array(n.x)[i];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		bottomup_ad_base& operator-=(const T& n) {
			addressof_multi_array(this->x)[0] -= n;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		bottomup_ad_base& operator*=(const bottomup_ad_base<T, index_tuple<size_t, Dims...>, U>& n) {
			typename multi_array<Base, Dims...>::type result = {};
			multi_loop_multiplication<typename multi_array<Base, Dims...>::type>::loop(result, this->x, n.x, 1);
			for (size_t i = 0; i < dimension<index_tuple<size_t, Dims...>>::value; ++i)
				addressof_multi_array(this->x)[i] = addressof_multi_array(result)[i];
			return *this;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		bottomup_ad_base& operator*=(const T& k) {
			for (size_t i = 0; i < dimension<index_tuple<size_t, Dims...>>::value; ++i)
				addressof_multi_array(this->x)[i] *= k;
			return *this;
		}
		template <class U, class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		bottomup_ad_base& operator/=(const bottomup_ad_base<T, index_tuple<size_t, Dims...>, U>& n) {
			return *this *= multiplication_traits<bottomup_ad<T, Dims...>>::inverse_element(n);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		bottomup_ad_base& operator/=(const T& k) {
			for (size_t i = 0; i < dimension<index_tuple<size_t, Dims...>>::value; ++i)
				addressof_multi_array(this->x)[i] /= k;
			return *this;
		}

		//添え字演算の継承
		using bottomup_ad_base<Base, index_tuple<size_t, Dims...>, typename T::algebraic_type>::operator[];


		//2項演算
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend bottomup_ad<Base, Dims...> operator+(const bottomup_ad<Base, Dims...>& lhs, const bottomup_ad<T, Dims...>& rhs) {
			return bottomup_ad<Base, Dims...>(addressof_multi_array(lhs[0])[0] + addressof_multi_array(rhs[0])[0]
				, (addressof_multi_array(lhs[0])[Indices] + addressof_multi_array(rhs[0])[Indices])...);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::add_value>::type>
		friend bottomup_ad<Base, Dims...> operator+(const bottomup_ad<T, Dims...>& lhs, const bottomup_ad<Base, Dims...>& rhs) {
			return bottomup_ad<Base, Dims...>(addressof_multi_array(lhs[0])[0] + addressof_multi_array(rhs[0])[0]
				, (addressof_multi_array(lhs[0])[Indices] + addressof_multi_array(rhs[0])[Indices])...);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::add_value>::type>
		friend bottomup_ad<Base, Dims...> operator+(const bottomup_ad<Base, Dims...>& lhs, const T& rhs) {
			return bottomup_ad<Base, Dims...>(addressof_multi_array(lhs[0])[0] + rhs
				, addressof_multi_array(lhs[0])[Indices]...);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::add_value>::type>
		friend bottomup_ad<Base, Dims...> operator+(const T& lhs, const bottomup_ad<Base, Dims...>& rhs) {
			return bottomup_ad<Base, Dims...>(lhs + addressof_multi_array(rhs[0])[0]
				, addressof_multi_array(rhs[0])[Indices]...);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend bottomup_ad<Base, Dims...> operator-(const bottomup_ad<Base, Dims...>& lhs, const bottomup_ad<T, Dims...>& rhs) {
			return bottomup_ad<Base, Dims...>(addressof_multi_array(lhs[0])[0] - addressof_multi_array(rhs[0])[0]
				, (addressof_multi_array(lhs[0])[Indices] - addressof_multi_array(rhs[0])[Indices])...);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::sub_value>::type>
		friend bottomup_ad<Base, Dims...> operator-(const bottomup_ad<T, Dims...>& lhs, const bottomup_ad<Base, Dims...>& rhs) {
			return bottomup_ad<Base, Dims...>(addressof_multi_array(lhs[0])[0] - addressof_multi_array(rhs[0])[0]
				, (addressof_multi_array(lhs[0])[Indices] - addressof_multi_array(rhs[0])[Indices])...);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::sub_value>::type>
		friend bottomup_ad<Base, Dims...> operator-(const bottomup_ad<Base, Dims...>& lhs, const T& rhs) {
			return bottomup_ad<Base, Dims...>(addressof_multi_array(lhs[0])[0] - rhs
				, addressof_multi_array(lhs[0])[Indices]...);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::sub_value>::type>
		friend bottomup_ad<Base, Dims...> operator-(const T& lhs, const bottomup_ad<Base, Dims...>& rhs) {
			return bottomup_ad<Base, Dims...>(lhs - addressof_multi_array(rhs[0])[0]
				, (-addressof_multi_array(rhs[0])[Indices])...);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend bottomup_ad<Base, Dims...> operator*(const bottomup_ad<Base, Dims...>& lhs, const bottomup_ad<T, Dims...>& rhs) {
			bottomup_ad<Base, Dims...> result;
			multi_loop_multiplication<typename multi_array<Base, Dims...>::type>::loop(addressof(result[0]), addressof(lhs[0]), addressof(rhs[0]), 1);
			return result;
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::mul_value>::type>
		friend bottomup_ad<Base, Dims...> operator*(const bottomup_ad<T, Dims...>& lhs, const bottomup_ad<Base, Dims...>& rhs) {
			bottomup_ad<Base, Dims...> result;
			multi_loop_multiplication<typename multi_array<Base, Dims...>::type>::loop(addressof(result[0]), addressof(lhs[0]), addressof(rhs[0]), 1);
			return result;
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::mul_value>::type>
		friend bottomup_ad<Base, Dims...> operator*(const bottomup_ad<Base, Dims...>& lhs, const T& rhs) {
			return bottomup_ad<Base, Dims...>(addressof_multi_array(lhs[0])[0] * rhs
				, (addressof_multi_array(lhs[0])[Indices] * rhs)...);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::mul_value>::type>
		friend bottomup_ad<Base, Dims...> operator*(const T& lhs, const bottomup_ad<Base, Dims...>& rhs) {
			return bottomup_ad<Base, Dims...>(lhs * addressof_multi_array(rhs[0])[0]
				, (lhs * addressof_multi_array(rhs[0])[Indices])...);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		friend bottomup_ad<Base, Dims...> operator/(const bottomup_ad<Base, Dims...>& lhs, const bottomup_ad<T, Dims...>& rhs) {
			return lhs * multiplication_traits<bottomup_ad<T, Dims...>>::inverse_element(rhs);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::div_value>::type>
		friend bottomup_ad<Base, Dims...> operator/(const bottomup_ad<T, Dims...>& lhs, const bottomup_ad<Base, Dims...>& rhs) {
			return lhs * multiplication_traits<bottomup_ad<Base, Dims...>>::inverse_element(rhs);
		}
		template <class = typename enable_if<is_operation<Base, T, Base>::div_value>::type>
		friend bottomup_ad<Base, Dims...> operator/(const bottomup_ad<Base, Dims...>& lhs, const T& rhs) {
			return bottomup_ad<Base, Dims...>(addressof_multi_array(lhs[0])[0] / rhs
				, (addressof_multi_array(lhs[0])[Indices] / rhs)...);
		}
		template <class = typename enable_if<is_operation<T, Base, Base>::div_value>::type>
		friend bottomup_ad<Base, Dims...> operator/(const T& lhs, const bottomup_ad<Base, Dims...>& rhs) {
			return lhs * multiplication_traits<bottomup_ad<Base, Dims...>>::inverse_element(rhs);
		}


		//比較演算
		template <class = typename enable_if<is_calcable<Base, T>::eq_value>::type>
		friend bool operator==(const bottomup_ad<Base, Dims...>& lhs, const bottomup_ad<T, Dims...>& rhs) { return addressof_multi_array(lhs[0])[0] == addressof_multi_array(rhs[0])[0]; }
		template <class = typename enable_if<is_calcable<T, Base>::eq_value>::type>
		friend bool operator==(const bottomup_ad<T, Dims...>& lhs, const bottomup_ad<Base, Dims...>& rhs) { return addressof_multi_array(lhs[0])[0] == addressof_multi_array(rhs[0])[0]; }
		template <class = typename enable_if<is_calcable<Base, T>::eq_value>::type>
		friend bool operator==(const bottomup_ad<Base, Dims...>& lhs, const T& rhs) { return addressof_multi_array(lhs[0])[0] == rhs; }
		template <class = typename enable_if<is_calcable<T, Base>::eq_value>::type>
		friend bool operator==(const T& lhs, const bottomup_ad<Base, Dims...>& rhs) { return lhs == addressof_multi_array(rhs[0])[0]; }
		template <class = typename enable_if<is_calcable<Base, T>::eq_value>::type>
		friend bool operator!=(const bottomup_ad<Base, Dims...>& lhs, const bottomup_ad<T, Dims...>& rhs) { return addressof_multi_array(lhs[0])[0] != addressof_multi_array(rhs[0])[0]; }
		template <class = typename enable_if<is_calcable<T, Base>::eq_value>::type>
		friend bool operator!=(const bottomup_ad<T, Dims...>& lhs, const bottomup_ad<Base, Dims...>& rhs) { return addressof_multi_array(lhs[0])[0] != addressof_multi_array(rhs[0])[0]; }
		template <class = typename enable_if<is_calcable<Base, T>::eq_value>::type>
		friend bool operator!=(const bottomup_ad<Base, Dims...>& lhs, const T& rhs) { return addressof_multi_array(lhs[0])[0] != rhs; }
		template <class = typename enable_if<is_calcable<T, Base>::eq_value>::type>
		friend bool operator!=(const T& lhs, const bottomup_ad<Base, Dims...>& rhs) { return lhs != addressof_multi_array(rhs[0])[0]; }
		template <class = typename enable_if<is_calcable<Base, T>::lt_value>::type>
		friend bool operator<(const bottomup_ad<Base, Dims...>& lhs, const bottomup_ad<T, Dims...>& rhs) { return addressof_multi_array(lhs[0])[0] < addressof_multi_array(rhs[0])[0]; }
		template <class = typename enable_if<is_calcable<T, Base>::lt_value>::type>
		friend bool operator<(const bottomup_ad<T, Dims...>& lhs, const bottomup_ad<Base, Dims...>& rhs) { return addressof_multi_array(lhs[0])[0] < addressof_multi_array(rhs[0])[0]; }
		template <class = typename enable_if<is_calcable<Base, T>::lt_value>::type>
		friend bool operator<(const bottomup_ad<Base, Dims...>& lhs, const T& rhs) { return addressof_multi_array(lhs[0])[0] < rhs; }
		template <class = typename enable_if<is_calcable<T, Base>::lt_value>::type>
		friend bool operator<(const T& lhs, const bottomup_ad<Base, Dims...>& rhs) { return lhs < addressof_multi_array(rhs[0])[0]; }
		template <class = typename enable_if<is_calcable<Base, T>::leq_value>::type>
		friend bool operator<=(const bottomup_ad<Base, Dims...>& lhs, const bottomup_ad<T, Dims...>& rhs) { return addressof_multi_array(lhs[0])[0] <= addressof_multi_array(rhs[0])[0]; }
		template <class = typename enable_if<is_calcable<T, Base>::leq_value>::type>
		friend bool operator<=(const bottomup_ad<T, Dims...>& lhs, const bottomup_ad<Base, Dims...>& rhs) { return addressof_multi_array(lhs[0])[0] <= addressof_multi_array(rhs[0])[0]; }
		template <class = typename enable_if<is_calcable<Base, T>::leq_value>::type>
		friend bool operator<=(const bottomup_ad<Base, Dims...>& lhs, const T& rhs) { return addressof_multi_array(lhs[0])[0] <= rhs; }
		template <class = typename enable_if<is_calcable<T, Base>::leq_value>::type>
		friend bool operator<=(const T& lhs, const bottomup_ad<Base, Dims...>& rhs) { return lhs <= addressof_multi_array(rhs[0])[0]; }
		template <class = typename enable_if<is_calcable<Base, T>::gt_value>::type>
		friend bool operator>(const bottomup_ad<Base, Dims...>& lhs, const bottomup_ad<T, Dims...>& rhs) { return addressof_multi_array(lhs[0])[0] > addressof_multi_array(rhs[0])[0]; }
		template <class = typename enable_if<is_calcable<T, Base>::gt_value>::type>
		friend bool operator>(const bottomup_ad<T, Dims...>& lhs, const bottomup_ad<Base, Dims...>& rhs) { return addressof_multi_array(lhs[0])[0] > addressof_multi_array(rhs[0])[0]; }
		template <class = typename enable_if<is_calcable<Base, T>::gt_value>::type>
		friend bool operator>(const bottomup_ad<Base, Dims...>& lhs, const T& rhs) { return addressof_multi_array(lhs[0])[0] > rhs; }
		template <class = typename enable_if<is_calcable<T, Base>::gt_value>::type>
		friend bool operator>(const T& lhs, const bottomup_ad<Base, Dims...>& rhs) { return lhs > addressof_multi_array(rhs[0])[0]; }
		template <class = typename enable_if<is_calcable<Base, T>::geq_value>::type>
		friend bool operator>=(const bottomup_ad<Base, Dims...>& lhs, const bottomup_ad<T, Dims...>& rhs) { return addressof_multi_array(lhs[0])[0] >= addressof_multi_array(rhs[0])[0]; }
		template <class = typename enable_if<is_calcable<T, Base>::geq_value>::type>
		friend bool operator>=(const bottomup_ad<T, Dims...>& lhs, const bottomup_ad<Base, Dims...>& rhs) { return addressof_multi_array(lhs[0])[0] >= addressof_multi_array(rhs[0])[0]; }
		template <class = typename enable_if<is_calcable<Base, T>::geq_value>::type>
		friend bool operator>=(const bottomup_ad<Base, Dims...>& lhs, const T& rhs) { return addressof_multi_array(lhs[0])[0] >= rhs; }
		template <class = typename enable_if<is_calcable<T, Base>::geq_value>::type>
		friend bool operator>=(const T& lhs, const bottomup_ad<Base, Dims...>& rhs) { return lhs >= addressof_multi_array(rhs[0])[0]; }
	};

	//ボトムアップ型自動微分
	template <class T, size_t... Dims>
	class bottomup_ad : public bottomup_ad_base<T, index_tuple<size_t, Dims...>, T>, public value_list_input<array_iterator<T>> {
		template <class, size_t...> friend class bottomup_ad;
		template <class, class, class, class, bool, bool> friend class bottomup_ad_base;
		template <class> friend struct multiplication_traits;
		template <class> friend struct addition_traits;

		static_assert(dimension< index_tuple<size_t, Dims...>>::value > 0, "number of array elements must greater than 0");

		//代入演算の補助
		template <class _T>
		static bottomup_ad* bottomup_ad_copy(bottomup_ad* const lhs, const bottomup_ad<_T, Dims...>& rhs) {
			//同じインスタンスでなければ代入
			//if (static_cast<void*>(addressof(lhs->x)) != static_cast<void*>(const_cast<_T*>(addressof(rhs[0]))))
				for (size_t i = 0; i < dimension<index_tuple<size_t, Dims...>>::value; ++i) addressof_multi_array(lhs->x)[i] = static_cast<T>(addressof_multi_array(rhs.x)[i]);
			return lhs;
		}
	public:
		//コンストラクタの継承
		using bottomup_ad_base<T, index_tuple<size_t, Dims...>, T>::bottomup_ad_base;

		using algebraic_type = T;
		using iterator = array_iterator<T>;
		using const_iterator = array_iterator<const T>;
		using dimension_type = index_tuple<size_t, Dims...>;

		template<class Other>
		struct rebind {
			using other = bottomup_ad<Other, Dims...>;
		};

		iterator begin() noexcept { return iterator(addressof_multi_array(this->x)); }
		const_iterator begin() const noexcept { return const_iterator(addressof_multi_array(this->x)); }
		iterator end() noexcept { return iterator(addressof(addressof_multi_array(this->x)[dimension<index_tuple<size_t, Dims...>>::value - 1]) + 1); }
		const_iterator end() const noexcept { return const_iterator(addressof(addressof_multi_array(this->x)[dimension<index_tuple<size_t, Dims...>>::value - 1]) + 1); }

		//単項演算の継承
		using bottomup_ad_base<T, index_tuple<size_t, Dims...>, T>::operator-;
		using bottomup_ad_base<T, index_tuple<size_t, Dims...>, T>::operator+;
		//代入演算の継承
		using bottomup_ad_base<T, index_tuple<size_t, Dims...>, T>::operator+=;
		using bottomup_ad_base<T, index_tuple<size_t, Dims...>, T>::operator-=;
		using bottomup_ad_base<T, index_tuple<size_t, Dims...>, T>::operator*=;
		using bottomup_ad_base<T, index_tuple<size_t, Dims...>, T>::operator/=;
		//二項演算の継承
		using value_list_input<array_iterator<T>>::operator<<;

		//代入演算
		bottomup_ad& operator=(const bottomup_ad& n) { return *bottomup_ad_copy(this, n); }
		template <class _T>
		bottomup_ad& operator=(const bottomup_ad<_T, Dims...>& n) { return *bottomup_ad_copy(this, n); }
		bottomup_ad& operator=(const bottomup_ad_base<T, index_tuple<size_t, Dims...>, T>& n) { return *bottomup_ad_copy(this, n); }
		template <class U>
		bottomup_ad& operator=(const bottomup_ad_base<T, index_tuple<size_t, Dims...>, U>& n) { return *bottomup_ad_copy(this, n); }
		template <class U, class _T, class = typename enable_if<is_inclusion<_T, T>::value>::type>
		bottomup_ad& operator=(const bottomup_ad_base<_T, index_tuple<size_t, Dims...>, U>& n) { return *bottomup_ad_copy(this, n); }

		//添え字演算の継承
		using bottomup_ad_base<T, index_tuple<size_t, Dims...>, T>::operator[];

		//ストリーム出力
		friend std::ostream& operator<<(std::ostream& os, const bottomup_ad& n) {
			os << addressof_multi_array(n.x)[0];
			for (size_t i = 1; i < dimension<index_tuple<size_t, Dims...>>::value; ++i) os << ',' << addressof_multi_array(n.x)[i];
			return os;
		}
		friend std::wostream& operator<<(std::wostream& os, const bottomup_ad& n) {
			os << addressof_multi_array(n.x)[0];
			for (size_t i = 1; i < dimension<index_tuple<size_t, Dims...>>::value; ++i) os << L',' << addressof_multi_array(n.x)[i];
			return os;
		}
	};


	//ボトムアップ型dimension<index_tuple<size_t, Dims...>>::value階自動微分の判定
	template <class T>
	struct _Is_bottomup_ad : false_type {};
	template <class T, size_t... Dims>
	struct _Is_bottomup_ad<bottomup_ad<T, Dims...>> : true_type {};
	template <class T>
	struct is_bottomup_ad : _Is_bottomup_ad<typename remove_cv<T>::type> {};

	//ボトムアップ型dimension<index_tuple<size_t, Dims...>>::value階自動微分の除去
	template <class T>
	struct remove_bottomup_ad {
		using type = T;
	};
	template <class T, size_t... Dims>
	struct remove_bottomup_ad<bottomup_ad<T, Dims...>> {
		using type = T;
	};

	//全てのボトムアップ型dimension<index_tuple<size_t, Dims...>>::value階自動微分の除去
	template <class T>
	struct remove_all_bottomup_ad {
		using type = T;
	};
	template <class T, size_t... Dims>
	struct remove_all_bottomup_ad<bottomup_ad<T, Dims...>> : remove_all_bottomup_ad<T> {};


	//N番目の変数を設定するための添え字の取得
	template <size_t N, size_t, class, bool = (N == 0)>
	struct ad_variable_suffix_impl;
	template <size_t Result, size_t... Indices>
	struct ad_variable_suffix_impl<0, Result, index_tuple<size_t, Indices...>, true> {
		static constexpr size_t value = Result;
	};
	template <size_t N, size_t Result, size_t First, size_t... Indices>
	struct ad_variable_suffix_impl<N, Result, index_tuple<size_t, First, Indices...>, false>
		: ad_variable_suffix_impl<N - 1, Result*First, index_tuple<size_t, Indices...>> {};
	template <size_t, class>
	struct ad_variable_suffix;
	template <size_t N, size_t... Dims>
	struct ad_variable_suffix<N, index_tuple<size_t, Dims...>>
		: ad_variable_suffix_impl<sizeof...(Dims) - 1 - N, 1, typename reverse_index_tuple<index_tuple<size_t, Dims...>>::type> {};
	//自動微分をするための変数
	template <class AD, size_t N, class T>
	AD ad_variable(const T& x) {
		AD result(x);
		addressof_multi_array(result[0])[ad_variable_suffix<N, typename AD::dimension_type>::value] = 1;
		return result;
	}


	template<class T, size_t... Dims>
	struct Ceil<bottomup_ad<T, Dims...>> {
		static constexpr bottomup_ad<T, Dims...> _ceil_(const bottomup_ad<T, Dims...>& x) { return bottomup_ad<T, Dims...>(ceil(x[0])); }
	};
	template<class T, size_t... Dims>
	struct Floor<bottomup_ad<T, Dims...>> {
		static constexpr bottomup_ad<T, Dims...> _floor_(const bottomup_ad<T, Dims...>& x) { return bottomup_ad<T, Dims...>(floor(x[0])); }
	};
	template <class T, size_t... Dims>
	struct Pi<bottomup_ad<T, Dims...>> {
		static constexpr auto _pi_(size_t n = 2) { return Pi<T>::_pi_(n); }
	};
}

namespace iml {

	//加法パラメータ取得
	template <class T, size_t... Dims>
	struct addition_traits<bottomup_ad<T, Dims...>> {
		//単位元の取得
		static constexpr bottomup_ad<T, Dims...> identity_element() {
			return bottomup_ad<T, Dims...>();
		}
		//逆元の取得
		template <class = typename enable_if<is_exist_add_inverse_element<T>::value>::type>
		static constexpr bottomup_ad<T, Dims...> inverse_element(const bottomup_ad<T, Dims...>& x) {
			return -x;
		}
	};
	//乗法パラメータ取得
	template <class T, size_t... Dims>
	struct multiplication_traits<bottomup_ad<T, Dims...>> {
	private:
		//除算用の多重ループ(bool:終了条件)
		template <class S, bool = (!is_same<typename remove_extent<S>::type, typename remove_all_extents<S>::type>::value)>
		struct multi_loop_division {};
		template <class S, size_t N>
		struct multi_loop_division<S[N], true> {
			//乗算に渡すためのループ(lhsとrhsが同じ型になるまで添え字アクセス)
			static void loop_impl(S& result, const S& lhs, const S& rhs, const T& c) {
				typename bottomup_ad<T, Dims...>::multi_loop_multiplication<S>::loop(result, lhs, rhs, c);
			}
			template <class U, size_t M, class = typename enable_if<!is_same<S, U>::value>::type>
			static void loop_impl(S& result, const S &lhs, const U& rhs, const T& c) {
				loop_impl(result, lhs, rhs[0], c);
			}


			//result:結果, x:元データ, table:xの冪乗テーブル, p:xの逆数の冪
			template <size_t M>
			static void loop(S(&result)[N], const S(&x)[N], const bottomup_ad<T, Dims...>(&table)[M], size_t p) {
				T temp1 = 1 / pow(addressof_multi_array(x)[0], p);
				for (size_t i = 1; i < N; ++i) {
					T temp2 = p * binom(p + i, i);
					for (size_t j = 1; j <= i; ++j) {
						//逆数の導関数の計算
						S val = { temp1 };
						multi_loop_division<S>::loop(val, x[0], table, j + p);

						temp2 *= -T(i + 1 - j) / j;
						loop_impl(result[i], table[j].x[i], val, temp2 / (p + j));
					}
				}
			}
		};
		template <class S, size_t N>
		struct multi_loop_division<S[N], false> {
			//tableとresultが同じ型になるまで添え字アクセス
			static void loop_impl(size_t i, S(&result)[N], const S(&table)[N], const T& c) {
				result[i] += c * table[i];
			}
			template <class U, size_t M, class = typename enable_if<!is_same<S, U>::value>::type>
			static void loop_impl(size_t i, S(&result)[N], const U(&table)[M], const T& c) {
				loop_impl(i, result, table[0], c);
			}

			template <size_t M>
			static void loop(S(&result)[N], const S(&x)[N], const bottomup_ad<T, Dims...>(&table)[M], size_t p) {
				//p*f^(-p)部の構築
				T temp1 = p / pow(addressof_multi_array(x)[0], p);

				for (size_t i = 1; i < N; ++i) {
					T temp2 = temp1 * binom(p + i, i);
					for (size_t j = 1; j <= i; ++j) {
						temp2 *= (i + 1 - j) / (-addressof_multi_array(x)[0] * j);
						loop_impl(i, result, table[j].x, temp2 / (p + j));
					}
				}
			}
		};
	public:
		//単位元の取得
		static constexpr bottomup_ad<T, Dims...> identity_element() {
			return bottomup_ad<T, Dims...>(multiplication_traits<T>::identity_element());
		}
		//逆元の取得
		static constexpr bottomup_ad<T, Dims...> inverse_element(const bottomup_ad<T, Dims...>& x) {
			bottomup_ad<T, Dims...> result(multiplicative_inverse(addressof_multi_array(x.x)[0]));
			bottomup_ad<T, Dims...> table[template_max<index_tuple<size_t, Dims...>>::value] = { identity_element() };		//xの冪乗のテーブル(1,x,x^2,...)
			//xの冪乗のテーブルの構築
			for (size_t i = 1; i < template_max<index_tuple<size_t, Dims...>>::value; ++i) table[i] = table[i - 1] * x;

			multi_loop_division<typename multi_array<T, Dims...>::type>::loop(result.x, x.x, table, 1);
			return result;
		}
		//吸収元
		static constexpr bottomup_ad<T, Dims...> absorbing_element() {
			return bottomup_ad<T, Dims...>();
		}
	};

}

namespace iml {

	//誤差評価
	template <class T, size_t... Dims>
	struct Error_evaluation<bottomup_ad<T, Dims...>> {
		static constexpr T epsilon() { return Error_evaluation<T>::epsilon(); }

		static bool _error_evaluation_(const bottomup_ad<T, Dims...>& lhs, const bottomup_ad<T, Dims...>& rhs) {
			return error_evaluation(addressof_multi_array(lhs[0])[0], addressof_multi_array(rhs[0])[0]);
		}
	};
}


namespace iml {

	/*//任意のスカラー場(φ(x1,x2,x3,...) = c)の勾配ベクトル
	template <class>
	class gradient_vector;
	template <class R, class... Args>
	class gradient_vector<R(Args...)> {
		function<bottomup_ad<R, 1>(bottomup_ad<Args, 1>...)> f_m;

		//連番リストからgradient_vectorの作成
		template <class T>
		struct make_gradient_vector_struct;
		template <size_t... Indices>
		struct make_gradient_vector_struct<index_tuple<size_t, Indices...>> {
			static vector<R, sizeof...(Args)> __make_gradient_vector(_Automatic_partial_differentiation<R(Args...), 1>& grad, Args... args) {
				return vector<R, sizeof...(Args)>(grad.get<Indices, 1>(iml::forward<Args>(args)...)...);
			}
		};
	public:
		constexpr gradient_vector() {}
		template <class F>
		constexpr gradient_vector(F f) : f_m(f) {}

		template <class F>
		void set(F f) { f_m = f; }

		vector<R, sizeof...(Args)> operator()(Args... args) {
			return make_gradient_vector_struct<typename index_imu_range<0, sizeof...(Args)>::type>::__make_gradient_vector(grad, forward<Args>(args)...);

		}
	};*/

}


#endif