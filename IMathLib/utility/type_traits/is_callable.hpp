#ifndef IMATH_UTILITY_TYPE_TRAITS_IS_CALLABLE_HPP
#define IMATH_UTILITY_TYPE_TRAITS_IS_CALLABLE_HPP

#include "IMathLib/utility/type_traits/integral_constant.hpp"
#include "IMathLib/utility/type_traits/declval.hpp"


namespace iml {

	//関数が呼び出し可能かの判定
	template <class F, class... Types>
	struct is_callable_impl {
	private:
		template <class G> static auto tester(G&) -> decltype(declval<G>()(declval<Types>()...), true_type());
		template <class G> static false_type tester(...);
	public:
		using type = decltype(tester<F>(declval<F&>()));
	};
	template <class F, class... Types>
	struct is_callable : is_callable_impl<F, Types...>::type {};
	template <class F, class... Types>
	constexpr bool is_callable_v = is_callable<F, Types...>::value;
}

#endif