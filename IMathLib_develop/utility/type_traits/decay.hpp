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
		using T1 = remove_reference_t<T>;

		using type = conditional_t<is_array_v<T1>, remove_extent_t<T1> *,
			conditional_t<is_function_v<T1>, add_pointer_t<T1>, remove_cv_t<T1>>>;
	};
	template <class T>
	using decay_t = typename decay<T>::type;
}

#endif