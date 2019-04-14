#ifndef IMATH_MATH_COMPLEX_HPP
#define IMATH_MATH_COMPLEX_HPP

#include "IMathLib/utility/utility/tuple_type.hpp"
#include "IMathLib/math/math/math_traits.hpp"
#include "IMathLib/math/math/type_parameter.hpp"
#include "IMathLib/container/array.hpp"


namespace iml {

	//複素数型の代入演算の定義
#define	IMATH_COMPLEX_COMPOUND_ASSIGNMENT_OPERATION \
	template <class = enable_if_t<is_inclusion_v<T, Base>>>\
	complex_base& operator=(const complex<T>& n) {\
		/*同じインスタンスでなければ代入*/\
		if (this != addressof(ma)) { this->x[0] = n.x[0]; this->x[1] = n.x[1]; } \
		return *this;\
	}\
	template <class = enable_if_t<is_inclusion_v<T, Base>>>\
	complex_base& operator=(const T& n) {\
		this->x[0] = n; this->x[1] = multiplication_traits<Base>::absorbing_element();\
		return *this;\
	}\
	template <class = enable_if_t<is_operation<Base, T, Base>::add_value>>\
	complex_base& operator+=(const complex<T>& n) {\
		this->x[0] += n.x[0]; this->x[1] += n.x[1];\
		return *this;\
	}\
	template <class = enable_if_t<is_operation<Base, T, Base>::add_value>>\
	complex_base& operator+=(const T& n) {\
		this->x[0] += n;\
		return *this;\
	}\
	template <class = enable_if_t<is_operation<Base, T, Base>::sub_value>>\
	complex_base& operator-=(const complex<T>& n) {\
		this->x[0] -= n.x[0]; this->x[1] -= n.x[1];\
		return *this;\
	}\
	template <class = enable_if_t<is_operation<Base, T, Base>::sub_value>>\
	complex_base& operator-=(const T& n) {\
		this->x[0] -= n;\
		return *this;\
	}\
	template <class = enable_if_t<is_operation<Base, T, Base>::mul_value>>\
	complex_base& operator*=(const complex<T>& n) {\
		Base temp[2] = { this->x[0] * n.x[0] - this->x[1] * n.x[1], this->x[0] * n.x[1] + this->x[1] * n.x[0] };\
		this->x[0] = temp[0]; this->x[1] = temp[1];\
		return *this;\
	}\
	template <class = enable_if_t<is_operation<Base, T, Base>::mul_value>>\
	complex_base& operator*=(const T& k) {\
		this->x[0] *= k; this->x[1] *= k;\
		return *this;\
	}\
	template <class = enable_if_t<is_operation<Base, T, Base>::div_value>>\
	complex_base& operator/=(const complex<T>& n) {\
		Base temp[3] = { this->x[0] * n.x[0] + this->x[1] * n.x[1], this->x[1] * n.x[0] - this->x[0] * n.x[1]\
			, n.x[0] * n.x[0] + n.x[1] * n.x[1] };\
		this->x[0] = temp[0] / temp[2]; this->x[1] = temp[1] / temp[2];\
		return *this;\
	}\
	template <class = enable_if_t<is_operation<Base, T, Base>::div_value>>\
	complex_base& operator/=(const T& k) {\
		this->x[0] /= k; this->x[1] /= k;\
		return *this;\
	}
	//Base == T の場合の複素数型の2項演算の定義
#define IMATH_COMPLEX_BINARY_OPERATION1 \
	template <class = enable_if_t<is_calcable<Base, T>::add_value>>\
	friend constexpr auto operator+(const complex<Base>& lhs, const complex<T>& rhs) {\
		return complex<add_result_t<Base, T>>(lhs[0] + rhs[0], lhs[1] + rhs[1]);\
	}\
	template <class = enable_if_t<is_operation<Base, T, Base>::add_value>>\
	friend constexpr complex<Base> operator+(const complex<Base>& lhs, const T& rhs) {\
		return complex<Base>(lhs[0] + rhs, lhs[1]);\
	}\
	template <class = enable_if_t<is_operation<T, Base, Base>::add_value>>\
	friend constexpr complex<Base> operator+(const T& lhs, const complex<Base>& rhs) {\
		return complex<Base>(lhs + rhs[0], rhs[1]);\
	}\
	template <class = enable_if_t<is_calcable<Base, T>::sub_value>>\
	friend constexpr auto operator-(const complex<Base>& lhs, const complex<T>& rhs) {\
		return complex<sub_result_t<Base, T>>(lhs[0] - rhs[0], lhs[1] - rhs[1]);\
	}\
	template <class = enable_if_t<is_operation<Base, T, Base>::sub_value>>\
	friend constexpr complex<Base> operator-(const complex<Base>& lhs, const T& rhs) {\
		return complex<Base>(lhs[0] - rhs, lhs[1]);\
	}\
	template <class = enable_if_t<is_operation<T, Base, Base>::add_value>>\
	friend constexpr complex<Base> operator-(const T& lhs, const complex<Base>& rhs) {\
		return complex<Base>(lhs - rhs[0], -rhs[1]);\
	}\
	template <class = enable_if_t<is_loop<mul_result_t<Base, T>>::add_value>>\
	friend constexpr auto operator*(const complex<Base>& lhs, const complex<T>& rhs) {\
		return complex<mul_result_t<Base, T>>(lhs[0] * rhs[0] - lhs[1] * rhs[1], lhs[0] * rhs[1] + lhs[1] * rhs[0]);\
	}\
	template <class = enable_if_t<is_calcable<Base, T>::mul_value>>\
	friend constexpr auto operator*(const complex<Base>& lhs, const T& rhs) {\
		return complex<mul_result_t<Base, T>>(lhs[0] * rhs, lhs[1] * rhs);\
	}\
	template <class = enable_if_t<is_calcable<T, Base>::mul_value>>\
	friend constexpr auto operator*(const T& lhs, const complex<Base>& rhs) {\
		return complex<mul_result_t<T, Base>>(lhs * rhs[0], lhs * rhs[1]);\
	}\
	template <class = enable_if_t<is_loop<mul_result_t<Base, T>>::add_value && is_loop<mul_result_t<Base, T>>::mul_value>>\
	friend constexpr auto operator/(const complex<Base>& lhs, const complex<T>& rhs) {\
		auto temp = rhs[0] * rhs[0] + rhs[1] * rhs[1];\
		return complex<mul_result_t<Base, T>>((lhs[0] * rhs[0] + lhs[1] * rhs[1]) / temp\
			, (lhs[1] * rhs[0] - lhs[0] * rhs[1]) / temp);\
	}\
	template <class = enable_if_t<is_calcable<Base, T>::div_value>>\
	friend constexpr auto operator/(const complex<Base>& lhs, const T& rhs) {\
		return complex<div_result_t<Base, T>>(lhs[0] / rhs, lhs[1] / rhs);\
	}\
	template <class = enable_if_t<is_loop<mul_result_t<Base, T>>::add_value && is_loop<mul_result_t<Base, T>>::mul_value>>\
	friend constexpr auto operator/(const T& lhs, const complex<Base>& rhs) {\
		auto temp = rhs[0] * rhs[0] + rhs[1] * rhs[1];\
		return complex<mul_result_t<Base, T>>(lhs * rhs[0] / temp, -lhs * rhs[1] / temp);\
	}\
	/*比較演算*/\
	template <class = enable_if_t<is_calcable<Base, T>::eq_value>>\
	friend constexpr bool operator==(const complex<Base>& lhs, const complex<T>& rhs) {\
		return (lhs[0] == rhs[0]) && (lhs[1] == rhs[1]);\
	}\
	template <class = enable_if_t<is_calcable<Base, T>::eq_value>>\
	friend constexpr bool operator==(const complex<Base>& lhs, T& rhs) {\
		return (lhs[0] == rhs) && is_absorbing_element(lhs[1]);\
	}\
	template <class = enable_if_t<is_calcable<T, Base>::eq_value>>\
	friend constexpr bool operator==(const T& lhs, const complex<Base>& rhs) {\
		return (lhs == rhs[0]) && is_absorbing_element(rhs[1]);\
	}\
	template <class = enable_if_t<is_calcable<Base, T>::eq_value>>\
	friend constexpr bool operator!=(const complex<Base>& lhs, const complex<T>& rhs) { return !(lhs == rhs); }\
	template <class = enable_if_t<is_calcable<Base, T>::eq_value>>\
	friend constexpr bool operator!=(const complex<Base>& lhs, T& rhs) { return !(lhs == rhs); }\
	template <class = enable_if_t<is_calcable<T, Base>::eq_value>>\
	friend constexpr bool operator!=(const T& lhs, const complex<Base>& rhs) { return !(lhs == rhs); }
	//Base != T の場合の複素数型の2項演算の定義
#define IMATH_COMPLEX_BINARY_OPERATION2 \
	IMATH_COMPLEX_BINARY_OPERATION1;\
	template <class = enable_if_t<is_calcable<T, Base>::add_value>>\
	friend constexpr auto operator+(const complex<T>& lhs, const complex<Base>& rhs) {\
		return complex<add_result_t<T, Base>>(lhs[0] + rhs[0], lhs[1] + rhs[1]);\
	}\
	template <class = enable_if_t<is_calcable<T, Base>::sub_value>>\
	friend constexpr auto operator-(const complex<T>& lhs, const complex<Base>& rhs) {\
		return complex<sub_result_t<T, Base>>(lhs[0] - rhs[0], lhs[1] - rhs[1]);\
	}\
	template <class = enable_if_t<is_loop<mul_result_t<T, Base>>::add_value>>\
	friend constexpr auto operator*(const complex<T>& lhs, const complex<Base>& rhs) {\
		return complex<mul_result_t<T, Base>>(lhs[0] * rhs[0] - lhs[1] * rhs[1], lhs[0] * rhs[1] + lhs[1] * rhs[0]);\
	}\
	template <class = enable_if_t<is_loop<mul_result_t<T, Base>>::add_value && is_loop<mul_result_t<T, Base>>::mul_value>>\
	friend constexpr auto operator/(const complex<T>& lhs, const complex<Base>& rhs) {\
		auto temp = rhs[0] * rhs[0] + rhs[1] * rhs[1];\
		return complex<mul_result_t<T, Base>>((lhs[0] * rhs[0] + lhs[1] * rhs[1]) / temp\
			, (lhs[1] * rhs[0] - lhs[0] * rhs[1]) / temp);\
	}\
	/*比較演算*/\
	template <class = enable_if_t<is_calcable<T, Base>::eq_value>>\
	friend constexpr bool operator==(const complex<T>& lhs, const complex<Base>& rhs) {\
		return (lhs[0] == rhs[0]) && (lhs[1] == rhs[1]);\
	}\
	template <class = enable_if_t<is_calcable<T, Base>::eq_value>>\
	friend constexpr bool operator!=(const complex<T>& lhs, const complex<Base>& rhs) { return !(lhs == rhs); }
	//Base == T の場合の複素数型のパラメータの2項演算の定義
#define IMATH_COMPLEX_PARAMETER_BINARY_OPERATION1 \
	template <class Re1, class Im1, class Re2, class Im2, class = enable_if_t<is_calcable<Base, T>::add_value>>\
	friend auto operator+(complex_parameter<Base, Re1, Im1>, complex_parameter<T, Re2, Im2>) {\
		return complex_parameter<add_result_t<Base, T>, decltype(Re1() + Re2()), decltype(Im1() + Im2())>();\
	}\
	template <class Re, class Im, class Param, class = enable_if_t<is_operation<Base, T, Base>::add_value>>\
	friend auto operator+(complex_parameter<Base, Re, Im>, type_parameter<T, Param> rhs) {\
		return complex_parameter<Base, decltype(Re() + rhs), Im>();\
	}\
	template <class Param, class Re, class Im, class = enable_if_t<is_operation<T, Base, Base>::add_value>>\
	friend auto operator+(type_parameter<T, Param> lhs, complex_parameter<Base, Re, Im>) {\
		return complex_parameter<Base, decltype(rhs + Re()), Im>();\
	}\
	template <class Re1, class Im1, class Re2, class Im2, class = enable_if_t<is_calcable<Base, T>::sub_value>>\
	friend auto operator-(complex_parameter<Base, Re1, Im1>, complex_parameter<T, Re2, Im2>) {\
		return complex_parameter<sub_result_t<Base, T>, decltype(Re1() - Re2()), decltype(Im1() - Im2())>();\
	}\
	template <class Re, class Im, class Param, class = enable_if_t<is_operation<Base, T, Base>::sub_value>>\
	friend auto operator-(complex_parameter<Base, Re, Im>, type_parameter<T, Param> rhs) {\
		return complex_parameter<Base, decltype(Re() - rhs), Im>();\
	}\
	template <class Param, class Re, class Im, class = enable_if_t<is_operation<T, Base, Base>::add_value>>\
	friend auto operator-(type_parameter<T, Param> lhs, complex_parameter<Base, Re, Im>) {\
		return complex_parameter<Base, decltype(rhs - Re()), decltype(-Im())>();\
	}\
	template <class Re1, class Im1, class Re2, class Im2, class = enable_if_t<is_loop<mul_result_t<Base, T>>::add_value>>\
	friend auto operator*(complex_parameter<Base, Re1, Im1>, complex_parameter<T, Re2, Im2>) {\
		return complex_parameter<mul_result_t<Base, T>, decltype(Re1() * Re2() - Im1() * Im2()), decltype(Re1() * Im2() + Im1() * Re2())>();\
	}\
	template <class Re, class Im, class Param, class = enable_if_t<is_calcable<Base, T>::mul_value>>\
	friend auto operator*(complex_parameter<Base, Re, Im>, type_parameter<T, Param> rhs) {\
		return complex_parameter<mul_result_t<Base, T>, decltype(Re() * rhs), decltype(Im() * rhs)>();\
	}\
	template <class Param, class Re, class Im, class = enable_if_t<is_calcable<T, Base>::mul_value>>\
	friend auto operator*(type_parameter<T, Param> lhs, complex_parameter<Base, Re, Im>) {\
		return complex_parameter<mul_result_t<T, Base>, decltype(lhs * Re()), decltype(lhs * Im())>();\
	}\
	template <class Re1, class Im1, class Re2, class Im2, class = enable_if_t<is_loop<mul_result_t<Base, T>>::add_value && is_loop<mul_result_t<Base, T>>::mul_value>>\
	friend auto operator/(complex_parameter<Base, Re1, Im1>, complex_parameter<T, Re2, Im2>) {\
		using type = decltype(Re2() * Re2() + Im2() * Im2());\
		return complex_parameter<mul_result_t<Base, T>, decltype((Re1() * Re2() + Im1() * Im2()) / type())\
			, decltype((Im1() * Re2() - Re1() * Im2()) / type())>();\
	}\
	template <class Re, class Im, class Param, class = enable_if_t<is_calcable<Base, T>::div_value>>\
	friend auto operator/(complex_parameter<Base, Re, Im>, type_parameter<T, Param> rhs) {\
		return complex_parameter<div_result_t<Base, T>, decltype(Re() / rhs), decltype(Im() / rhs)>();\
	}\
	template <class Param, class Re, class Im, class = enable_if_t<is_loop<mul_result_t<Base, T>>::add_value && is_loop<mul_result_t<Base, T>>::mul_value>>\
	friend auto operator/(type_parameter<T, Param> lhs, complex_parameter<Base, Re, Im>) {\
		using type = decltype(Re() * Re() + Im() * Im());\
		return complex<mul_result_t<Base, T>, decltype(lhs * Re() / type()), decltype(-lhs * Im() / type())>();\
	}\
	/*比較演算*/\
	template <class Re1, class Im1, class Re2, class Im2, class = enable_if_t<is_calcable<Base, T>::eq_value>>\
	friend auto operator==(complex_parameter<Base, Re1, Im1>, complex_parameter<T, Re2, Im2>) {\
		return (Re1() == Re2()) && (Im1() == Im2());\
	}\
	template <class Re, class Im, class Param, class = enable_if_t<is_calcable<Base, T>::eq_value>>\
	friend auto operator==(complex_parameter<Base, Re, Im>, type_parameter<T, Param> rhs) {\
		return (Re() == rhs) && int_parameter<bool, is_absorbing_element(Im::value)>();\
	}\
	template <class Param, class Re, class Im, class = enable_if_t<is_calcable<T, Base>::eq_value>>\
	friend auto operator==(type_parameter<T, Param> lhs, complex_parameter<Base, Re, Im>) {\
		return (lhs == Re()) && int_parameter<bool, is_absorbing_element(Im::value)>();\
	}\
	template <class Re1, class Im1, class Re2, class Im2, class = enable_if_t<is_calcable<Base, T>::eq_value>>\
	friend constexpr auto operator!=(complex_parameter<Base, Re1, Im1> lhs, complex_parameter<T, Re2, Im2> rhs) { return !(lhs == rhs); }\
	template <class Re, class Im, class Param, class = enable_if_t<is_calcable<Base, T>::eq_value>>\
	friend constexpr auto operator!=(complex_parameter<Base, Re, Im> lhs, type_parameter<T, Param> rhs) { return !(lhs == rhs); }\
	template <class Param, class Re, class Im, class = enable_if_t<is_calcable<T, Base>::eq_value>>\
	friend constexpr auto operator!=(type_parameter<T, Param> lhs, complex_parameter<Base, Re, Im> rhs) { return !(lhs == rhs); }
	//Base != T の場合の順序を考慮した複素数型のパラメータの2項演算の定義
#define IMATH_COMPLEX_PARAMETER_BINARY_OPERATION2 \
	IMATH_COMPLEX_PARAMETER_BINARY_OPERATION1;\
	template <class Re1, class Im1, class Re2, class Im2, class = enable_if_t<is_calcable<T, Base>::add_value>>\
	friend auto operator+(complex_parameter<T, Re1, Im1>, complex_parameter<Base, Re2, Im2>) {\
		return complex_parameter<add_result_t<T, Base>, decltype(Re1() + Re2()), decltype(Im1() + Im2())>();\
	}\
	template <class Re1, class Im1, class Re2, class Im2, class = enable_if_t<is_calcable<T, Base>::sub_value>>\
	friend auto operator-(complex_parameter<T, Re1, Im1>, complex_parameter<Base, Re2, Im2>) {\
		return complex_parameter<sub_result_t<T, Base>, decltype(Re1() - Re2()), decltype(Im1() - Im2())>();\
	}\
	template <class Re1, class Im1, class Re2, class Im2, class = enable_if_t<is_loop<mul_result_t<T, Base>>::add_value>>\
	friend auto operator*(complex_parameter<T, Re1, Im1>, complex_parameter<Base, Re2, Im2>) {\
		return complex_parameter<mul_result_t<T, Base>, decltype(Re1() * Re2() - Im1() * Im2()), decltype(Re1() * Im2() + Im1() * Re2())>();\
	}\
	template <class Re1, class Im1, class Re2, class Im2, class = enable_if_t<is_loop<mul_result_t<T, Base>>::add_value && is_loop<mul_result_t<Base, T>>::mul_value>>\
	friend auto operator/(complex_parameter<T, Re1, Im1>, complex_parameter<Base, Re2, Im2>) {\
		using type = decltype(Re2() * Re2() + Im2() * Im2());\
		return complex_parameter<mul_result_t<T, Base>, decltype((Re1() * Re2() + Im1() * Im2()) / type())\
			, decltype((Im1() * Re2() - Re1() * Im2()) / type())>();\
	}\
	/*比較演算*/\
	template <class Re1, class Im1, class Re2, class Im2, class = enable_if_t<is_calcable<T, Base>::eq_value>>\
	friend auto operator==(complex_parameter<T, Re1, Im1>, complex_parameter<Base, Re2, Im2>) {\
		return (Re1() == Re2()) && (Im1() == Im2());\
	}\
	template <class Re1, class Im1, class Re2, class Im2, class = enable_if_t<is_calcable<T, Base>::eq_value>>\
	friend auto operator==(complex_parameter<T, Re1, Im1> lhs, complex_parameter<Base, Re2, Im2> rhs) { return !(lhs == rhs); }


