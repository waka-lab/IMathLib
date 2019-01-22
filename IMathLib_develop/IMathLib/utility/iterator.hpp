#ifndef _ITERATOR_HPP
#define _ITERATOR_HPP

//イテレータの実装


#include "IMathLib/utility/utility.hpp"

//ポインタ情報
namespace iml {

	template<class T>
	struct _Get_first_parameter;
	template<template<class, class...> class T, class First, class... Rest>
	struct _Get_first_parameter<T<First, Rest...>> {
		using type = First;
	};

	template<class _Newfirst, class _Ty>
	struct _Replace_first_parameter;
	template<class _Newfirst, template<class, class...> class _Ty, class _First, class... _Rest>
	struct _Replace_first_parameter<_Newfirst, _Ty<_First, _Rest...>> {
		using type = _Ty<_Newfirst, _Rest...>;
	};

	template <class T>
	struct _Get_element_type {
		template <class S>
		static auto tester(int)->_Identity<typename S::element_type> {}
		template <class S>
		static auto tester(_Wrap_int)->_Identity<typename _Get_first_parameter<S>::type> {}

		using Decltype = decltype(tester<T>(0));			//戻り値型が欲しいだけ
		using type = typename Decltype::type;
	};
	template <class T>
	struct _Get_ptr_difference_type {
		template <class S>
		static auto tester(int)->_Identity<typename S::difference_type> {}
		template <class S>
		static auto tester(_Wrap_int)->_Identity<ptrdiff_t> {}

		using Decltype = decltype(tester<T>(0));			//戻り値型が欲しいだけ
		using type = typename Decltype::type;
	};
	template <class T, class _Other>
	struct _Get_rebind_type {
		template <class S>
		static auto tester(int)->_Identity<typename S::template rebind<_Other>::other> {}
		template <class S>
		static auto tester(_Wrap_int)->_Identity<typename _Replace_first_parameter<_Other, S>::type> {}

		using Decltype = decltype(tester<T>(0));			//戻り値型が欲しいだけ
		using type = typename Decltype::type;
	};

	//ポインタとみなせる型の情報
	//スマートポインター等
	template<class T>
	struct pointer_traits {
		using element_type = typename _Get_element_type<T>::type;			//ポインタが示す要素型
		using pointer = T;													//ポインタとみなせる型
		using difference_type = typename _Get_ptr_difference_type<T>::type;	//ポインタの差を示す符号あり整数型

		template<class Other>
		using rebind = typename _Get_rebind_type<T, Other>::type;			//型の再束縛(rebind<U>をもっているとき)

		using _Reftype = typename conditional<is_void<element_type>::value,
			char&, typename add_lvalue_reference<element_type>::type>::type;		//参照型
																					//変数へのポインタの取得
		static pointer pointer_to(_Reftype v) {
			return (T::pointer_to(v));
		}
	};
	//通常のポインター
	template<class T>
	struct pointer_traits<T*> {
		using element_type = T;
		using pointer = T*;
		using difference_type = ptrdiff_t;

		template<class Other>
		using rebind = Other*;

		using _Reftype = typename conditional<is_void<T>::value,
			char&, typename add_lvalue_reference<T>::type>::type;

		//変数へのポインタの取得
		static pointer pointer_to(_Reftype v) {
			//生の参照をポインタに変換する
			return addressof(v);
		}
	};

	//任意の配列やポインタを1次元の先頭ポインタにキャスト
	template <class, bool>
	struct _Pointer_cast;
	template <class T>
	struct _Pointer_cast<T*, false> {
		template <class = typename enable_if<!is_pointer<T>::value>::type>
		static constexpr T* pointer_cast(T* p) {
			return p;
		}
	};
	template <class T>
	struct _Pointer_cast<T*, true> {
		static constexpr auto pointer_cast(T* p) {
			return _Pointer_cast<typename remove_extent<T>::type*, is_array<typename remove_extent<T>::type>::value>::pointer_cast(&(*p)[0]);
		}
	};
	template <class T, bool f>
	struct _Pointer_cast<T**, f> {
		static constexpr auto pointer_cast(T** p) {
			return _Pointer_cast<T*, is_array<T>::value>::pointer_cast(*p);
		}
	};
	template <class T>
	constexpr auto pointer_cast(T* p) { return _Pointer_cast<T*, is_array<T>::value>::pointer_cast(p); }

}

