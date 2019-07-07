#ifndef IMATH_UTILITY_ITERATOR_HPP
#define IMATH_UTILITY_ITERATOR_HPP

#include "IMathLib/utility/utility.hpp"
#include "IMathLib/utility/type_traits.hpp"


// ポインタ情報
namespace iml {

	// アドレスを返す
	template <class T, bool = is_function_v<T>>
	struct Addressof {
		// 関数のアドレス
		static constexpr T* _addressof_(T& val) noexcept { return val; }
	};
	template <class T>
	struct Addressof<T, false> {
		// オブジェクトのアドレス(operator&()対策)
		static constexpr T* _addressof_(T& val) noexcept {
			return (reinterpret_cast<T*>(&const_cast<char&>(reinterpret_cast<const volatile char&>(val))));
		}
	};
	template<class T>
	inline constexpr T* addressof(T& val) noexcept {
		return Addressof<T>::_addressof_(val);
	}
	template <class T>
	constexpr T* addressof(const T&&) = delete;


	// ポインタとみなせるオブジェクト(スマートポインタやイテレータ等)からアドレスを取得する
	template <class Ptr>
	inline auto to_address(Ptr& p) noexcept { return p.operator->(); }
	template <class T>
	constexpr T* to_address(T* p) noexcept { return p; }


	// 任意の配列やポインタを1次元の先頭ポインタにキャスト
	template <class, bool>
	struct Pointer_cast;
	template <class T>
	struct Pointer_cast<T*, false> {
		template <class = enable_if_t<!is_pointer_v<T>>>
		static constexpr T* pointer_cast(T* p) { return p; }
	};
	template <class T>
	struct Pointer_cast<T*, true> {
		static constexpr auto pointer_cast(T* p) {
			return Pointer_cast<remove_extent_t<T>*, is_array_v<remove_extent_t<T>>>::pointer_cast(&(*p)[0]);
		}
	};
	template <class T, bool F>
	struct Pointer_cast<T**, F> {
		static constexpr auto pointer_cast(T** p) { return Pointer_cast<T*, is_array_v<T>>::pointer_cast(*p); }
	};
	template <class T>
	inline constexpr auto pointer_cast(T* p) { return Pointer_cast<T*, is_array_v<T>>::pointer_cast(p); }



	// スマートポインタのパラメータを取得するためのもの
	// 型の最初のテンプレートパラメータの取得
	template <class>
	struct get_first_param;
	template <template <class, class...> class T, class First, class... Rest>
	struct get_first_param<T<First, Rest...>> {
		using type = First;
	};
	// 型の最初のテンプレートパラメータを置き換える
	template <class, class>
	struct replace_first_param;
	template <class Replace, template <class, class...> class T, class First, class... Rest>
	struct replace_first_param<Replace, T<First, Rest...>> {
		using type = T<Replace, Rest...>;
	};

	template <class T>
	struct get_element_type {
		// pointer_traitsを想定
		template <class S> static auto tester(int) -> identity<typename S::element_type> {}
		// スマートポインタの場合を想定
		template <class S> static auto tester(int_wrapper) -> identity<typename get_first_param<S>::type> {}

		using type = typename decltype(tester<T>(0))::type;
	};
	template <class T>
	struct get_ptrdiff_type {
		template <class S> static auto tester(int) -> identity<typename S::difference_type> {}
		template <class S> static auto tester(int_wrapper) -> identity<ptrdiff_t> {}

		using type = typename decltype(tester<T>(0))::type;
	};
	template <class T, class Other>
	struct get_rebind_type {
		template <class S> static auto tester(int) -> identity<typename S::template rebind<Other>::other> {}
		template <class S> static auto tester(int_wrapper) -> identity<typename replace_first_param<Other, S>::type> {}

		using type = typename decltype(tester<T>(0))::type;
	};


	// ポインタとみなせる型の情報
	// スマートポインター等
	template<class T>
	struct pointer_traits {
		using pointer = T;
		using element_type = typename get_element_type<T>::type;
		using difference_type = typename get_ptrdiff_type<T>::type;

		template<class Other>
		using rebind = typename get_rebind_type<T, Other>::type;

