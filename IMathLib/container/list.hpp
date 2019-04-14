#ifndef _LIST_HPP
#define _LIST_HPP

#include "IMathLib/container/array.hpp"

//連結リスト

//双方向連結リスト
namespace iml {

	//双方向連結リストのイテレータ
	template <class T>
	class list_iterator
		: public iterator_base<bidirectional_iterator_tag, T> {
		template <class _T>
		friend class list_iterator;

		using this_type = list_iterator<T>;

	public:
		//双方向連結リストのセル
		struct cell {
			cell*	front;			//前のセル
			cell*	back;			//次のセル
			T		value;
			cell() :value{}, front(nullptr), back(nullptr) {}
			cell(const T& v) :value(v), front(nullptr), back(nullptr) {}
		};
	private:
		cell*	current;
	public:
		//直接データの取得
		cell* _Ptr() { return current; }
		cell* _Ptr() const { return current; }

		using iterator_category = bidirectional_iterator_tag;
		using value_type = T;
		using difference_type = ptrdiff_t;
		using pointer = T*;
		using reference = T&;

		template<class Other>
		struct rebind {
			using other = list_iterator<Other>;
		};

		constexpr list_iterator() :current(nullptr) {}
		constexpr list_iterator(cell* itr) : current(itr) {}
		template <class S>
		constexpr list_iterator(const list_iterator<S>& itr)
			: current(reinterpret_cast<cell*>(itr.current)) {}
		
		reference operator*() const { return current->value; }
		pointer operator->() const { return &current->value; }
		this_type& operator++() { current = current->back; return *this; }
		this_type operator++(int) { this_type temp = *this; ++*this; return temp; }
		this_type& operator--() { current = current->front; return *this; }
		this_type operator--(int) { this_type temp = *this; --*this; return temp; }

		reference operator[](difference_type n) const { return *next(*this, n); }
		
		bool operator==(const this_type& itr) const { return current == itr.current; }
		bool operator!=(const this_type& itr) const { return !(*this == itr); }

	};

	//双線形連結リストのコンテナ
	template <class T, class Allocator>
	class list_container :public container_base<T, list_iterator<T>, Allocator> {
	public:
		using typename container_base<T, list_iterator<T>, Allocator>::value_type;
		using typename container_base<T, list_iterator<T>, Allocator>::iterator;
		using typename container_base<T, list_iterator<T>, Allocator>::const_iterator;
		using typename container_base<T, list_iterator<T>, Allocator>::allocator_type;
	private:
		using this_type = list_container;
	protected:
		Allocator	_allo;				//アロケータ
		iterator	_header;			//一番最初の位置-1かつ一番最後の位置+1(基点となる位置)
		size_t	_size = 0;			//要素数

	public:
		list_container() :_header(_allo.create(1)) {
			_header._Ptr()->back = _header._Ptr()->front = _header._Ptr();
		}
		virtual ~list_container() = 0 {}

		virtual iterator begin() noexcept = 0;
		virtual const_iterator begin() const noexcept = 0;
		virtual iterator end() noexcept = 0;
		virtual const_iterator end() const noexcept = 0;

		//サイズの取得
		size_t size() const { return _size; }
		//コンテナの入れ替え
		void swap(const this_type& c) {
			iml::swap(this->_allo, c._allo);
			iml::swap(this->_size, c._size);
		}
		//前方探索
		size_t find(const T& key, size_t pos = 0) {
			if (_size <= pos) return npos;
			iterator temp = next(_header, pos + 1);
			for (size_t i = pos; i < _size; ++i, ++temp)
				if (*temp == key) return i;
			return npos;
		}
		//後方探索
		size_t rfind(const T& key, size_t pos = npos) {
			iterator temp = next(_header, (iml::min)(pos, this->_size) + 1);
			for (size_t i = (iml::min)(pos, this->_size); i != 0; --i, --temp)
				if (*temp == key) return i;
			return npos;
		}

		//空かの判定
		virtual bool empty() const noexcept = 0;

		//メモリの解放
		void clear() {
			for (iterator itr = begin(); itr != end();) _allo.destroy(itr++, 1);
			_size = 0;
		}

		//要素の削除
		void erase(const_iterator p) {
			to_iterator(p)._Ptr()->front->back = to_iterator(p)._Ptr()->back;
			to_iterator(p)._Ptr()->back->front = to_iterator(p)._Ptr()->front;
			--_size;
			_allo.destroy(to_iterator(p), 1);
		}
		void erase(const_iterator first, const_iterator last) {
			for (const_iterator itr = first; itr != last; ++itr)
				erase(itr);
		}

		//要素の挿入
		void insert(const_iterator p, const T& c) {
			iterator temp = _allo.create(1);
			temp._Ptr()->front = to_iterator(p)._Ptr()->front;
			temp._Ptr()->back = to_iterator(p)._Ptr();
			temp._Ptr()->value = c;
			to_iterator(p)._Ptr()->front->back = temp._Ptr();
			to_iterator(p)._Ptr()->front = temp._Ptr();
			++_size;
		}
		template <class InputIterator>
		void insert(const_iterator p, InputIterator first, InputIterator last) {
			//入力イテレータでなければならない
			static_assert(is_base_of<input_iterator_tag, typename iterator_traits<InputIterator>::iterator_category>::value
				, "The type of iterator is different.");

			for (InputIterator itr = first; itr != last; ++itr, ++p)
				insert(p, static_cast<T>(*itr));
		}

