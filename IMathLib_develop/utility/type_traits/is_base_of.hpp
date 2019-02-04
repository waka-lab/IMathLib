#ifndef IMATH_UTILITY_TYPE_TRAITS_IS_BASE_OF_HPP
#define IMATH_UTILITY_TYPE_TRAITS_IS_BASE_OF_HPP

#include "IMathLib/utility/type_traits/integral_constant.hpp"
#include "IMathLib/utility/type_traits/is_type.hpp"
#include "IMathLib/utility/type_traits/remove_decoration.hpp"
#include "IMathLib/utility/type_traits/is_convertible.hpp"


namespace iml {

	//BaseがDerivedの基底クラスであるかの判定
	template <class Base, class Derived>
	struct is_base_of_impl {
		using ncvB = typename remove_cv<Base>::type;
		using ncvD = typename remove_cv<Derived>::type;
		//クラスであり同じ型かポインタが暗黙的に変換可能か
		static constexpr bool value =
			(is_same<ncvB, ncvD>::value && is_class<ncvB>::value) ||
			(is_class<ncvB>::value && is_class<ncvD>::value && is_convertible<ncvD*, ncvB*>::value);
	};
	template <class Base, class Derived>
	struct is_base_of : cat_bool<is_base_of_impl<Base, Derived>::value> {};
	template <class Base, class Derived>
	struct is_base_of<Base, Derived&> : false_type {};
	template <class Base, class Derived>
	struct is_base_of<Base&, Derived&> : false_type {};
	template <class Base, class Derived>
	struct is_base_of<Base&, Derived> : false_type {};

}

#endif