	template <class>
	class complex;


	//複素数型のパラメータ
	template <class T, class Re, class Im>
	using complex_parameter = type_parameter<complex<T>, type_tuple<Re, Im>>;
	template <class T, class ReP, class ImP>
	struct type_parameter<complex<T>, type_tuple<type_parameter<T, ReP>, type_parameter<T, ImP>>> {
		using type = complex<T>;
		static constexpr type value = type(type_parameter<T, ReP>::value, type_parameter<T, ImP>::value);

		template <class = enable_if_t<is_exist_additive_inverse_v<T>>>
		auto operator-() const { return complex_parameter<T, decltype(-type_parameter<T, ReP>()), decltype(-type_parameter<T, ImP>())>(); }
		auto operator+() const { return *this; }

		template <class T, T Val, class = enable_if_t<(Val >= 0) && (Val < 2)>>
		auto operator[](int_parameter<T, Val>) const { return at_type_tuple_t<Val, type_tuple<type_parameter<T, ReP>, type_parameter<T, ImP>>>(); }
	};


	template <class Base, class T, bool = is_algebraic_structure_v<T>, bool = is_same_v<Base, T>>
	class complex_base;


	//下に階層が存在しないかつBase == T
	template <class Base, class T>
	class complex_base<Base, T, false, true> {
		template <class> friend class complex;
		template <class, class, bool, bool> friend class complex_base;
	protected:
		Base x[2];
	public:
		constexpr complex_base() : x{} {}
		constexpr complex_base(const Base& re, const Base& im) : x{ re,im } {}
		constexpr complex_base(const Base& re) : x{ re } {}
		constexpr complex_base(const complex<Base>& c) : x{ c.x[0],c.x[1] } {}

