#ifndef IMATH_UTILITIY_TUPLE_HPP
#define IMATH_UTILITIY_TUPLE_HPP

#include "IMathLib/IMathLib_config.hpp"
#include "IMathLib/utility/utility.hpp"
#include "IMathLib/container/container.hpp"
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
		template<imsize_t N>
		struct Get;
		template<>
		struct Get<0> {
			static constexpr first_type _get_(const pair& v) { return v.first; }
			static first_type& _get_(pair& v) { return v.first; }
		};
		template<>
		struct Get<1> {
			static constexpr second_type _get_(const pair& v) { return v.second; }
			static second_type& _get_(pair& v) { return v.second; }
		};
	public:
		template <imsize_t N>
		constexpr auto& get() const { return Get<N>::_get_(*this); }
		template <imsize_t N>
		auto& get() { return Get<N>::_get_(*this); }
	};
}

//タプル
namespace iml {


	//変数ホルダー
	template <class T, imsize_t N>
	class _Value_holder {
		template <class, class...>
		friend class tuple_base;

		T value;
	public:
		constexpr _Value_holder() : value() {}
		constexpr _Value_holder(const T& arg) : value(static_cast<T>(arg)) {}
		~_Value_holder() {}
	};
	//タプルの基底
	template <class Indices, class... Types>
	class tuple_base;
	template <imsize_t... Indices, class... Types>
	class tuple_base<index_imu_tuple<Indices...>, Types...> : public _Value_holder<Types, Indices>... {

		//要素取得のため
		template <imsize_t N, class T>
		static constexpr T get_impl(const _Value_holder<T, N>& t) { return t.value; }
		template <imsize_t N, class T>
		static T& get_impl(_Value_holder<T, N>& t) { return t.value; }

	public:
		constexpr tuple_base() : _Value_holder<Types, Indices>()... {}
		constexpr explicit tuple_base(const Types&... args) : _Value_holder<Types, Indices>(args)... {}
		template <class... UTypes>
		constexpr explicit tuple_base(UTypes&&... args) : _Value_holder<Types, Indices>(args)... {}
		tuple_base(const tuple_base&) = default;
		tuple_base(tuple_base&&) = default;

		using sequence_type = index_imu_tuple<Indices...>;

		//要素取得
		template <imsize_t N>
		constexpr auto get() const { return get_impl<N>(*this); }
		template <imsize_t N>
		auto& get() { return get_impl<N>(*this); }
		//要素型の数
		static constexpr imsize_t size() { return sizeof...(Types); }

		tuple_base& operator=(const tuple_base&) = default;
		tuple_base& operator=(tuple_base&& im) noexcept = default;

		void swap(tuple_base& t) { iml::swap(*this, t); }
	};
	template <class... Types>
	class tuple : public tuple_base<typename index_imu_range<0, sizeof...(Types)>::type, Types...> {
	public:
		using tuple_base<typename index_imu_range<0, sizeof...(Types)>::type, Types...>::tuple_base;
	};


	//多次元配列の構築
	template <class T, class Indices>
	struct multi_array_impl;
	template <class T, imsize_t N>
	struct multi_array_impl<T, index_imu_tuple<N>> {
		using type = T[N];
	};
	template <class T, imsize_t First, imsize_t... Indices>
	struct multi_array_impl<T, index_imu_tuple<First, Indices...>> : multi_array_impl<T[First], index_imu_tuple<Indices...>> {};
	template <class T, imsize_t First, imsize_t... Indices>
	struct multi_array : multi_array_impl<T, typename reverse_index_tuple<index_imu_tuple<First, Indices...>>::type> {};

