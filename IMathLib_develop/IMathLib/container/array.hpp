#ifndef _ARRAY_HPP
#define _ARRAY_HPP

#include "IMathLib/utility/algorithm.hpp"
#include "IMathLib/container/container.hpp"
#include "IMathLib/utility/smart_ptr.hpp"

//配列リストを扱うクラス

namespace iml {

	//配列リストのイテレータ
	template <class T>
	class array_iterator : public iterator_base<random_access_iterator_tag, T> {
		template <class> friend class array_iterator;

		using this_type = array_iterator<T>;

		T*	current;

	public:
		//直接データの取得
		T* _Ptr() { return current; }
		T* _Ptr() const { return current; }

		using iterator_category = random_access_iterator_tag;
		using value_type = T;
		using difference_type = ptrdiff_t;
		using pointer = T*;
		using reference = T&;

		template<class Other>
		struct rebind {
			using other = array_iterator<Other>;
		};

		constexpr array_iterator() :current(nullptr) {}
		constexpr array_iterator(T* itr) : current(itr) {}
		template <class S>
		constexpr array_iterator(const array_iterator<S>& itr) :current(reinterpret_cast<S*>(itr.current)) {}

		reference operator*() const { return *current; }
		pointer operator->() const { return current; }
		this_type& operator++() { ++current; return *this; }
		this_type operator++(int) { this_type temp = *this; ++*this; return temp; }
		this_type& operator--() { --current; return *this; }
		this_type operator--(int) { this_type temp = *this; --*this; return temp; }

		this_type& operator+=(difference_type n) { current += n; return *this; }
		this_type& operator-=(difference_type n) { current -= n; return *this; }
		this_type operator+(difference_type n) const { this_type temp = *this; return temp += n; }
		this_type operator-(difference_type n) const { this_type temp = *this; return temp -= n; }
		difference_type operator-(const this_type& itr) const { return current - itr.current; }

		reference operator[](difference_type n) const { return *next(*this, n); }

		bool operator==(const this_type& itr) const { return current == itr.current; }
		bool operator!=(const this_type& itr) const { return !(*this == itr); }
		bool operator<(const this_type& itr) const { return current < itr.current; }
		bool operator>(const this_type& itr) const { return itr < *this; }
		bool operator<=(const this_type& itr) const { return !(itr < *this); }
		bool operator>=(const this_type& itr) const { return !(*this < itr); }

		//operator T*() { return current; }
		//operator const T*() const { return current; }
	};

	//静的配列リスト用コンテナ
	template <class T, class Allocator>
	class static_array_container :public container_base<T, array_iterator<T>, Allocator> {
	public:
		using typename container_base<T, array_iterator<T>, Allocator>::value_type;
		using typename container_base<T, array_iterator<T>, Allocator>::iterator;
		using typename container_base<T, array_iterator<T>, Allocator>::const_iterator;
		using typename container_base<T, array_iterator<T>, Allocator>::allocator_type;
	protected:
		Allocator	_allo;				//アロケータ
		iterator	_p = nullptr;		//確保したメモリの先頭アドレス
		imsize_t	_size = 0;			//確保したメモリサイズ
		imsize_t	_use_size = 0;		//使用中のメモリサイズ

	public:
		constexpr static_array_container() {}
		virtual ~static_array_container() = 0 {}

		virtual iterator begin() noexcept = 0;
		virtual const_iterator begin() const noexcept = 0;
		virtual iterator end() noexcept = 0;
		virtual const_iterator end() const noexcept = 0;

		//使用中のサイズの取得
		imsize_t size() const { return _use_size; }

		//内部イテレータ
		template <class F>
		F for_each(F f) const { return iml::for_each(begin(), end(), f); }
	};

	//動的配列リスト用コンテナ
	template <class T, class Allocator>
	class dynamic_array_container : public static_array_container<T, Allocator> {
	protected:
		//xより大きい2の冪乗の取得
		static imsize_t get_power_of_2(imsize_t x) {
			imsize_t result = 0;
			for (imsize_t i = (sizeof(imsize_t) << 3); result < i; ++result)
				if ((x >> result) == 0) break;
			return (1 << result);
		}
	public:
		using static_array_container<T, Allocator>::static_array_container;
		virtual ~dynamic_array_container() {}

		using typename static_array_container<T, Allocator>::value_type;
		using typename static_array_container<T, Allocator>::iterator;
		using typename static_array_container<T, Allocator>::const_iterator;
		using typename static_array_container<T, Allocator>::allocator_type;

		virtual iterator begin() noexcept = 0;
		virtual const_iterator begin() const noexcept = 0;
		virtual iterator end() noexcept = 0;
		virtual const_iterator end() const noexcept = 0;

		//空かの判定
		virtual bool empty() const noexcept = 0;