		//単項演算
		template <class = enable_if_t<is_exist_additive_inverse_v<Base>>>
		complex<Base> operator-() const { return complex<Base>(-this->x[0], -this->x[1]); }
		complex<Base> operator+() const { return complex<Base>(*this); }

		//代入演算
		IMATH_COMPLEX_COMPOUND_ASSIGNMENT_OPERATION;

		//アクセサ
		const constexpr Base& operator[](size_t index) const { return this->x[index]; }
		constexpr Base& operator[](size_t index) { return this->x[index]; }

		//2項演算
		IMATH_COMPLEX_BINARY_OPERATION1;
		IMATH_COMPLEX_PARAMETER_BINARY_OPERATION1;
	};
	//下に階層が存在しないかつBase != T
	template <class Base, class T>
	class complex_base<Base, T, false, false> {
		template <class> friend class complex;
		template <class, class, bool, bool> friend class complex_base;
	protected:
		Base x[2];
	public:
		constexpr complex_base() : x{} {}
		constexpr complex_base(const Base& re, const Base& im) : x{ re,im } {}
		constexpr complex_base(const Base& re) : x{ re } {}
		constexpr complex_base(const complex<Base>& c) : x{ c.x[0],c.x[1] } {}
		template <class = enable_if_t<is_inclusion_v<T, Base>>>
		constexpr complex_base(const T& re, const T& im) : x{ static_cast<Base>(re),static_cast<Base>(im) } {}
		template <class = enable_if_t<is_inclusion_v<T, Base>>>
		constexpr complex_base(const T& re) : x{ static_cast<Base>(re) } {}
		template <class = enable_if_t<is_inclusion_v<T, Base>>>
		constexpr complex_base(const complex<T>& c) : x{ static_cast<Base>(c.x[0]),static_cast<Base>(c.x[1]) } {}

