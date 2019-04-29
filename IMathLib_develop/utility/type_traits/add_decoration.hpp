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
	template<class T>
	using add_const_t = typename add_const<T>::type;


	//volatileを加える
	template<class T>
	struct add_volatile {
		using type = volatile T;
	};
	template<class T>
	using add_volatile_t = typename add_volatile<T>::type;


	//cvを加える
	template<class T>
	struct add_cv {
		using type = const volatile T;
	};
	template<class T>
	using add_cv_t = typename add_cv<T>::type;


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
	template<class T>
	using add_lvalue_reference_t = typename add_lvalue_reference<T>::type;
	//右辺値参照の追加
	template<class T>
	struct add_rvalue_reference {
		using type = typename add_reference_impl<T>::r_value;
	};
	template<class T>
	using add_rvalue_reference_t = typename add_rvalue_reference<T>::type;


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
	template<class T>
	using add_pointer_t = typename add_pointer<T>::type;

}

#endif