		//メモリの解放
		void clear() { _allo.destroy(_p, _size); _size = _use_size = 0; _p = nullptr; }

		//要素の削除(posからn個)
		void erase(imsize_t pos = 0, imsize_t n = npos) {
			if (_use_size <= pos) return;
			copy_order(&this->_p[pos], &this->_p[(iml::min)(pos + n, _use_size)], &this->_p[_use_size]);
			_use_size -= (_use_size - pos <= n) ? (this->end() - (this->begin() + pos)) : n;
		}
		//itrの位置
		void erase(const_iterator itr) {
			if (!((this->begin() <= itr) || (itr < this->end()))) return;		//イテレータの範囲外
			copy_order(to_iterator(itr), itr + 1, &this->_p[_use_size]);
			--_use_size;
		}
		//[first,last)の範囲
		void erase(const_iterator first, const_iterator last) {
			if (!((this->begin() <= first) || (first < this->end()))) return;		//イテレータの範囲外
			if (first >= last) return;												//イテレータの順序
			copy_order(to_iterator(first), last, &this->_p[_use_size]);
			_use_size -= (last >= this->end()) ? (this->end() - first) : (last - first);
		}

		//要素の挿入(posに対してc)
		void insert(imsize_t pos, const T& c) {
			//配列シフトを必要としないとき
			if (_use_size == 0) {
				if (this->_size < this->_use_size + 1) this->reserve(get_power_of_2(this->_use_size + 1));
				this->_p[pos] = c;
				++this->_use_size;
				return;
			}
			if (_use_size < pos) return;
			if (this->_size < this->_use_size + 1) this->reserve(get_power_of_2(this->_use_size + 1));
			copy_reverse_order(&this->_p[pos + 1], &this->_p[pos], &this->_p[_use_size]);
			this->_p[pos] = c;
			++this->_use_size;
		}
		//posに対して[first,last)
		template <class InputIterator>
		void insert(imsize_t pos, InputIterator first, InputIterator last) {
			//入力イテレータでなければならない
			static_assert(is_base_of<input_iterator_tag, typename iterator_traits<InputIterator>::iterator_category>::value
				, "The type of iterator is different.");
			imsize_t size = distance(first, last);

			//配列シフトを必要としないとき
			if ((_use_size == 0) && (pos == 0)) {
				if (this->_size < size) this->reserve(get_power_of_2(size));
				copy_order(&this->_p[pos], first, last);
				this->_use_size = size;
				return;
			}
			if (_use_size < pos) return;
			if (this->_size < this->_use_size + size) this->reserve(get_power_of_2(this->_use_size + size));
			copy_reverse_order(&this->_p[pos + size], &this->_p[pos], &this->_p[_use_size]);
			copy_order(&this->_p[pos], first, last);
			this->_use_size += size;
		}
		//itrに対してc
		void insert(const_iterator itr, const T& c) {
			if (!((this->begin() <= itr) && (itr <= this->end()))) return;		//イテレータの範囲外
			if (this->_size < this->_use_size + 1) this->reserve(get_power_of_2(this->_use_size + 1));
			copy_reverse_order(to_iterator(itr), itr + 1, &this->_p[_use_size]);
			*to_iterator(itr) = c;
			++this->_use_size;
		}
		//itrに対して[first,last)
		template <class InputIterator>
		void insert(const_iterator itr, InputIterator first, InputIterator last) {
			//入力イテレータでなければならない
			static_assert(is_base_of<input_iterator_tag, typename iterator_traits<InputIterator>::iterator_category>::value
				, "The type of iterator is different.");
			insert(to_iterator(itr) - begin(), first, last);
		}

		//メモリの再確保
		void reserve(imsize_t size) {
			T* buf = _allo.create(size);
			copy_order(buf, _p, _use_size);
			_allo.destroy(_p, _size);
			_size = size;
			_p = buf;
		}
		void reserve(imsize_t size, const_iterator data, imsize_t data_size) {
			T* buf = _allo.create(size);
			copy_order(buf, data, (iml::min)(size, data_size));
			_allo.destroy(_p, _size);
			_size = size; _use_size = data_size;
			_p = buf;
		}
		void reserve(imsize_t size, const T* data, imsize_t data_size) {
			T* buf = _allo.create(size);
			copy_order(buf, data, data_size);
			_allo.destroy(_p, _size);
			_size = size; _use_size = data_size;
			_p = buf;
		}
		void reserve(imsize_t size, const dynamic_array_container& c) {
			T* buf = _allo.create(size);
			copy_order(buf, c._p, c._use_size);
			_allo.destroy(_p, _size);
			_size = size; _use_size = c._use_size;
			_p = buf;
		}
		//メモリの再設定
		void resize(imsize_t size) {
			//確保されているメモリが十分に大きいときはそのまま
			if (_size >= size) {
				_use_size = size;
				return;
			}
			T* temp = _allo.create(size);
			copy_order(temp, _p, _use_size);
			_allo.destroy(_p, _size);
			_size = _use_size = size;
			_p = temp;
		}
		//後方にデータの挿入
		virtual void push_back(const T& data) = 0;
		//後方のデータの削除
		virtual void pop_back() = 0;
		//メモリを使用サイズにフィットさせる
		void shrink_to_fit() {
			if (_size == _use_size) return;
			this->resize(_use_size);
		}
	};