		//単項演算
		template <class = enable_if_t<is_exist_additive_inverse_v<Base>>>
		constexpr complex<Base> operator-() const { return complex<Base>(-this->x[0], -this->x[1]); }
		constexpr complex<Base> operator+() const { return complex<Base>(*this); }

		//代入演算
		IMATH_COMPLEX_COMPOUND_ASSIGNMENT_OPERATION;

		//アクセサ
		const constexpr Base& operator[](size_t index) const { return this->x[index]; }
		constexpr Base& operator[](size_t index) { return this->x[index]; }

		//2項演算
		IMATH_COMPLEX_BINARY_OPERATION2;
		IMATH_COMPLEX_PARAMETER_BINARY_OPERATION2;
	};
	//下に階層が存在するかつBase == T
	template <class Base, class T>
	class complex_base<Base, T, true, true> : public complex_base<Base, typename T::algebraic_type> {
		template <class> friend class complex;
		template <class, class, bool, bool> friend class complex_base;
	public:
		//コンストラクタ
		using complex_base<Base, typename T::algebraic_type>::complex_base;

		//単項演算
		using complex_base<Base, typename T::algebraic_type>::operator+;
		using complex_base<Base, typename T::algebraic_type>::operator-;

		//代入演算
		using complex_base<Base, typename T::algebraic_type>::operator+=;
		using complex_base<Base, typename T::algebraic_type>::operator-=;
		using complex_base<Base, typename T::algebraic_type>::operator*=;
		using complex_base<Base, typename T::algebraic_type>::operator/=;
		IMATH_COMPLEX_COMPOUND_ASSIGNMENT_OPERATION;

