#ifndef IMATH_MATH_MATH_CONJ_HPP
#define IMATH_MATH_MATH_CONJ_HPP

namespace iml {

	//共役
	template <class T>
	struct Conj {
		static constexpr T _conj_(const T& x) { return x; }
	};
	template <class T>
	inline constexpr auto conj(const T& x) { return Conj<T>::_conj_(x); }
}


#endif