	//固定長アロケータ(static_array専用)
	template <class T, class Iterator, imsize_t N>
	class static_allocator : public allocator_base<T, Iterator> {
		using typename allocator_base<T, Iterator>::_Pointer;
		using typename allocator_base<T, Iterator>::_Value_type;

		_Value_type		_array[N];
		bool			_use[N] = {};		//メモリの使用状態
	public:
		static_allocator() {}
		static_allocator(static_allocator& allocator) {}
		template <class U>
		static_allocator(static_allocator<U, Iterator, N>& allocator) {}
		~static_allocator() {}

		//メモリ確保
		_Pointer allocate(imsize_t n) {
			return _array;
			for (imsize_t i = 0, j = 0; i < N; ++i) {
				j = (_use[i]) ? 0 : j + 1;
				if (j == n) return &_array[i - j];
			}
			return nullptr;
		}
		//メモリ解放
		void deallocate(Iterator p, imsize_t n) {}
	};
	//比較演算
	template <class T, class U, class Iterator1, class Iterator2, imsize_t N>
	bool operator==(const static_allocator<T, Iterator1, N>&, const static_allocator<U, Iterator2, N>&) {
		return true;
	}
	template <class T, class U, class Iterator1, class Iterator2, imsize_t N>
	bool operator!=(const static_allocator<T, Iterator1, N>&, const static_allocator<U, Iterator2, N>&) {
		return false;
	}

	//静的配列リスト
	template <class T, imsize_t N>
	class static_array : public static_array_container<T, static_allocator<T, array_iterator<T>, N>>, public value_list_input<array_iterator<T>> {
	public:
		static_array() {
			this->_p = this->_allo.create(N);
			this->_size = this->_use_size = N;
		}
		template <class InputIterator>
		static_array(InputIterator first, InputIterator last) {
			this->_p = this->_allo.create(N);
			this->_size = this->_use_size = N;
			imsize_t dis = distance(first, last);
			copy_order(this->_p, first, iml::min(N, dis));
		}
		static_array(const static_array& s) {
			this->_p = this->_allo.create(N);
			this->_size = this->_use_size = N;
			copy_order(this->_p, s._p, N);
		}
		static_array(static_array&& s) {
			this->_size = this->_use_size = N;
			*this = move(s);
		}
		~static_array() { _allo.destroy(_p, N); }

		using typename static_array_container<T, static_allocator<T, array_iterator<T>, N>>::value_type;
		using typename static_array_container<T, static_allocator<T, array_iterator<T>, N>>::iterator;
		using typename static_array_container<T, static_allocator<T, array_iterator<T>, N>>::const_iterator;
		using typename static_array_container<T, static_allocator<T, array_iterator<T>, N>>::allocator_type;

		//イテレータの取得
		iterator begin() noexcept { return _p; }
		const_iterator begin() const noexcept { return _p; }
		iterator end() noexcept { return &_p[N]; }
		const_iterator end() const noexcept { return &_p[N]; }

		//配列を同一要素で埋める
		void fill(const T& n) { iml::fill(begin(), end(), n); }

		//代入
		static_array& operator=(const static_array& s) {
			copy_order(this->_p, s._p, N);
			return *this;
		}
		static_array& operator=(static_array&& s) {
			if (s._p == this->_p) return *this;
			//this->_p = s._p;
			this->_allo = s._allo;
			return *this;
		}

		//コンテナの中身の入れ替え
		void swap(static_array& c) {
			for (iterator itr1 = this->_p, itr2 = c._p; itr1 != end(); ++itr1, ++itr2)
				iml::swap(*itr1, *itr2);
		}

		T& operator[](imsize_t index) noexcept { return _p[index]; }
		const T& operator[](imsize_t index) const noexcept { return _p[index]; }
	};

	//動的配列リスト
	template <class T, class Allocator = allocator<T, array_iterator<T>>>
	class dynamic_array : public dynamic_array_container<T, Allocator>, public value_list_input<array_iterator<T>> {
	public:
		dynamic_array() {}
		dynamic_array(imsize_t size) { this->reserve(size); }
		dynamic_array(const T* s, imsize_t size) { this->reserve(get_power_of_2(size), s, size); }
		dynamic_array(const dynamic_array& s) { *this = s; }
		~dynamic_array() { clear(); }

