#ifndef IMATH_UTILITY_TYPE_TRAITS_INTEGRAL_CONSTANT_HPP
#define IMATH_UTILITY_TYPE_TRAITS_INTEGRAL_CONSTANT_HPP


//整数型定数

namespace iml {

	//整数型定数
	template<class T, T Val>
	struct integral_constant {
		static constexpr T value = Val;

		using value_type = T;
		using type = integral_constant<T, Val>;

		constexpr operator value_type() const noexcept { return value; }
		constexpr value_type operator()() const noexcept { return value; }

	};
	//bool型の定数
	template<bool Val>
	using bool_constant = integral_constant<bool, Val>;
	using true_type = bool_constant<true>;
	using false_type = bool_constant<false>;


	//叙述の補助
	template <bool Val>
	struct cat_bool : bool_constant<Val> {};
	template <bool Val, bool... Vals>
	struct and_bool : bool_constant<Val && and_bool<Vals...>::value> {};
	template <bool Val>
	struct and_bool<Val> : bool_constant<Val> {};
	template <bool... Vals>
	constexpr bool and_bool_v = and_bool<Vals...>::value;
	template <bool Val, bool... Vals>
	struct or_bool : bool_constant<Val || and_bool<Vals...>::value> {};
	template <bool Val>
	struct or_bool<Val> : bool_constant<Val> {};
	template <bool... Vals>
	constexpr bool or_bool_v = or_bool<Vals...>::value;
}

#endif