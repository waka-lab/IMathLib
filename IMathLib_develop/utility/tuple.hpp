#ifndef IMATH_UTILITIY_TUPLE_HPP
#define IMATH_UTILITIY_TUPLE_HPP

#include "IMathLib/IMathLib_config.hpp"
#include "IMathLib/utility/utility.hpp"
#include "IMathLib/utility/functional.hpp"


//ペア
namespace iml {

	template <class T1, class T2>
	struct pair {
		template <class U1, class U2>
		friend struct pair;
		T1	first;
		T2	second;

		using first_type = T1;
		using second_type = T2;
		
		constexpr pair() : first(), second() {}
		constexpr pair(const pair& p) : first(p.first), second(p.second) {}
		template <class U1, class U2>
		constexpr pair(const pair<U1, U2>& p) : first(p.first), second(p.second) {}
		constexpr pair(pair&& p) : first(p.first), second(p.second) {}
		template <class U1, class U2>
		constexpr pair(pair<U1, U2>&& p) : first(p.first), second(p.second) {}
		constexpr pair(const T1& x, const T2& y) : first(x), second(y) {}
		template <class U1, class U2>
		constexpr pair(const U1& x, const U2& y) : first(x), second(y) {}
		constexpr pair(T1&& x, T2&& y) : first(x), second(y) {}
		template <class U1, class U2>
		constexpr pair(U1&& x, U2&& y) : first(x), second(y) {}

		void swap(pair& p) { iml::swap(*this, p); }

		pair& operator=(const pair& p) {
			first = p.first;
			second = p.second;
			return *this;
		}
		template <class U1, class U2>
		pair& operator=(const pair<U1, U2>& p) {
			first = p.first;
			second = p.second;
			return *this;
		}
		pair& operator=(pair&& p) noexcept {
			first = p.first;
			second = p.second;
			return *this;
		}
		template <class U1, class U2>
		pair& operator=(pair<U1, U2>&& p) noexcept {
			first = p.first;
			second = p.second;
			return *this;
		}

	private:
		template<size_t N>
		struct Get;
		template<>
		struct Get<0> {
			static const constexpr typename remove_reference<first_type>& _get_(const pair& v) { return v.first; }
			static typename remove_reference<first_type>& _get_(pair& v) { return v.first; }
		};
		template<>
		struct Get<1> {
			static const constexpr second_type& _get_(const pair& v) { return v.second; }
			static second_type& _get_(pair& v) { return v.second; }
		};
	public:
		template <size_t N>
		const constexpr auto& get() const { return Get<N>::_get_(*this); }
		template <size_t N>
		auto& get() { return Get<N>::_get_(*this); }
	};
}

//タプル
namespace iml {


	//変数ホルダー
	template <class T, size_t N>
	struct value_holder {
		T value;

		constexpr value_holder() : value() {}
		constexpr explicit value_holder(const T& arg) : value(arg) {}
		template <class U>
		constexpr explicit value_holder(U&& arg) : value(iml::forward<U>(arg)) {}
		value_holder(const value_holder&) = default;
		value_holder(value_holder&&) = default;
	};
	//タプルの基底
	template <class, class...>
	class tuple_base;
	template <size_t... Indices, class... Types>
	class tuple_base<index_tuple<size_t, Indices...>, Types...> : public value_holder<Types, Indices>... {

		//要素取得のため
		template <size_t N, class T>
		static const constexpr remove_reference_t<T>& get_impl(const value_holder<T, N>& t) { return t.value; }
		template <size_t N, class T>
		static constexpr remove_reference_t<T>& get_impl(value_holder<T, N>& t) { return t.value; }