//イテレータ情報
namespace iml {

	//イテレータの識別
	//入力イテレータ
	struct input_iterator_tag {};
	//出力イテレータ
	struct output_iterator_tag {};
	//前方向イテレータ
	struct forward_iterator_tag : input_iterator_tag, output_iterator_tag {};
	//双方向イテレータ
	struct bidirectional_iterator_tag : forward_iterator_tag {};
	//ランダムアクセスイテレータ
	struct random_access_iterator_tag : bidirectional_iterator_tag {};

	//一般的なケース、特別な最適化なし
	struct _General_ptr_iterator_tag {};
	//iteratorがコピー可能な型へのポインタ
	struct _Trivially_copyable_ptr_iterator_tag : _General_ptr_iterator_tag {};
	//iteratorは自明な型へのポインタ
	struct _Really_trivial_ptr_iterator_tag : _Trivially_copyable_ptr_iterator_tag {};
	//一般的なフォールバック/デフォルト/タグディスパッチの「その他の」ターゲット
	struct _Any_tag {
		constexpr _Any_tag() noexcept = default;
		template<class T>
		constexpr _Any_tag(T&&) noexcept {}
	};

	//voidのとき
	template<class, class = void>
	struct _Iterator_traits_base {};
	//Iter::*型が存在するとき(iterator等のとき)
	template<class Iter>
	struct _Iterator_traits_base<Iter, void_t<
		typename Iter::iterator_category,
		typename Iter::value_type,
		typename Iter::difference_type,
		typename Iter::pointer,
		typename Iter::reference>> {
		using iterator_category = typename Iter::iterator_category;	//イテレータの分類
		using value_type = typename Iter::value_type;				//イテレータの型
		using difference_type = typename Iter::difference_type;		//イテレータの差
		using pointer = typename Iter::pointer;						//ポインタ
		using reference = typename Iter::reference;					//参照
	};
	//イテレータに関する型情報の取得
	template<class Iter>
	struct iterator_traits: _Iterator_traits_base<Iter>{};
	template<class T>
	struct iterator_traits<T*> {
		using iterator_category = random_access_iterator_tag;	//イテレータの分類
		using value_type = typename remove_cv<T>::type;			//イテレータの型
		using difference_type = ptrdiff_t;						//イテレータの差

		using pointer = T*;										//ポインタ
		using reference = T&;									//参照
	};
	template<class T>
	struct iterator_traits<const T*>{
		using iterator_category = random_access_iterator_tag;	//イテレータの分類
		using value_type = T;									//イテレータの型
		using difference_type = ptrdiff_t;						//イテレータの差

		using pointer = const T*;								//ポインタ
		using reference = const T&;								//参照
	};

	//イテレータの基底(イテレータは事実上無限に進めることができなければならない)
	template<class Category, class T, class Distance = ptrdiff_t, class Pointer = T*, class Reference = T&>
	struct iterator_base {
		using iterator_category = Category;			//イテレータの分類
		using value_type = T;						//イテレータの型
		using difference_type = Distance;			//イテレータの差

		using pointer = Pointer;					//ポインタ
		using reference = Reference;				//参照
	};
	//出力イテレータの基底
	using _Output_itr = iterator_base<output_iterator_tag, void, void, void, void>;

	template<class T, class = void>
	struct _Is_checked_helper : false_type {};
	//typename T::_Unchecked_typeが存在するとき
	template<class T>
	struct _Is_checked_helper<T, void_t<typename T::_Unchecked_type>> : true_type {};
	//イテレータがチェックされているならば戻り値はtrue_type
	template<class _Iter>
	inline typename _Is_checked_helper<_Iter>::type _Is_checked(_Iter) { return{}; }
}

//イテレータの進行操作
namespace iml {

