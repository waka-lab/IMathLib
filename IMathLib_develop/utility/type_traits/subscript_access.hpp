#ifndef IMATH_UTILITY_TYPE_TRAITS_IS_SUBSCRIPT_ACCESS_HPP
#define IMATH_UTILITY_TYPE_TRAITS_IS_SUBSCRIPT_ACCESS_HPP

#include "IMathLib/utility/type_traits/type_comparison.hpp"
#include "IMathLib/utility/type_traits/remove_decoration.hpp"
#include "IMathLib/utility/type_traits/declval.hpp"


namespace iml {

	//T1がT2で添え字アクセス可能か
	template <class T1, class T2>
	struct is_subscript_access_impl {
	private:
		template <class S1, class S2> static auto tester(S1, S2) -> decltype(declval<S1>()[declval<S2>()], true_type());
		template <class S1, class S2> static false_type tester(...);
	public:
		using type = decltype(tester<T1, T2>(declval<T1>(), declval<T2>()));
	};
	template <class T1, class T2>
	struct is_subscript_access : is_subscript_access_impl<T1, T2>::type {};
	template <class T1, class T2>
	constexpr bool is_subscript_access_v = is_subscript_access<T1, T2>::value;


	//T2でT1を添え字アクセスしたときの型(添え字アクセス不可のときはvoid)
	template <class T1, class T2>
	struct subscript_access_result {
		using type = conditional_t<is_subscript_access_v<T1, T2>, remove_reference_t<decltype(declval<T1>()[declval<T2>()])>, void>;
	};
	template <class T1, class T2>
	using subscript_access_result_t = typename subscript_access_result<T1, T2>::type;
}

#endif