		//添え字演算
		using complex_base<Base, typename T::algebraic_type>::operator[];

		//2項演算
		IMATH_COMPLEX_BINARY_OPERATION1;
		IMATH_COMPLEX_PARAMETER_BINARY_OPERATION1;
	};
	//下に階層が存在するかつBase != T
	template <class Base, class T>
	class complex_base<Base, T, true, false> : public complex_base<Base, typename T::algebraic_type> {
		template <class> friend class complex;
		template <class, class, bool, bool> friend class complex_base;
	public:
		//コンストラクタ
		using complex_base<Base, typename T::algebraic_type>::complex_base;
		constexpr complex_base() : complex_base<Base, typename T::algebraic_type>() {}
		template <class = enable_if_t<is_inclusion_v<T, Base>>>
		constexpr complex_base(const T& re, const T& im) : x{ static_cast<Base>(re),static_cast<Base>(im) } {}
		template <class = enable_if_t<is_inclusion_v<T, Base>>>
		constexpr complex_base(const T& re) : x{ static_cast<Base>(re) } {}

		//単項演算
		using complex_base<Base, typename T::algebraic_type>::operator+;
		using complex_base<Base, typename T::algebraic_type>::operator-;

		//代入演算
		using complex_base<Base, typename T::algebraic_type>::operator+=;
		using complex_base<Base, typename T::algebraic_type>::operator-=;
		using complex_base<Base, typename T::algebraic_type>::operator*=;
		using complex_base<Base, typename T::algebraic_type>::operator/=;
		IMATH_COMPLEX_COMPOUND_ASSIGNMENT_OPERATION;

