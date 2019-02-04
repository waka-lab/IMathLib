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
	struct _Reverse_type_tuple;
	template <class... Inv, class First, class... Types>
	struct _Reverse_type_tuple<type_tuple<Inv...>, type_tuple<First, Types...>>
		: public _Reverse_type_tuple<type_tuple<First, Inv...>, type_tuple<Types...>> {};
	template <class... Inv>
	struct _Reverse_type_tuple<type_tuple<Inv...>, type_tuple<>> {
		using type = type_tuple<Inv...>;
	};
	template <class>
	struct reverse_type_tuple;
	template <class... Types>
	struct reverse_type_tuple<type_tuple<Types...>>
		:_Reverse_type_tuple<type_tuple<>, type_tuple<Types...>> {};
	//2つのシーケンスの結合
	template<class, class>
	struct cat_type_tuple;
	template<class... Types1, class... Types2>
	struct cat_type_tuple<type_tuple<Types1...>, type_tuple<Types2...>> {
		using type = type_tuple<Types1..., Types2...>;
	};
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
	struct _Unit_type_tuple;
	template <class... Types1, class T, class... Types2>
	struct _Unit_type_tuple<type_tuple<Types1...>, type_tuple<T, Types2...>>
		: _Unit_type_tuple<typename cat_type_tuple<type_tuple<Types1...>, typename unit_type_tuple<T>::type>::type
		, type_tuple<Types2...>> {};
	template <class T>
	struct _Unit_type_tuple<T, type_tuple<>> {
		using type = T;
	};
	template <class... Types>
	struct unit_type_tuple<type_tuple<Types...>> : _Unit_type_tuple<type_tuple<>, type_tuple<Types...>> {};


	//インデックスのタプル
	template<imsize_t... Indices>
	struct index_tuple {
		static constexpr imsize_t value = sizeof...(Indices);
	};
	//index_tupleを逆順にする
	template <class, class>
	struct _Reverse_index_tuple;
	template <imsize_t... Inv, imsize_t First, imsize_t... Indices>
	struct _Reverse_index_tuple<index_tuple<Inv...>, index_tuple<First, Indices...>>
		: public _Reverse_index_tuple<index_tuple<First, Inv...>, index_tuple<Indices...>> {};
	template <imsize_t... Inv>
	struct _Reverse_index_tuple<index_tuple<Inv...>, index_tuple<>> {
		using type = index_tuple<Inv...>;
	};
	template <class>
	struct reverse_index_tuple;
	template <imsize_t... Indices>
	struct reverse_index_tuple<index_tuple<Indices...>>
		:_Reverse_index_tuple<index_tuple<>, index_tuple<Indices...>> {};
	//2つのシーケンスの結合
	template<class, class>
	struct cat_index_tuple;
	template<imsize_t... Indices1, imsize_t... Indices2>
	struct cat_index_tuple<index_tuple<Indices1...>, index_tuple<Indices2...>> {
		using type = index_tuple<Indices1..., Indices2...>;
	};
	//N個の同じ要素で構築
	template <imsize_t Index, imsize_t N, imsize_t... Indices>
	struct same_index_tuple : same_index_tuple<Index, N - 1, Index, Indices...> {};
	template <imsize_t Index, imsize_t... Indices>
	struct same_index_tuple<Index, 0, Indices...> {
		using type = index_tuple<Indices...>;
	};


	//index_tupleの生成([First,Last)の範囲)
	template<imsize_t First, imsize_t Last, class result = index_tuple<>, bool flag = (First >= Last)>
	struct index_range {
		using type = result;
	};
	template<imsize_t Step, imsize_t Last, imsize_t... Indices>
	struct index_range<Step, Last, index_tuple<Indices...>, false>
		: index_range<Step + 1, Last, index_tuple<Indices..., Step>> {};
}

#endif