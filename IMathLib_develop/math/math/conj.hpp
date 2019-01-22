#ifndef _IMATH_MATH_CONJ_HPP
#define _IMATH_MATH_CONJ_HPP

namespace iml {

	//共役
	template <class T>
	struct _Conj {
		static constexpr T __conj(const T& x) { return x; }
	};
	template <class T>
	inline constexpr auto conj(const T& x) { return _Conj<T>::__conj(x); }
}


#endif