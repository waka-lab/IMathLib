#ifndef IMATH_MATH_MATH_ARG_HPP
#define IMATH_MATH_MATH_ARG_HPP


namespace iml {

	//極形式のための偏角
	template<class T>
	struct Arg {
		static constexpr T _arg_(T x) { return addition_traits<T>::identity_element(); }
	};

	template <class T>
	inline constexpr auto arg(const T& x) { return Arg<T>::_arg_(x); }
}


#endif