	template <class InputIterator, class Distance>
	inline void _Advance(InputIterator& i, Distance n, input_iterator_tag) {
		//assert(n >= 0);
		//n<0のとき例外を投げる
		for (; 0 < n; --n) { ++i; }
	}
	template <class BidirectionalIterator, class Distance>
	inline void _Advance(BidirectionalIterator& i, Distance n, bidirectional_iterator_tag) {
		if (n > 0) for (; 0 < n; --n) { ++i; }
		else for (; n < 0; ++n) { --i; }
	}
	template <class RandomAccessIterator, class Distance>
	inline void _Advance(RandomAccessIterator& i, Distance n, random_access_iterator_tag) {
		i += n;
	}
	//イテレータを進める(出力イテレータ以外)
	template <class Iterator, class Distance>
	inline void advance(Iterator& i, Distance n) {
		//イテレータカテゴリごとに分岐
		_Advance(i, n, typename iterator_traits<Iterator>::iterator_category());
	}

	template <class InputIterator>
	inline typename iterator_traits<InputIterator>::difference_type _Distance(InputIterator first, InputIterator last, input_iterator_tag) {
		using result_type = typename iterator_traits<InputIterator>::difference_type;
		result_type n = 0;
		for (; first != last; ++first) ++n;
		return n;
	}
	template <class RandomAccessIterator>
	inline typename iterator_traits<RandomAccessIterator>::difference_type _Distance(RandomAccessIterator first, RandomAccessIterator last, random_access_iterator_tag) {
		return last - first;
	}
	//イテレータ間の距離を求める
	template <class InputIterator>
	inline typename iterator_traits<InputIterator>::difference_type distance(InputIterator first, InputIterator last) {
		//イテレータカテゴリごとに分岐
		return _Distance(first, last, typename iterator_traits<InputIterator>::iterator_category());
	}

	//イテレータをn回進めたものを返す
	template <class InputIterator>
	inline InputIterator next(InputIterator x, typename iterator_traits<InputIterator>::difference_type n = 1) {
		advance(x, n);
		return x;
	}
	//イテレータを逆にn回進めたものを返す
	template <class BidirectionalIterator>
	inline BidirectionalIterator prev(BidirectionalIterator x, typename iterator_traits<BidirectionalIterator>::difference_type n = 1) {
		advance(x, -n);
		return x;
	}

	template <class RandomAccessIterator>
	inline bool _Lt(RandomAccessIterator itr1, RandomAccessIterator itr2, random_access_iterator_tag) {
		return itr1 < itr2;
	}
	//イテレータ位置の小なり(itr1 < itr2)
	template <class InputIterator>
	inline bool lt(InputIterator itr1, InputIterator itr2) {

	}
}

//逆順イテレータ
namespace iml {