		// 変数へのポインタの取得
		using reference_type = conditional_t<is_void_v<element_type>, char&, add_lvalue_reference_t<element_type>>;
		static constexpr pointer pointer_to(reference_type v) {
			return T::pointer_to(v);
		}
	};
	// 通常のポインター
	template<class T>
	struct pointer_traits<T*> {
		using pointer = T * ;
		using element_type = T;
		using difference_type = ptrdiff_t;

		template<class Other>
		using rebind = Other * ;

		// 変数へのポインタの取得
		using reference_type = conditional_t<is_void_v<T>, char&, add_lvalue_reference_t<element_type>>;
		static constexpr pointer pointer_to(reference_type v) noexcept { return addressof(v); }
	};

}

// イテレータ情報
namespace iml {

	// イテレータを示すタグ
	// 入力イテレータ(++移動と*参照が可能)
	struct input_iterator_tag {};
	// 出力イテレータ(++移動と*出力が可能)
	struct output_iterator_tag {};
	// 前方向イテレータ(input+outout)
	struct forward_iterator_tag : input_iterator_tag, output_iterator_tag {};
	// 双方向イテレータ(--移動+forward)
	struct bidirectional_iterator_tag : forward_iterator_tag {};
	// ランダムアクセスイテレータ(ランダムアクセス可能)
	struct random_access_iterator_tag : bidirectional_iterator_tag {};


	//iterator_tagの判定
	template <class IteratorTag>
	struct is_iterator_tag : cat_bool<is_base_of_v<input_iterator_tag, IteratorTag> || is_base_of_v<output_iterator_tag, IteratorTag>> {};
	template <class T>
	constexpr bool is_iterator_tag_v = is_iterator_tag<T>::value;


	// voidのとき
	template<class, class = void>
	struct iterator_traits_base {};
	// iterator_traitsをもつ対象のとき
	template<class Iterator>
	struct iterator_traits_base<Iterator, void_t<
		typename Iterator::iterator_category,
		typename Iterator::value_type,
		typename Iterator::difference_type,
		typename Iterator::pointer,
		typename Iterator::reference>> {
		using iterator_category = typename Iterator::iterator_category;	// イテレータの分類
		using value_type = typename Iterator::value_type;				// イテレータの型
		using difference_type = typename Iterator::difference_type;		// イテレータの差
		using pointer = typename Iterator::pointer;						// ポインタ
		using reference = typename Iterator::reference;					// 参照
	};
	// イテレータの特性
	template<class Iterator>
	struct iterator_traits: iterator_traits_base<Iterator>{};
	// ポインタのとき
	template<class T>
	struct iterator_traits<T*> {
		using iterator_category = random_access_iterator_tag;
		using value_type = remove_cv_t<T>;
		using difference_type = ptrdiff_t;
		using pointer = T*;
		using reference = T&;
	};
	// ポインタが示す実体が変更不可のとき
	template<class T>
	struct iterator_traits<const T*>{
		using iterator_category = random_access_iterator_tag;
		using value_type = T;
		using difference_type = ptrdiff_t;
		using pointer = const T*;
		using reference = const T&;
	};

	// イテレータの基底(イテレータは事実上無限に進めることができなければならない)
	template<class Category, class T, class Distance = ptrdiff_t, class Pointer = T*, class Reference = T&>
	struct iterator_base {
		using iterator_category = Category;			// イテレータの分類
		using value_type = T;						// イテレータの型
		using difference_type = Distance;			// イテレータの差

		using pointer = Pointer;					// ポインタ
		using reference = Reference;				// 参照
	};
	// 出力イテレータの基底
	using _Output_itr = iterator_base<output_iterator_tag, void, void, void, void>;


	// iteratorからconst_iteratorに変換
	template <class Iterator>
	inline constexpr auto to_const_iterator(Iterator itr) {
		using result_type = typename Iterator::template rebind_t<const typename Iterator::value_type>;
		return *reinterpret_cast<result_type*>(&itr);
	}
	// const_iteratorからiteratorに変換
	template <class ConstIterator>
	inline constexpr auto to_iterator(ConstIterator itr) {
		using result_type = typename ConstIterator::template rebind_t<remove_cv_t<typename ConstIterator::value_type>>;
		return *reinterpret_cast<result_type*>(&itr);
	}
}

