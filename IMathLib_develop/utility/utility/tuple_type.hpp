#ifndef IMATH_UTILITY_UTILITY_TUPLE_TYPE_HPP
#define IMATH_UTILITY_UTILITY_TUPLE_TYPE_HPP

#include "IMathLib/IMathLib_config.hpp"

//型等のタプル

namespace iml {

	//可変長引数のタプル
	template<class... Types>
	struct type_tuple {
		static constexpr imsize_t value = sizeof...(Types);
	};


	//type_tupleを逆順にする
	template <class, class>
	struct reverse_type_tuple_impl;
	template <class... Inv, class First, class... Types>
	struct reverse_type_tuple_impl<type_tuple<Inv...>, type_tuple<First, Types...>>
		: public reverse_type_tuple_impl<type_tuple<First, Inv...>, type_tuple<Types...>> {};
	template <class... Inv>
	struct reverse_type_tuple_impl<type_tuple<Inv...>, type_tuple<>> {
		using type = type_tuple<Inv...>;
	};
	template <class>
	struct reverse_type_tuple;
	template <class... Types>
	struct reverse_type_tuple<type_tuple<Types...>>
		:reverse_type_tuple_impl<type_tuple<>, type_tuple<Types...>> {};


	//シーケンスの結合
	template<class, class...>
	struct cat_type_tuple_impl;
	template<class... Types1, class... Types2, class... TypeTuples>
	struct cat_type_tuple_impl<type_tuple<Types1...>, type_tuple<Types2...>, TypeTuples...>
		: cat_type_tuple_impl<type_tuple<Types1..., Types2...>, TypeTuples...> {};
	template<class... Types>
	struct cat_type_tuple_impl<type_tuple<Types...>> {
		using type = type_tuple<Types...>;
	};
	template <class... TypeTuples>
	struct cat_type_tuple : cat_type_tuple_impl<TypeTuples...> {};


	//N個の同じ型で構築
	template <class T, imsize_t N, class... Types>
	struct same_type_tuple : same_type_tuple<T, N - 1, Types..., T> {};
	template <class T, class... Types>
	struct same_type_tuple<T, 0, Types...> {
		using type = type_tuple<Types...>;
	};


	//多層構造となっているtype_tupleを1階層にまとめる
	template <class T>
	struct unit_type_tuple {
		using type = type_tuple<T>;
	};
	template <class, class>
	struct unit_type_tuple_impl;
	template <class... Types1, class T, class... Types2>
	struct unit_type_tuple_impl<type_tuple<Types1...>, type_tuple<T, Types2...>>
		: unit_type_tuple_impl<typename cat_type_tuple<type_tuple<Types1...>, typename unit_type_tuple<T>::type>::type
		, type_tuple<Types2...>> {};
	template <class T>
	struct unit_type_tuple_impl<T, type_tuple<>> {
		using type = T;
	};
	template <class... Types>
	struct unit_type_tuple<type_tuple<Types...>> : unit_type_tuple_impl<type_tuple<>, type_tuple<Types...>> {};

}

namespace iml {

	//インデックスのタプル
	template <class T, T... Indices>
	struct index_tuple {
		static constexpr imsize_t value = sizeof...(Indices);
		using type = T;
	};
	template<imsize_t... Indices>
	using index_imu_tuple = index_tuple<imsize_t, Indices...>;
	template<imint_t... Indices>
	using index_imi_tuple = index_tuple<imint_t, Indices...>;


	//index_tupleを引数にとるものはimsize_tおよびimint_tに関わらず共通の宣言

	//index_imu_tupleを逆順にする
	template <class, class>
	struct reverse_index_tuple_impl;
	template <class T, T... Inv, T First, T... Indices>
	struct reverse_index_tuple_impl<index_tuple<T, Inv...>, index_tuple<T, First, Indices...>>
		: public reverse_index_tuple_impl<index_tuple<T, First, Inv...>, index_tuple<T, Indices...>> {};
	template <class T, T... Inv>
	struct reverse_index_tuple_impl<index_tuple<T, Inv...>, index_tuple<T>> {
		using type = index_tuple<T, Inv...>;
	};
	template <class>
	struct reverse_index_tuple;
	template <class T, T... Indices>
	struct reverse_index_tuple<index_tuple<T, Indices...>>
		:reverse_index_tuple_impl<index_tuple<T>, index_tuple<T, Indices...>> {};


	//シーケンスの結合
	template<class, class...>
	struct cat_index_tuple_impl;
	template<class T, T... Indices1, T... Indices2, class... IndexTuples>
	struct cat_index_tuple_impl<index_tuple<T, Indices1...>, index_tuple<T, Indices2...>, IndexTuples...>
		: cat_index_tuple_impl<index_tuple<T, Indices1..., Indices2...>, IndexTuples...> {};
	template<class T, T... Indices>
	struct cat_index_tuple_impl<index_tuple<T, Indices...>> {
		using type = index_tuple<T, Indices...>;
	};
	template <class T, class... IndexTuples>
	struct cat_index_tuple : cat_index_tuple_impl<index_tuple<typename T::type>, T, IndexTuples...> {};


	//N個の同じ要素で構築
	template <class T, T Index, imsize_t N, T... Indices>
	struct same_index_tuple : same_index_tuple<T, Index, N - 1, Index, Indices...> {};
	template <class T, T Index, T... Indices>
	struct same_index_tuple<T, Index, 0, Indices...> {
		using type = index_tuple<T, Indices...>;
	};
	template <imsize_t Index, imsize_t N, imsize_t... Indices>
	using same_index_imu_tuple = same_index_tuple<imsize_t, Index, N, Indices...>;
	template <imint_t Index, imsize_t N, imint_t... Indices>
	using same_index_imi_tuple = same_index_tuple<imint_t, Index, N, Indices...>;


	//index_tupleの生成([First,Last)の範囲)
	template<class T, T First, T Last, class result = index_tuple<T>, bool flag = (First >= Last)>
	struct index_range {
		using type = result;
	};
	template<class T, T Step, T Last, T... Indices>
	struct index_range<T, Step, Last, index_tuple<T, Indices...>, false>
		: index_range<T, Step + 1, Last, index_tuple<T, Indices..., Step>> {};
	template<imsize_t First, imsize_t Last>
	using index_imu_range = index_range<imsize_t, First, Last>;
	template<imint_t First, imint_t Last>
	using index_imi_range = index_range<imint_t, First, Last>;
}

#endif