	//逆順イテレータ
	template <class Iterator>
	class reverse_iterator
		: public iterator_base<typename iterator_traits<Iterator>::iterator_category,
		typename iterator_traits<Iterator>::value_type,
		typename iterator_traits<Iterator>::difference_type,
		typename iterator_traits<Iterator>::pointer,
		typename iterator_traits<Iterator>::reference> {

		using this_type = reverse_iterator<Iterator>;

	public:
		using difference_type = typename iterator_traits<Iterator>::difference_type;
		using pointer = typename iterator_traits<Iterator>::pointer;
		using reference = typename iterator_traits<Iterator>::reference;
		using iterator_type = Iterator;
	protected:
		Iterator current;		//ラップされたイテレータ
	public:
		reverse_iterator() : current() {}
		explicit reverse_iterator(Iterator v) : current(v) {}
		template<class S>
		reverse_iterator(const reverse_iterator<S>& ritr) : current(ritr.base()) {}

		template<class S>
		this_type& operator=(const reverse_iterator<S>& ritr) {
			current = ritr.base();
			return *this;
		}

		//元のイテレータの取得
		Iterator base() const { return current; }

		reference operator*() const { Iterator temp = current; return *--temp; }
		pointer operator->() const { return (iml::pointer_traits<pointer>::pointer_to(**this)); }
		this_type& operator++() { --current; return *this; }
		this_type operator++(int) { this_type temp = *this; --current; return temp; }
		this_type& operator--() { ++current; return *this; }
		this_type operator--(int) { this_type temp = *this; ++current; return temp; }

		//N.B.この点を超えてのみランダムアクセスイテレータに有効な関数

		this_type& operator+=(difference_type n) { current -= n; return *this; }
		this_type& operator-=(difference_type n) { current += n; return *this; }
		this_type operator+(difference_type n) const { return this_type(current - n); }
		this_type operator-(difference_type n) const { return this_type(current + n); }

		reference operator[](difference_type index) const { return (*(*this + index)); }
	};
	template<class Iterator>
	inline reverse_iterator<Iterator> operator+(typename reverse_iterator<Iterator>::difference_type n, const reverse_iterator<Iterator>& ritr) {
		return ritr + n;
	}
	template<class Iterator1, class Iterator2>
	inline auto operator-(const reverse_iterator<Iterator1>& itr1, const reverse_iterator<Iterator2>& itr2)-> decltype(itr2.base() - itr1.base()) {
		return itr2.base() - itr1.base();
	}
	template<class Iterator1, class Iterator2>
	inline bool operator==(const reverse_iterator<Iterator1>& itr1, const reverse_iterator<Iterator2>& itr2) {
		return itr1.base() == itr2.base();
	}
	template<class Iterator1, class Iterator2>
	inline bool operator!=(const reverse_iterator<Iterator1>& itr1, const reverse_iterator<Iterator2>& itr2) {
		return !(itr1 == itr2);
	}
	template<class Iterator1, class Iterator2>
	inline bool operator<(const reverse_iterator<Iterator1>& itr1, const reverse_iterator<Iterator2>& itr2) {
		return itr2.base() < itr1.base();
	}
	template<class Iterator1, class Iterator2>
	inline bool operator>(const reverse_iterator<Iterator1>& itr1, const reverse_iterator<Iterator2>& itr2) {
		return itr2 < itr1;
	}
	template<class Iterator1, class Iterator2>
	inline bool operator<=(const reverse_iterator<Iterator1>& itr1, const reverse_iterator<Iterator2>& itr2) {
		return !(itr2 < itr1);
	}
	template<class Iterator1, class Iterator2>
	inline bool operator>=(const reverse_iterator<Iterator1>& itr1, const reverse_iterator<Iterator2>& itr2) {
		return !(itr1 < itr2);
	}
	template<class Iterator>
	struct _Is_checked_helper<reverse_iterator<Iterator>> : public _Is_checked_helper<Iterator> {};
	//reverse_iteratorの構築
	template<class Iterator>
	inline reverse_iterator<Iterator> make_reverse_iterator(Iterator itr) {
		return reverse_iterator<Iterator>(itr);
	}
}

//間接参照先をムーブするイテレータ
namespace iml {

	//間接参照先をムーブするイテレータ
	template<class Iterator>
	class move_iterator {
		using this_type = move_iterator<Iterator>;
	protected:
		Iterator current;			//ラップされたイテレータ
	public:
		using iterator_category = typename iterator_traits<Iterator>::iterator_category;
		using value_type = typename iterator_traits<Iterator>::value_type;
		using difference_type = typename iterator_traits<Iterator>::difference_type;
		using pointer = Iterator;
		using _Ref0 = typename iterator_traits<Iterator>::reference;
		using reference = typename conditional<is_reference<_Ref0>::value,
			typename remove_reference<_Ref0>::type&&, _Ref0>::type;
		using iterator_type = Iterator;

		move_iterator() : current() {}
		explicit move_iterator(iterator_type itr) : current(itr) {}
		template<class Iterator2>
		move_iterator(const move_iterator<Iterator2>& itr) : current(itr.base()) {}

		template<class Iterator2>
		this_type& operator=(const move_iterator<Iterator2>& itr) { current = itr.base(); return *this; }

		//ラップされているイテレータの取得
		Iterator base() const { return current; }

		reference operator*() const { return static_cast<reference>(*current); }
		pointer operator->() const { return current; }
		this_type& operator++() { ++current; return *this; }
		this_type operator++(int) { this_type temp = *this; ++current; return temp; }
		this_type& operator--() { --current; return *this; }
		this_type operator--(int) { this_type temp = *this; --current; return temp; }

		//N.B.のイテレータこの点を越えてのみ、ランダムアクセスイテレータに対して有効な関数

		this_type& operator+=(difference_type n) { current += n; return *this; }
		this_type& operator-=(difference_type n) { current -= n; return *this; }
		this_type operator+(difference_type n) const { return this_type(current + n); }
		this_type operator-(difference_type n) const { return this_type(current - n); }
		difference_type operator-(const this_type& itr) const { return current - itr.base(); }