// イテレータの進行操作
namespace iml {

	// イテレータをnだけ進める
	template <class InputIterator, class Distance>
	inline constexpr void advance_impl(InputIterator& i, Distance n, input_iterator_tag) {
		if (n < 0) throw std::domain_error("negative advance of non-bidirectional iterator.");

		for (; 0 < n; --n) { ++i; }
	}
	template <class BidirectionalIterator, class Distance>
	inline constexpr void advance_impl(BidirectionalIterator& i, Distance n, bidirectional_iterator_tag) {
		if (n > 0) for (; 0 < n; --n) { ++i; }
		else for (; n < 0; ++n) { --i; }
	}
	template <class RandomAccessIterator, class Distance>
	inline constexpr void advance_impl(RandomAccessIterator& i, Distance n, random_access_iterator_tag) {
		i += n;
	}
	template <class Iterator, class Distance>
	inline constexpr void advance(Iterator& i, Distance n) {
		advance_impl(i, n, typename iterator_traits<Iterator>::iterator_category());
	}


	// イテレータ間の距離を求める
	template <class InputIterator>
	inline constexpr typename iterator_traits<InputIterator>::difference_type distance_impl(InputIterator first, InputIterator last, input_iterator_tag) {
		using result_type = typename iterator_traits<InputIterator>::difference_type;
		result_type n = 0;
		for (; first != last; ++first) ++n;
		return n;
	}
	template <class RandomAccessIterator>
	inline constexpr typename iterator_traits<RandomAccessIterator>::difference_type distance_impl(RandomAccessIterator first, RandomAccessIterator last, random_access_iterator_tag) {
		return last - first;
	}
	template <class InputIterator>
	inline constexpr typename iterator_traits<InputIterator>::difference_type distance(InputIterator first, InputIterator last) {
		return distance_impl(first, last, typename iterator_traits<InputIterator>::iterator_category());
	}


	// イテレータをn回進めたものを返す
	template <class InputIterator>
	inline constexpr InputIterator next(InputIterator x, typename iterator_traits<InputIterator>::difference_type n = 1) {
		advance(x, n); return x;
	}
	// イテレータを逆にn回進めたものを返す
	template <class BidirectionalIterator>
	inline constexpr BidirectionalIterator prev(BidirectionalIterator x, typename iterator_traits<BidirectionalIterator>::difference_type n = 1) {
		advance(x, -n); return x;
	}
}

//逆順イテレータ
namespace iml {

	// 逆順イテレータ
	template <class Iterator>
	class reverse_iterator {
	protected:
		Iterator current;		// ラップされたイテレータ
	public:
		using iterator_category = typename iterator_traits<Iterator>::iterator_category;
		using difference_type = typename iterator_traits<Iterator>::difference_type;
		using pointer = typename iterator_traits<Iterator>::pointer;
		using reference = typename iterator_traits<Iterator>::reference;
		using iterator_type = Iterator;

		constexpr reverse_iterator() : current() {}
		constexpr explicit reverse_iterator(Iterator v) : current(v) {}
		constexpr reverse_iterator(const reverse_iterator& ritr) : current(ritr.current) {}
		template<class U>
		constexpr reverse_iterator(const reverse_iterator<U>& ritr) : current(ritr.base()) {}

		constexpr reverse_iterator& operator=(const reverse_iterator& ritr) {
			current = ritr.current;
			return *this;
		}
		template<class U>
		constexpr reverse_iterator& operator=(const reverse_iterator<U>& ritr) {
			current = ritr.base();
			return *this;
		}

		//元のイテレータの取得
		constexpr Iterator base() const { return current; }

		constexpr reference operator*() const { Iterator temp = current; return *--temp; }
		constexpr pointer operator->() const { return (iml::pointer_traits<pointer>::pointer_to(**this)); }
		constexpr reverse_iterator& operator++() { --current; return *this; }
		constexpr reverse_iterator operator++(int) { reverse_iterator temp = *this; --current; return temp; }
		constexpr reverse_iterator& operator--() { ++current; return *this; }
		constexpr reverse_iterator operator--(int) { reverse_iterator temp = *this; ++current; return temp; }


