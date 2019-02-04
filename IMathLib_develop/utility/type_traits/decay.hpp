#ifndef IMATH_UTILITY_TYPE_TRAITS_DECAY_HPP
#define IMATH_UTILITY_TYPE_TRAITS_DECAY_HPP

#include "IMathLib/utility/type_traits/is_type.hpp"
#include "IMathLib/utility/type_traits/remove_decoration.hpp"
#include "IMathLib/utility/type_traits/add_decoration.hpp"
#include "IMathLib/utility/type_traits/type_comparison.hpp"


namespace iml {

	//配列と関数ポインタに関して関数テンプレートと同様に推論された型を取得(配列ならポインタに、関数型なら関数ポインタに)
	template<class T>
	struct decay {
		using T1 = typename remove_reference<T>::type;

		using type = typename conditional<is_array<T1>::value, typename remove_extent<T1>::type *,
			typename conditional<is_function<T1>::value, typename add_pointer<T1>::type, typename remove_cv<T1>::type>::type>::type;
	};
}

#endif