	public:
		constexpr tuple_base() : value_holder<Types, Indices>()... {}
		constexpr explicit tuple_base(const Types&... args) : value_holder<Types, Indices>(args)... {}
		template <class... UTypes>
		constexpr tuple_base(UTypes&&... args) : value_holder<Types, Indices>(iml::forward<UTypes>(args))... {}
		tuple_base(const tuple_base&) = default;
		tuple_base(tuple_base&&) = default;
		template <class... UTypes>
		constexpr tuple_base(const tuple_base<UTypes...>& t) : value_holder<Types, Indices>(t.get<Indices>())... {}
		template <class... UTypes>
		constexpr tuple_base(tuple_base<UTypes...>&& t) : value_holder<Types, Indices>(iml::forward<UTypes>(t.get<Indices>()))... {}


		using sequence_type = index_tuple<size_t, Indices...>;
		using type_tuple_type = type_tuple<Types...>;


		//要素取得
		template <size_t N>
		const constexpr auto& get() const { return get_impl<N>(*this); }
		template <size_t N>
		constexpr auto& get() { return get_impl<N>(*this); }
		//要素型の数
		static constexpr size_t size() { return sizeof...(Types); }

		tuple_base& operator=(const tuple_base&) = default;
		tuple_base& operator=(tuple_base&& im) noexcept = default;

		void swap(tuple_base& t) { iml::swap(*this, t); }
	};
	//1以上の要素をもつタプル
	template <class... Types>
	class tuple : public tuple_base<index_range_t<size_t, 0, sizeof...(Types)>, Types...> {
	public:
		using tuple_base<index_range_t<size_t, 0, sizeof...(Types)>, Types...>::tuple_base;
	};
	//空タプル
	template <>
	class tuple<> {
	public:
		using sequence_type = index_tuple<size_t>;

		//要素型の数
		static constexpr size_t size() { return 0; }
	};


	//タプルのサイズの取得
	template <class>
	struct tuple_size {
		static constexpr size_t value = 0;
	};
	template <class... Types>
	struct tuple_size<tuple<Types...>> {
		static constexpr size_t value = sizeof...(Types);
	};
	template <class T>
	constexpr size_t tuple_size_v = tuple_size<T>::value;



	//タプルの判定
	template <class>
	struct is_tuple_impl : false_type {};
	template <class... Types>
	struct is_tuple_impl<tuple<Types...>> : true_type {};
	template <class T>
	struct is_tuple : is_tuple_impl<remove_cv_t<T>> {};
	template <class T>
	constexpr bool is_tuple_v = is_tuple<T>::value;


	//多次元配列の構築
	template <class, class, size_t>
	struct multi_array_impl;
	template <class T, size_t N, size_t Dim>
	struct multi_array_impl<T, index_tuple<size_t, N>, Dim> {
		using type = T[N];
		static constexpr size_t value = Dim * N;
	};
	template <class T, size_t First, size_t Second, size_t... Indices, size_t Dim>
	struct multi_array_impl<T, index_tuple<size_t, First, Second, Indices...>, Dim> : multi_array_impl<T[First], index_tuple<size_t, Second, Indices...>, Dim * First> {};
	template <class T, size_t... Indices>
	struct multi_array : multi_array_impl<T, reverse_index_tuple_t<index_tuple<size_t, Indices...>>, 1> {};
	template <class T, size_t... Indices>
	using multi_array_t = typename multi_array<T, Indices...>::type;


	//配列の添え字リスト等から次元の取得
	template <size_t, class>
	struct dimension_impl;
	template <size_t Dim>
	struct dimension_impl<Dim, index_tuple<size_t>> {
		static_assert(Dim > 0, "0 parameter should not exist.");
		static constexpr size_t value = Dim;
	};
	template <size_t Dim, size_t First, size_t... Indices>
	struct dimension_impl<Dim, index_tuple<size_t, First, Indices...>> : dimension_impl<Dim*First, index_tuple<size_t, Indices...>> {};
	template <class>
	struct dimension;
	template <size_t First, size_t... Indices>
	struct dimension<index_tuple<size_t, First, Indices...>> : dimension_impl<First, index_tuple<size_t, Indices...>> {};
	template<class, size_t>
	class vector;
	template <class T, size_t N>
	struct dimension<vector<T, N>> {
		static constexpr size_t value = N;
	};
	template <class, size_t, size_t>
	class matrix;
	template <class T, size_t M, size_t N>
	struct dimension<matrix<T, M, N>> {
		static constexpr size_t value = M * N;
	};
	template <class T>
	constexpr size_t dimension_v = dimension<T>::value;