		// ランダムアクセスイテレータのみ有効
		constexpr reverse_iterator& operator+=(difference_type n) { current -= n; return *this; }
		constexpr reverse_iterator& operator-=(difference_type n) { current += n; return *this; }
		constexpr reverse_iterator operator+(difference_type n) const { return reverse_iterator(current - n); }
		constexpr reverse_iterator operator-(difference_type n) const { return reverse_iterator(current + n); }
		constexpr reference operator[](difference_type index) const { return *(*this + index); }
	};
	template<class Iterator>
	inline constexpr reverse_iterator<Iterator> operator+(typename reverse_iterator<Iterator>::difference_type n, const reverse_iterator<Iterator>& ritr) {
		return ritr + n;
	}
	template<class Iterator1, class Iterator2>
	inline constexpr auto operator-(const reverse_iterator<Iterator1>& itr1, const reverse_iterator<Iterator2>& itr2) -> decltype(itr2.base() - itr1.base()) {
		return itr2.base() - itr1.base();
	}
	template<class Iterator1, class Iterator2>
	inline constexpr bool operator==(const reverse_iterator<Iterator1>& itr1, const reverse_iterator<Iterator2>& itr2) {
		return itr1.base() == itr2.base();
	}
	template<class Iterator1, class Iterator2>
	inline constexpr bool operator!=(const reverse_iterator<Iterator1>& itr1, const reverse_iterator<Iterator2>& itr2) {
		return !(itr1 == itr2);
	}
	template<class Iterator1, class Iterator2>
	inline constexpr bool operator<(const reverse_iterator<Iterator1>& itr1, const reverse_iterator<Iterator2>& itr2) {
		return itr2.base() < itr1.base();
	}
	template<class Iterator1, class Iterator2>
	inline constexpr bool operator>(const reverse_iterator<Iterator1>& itr1, const reverse_iterator<Iterator2>& itr2) {
		return itr2 < itr1;
	}
	template<class Iterator1, class Iterator2>
	inline constexpr bool operator<=(const reverse_iterator<Iterator1>& itr1, const reverse_iterator<Iterator2>& itr2) {
		return !(itr2 < itr1);
	}
	template<class Iterator1, class Iterator2>
	inline constexpr bool operator>=(const reverse_iterator<Iterator1>& itr1, const reverse_iterator<Iterator2>& itr2) {
		return !(itr1 < itr2);
	}
	// reverse_iteratorの構築
	template<class Iterator>
	inline constexpr reverse_iterator<Iterator> make_reverse_iterator(Iterator itr) {
		return reverse_iterator<Iterator>(itr);
	}
}

//間接参照先をムーブするイテレータ
namespace iml {

	// 間接参照先をムーブするイテレータ
	template<class Iterator>
	class move_iterator {
		using itr_ref = typename iterator_traits<Iterator>::reference;
	protected:
		Iterator current;			// ラップされたイテレータ
	public:
		using iterator_category = typename iterator_traits<Iterator>::iterator_category;
		using value_type = typename iterator_traits<Iterator>::value_type;
		using difference_type = typename iterator_traits<Iterator>::difference_type;
		using pointer = Iterator;
		using reference = conditional_t<is_reference_v<itr_ref>, remove_reference_t<itr_ref>&&, itr_ref>;
		using iterator_type = Iterator;

		constexpr move_iterator() : current() {}
		constexpr explicit move_iterator(Iterator itr) : current(itr) {}
		template<class U>
		constexpr move_iterator(const move_iterator<U>& itr) : current(itr.base()) {}

		constexpr move_iterator& operator=(const move_iterator& itr) { current = itr.current; return *this; }
		template<class U>
		constexpr move_iterator& operator=(const move_iterator<U>& itr) { current = itr.base(); return *this; }

		// ラップされているイテレータの取得
		constexpr Iterator base() const { return current; }

		constexpr reference operator*() const { return static_cast<reference>(*current); }
		constexpr pointer operator->() const { return current; }
		constexpr move_iterator& operator++() { ++current; return *this; }
		constexpr move_iterator operator++(int) { move_iterator temp = *this; ++current; return temp; }
		constexpr move_iterator& operator--() { --current; return *this; }
		constexpr move_iterator operator--(int) { move_iterator temp = *this; --current; return temp; }