		//添え字演算
		using complex_base<Base, typename T::algebraic_type>::operator[];

		//2項演算
		IMATH_COMPLEX_BINARY_OPERATION2;
		IMATH_COMPLEX_PARAMETER_BINARY_OPERATION2;
	};

	//複素数型
	template <class T>
	class complex : public complex_base<T, T>, public value_list_input<array_iterator<T>> {
		template <class> friend class complex;
		template <class, class, bool, bool> friend class complex_base;
	public:
		using complex_base<T, T>::complex_base;

		using algebraic_type = T;
		using iterator = array_iterator<T>;
		using const_iterator = array_iterator<const T>;

		template<class Other>
		struct rebind {
			using other = complex<Other>;
		};

		constexpr iterator begin() noexcept { return iterator(&x[0]); }
		constexpr const_iterator begin() const noexcept { return const_iterator(&x[0]); }
		constexpr iterator end() noexcept { return iterator(&x[1] + 1); }
		constexpr const_iterator end() const noexcept { return const_iterator(&x[1] + 1); }

		//単項演算
		using complex_base<T, T>::operator-;
		using complex_base<T, T>::operator+;
		//代入演算
		using complex_base<T, T>::operator+=;
		using complex_base<T, T>::operator-=;
		using complex_base<T, T>::operator*=;
		using complex_base<T, T>::operator/=;
		//2項演算の継承
		using value_list_input<array_iterator<T>>::operator<<;