	//元の型でのタプルの構築
	template <class... Types>
	inline constexpr tuple<reference_unwrapper_t<Types>...> make_tuple(Types&&... args) {
		return tuple<reference_unwrapper_t<Types>...>(iml::forward<Types>(args)...);
	}
	//参照でのタプルの構築
	template<class... Types>
	inline constexpr tuple<Types&...> tie(Types&... args) noexcept {
		return tuple<Types&...>(args...);
	}
	//右辺値でのタプルの構築
	template<class... Types>
	inline constexpr tuple<Types&&...> forward_as_tuple(Types&&... args) noexcept {
		return tuple<Types&&...>(iml::forward<Types>(args)...);
	}


	//tupleを展開して関数の引数にする
	template <class>
	struct Apply1;
	template <size_t... Indices>
	struct Apply1<index_tuple<size_t, Indices...>> {
		template <class F, class Tuple>
		static constexpr auto _apply_(F&& f, Tuple&& t) {
			return invoke(iml::forward<F>(f), iml::forward<Tuple>(t).get<Indices>()...);
		}
	};
	template <class F, class Tuple>
	inline constexpr auto apply(F&& f, Tuple&& t) {
		using tuple_type = typename decay<Tuple>::type;
		return Apply1<typename tuple_type::sequence_type>::_apply_(iml::forward<F>(f), iml::forward<Tuple>(t));
	}
	template <class, class>
	struct Apply2;
	template <size_t... Indices1, size_t... Indices2>
	struct Apply2<index_tuple<size_t, Indices1...>, index_tuple<size_t, Indices2...>> {
		template <class TupleFs, class Tuple>
		static constexpr auto _apply_(TupleFs&& f, Tuple&& t) {
			return make_tuple(invoke(forward<TupleFs>(f).get<Indices1>(), forward<Tuple>(t).get<Indices2>()...)...);
		}
	};
	//applyの関数がtupleで構成されているバージョン(Fsの各関数の引数の数は、Typeの数に等しくなければならない)
	template <class... Fs, class Tuple>
	inline constexpr auto apply(tuple<Fs...>&& f, Tuple&& t) {
		using tuple_type = typename decay<Tuple>::type;
		return Apply2<typename tuple<Fs...>::sequence_type, typename tuple_type::sequence_type>::_apply_(f, t);
	}
	