		reference operator[](difference_type n) const { return (iml::move(current[n])); }


		template<class Iterator2>
		bool _Equal(const move_iterator<Iterator2>& itr) const {
			return (current == itr.base());
		}
		template<class Iterator2>
		bool _Less(const move_iterator<Iterator2>& itr2) const
		{	// test if this < itr2
			return (current < itr2.base());
		}

	};
	template<class Iterator, class Distance>
	inline move_iterator<Iterator> operator+(Distance n, const move_iterator<Iterator>& itr) {
		return itr + n;
	}
	template<class Iterator1, class Iterator2>
	inline auto operator-(move_iterator<Iterator1>& itr1, const move_iterator<Iterator2>& itr2)-> decltype(itr1.base() - itr2.base()) {
		return itr1.base() - itr2.base();
	}
	template<class Iterator1, class Iterator2>
	inline bool operator==(const move_iterator<Iterator1>& itr1, const move_iterator<Iterator2>& itr2) {
		return itr1.base() == itr2.base();
	}
	template<class Iterator1, class Iterator2>
	inline bool operator!=(const move_iterator<Iterator1>& itr1, const move_iterator<Iterator2>& itr2) {
		return !(itr1 == itr2);
	}
	template<class Iterator1, class Iterator2>
	inline bool operator<(const move_iterator<Iterator1>& itr1, const move_iterator<Iterator2>& itr2) {
		return itr1.base() < itr2.base();
	}
	template<class Iterator1, class Iterator2>
	inline bool operator>(const move_iterator<Iterator1>& itr1, const move_iterator<Iterator2>& itr2) {
		return itr2 < itr1;
	}
	template<class Iterator1, class Iterator2>
	inline bool operator<=(const move_iterator<Iterator1>& itr1, const move_iterator<Iterator2>& itr2) {
		return !(itr2 < itr1);
	}
	template<class Iterator1, class Iterator2>
	inline bool operator>=(const move_iterator<Iterator1>& itr1, const move_iterator<Iterator2>& itr2) {
		return !(itr1 < itr2);
	}
	template<class Iterator>
	struct _Is_checked_helper<move_iterator<Iterator>> : public _Is_checked_helper<Iterator> {};
	//make_move_iteratorの構築
	template<class Iterator>
	inline move_iterator<Iterator> make_move_iterator(Iterator itr) {
		return move_iterator<Iterator>(itr);
	}
}

//挿入イテレータ
namespace iml {

	//末尾に対する出力イテレータ
	template<class Container>
	class back_insert_iterator : public _Output_itr {
		using this_type = back_insert_iterator<Container>;
	protected:
		Container *cont;				//コンテナに対するポインタ
	public:
		using container_type = Container;
		using value_type = typename Container::value_type;

		explicit back_insert_iterator(Container& c) : cont(std::addressof(c)) {}

		this_type& operator=(const value_type& v) { cont->push_back(v); return *this; }
		this_type& operator=(value_type&& v) { cont->push_back(iml::forward<value_type>(v)); return *this; }

		this_type& operator*() { return *this; }
		this_type& operator++() { return *this; }
		this_type operator++(int) { return *this; }
	};
	template<class Container>
	struct _Is_checked_helper<back_insert_iterator<Container>> : public true_type {};
	//back_insert_iteratorの構築
	template<class Container>
	inline back_insert_iterator<Container> back_inserter(Container& c) {
		return back_insert_iterator<Container>(c);
	}

	//先頭に対する出力イテレータ
	template<class Container>
	class front_insert_iterator : public _Output_itr {
		using this_type = front_insert_iterator<Container>;
	protected:
		Container *cont;		//コンテナのポインタ
	public:
		using container_type = Container;
		using value_type = typename Container::value_type;

		explicit front_insert_iterator(Container& c) : cont(std::addressof(c)) {}

		this_type& operator=(const value_type& v) { cont->push_front(v); return *this; }
		this_type& operator=(value_type&& v) { cont->push_front(iml::forward<value_type>(v)); return *this; }