		// ランダムアクセスイテレータのみ有効
		constexpr move_iterator& operator+=(difference_type n) { current += n; return *this; }
		constexpr move_iterator& operator-=(difference_type n) { current -= n; return *this; }
		constexpr move_iterator operator+(difference_type n) const { return move_iterator(current + n); }
		constexpr move_iterator operator-(difference_type n) const { return move_iterator(current - n); }
		constexpr difference_type operator-(const move_iterator& itr) const { return current - itr.base(); }
		constexpr reference operator[](difference_type n) const { return move(current[n]); }
	};
	template<class Iterator, class Distance>
	inline constexpr move_iterator<Iterator> operator+(Distance n, const move_iterator<Iterator>& itr) {
		return itr + n;
	}
	template<class Iterator1, class Iterator2>
	inline constexpr auto operator-(move_iterator<Iterator1>& itr1, const move_iterator<Iterator2>& itr2)-> decltype(itr1.base() - itr2.base()) {
		return itr1.base() - itr2.base();
	}
	template<class Iterator1, class Iterator2>
	inline constexpr bool operator==(const move_iterator<Iterator1>& itr1, const move_iterator<Iterator2>& itr2) {
		return itr1.base() == itr2.base();
	}
	template<class Iterator1, class Iterator2>
	inline constexpr bool operator!=(const move_iterator<Iterator1>& itr1, const move_iterator<Iterator2>& itr2) {
		return !(itr1 == itr2);
	}
	template<class Iterator1, class Iterator2>
	inline constexpr bool operator<(const move_iterator<Iterator1>& itr1, const move_iterator<Iterator2>& itr2) {
		return itr1.base() < itr2.base();
	}
	template<class Iterator1, class Iterator2>
	inline constexpr bool operator>(const move_iterator<Iterator1>& itr1, const move_iterator<Iterator2>& itr2) {
		return itr2 < itr1;
	}
	template<class Iterator1, class Iterator2>
	inline constexpr bool operator<=(const move_iterator<Iterator1>& itr1, const move_iterator<Iterator2>& itr2) {
		return !(itr2 < itr1);
	}
	template<class Iterator1, class Iterator2>
	inline constexpr bool operator>=(const move_iterator<Iterator1>& itr1, const move_iterator<Iterator2>& itr2) {
		return !(itr1 < itr2);
	}
	// move_iteratorの構築
	template<class Iterator>
	inline constexpr move_iterator<Iterator> make_move_iterator(Iterator itr) {
		return move_iterator<Iterator>(itr);
	}
}

// 挿入イテレータ
namespace iml {

	// 末尾に対する出力イテレータ
	template <class Container>
	class back_insert_iterator {
	protected:
		Container *cont;				// コンテナに対するポインタ
	public:
		using container_type = Container;
		using value_type = typename Container::value_type;
		using iterator_category = output_iterator_tag;
		using difference_type = void;
		using pointer = void;
		using reference = void;

		constexpr explicit back_insert_iterator(Container& c) : cont(addressof(c)) {}

		constexpr back_insert_iterator& operator=(const value_type& v) { cont->push_back(v); return *this; }
		constexpr back_insert_iterator& operator=(value_type&& v) { cont->push_back(forward<value_type>(v)); return *this; }

		constexpr back_insert_iterator& operator*() { return *this; }
		constexpr back_insert_iterator& operator++() { return *this; }
		constexpr back_insert_iterator operator++(int) { return *this; }
	};
	// back_insert_iteratorの構築
	template<class Container>
	inline constexpr back_insert_iterator<Container> back_inserter(Container& c) {
		return back_insert_iterator<Container>(c);
	}

	// 先頭に対する出力イテレータ
	template<class Container>
	class front_insert_iterator {
	protected:
		Container *cont;			// コンテナのポインタ
	public:
		using container_type = Container;
		using value_type = typename Container::value_type;
		using iterator_category = output_iterator_tag;
		using difference_type = void;
		using pointer = void;
		using reference = void;

		constexpr explicit front_insert_iterator(Container& c) : cont(addressof(c)) {}

		constexpr front_insert_iterator& operator=(const value_type& v) { cont->push_front(v); return *this; }
		constexpr front_insert_iterator& operator=(value_type&& v) { cont->push_front(forward<value_type>(v)); return *this; }