	//2項演算の定義
	template <class, class...>
	struct tuple_add_impl;
	template <size_t... Indices, class... Types>
	struct tuple_add_impl<index_tuple<size_t, Indices...>, Types...> {
		static tuple<Types...> _tuple_add(const tuple<Types...>& t1, const tuple<Types...>& t2) {
			return tuple<Types...>(forward<Types>(t1.get<Indices>() + t2.get<Indices>())...);
		}
	};
	template <class... Types>
	inline tuple<Types...> operator+(const tuple<Types...>& t1, const tuple<Types...>& t2) {
		return tuple_add_impl<typename tuple<Types...>::sequence_type, Types...>::_tuple_add(t1, t2);
	}
	template <class, class...>
	struct tuple_sub_impl;
	template <size_t... Indices, class... Types>
	struct tuple_sub_impl<index_tuple<size_t, Indices...>, Types...> {
		static tuple<Types...> _tuple_sub(const tuple<Types...>& t1, const tuple<Types...>& t2) {
			return tuple<Types...>(forward<Types>(t1.get<Indices>() - t2.get<Indices>())...);
		}
	};
	template <class... Types>
	inline tuple<Types...> operator-(const tuple<Types...>& t1, const tuple<Types...>& t2) {
		return tuple_sub_impl<typename tuple<Types...>::sequence_type, Types...>::_tuple_sub(t1, t2);
	}
	template <class, class, class...>
	struct tuple_mul_impl;
	template <size_t... Indices, class T, class... Types>
	struct tuple_mul_impl<index_tuple<size_t, Indices...>, T, Types...> {
		static tuple<Types...> _tuple_mul(const T& t1, const tuple<Types...>& t2) {
			return tuple<Types...>(forward<Types>(t1 * t2.get<Indices>())...);
		}
		static tuple<Types...> _tuple_mul(const tuple<Types...>& t1, const T& t2) {
			return tuple<Types...>(forward<Types>(t1.get<Indices>() * t2)...);
		}
	};
	template <class T, class... Types>
	inline tuple<Types...> operator*(const T& t1, const tuple<Types...>& t2) {
		return tuple_mul_impl<typename tuple<Types...>::sequence_type, T, Types...>::_tuple_mul(t1, t2);
	}
	template <class T, class... Types>
	inline tuple<Types...> operator*(const tuple<Types...>& t1, const T& t2) {
		return tuple_mul_impl<typename tuple<Types...>::sequence_type, T, Types...>::_tuple_mul(t1, t2);
	}
	template <class, class, class...>
	struct tuple_div_impl;
	template <size_t... Indices, class T, class... Types>
	struct tuple_div_impl<index_tuple<size_t, Indices...>, T, Types...> {
		static tuple<Types...> _tuple_div(const tuple<Types...>& t1, const T& t2) {
			return tuple<Types...>(forward<Types>(t1.get<Indices>() / t2)...);
		}
	};
	template <class T, class... Types>
	inline tuple<Types...> operator/(const tuple<Types...>& t1, const T& t2) {
		return tuple_div_impl<typename tuple<Types...>::sequence_type, T, Types...>::_tuple_div(t1, t2);
	}


	//タプルの連結のプロパティの構築(型およびインデックス)
	template <size_t N, class, class, class, class...>
	struct tuple_cat_traits_impl;
	template <size_t N, class... Args, size_t... Indices1, size_t... Indices2, class... Types, class... Tuples>
	struct tuple_cat_traits_impl<N, tuple<Args...>, index_tuple<size_t, Indices1...>, index_tuple<size_t, Indices2...>, tuple<Types...>, Tuples...>
		: tuple_cat_traits_impl<N + 1, tuple<Args..., Types...>
		, cat_index_tuple_t<index_tuple<size_t, Indices1...>, same_index_tuple_t<size_t, N, sizeof...(Types)>>
		, cat_index_tuple_t<index_tuple<size_t, Indices2...>, typename tuple<Types...>::sequence_type>
		, Tuples...> {};
	template <size_t N, class T, class Index1, class Index2>
	struct tuple_cat_traits_impl<N, T, Index1, Index2> {
		using type = T;
		using index1 = Index1;
		using index2 = Index2;
	};
	template <class... Tuples>
	struct tuple_cat_traits : tuple_cat_traits_impl<0, tuple<>, index_tuple<size_t>, index_tuple<size_t>, decay_t<Tuples>...> {};
	//タプルの連結
	template <class, class, class>
	struct tuple_cat_impl;
	template <class ResultT, size_t... Indices1, size_t... Indices2>
	struct tuple_cat_impl<ResultT, index_tuple<size_t, Indices1...>, index_tuple<size_t, Indices2...>> {
		template <class Tuple>
		static constexpr ResultT __tuple_cat(Tuple&& t) {
			return ResultT(iml::forward<Tuple>(t).get<Indices1>().get<Indices2>()...);
		}
	};
	template <class... Tuples>
	inline constexpr auto tuple_cat(Tuples&&... t) {
		using type = tuple_cat_traits<Tuples...>;
		return tuple_cat_impl<typename type::type, typename type::index1, typename type::index2>::__tuple_cat(
			iml::forward_as_tuple(iml::forward<Tuples>(t)...));
	}
}

#endif