		//添え字演算
		using complex_base<T, T>::operator[];

		//メンバ取得
		template <class S, class = typename enable_if<is_inclusion_v<S, T>>::type>
		void real(const S& re) { this->x[0] = re; }
		template <class S, class = typename enable_if<is_inclusion_v<S, T>>::type>
		void imag(const S& im) { this->x[1] = im; }
		constexpr T real() const { return this->x[0]; }
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
#undef IMATH_MATRIX_ASSIGNMENT_OPERATION
#undef IMATH_MATRIX_BINARY_OPERATION1
#undef IMATH_MATRIX_BINARY_OPERATION2
#undef IMATH_MATRIX_PARAMETER_BINARY_OPERATION1
#undef IMATH_MATRIX_PARAMETER_BINARY_OPERATION2


	//複素数の判定
	template <class T>
	struct is_complex_impl : false_type {};
	template <class T>
	struct is_complex_impl<complex<T>> : true_type {};
	template <class T>
	struct is_complex : is_complex_impl<decay_t<T>> {};
	template <class T>
	constexpr bool is_complex_v = is_complex<T>::value;


	//複素数の除去
	template <class T>
	struct remove_complex {
		using type = T;
	};
	template <class T>
	struct remove_complex<complex<T>> {
		using type = T;
	};
	template <class T>
	using remove_complex_t = typename remove_complex<complex<T>>::type;

}

namespace iml {