		constexpr front_insert_iterator& operator*() { return *this; }
		constexpr front_insert_iterator& operator++() { return *this; }
		constexpr front_insert_iterator operator++(int) { return *this; }
	};
	// front_insert_iteratorの構築
	template<class Container>
	inline constexpr front_insert_iterator<Container> front_inserter(Container& c) {
		return front_insert_iterator<Container>(c);
	}

	// 任意の位置に対する出力イテレータ
	template<class Container>
	class insert_iterator {
	protected:
		Container *cont;							//コンテナのポインタ
		typename Container::iterator iter;			//コンテナのイテレータ
	public:
		using container_type = Container;
		using value_type = typename Container::value_type;
		using iterator_category = output_iterator_tag;
		using difference_type = void;
		using pointer = void;
		using reference = void;

		constexpr insert_iterator(Container& c, typename Container::iterator itr) : cont(addressof(c)), iter(itr) {}

		constexpr insert_iterator& operator=(const value_type& v) { iter = cont->insert(iter, v); ++iter; return (*this); }
		constexpr insert_iterator& operator=(value_type&& v) { iter = cont->insert(iter, iml::forward<value_type>(v)); ++iter; return *this; }

		constexpr insert_iterator& operator*() { return *this; }
		constexpr insert_iterator& operator++() { return *this; }
		constexpr insert_iterator& operator++(int) { return *this; }
	};
	// insert_iteratorの構築
	template<class Container>
	inline constexpr insert_iterator<Container> inserter(Container& c, typename Container::iterator itr) {
		return insert_iterator<Container>(c, itr);
	}

}

// イテレータ位置
namespace iml {

	// 先頭イテレータ
	template<class Container>
	inline constexpr auto begin(Container& c) -> decltype(c.begin()) { return c.begin(); }
	template<class Container>
	inline constexpr auto begin(const Container& c) -> decltype(c.begin()) { return c.begin(); }
	template<class T, size_t N>
	inline constexpr T* begin(T(&a)[N]) noexcept { return a; }
	// 末尾イテレータ
	template<class Container>
	inline constexpr auto end(Container& c) -> decltype(c.end()) { return c.end(); }
	template<class Container>
	inline constexpr auto end(const Container& c) -> decltype(c.end()) { return c.end(); }
	template<class T, size_t N>
	inline constexpr T* end(T(&a)[N]) noexcept { return a + N; }


	// 先頭逆順イテレータ
	template<class Container>
	inline constexpr auto rbegin(Container& c) -> decltype(c.rbegin()) { return c.rbegin(); }
	template<class Container>
	inline constexpr auto rbegin(const Container& c) -> decltype(c.rbegin()) { return c.rbegin(); }
	template<class T, size_t N>
	inline constexpr reverse_iterator<T*> rbegin(T(&a)[N]) { return reverse_iterator<T*>(a + N); }
	// 末尾逆順イテレータ
	template<class Container>
	inline constexpr auto rend(Container& c) -> decltype(c.rend()) { return c.rend(); }
	template<class Container>
	inline constexpr auto rend(const Container& c) -> decltype(c.rend()) { return c.rend(); }
	template<class T, size_t N>
	inline constexpr reverse_iterator<T*> rend(T(&a)[N]) { return reverse_iterator<T *>(a); }


	// コンテナサイズ
	template<class Container>
	inline constexpr auto size(const Container& c) -> decltype(c.size()) { return c.size(); }
	template<class T, size_t N>
	inline constexpr size_t size(const T(&)[N]) noexcept { return N; }


	// 空の判定
	template<class Container>
	inline constexpr auto empty(const Container& c) -> decltype(c.empty()) { return c.empty(); }
	template<class T, size_t N>
	inline constexpr bool empty(const T(&)[N]) noexcept { return false; }


	// イテレータの判定(IteratorがIteratorTagなイテレータである)
	template <class Iterator, class IteratorTag>
	struct is_iterator : cat_bool<is_iterator_tag_v<IteratorTag> && is_base_of_v<IteratorTag, typename iterator_traits<Iterator>::iterator_category>> {};
	template <class Iterator, class IteratorTag>
	constexpr bool is_iterator_v = is_iterator<Iterator, IteratorTag>::value;

}

#endif