		using typename dynamic_array_container<T, Allocator>::value_type;
		using typename dynamic_array_container<T, Allocator>::iterator;
		using typename dynamic_array_container<T, Allocator>::const_iterator;
		using typename dynamic_array_container<T, Allocator>::allocator_type;

		//イテレータの取得
		iterator begin() noexcept { return _p; }
		const_iterator begin() const noexcept { return _p; }
		iterator end() noexcept { return &_p[_use_size]; }
		const_iterator end() const noexcept { return &_p[_use_size]; }

		//空かの判定
		bool empty() const noexcept { return ((_use_size == 0) || (_p == nullptr)); }

		//後方にデータの挿入
		void push_back(const T& data) {
			if (this->_size < this->_use_size + 1) this->reserve(get_power_of_2(this->_use_size + 1));
			this->_p[this->_use_size] = data;
			++this->_use_size;
			return;
		}
		//後方のデータの削除
		void pop_back() {
			if (_use_size == 0) return;
			--_use_size;
		}

		//代入
		dynamic_array& operator=(const dynamic_array& s) {
			if (s._p == this->_p) return *this;
			this->_allo = s._allo;
			if (this->_size < s._use_size) {
				this->reserve(get_power_of_2(s._use_size), s);
				return *this;
			}
			this->_use_size = s._use_size;
			copy_order(this->_p, s._p, this->_use_size);
			return *this;
		}
		dynamic_array& operator=(dynamic_array&& s) {
			if (s._p == this->_p) return *this;
			this->_allo = s._allo;
			this->_size = s._size;
			this->_use_size = s._use_size;
			this->_p = s._p;
			return *this;
		}

		//コンテナの中身の入れ替え
		void swap(dynamic_array& c) {
			iml::swap(*this, c);
		}

		T& operator[](imsize_t index) noexcept { return _p[index]; }
		const T& operator[](imsize_t index) const noexcept { return _p[index]; }
	};

	//シェア配列リスト
	template <class T, class Allocator = allocator<T, array_iterator<T>>>
	class shared_array : public dynamic_array_container<T, Allocator>, public value_list_input<array_iterator<T>> {
		shared_count<T, deallocator<T>::function>	sc;			//シェアカウンタ(clearで解放のためハンドルを渡す必要はない)
	public:
		shared_array() :sc(nullptr, [&](T*) {clear(); }) {}
		shared_array(imsize_t size) :sc(nullptr, [&](T*) {clear(); }) { this->reserve(size); }
		shared_array(const T* s, imsize_t size) :sc(nullptr, [&](T*) {clear(); }) { this->reserve(get_power_of_2(size), s, size); }
		shared_array(const shared_array& s) :sc(nullptr, [&](T*) {clear(); }) { *this = s; }
		~shared_array() {}

		using value_type = T;
		using iterator = array_iterator<T>;
		using const_iterator = array_iterator<const T>;

		//イテレータの取得
		iterator begin() noexcept { return _p; }
		const_iterator begin() const noexcept { return _p; }
		iterator end() noexcept { return &_p[_use_size]; }
		const_iterator end() const noexcept { return &_p[_use_size]; }

		//空かの判定
		bool empty() const noexcept { return ((_use_size == 0) || (_p == nullptr)); }

		//後方にデータの挿入
		void push_back(const T& data) {
			if (this->_size < this->_use_size + 1) this->reserve(get_power_of_2(this->_use_size + 1));
			this->_p[this->_use_size] = data;
			++this->_use_size;
			return;
		}
		//後方のデータの削除
		void pop_back() {
			if (_use_size == 0) return;
			--_use_size;
		}

		//代入
		shared_array& operator=(const shared_array& s) {
			if (s._p == this->_p) return *this;
			this->sc = s.sc;
			this->_allo = s._allo;
			this->_size = s._size;
			this->_use_size = s._use_size;
			this->_p = s._p;
			return *this;
		}
		shared_array& operator=(shared_array&& s) {
			if (s._p == this->_p) return *this;
			this->sc = s.sc;
			this->_allo = s._allo;
			this->_size = s._size;
			this->_use_size = s._use_size;
			this->_p = s._p;
			return *this;
		}

		//コンテナの中身の入れ替え
		void swap(shared_array& c) {
			iml::swap(*this, c);
		}

		//インスタンスが唯一かの判定
		bool unique() const { return sc.unique(); }
		//カウントの取得
		imsize_t use_count() const { return sc.use_count(); }

		T& operator[](imsize_t index) noexcept { return _p[index]; }
		const T& operator[](imsize_t index) const noexcept { return _p[index]; }
	};
}

namespace iml {

	//配列の判定の登録
	template<class T, imsize_t N>
	struct is_array<static_array<T, N>> : true_type {};
	template<class T, class Allocator>
	struct is_array<dynamic_array<T, Allocator>> : true_type {};

}

#endif