	//加法の特性
	template <class T>
	struct addition_traits<complex<T>> {
		//単位元
		template <class = enable_if_t<is_exist_additive_identity_v<T>>>
		static constexpr complex<T> identity_element() { return T(); }
		//結合律
		static constexpr bool associative_value = addition_traits<T>::associative_value;
		//消約律
		static constexpr bool cancellative_value = addition_traits<T>::cancellative_value;
		//可換律
		static constexpr bool commutative_value = addition_traits<T>::commutative_value;
	};
	//乗法の特性
	template <class T>
	struct multiplication_traits<complex<T>> {
		//単位元
		template <class = enable_if_t<is_exist_multiplicative_identity_v<T>>>
		static constexpr auto identity_element() { return addition_traits<T>::identity_element(); }
		//吸収元
		template <class = enable_if_t<is_exist_absorbing_element_v<T>>>
		static constexpr T absorbing_element() { return T(); }
		//結合律
		static constexpr bool associative_value = multiplication_traits<T>::associative_value;
		//消約律
		static constexpr bool cancellative_value = multiplication_traits<T>::cancellative_value;
		//可換律
		static constexpr bool commutative_value = multiplication_traits<T>::commutative_value;
		//分配律
		static constexpr bool distributive_value = multiplication_traits<T>::distributive_value;
	};
}

#endif