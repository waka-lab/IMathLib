#ifndef IMATH_UTILITY_TYPE_TRAITS_CHANGE_SIGN_HPP
#define IMATH_UTILITY_TYPE_TRAITS_CHANGE_SIGN_HPP

#include "IMathLib/utility/type_traits/remove_decoration.hpp"

namespace iml {

	// 符号の付け替え
	template <class>
	struct change_sign;
#define IMATH_CHANGE_SIGN(TYPE, SIGNED_TYPE, UNSIGNED_TYPE)\
	template <>\
	struct change_sign<TYPE> {\
		using signed_type = SIGNED_TYPE;\
		using unsigned_type = UNSIGNED_TYPE;\
	};
	IMATH_CHANGE_SIGN(char, char, unsigned char);
	IMATH_CHANGE_SIGN(unsigned char, char, unsigned char);
	IMATH_CHANGE_SIGN(short, short, unsigned short);
	IMATH_CHANGE_SIGN(unsigned short, short, unsigned short);
	IMATH_CHANGE_SIGN(int, int, unsigned int);
	IMATH_CHANGE_SIGN(unsigned int, int, unsigned int);
	IMATH_CHANGE_SIGN(long, long, unsigned long);
	IMATH_CHANGE_SIGN(unsigned long, long, unsigned long);
	IMATH_CHANGE_SIGN(long long, long long, unsigned long long);
	IMATH_CHANGE_SIGN(unsigned long long, long long, unsigned long long);
#undef IMATH_CHANGE_SIGN


	template <class T>
	struct make_signed {
		using type = typename change_sign<remove_cv_t<T>>::signed_type;
	};
	template <class T>
	using make_signed_t = typename make_signed<T>::type;


	template <class T>
	struct make_unsigned {
		using type = typename change_sign<remove_cv_t<T>>::unsigned_type;
	};
	template <class T>
	using make_unsigned_t = typename make_unsigned<T>::type;

}

#endif