		//前後への挿入
		void push_front(const T& v) { insert(begin(), v); }
		void push_back(const T& v) { insert(end(), v); }
		void pop_front() { erase(begin()); }
		void pop_back() { erase(--end()); }

		//前後へのアクセス
		T& front() { return *begin(); }
		const T& front() const { return *begin(); }
		T& back() { return *--end(); }
		const T& back() const { return *--end(); }

		//内部イテレータ
		template <class F>
		F for_each(F f) const { return iml::for_each(begin(), end(), f); }
	};

	//双方向連結リスト
	template <class T, class Allocator = allocator<typename list_iterator<T>::cell, list_iterator<T>>>
	class list : public list_container<T, Allocator> {
	public:
		using typename list_container<T, Allocator>::value_type;
		using typename list_container<T, Allocator>::iterator;
		using typename list_container<T, Allocator>::const_iterator;
		using typename list_container<T, Allocator>::allocator_type;
	private:

	public:
		constexpr list() {}
		template <class InputIterator>
		list(InputIterator first, InputIterator last) { insert(begin(), first, last); }
		list(const list& list) { insert(begin(), list.begin(), list.end()); }
		~list() { clear(); _allo.destroy(_header, 1); }

		//イテレータの取得
		iterator begin() noexcept { return next(_header, 1); }
		const_iterator begin() const noexcept { return to_const_iterator(next(_header, 1)); }
		iterator end() noexcept { return _header; }
		const_iterator end() const noexcept { return to_const_iterator(_header); }

		bool empty() const noexcept { return _size == 0; }

		//代入
		list& operator=(const list& b) {
			if (this->_size > b._size) {
				//多い分を削除
				this->erase(next(this->begin(), this->_size - b._size), this->end());
				copy_order(this->begin(), b.begin(), b.end());
			}
			else {
				copy_order(this->begin(), b.begin(), b._size - this->_size);
				//新たにメモリを確保して追加
				this->insert(--this->end(), next(b.begin(), this->_size), b.end());
			}
			return *this;
		}
		list& operator=(list&& b) {
			if (this->_header == b._header) return *this;
			this->_header = b._header; b._header == nullptr;
			return *this;
		}

		//コンテナの中身の入れ替え
		void swap(list& c) {
			iml::swap(*this, c);
		}

		T& operator[](size_t index) noexcept { return (index < _size / 2) ? _header[index + 1] : *prev(_header, _size - index); }
		const T& operator[](size_t index) const noexcept { return (index < _size / 2) ? _header[index + 1] : *prev(_header, _size - index); }
	};

	//シェア双方向連結リスト
	template <class T, class Allocator = allocator<typename list_iterator<T>::cell, list_iterator<T>>>
	class shared_list : public list_container<T, Allocator> {
		shared_count<T, deallocator<T>::function>	sc;			//シェアカウンタ(clearで解放のためハンドルを渡す必要はない)
	public:
		constexpr shared_list() :sc(nullptr, [&](T*) {clear(); _allo.destroy(_header, 1);  }) {}
		template <class InputIterator>
		shared_list(InputIterator first, InputIterator last) : sc(nullptr, [&](T*) {clear(); _allo.destroy(_header, 1);  }) {
			insert(begin(), first, last);
		}
		shared_list(const shared_list& list) :sc(nullptr, [&](T*) {clear(); _allo.destroy(_header, 1);  }) {
			insert(begin(), list.begin(), list.end());
		}
		~shared_list() {}

		//イテレータの取得
		iterator begin() noexcept { return next(_header, 1); }
		const_iterator begin() const noexcept { return to_const_iterator(next(_header, 1)); }
		iterator end() noexcept { return _header; }
		const_iterator end() const noexcept { return to_const_iterator(_header); }

		bool empty() const noexcept { return _size == 0; }

		//代入
		shared_list& operator=(const shared_list& b) {
			if (this->_header == b._header) return *this;
			this->sc = b.sc;
			this->_size = b._size;
			this->_allo = b._allo;
			this->_header = b._header;
			return *this;
		}
		shared_list& operator=(shared_list&& b) {
			if (this->_header == b._header) return *this;
			this->sc = b.sc;
			this->_size = b._size;
			this->_allo = b._allo;
			this->_header = b._header;
			return *this;
		}

		//コンテナの中身の入れ替え
		void swap(shared_list& c) {
			iml::swap(*this, c);
		}

		//インスタンスが唯一かの判定
		bool unique() const { return sc.unique(); }
		//カウントの取得
		size_t use_count() const { return sc.use_count(); }

		T& operator[](size_t index) noexcept { return (index < _size / 2) ? _header[index + 1] : *prev(_header, _size - index); }
		const T& operator[](size_t index) const noexcept { return (index < _size / 2) ? _header[index + 1] : *prev(_header, _size - index); }
	};

}


#endif