		this_type& operator*() { return *this; }
		this_type& operator++() { return *this; }
		this_type operator++(int) { return *this; }
	};
	template<class Container>
	struct _Is_checked_helper<front_insert_iterator<Container>> : public true_type {};
	//front_insert_iteratorの構築
	template<class Container>
	inline front_insert_iterator<Container> front_inserter(Container& c) {
		return front_insert_iterator<Container>(c);
	}

	//任意の位置に対する出力イテレータ
	template<class Container>
	class insert_iterator: public _Output_itr{
		using this_type = insert_iterator<Container>;
	protected:
		Container *cont;							//コンテナのポインタ
		typename Container::iterator iter;			//コンテナのイテレータ
	public:
		using container_type = Container;
		using value_type = typename Container::value_type;

		insert_iterator(Container& c, typename Container::iterator itr) : cont(std::addressof(c)), iter(itr) {}

		this_type& operator=(const value_type& v) { iter = cont->insert(iter, v); ++iter; return (*this); }
		this_type& operator=(value_type&& v) { iter = cont->insert(iter, iml::forward<value_type>(v)); ++iter; return *this; }

		this_type& operator*() { return *this; }
		this_type& operator++() { return *this; }
		this_type& operator++(int) { return *this; }
	};
	template<class Container>
	struct _Is_checked_helper<insert_iterator<Container>> : public true_type {};
	//insert_iteratorの構築
	template<class Container>
	inline insert_iterator<Container> inserter(Container& c, typename Container::iterator itr) {
		return insert_iterator<Container>(c, itr);
	}

}

//イテレータ位置
namespace iml {

	//先頭イテレータ
	template<class Container>
	inline auto begin(Container& c) -> decltype(c.begin()) { return c.begin(); }
	template<class Container>
	inline auto begin(const Container& c) -> decltype(c.begin()) { return c.begin(); }
	template<class T, imsize_t N>
	inline constexpr T* begin(T(&a)[N]) noexcept { return a; }
	//末尾イテレータ
	template<class Container>
	inline auto end(Container& c) -> decltype(c.end()) { return c.end(); }
	template<class Container>
	inline auto end(const Container& c) -> decltype(c.end()) { return c.end(); }
	template<class T, imsize_t N>
	inline constexpr T* end(T(&a)[N]) noexcept { return a + N; }

	//先頭逆順イテレータ
	template<class Container>
	inline auto rbegin(Container& c) -> decltype(c.rbegin()) { return c.rbegin(); }
	template<class Container>
	inline auto rbegin(const Container& c) -> decltype(c.rbegin()) { return c.rbegin(); }
	template<class T, imsize_t N>
	inline reverse_iterator<T *> rbegin(T(&a)[N]) { return reverse_iterator<T*>(a + N); }
	//末尾逆順イテレータ
	template<class Container>
	inline auto rend(Container& c) -> decltype(c.rend()) { return c.rend(); }
	template<class Container>
	inline auto rend(const Container& c) -> decltype(c.rend()) { return c.rend(); }
	template<class T, imsize_t N>
	inline reverse_iterator<T*> rend(T(&a)[N]) { return reverse_iterator<T *>(a); }

	//コンテナサイズ
	template<class Container>
	inline constexpr auto size(const Container& c)-> decltype(c.size()) { return c.size(); }
	template<class T, imsize_t N>
	inline constexpr imsize_t size(const T(&)[N]) noexcept { return N; }

	//空の判定
	template<class Container>
	inline constexpr auto empty(const Container& c) -> decltype(c.empty()) { return c.empty(); }
	template<class T, imsize_t N>
	inline constexpr bool empty(const T(&)[N]) noexcept { return false; }

	//iterator_tagの判定
	template <class IteratorTag>
	struct _Is_iterator_tag {
		static constexpr bool value =
			is_base_of<input_iterator_tag, IteratorTag>::value
			|| is_base_of<output_iterator_tag, IteratorTag>::value;
	};

	//イテレータの判定
	template <class Iterator, class IteratorTag>
	struct is_iterator {
		static constexpr bool value =
			_Is_iterator_tag<IteratorTag>::value
			&& is_base_of<IteratorTag, typename iterator_traits<Iterator>::iterator_category>::value;
	};
}

#endif