	//配列の次元の取得
	template <imsize_t, class>
	struct _Dimension;
	template <imsize_t Dim>
	struct _Dimension<Dim, index_imu_tuple<>> {
		static_assert(Dim > 0, "0 parameter should not exist.");
		static constexpr imsize_t value = Dim;
	};
	template <imsize_t Dim, imsize_t First, imsize_t... Indices>
	struct _Dimension<Dim, index_imu_tuple<First, Indices...>> : _Dimension<Dim*First, index_imu_tuple<Indices...>> {};
	template <imsize_t First, imsize_t... Indices>
	struct dimension : _Dimension<First, index_imu_tuple<Indices...>> {};

	//元の型でのタプルの構築
	template <class... Types>
	inline constexpr tuple<typename reference_unwrapper<Types>::type...> make_tuple(Types&&... args) {
		return tuple<typename reference_unwrapper<Types>::type...>(forward<Types>(args)...);
	}
	//参照でのタプルの構築
	template<class... Types>
	inline constexpr tuple<Types&...> tie(Types&... args) noexcept {
		return tuple<Types&...>(args...);
	}
	//右辺値でのタプルの構築
	template<class... Types>
	inline constexpr tuple<Types&&...> forward_as_tuple(Types&&... args) noexcept {
		return tuple<Types&&...>(forward<Types>(args)...);
	}


	//tupleを展開して関数の引数にする
	template <class>
	struct Apply1;
	template <imsize_t... Indices>
	struct Apply1<index_imu_tuple<Indices...>> {
		template <class F, class Tuple>
		static constexpr auto _apply_(F&& f, Tuple&& t) {
			return forward<F>(f)(forward<Tuple>(t).get<Indices>()...);
		}
	};
	template <class F, class Tuple>
	inline constexpr auto apply(F&& f, Tuple&& t) {
		return Apply1<typename Tuple::sequence_type>::_apply_(forward<F>(f), forward<Tuple>(t));
	}
	template <class, class>
	struct Apply2;
	template <imsize_t... Indices1, imsize_t... Indices2>
	struct Apply2<index_imu_tuple<Indices1...>, index_imu_tuple<Indices2...>> {
		template <class TupleFs, class Tuple>
		static constexpr auto _apply_(TupleFs&& f, Tuple&& t) {
			return make_tuple(forward<TupleFs>(f).get<Indices1>()(forward<Tuple>(t).get<Indices2>()...)...);
		}
	};
	//applyの関数がtupleで構成されているバージョン(Fsの各関数の引数の数は、Typeの数に等しくなければならない)
	template <class... Fs, class Tuple>
	inline constexpr auto apply(tuple<Fs...>&& f, Tuple&& t) {
		return Apply2<typename tuple<Fs...>::sequence_type, typename Tuple::sequence_type>::_apply_(f, t);
	}
	
