#ifndef IMATH_UTILITY_TYPE_TRAITS_ADD_DECORATION_HPP
#define IMATH_UTILITY_TYPE_TRAITS_ADD_DECORATION_HPP

#include "IMathLib/utility/type_traits/is_type.hpp"


//修飾の付与

namespace iml {

	//constの追加
	template<class T>
	struct add_const {
		using type = const T;
	};


	//volatileを加える
	template<class T>
	struct add_volatile {
		using type = volatile T;
	};


	//cvを加える
	template<class T>
	struct add_cv {
		using type = const T;
	};


	//参照の追加
	template<class T, bool = is_function_impl<T>::wrap || is_void<T>::value>
	struct add_reference_impl {
		using l_value = T;
		using r_value = T;
	};
	template<class T>
	struct add_reference_impl<T, false> {
		using l_value = T & ;
		using r_value = T && ;
	};

	//左辺値参照の追加
	template<class T>
	struct add_lvalue_reference {
		using type = typename add_reference_impl<T>::l_value;
	};
	//右辺値参照の追加
	template<class T>
	struct add_rvalue_reference {
		using type = typename add_reference_impl<T>::r_value;
	};


	//ポインターの追加
	template<class T, bool = is_function_impl<T>::wrap>
	struct add_pointer_impl {
		using type = T;
	};
	template<class T>
	struct add_pointer_impl<T, false> {
		using type = typename remove_reference<T>::type*;
	};
	template<class T>
	struct add_pointer {
		using type = typename add_pointer_impl<T>::type;
	};

}

#endif