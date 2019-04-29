#ifndef IMATH_UTILITY_TYPE_TRAITS_DECLVAL_HPP
#define IMATH_UTILITY_TYPE_TRAITS_DECLVAL_HPP

#include "IMathLib/utility/type_traits/add_decoration.hpp"

namespace iml {

	//型情報だけをもつインスタンスの取得
	template<class T>
	typename add_rvalue_reference<T>::type declval() noexcept;
}

#endif