	//二項演算の定義
	template <class, class...>
	struct tuple_add_impl;
	template <imsize_t... Indices, class... Types>
	struct tuple_add_impl<index_imu_tuple<Indices...>, Types...> {
		static tuple<Types...> __tuple_add(const tuple<Types...>& t1, const tuple<Types...>& t2) {
			return tuple<Types...>(forward<Types>(t1.get<Indices>() + t2.get<Indices>())...);
		}
	};
	template <class... Types>
	inline tuple<Types...> operator+(const tuple<Types...>& t1, const tuple<Types...>& t2) {
		return tuple_add_impl<typename tuple<Types...>::sequence_type, Types...>::__tuple_add(t1, t2);
	}
	template <class, class...>
	struct tuple_sub_impl;
	template <imsize_t... Indices, class... Types>
	struct tuple_sub_impl<index_imu_tuple<Indices...>, Types...> {
		static tuple<Types...> __tuple_sub(const tuple<Types...>& t1, const tuple<Types...>& t2) {
			return tuple<Types...>(forward<Types>(t1.get<Indices>() - t2.get<Indices>())...);
		}
	};
	template <class... Types>
	inline tuple<Types...> operator-(const tuple<Types...>& t1, const tuple<Types...>& t2) {
		return tuple_sub_impl<typename tuple<Types...>::sequence_type, Types...>::__tuple_sub(t1, t2);
	}
	template <class, class, class...>
	struct tuple_mul_impl;
	template <imsize_t... Indices, class T, class... Types>
	struct tuple_mul_impl<index_imu_tuple<Indices...>, T, Types...> {
		static tuple<Types...> __tuple_mul(const T& t1, const tuple<Types...>& t2) {
			return tuple<Types...>(forward<Types>(t1 * t2.get<Indices>())...);
		}
		static tuple<Types...> __tuple_mul(const tuple<Types...>& t1, const T& t2) {
			return tuple<Types...>(forward<Types>(t1.get<Indices>() * t2)...);
		}
	};
	template <class T, class... Types>
	inline tuple<Types...> operator*(const T& t1, const tuple<Types...>& t2) {
		return tuple_mul_impl<typename tuple<Types...>::sequence_type, T, Types...>::__tuple_mul(t1, t2);
	}
	template <class T, class... Types>
	inline tuple<Types...> operator*(const tuple<Types...>& t1, const T& t2) {
		return tuple_mul_impl<typename tuple<Types...>::sequence_type, T, Types...>::__tuple_mul(t1, t2);
	}
	template <class, class, class...>
	struct tuple_div_impl;
	template <imsize_t... Indices, class T, class... Types>
	struct tuple_div_impl<index_imu_tuple<Indices...>, T, Types...> {
		static tuple<Types...> __tuple_div(const tuple<Types...>& t1, const T& t2) {
			return tuple<Types...>(forward<Types>(t1.get<Indices>() / t2)...);
		}
	};
	template <class T, class... Types>
	inline tuple<Types...> operator/(const tuple<Types...>& t1, const T& t2) {
		return tuple_div_impl<typename tuple<Types...>::sequence_type, T, Types...>::__tuple_div(t1, t2);
	}


	//タプルの連結のプロパティの構築(型およびインデックス)
	template <imsize_t N, class, class, class, class...>
	struct tuple_cat_traits_impl;
	template <imsize_t N, class... Args, imsize_t... Indices1, imsize_t... Indices2, class... Types, class... Tuples>
	struct tuple_cat_traits_impl<N, tuple<Args...>, index_imu_tuple<Indices1...>, index_imu_tuple<Indices2...>, tuple<Types...>, Tuples...>
		: tuple_cat_traits_impl<N + 1, tuple<Args..., Types...>
		, typename cat_index_tuple<index_imu_tuple<Indices1...>, typename same_index_imu_tuple<N, sizeof...(Types)>::type>::type
		, typename cat_index_tuple<index_imu_tuple<Indices2...>, typename tuple<Types...>::sequence_type>::type
		, Tuples...> {};
	template <imsize_t N, class T, class Index1, class Index2>
	struct tuple_cat_traits_impl<N, T, Index1, Index2> {
		using type = T;
		using index1 = Index1;
		using index2 = Index2;
	};
	template <class... Tuples>
	struct tuple_cat_traits : tuple_cat_traits_impl<0, tuple<>, index_imu_tuple<>, index_imu_tuple<>, typename decay<Tuples>::type...> {};
	//タプルの連結
	template <class, class, class>
	struct tuple_cat_impl;
	template <class ResultT, imsize_t... Indices1, imsize_t... Indices2>
	struct tuple_cat_impl<ResultT, index_imu_tuple<Indices1...>, index_imu_tuple<Indices2...>> {
		template <class Tuple>
		static constexpr ResultT __tuple_cat(Tuple&& t) {
			return ResultT(forward<Tuple>(t).get<Indices1>().get<Indices2>()...);
		}
	};
	template <class... Tuples>
	inline constexpr auto tuple_cat(Tuples&&... t) {
		using type = tuple_cat_traits<Tuples...>;
		return tuple_cat_impl<typename type::type, typename type::index1, typename type::index2>::__tuple_cat(
			forward_as_tuple(forward<Tuples>(t)...));
	}
}

#endif