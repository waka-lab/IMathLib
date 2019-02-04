#ifndef IMATH_CONTAINER_CONTAINER_HPP
#define IMATH_CONTAINER_CONTAINER_HPP

#include "IMathLib/utility/iterator.hpp"

//汎用的なアルゴリズムを提供するためのコンテナ等の実装

namespace iml {

	//アロケータの基底クラス
	template <class T, class Iterator>
	class allocator_base {
	protected:
		using _Value_type = T;				//確保される型
		using _Pointer = T*;

		//イテレータからイテレータの管理しているポインタ型への変換
		template <bool flag = is_same<_Pointer, Iterator>::value>
		static _Pointer to_pointer(Iterator itr) {
			return itr._Ptr();
		}
		template <>
		static _Pointer to_pointer<true>(Iterator itr) {
			return itr;
		}

		//_Pointer用のコンストラクタ
		template <class... Args>
		static void _Constructor(_Pointer p, Args... args) {
			::new ((void*)p) _Value_type(iml::forward<Args>(args)...);
		}
	public:
		allocator_base() {}
		~allocator_base() {}

		//メモリ確保
		virtual _Pointer allocate(imsize_t n) = 0;
		//メモリ解法
		virtual void deallocate(Iterator p, imsize_t n) = 0;

		//コンストラクタ呼び出し
		template <class... Args>
		static void constructor(Iterator p, Args... args) {
			::new ((void*)to_pointer(p)) _Value_type(iml::forward<Args>(args)...);
		}
		//デストラクタ呼び出し
		static void destructor(Iterator p) {
			to_pointer(p)->~_Value_type();
		}

		//オブジェクトの作成(メモリを確保しコンストラクタを呼び出す)
		template <class... Args>
		_Pointer create(imsize_t n, Args... args) {
			_Pointer temp = allocate(n);
			for (imsize_t i = 0; i < n; ++i) _Constructor(temp + i, args...);
			return temp;
		}
		//オブジェクトの破棄(デストラクタを呼び出しメモリを破棄する)
		void destroy(Iterator p, imsize_t n) {
			Iterator temp = p;
			if (n != 0) {
				for (imsize_t i = 0; i < n - 1; ++i) destructor(temp++);
				destructor(temp);
			}
			deallocate(p, n);
		}
	};
	//汎用アロケータクラス
	template <class T, class Iterator = T*>
	class allocator : public allocator_base<T, Iterator> {
		using typename allocator_base<T, Iterator>::_Pointer;
		using typename allocator_base<T, Iterator>::_Value_type;
	public:
		allocator() {}
		allocator(allocator& allocator) {}
		template <class U>
		allocator(allocator<U, Iterator>& allocator) {}
		~allocator() {}

		//メモリ確保
		_Pointer allocate(imsize_t n) {
			//newのオーバーロード対策
			return static_cast<_Pointer>(::operator new(n * sizeof(_Value_type)));
		}
		//メモリ解放
		void deallocate(Iterator p, imsize_t n) {
			//deleteのオーバーロード対策
			::operator delete((void*)to_pointer(p));
		}
	};
	//比較演算
	template <class T, class U, class Iterator1, class Iterator2>
	bool operator==(const allocator<T, Iterator1>&, const allocator<U, Iterator2>&) {
		return true;
	}
	template <class T, class U, class Iterator1, class Iterator2>
	bool operator!=(const allocator<T, Iterator1>&, const allocator<U, Iterator2>&) {
		return false;
	}

	//汎用ディアロケータ(デリーター)クラス
	template <class T>
	class deallocator {
	public:
		static constexpr imsize_t variable = 0;
		static constexpr imsize_t array_variable = 1;
		static constexpr imsize_t function = 2;
	private:

		//ホルダーの基底
		struct holder_base {
			T*	value;
			constexpr holder_base() :value(nullptr) {}
			constexpr holder_base(T* p) :value(p) {}
			virtual ~holder_base() = 0 {}

			//ホールドしている変数の解放
			virtual void dispose(T* p) = 0;
			virtual void dispose() = 0;
		};
		//配列ではない変数
		struct holder_variable :holder_base {
			using holder_base::holder_base;
			using holder_base::value;
			~holder_variable() { dispose(); }

			void dispose(T* p) { if (value != nullptr) delete value; value = p; }
			void dispose() { if (value != nullptr) delete value; }
		};
		//配列変数
		struct holder_array_variable :holder_base {
			using holder_base::holder_base;
			using holder_base::value;
			~holder_array_variable() { dispose(); }

			void dispose(T* p) { if (value != nullptr) delete[] value; value = p; }
			void dispose() { if (value != nullptr) delete[] value; }
		};
		//関数
		template <class F>
		struct holder_function :holder_base {
			using holder_base::value;

			F f;

			constexpr holder_function(T* p, F f = nullptr) :holder_base(p), f(f) {}
			~holder_function() { dispose(); }

			void dispose(T* p) { if (value != nullptr) f(value); value = p; }
			void dispose() { if (value != nullptr) f(value); }
		};

		//アロケートホルダーの補助構造体(部分特殊化に定数を使うと何故かエラー)
		template <imsize_t N, class... Args>
		struct allocate_holder_struct;
		template <class... Args>
		struct allocate_holder_struct<0, Args...> {
			static holder_base* __allocate_holder(T* p, Args... args) {
				return new holder_variable(p, iml::forward<Args>(args)...);
			}
		};
		template <class... Args>
		struct allocate_holder_struct<1, Args...> {
			static holder_base* __allocate_holder(T* p, Args... args) {
				return new holder_array_variable(p, iml::forward<Args>(args)...);
			}
		};
		template <class... Args>
		struct allocate_holder_struct<2, Args...> {
			static holder_base* __allocate_holder(T* p, Args... args) {
				return new holder_function<Args...>(p, iml::forward<Args>(args)...);
			}
		};
		template <imsize_t N, class... Args>
		static holder_base* allocate_holder(T* p, Args... args) {
			return allocate_holder_struct<N, Args...>::__allocate_holder(p, iml::forward<Args>(args)...);
		}

		deallocator& operator=(const deallocator &) = delete;
		deallocator& operator=(deallocator &&d) { this->ptr = d.ptr; d.ptr = nullptr; }

		holder_base* ptr;

		template <imsize_t N, class... Args>
		constexpr deallocator(placeholder<N> ident, T* p, Args... args) :ptr(allocate_holder<N>(p, args...)) {}

	public:
		void dispose(T* p = nullptr) { ptr->dispose(p); }
		void destroy() { delete this; }

		//インスタンスの取得
		template <imsize_t N, class... Args>
		static deallocator* get(T* p, Args... args) { return new deallocator(placeholder<N>(), p, iml::forward<Args>(args)...); }
	};

	//コンテナの基底
	template <class T, class Iterator, class Allocator>
	class container_base {
	public:
		using value_type = T;
		using iterator = Iterator;
		using const_iterator = typename Iterator::template rebind<const value_type>::other;
		using allocator_type = Allocator;

		static const imsize_t npos = -1;			//無効な位置

		constexpr container_base() {}
		virtual ~container_base() = 0 {}

		//イテレータ位置
		virtual iterator begin() noexcept = 0;
		virtual const_iterator begin() const noexcept = 0;
		virtual iterator end() noexcept = 0;
		virtual const_iterator end() const noexcept = 0;
	};

	//iteratorからconst_iteratorに変換
	template <class Iterator>
	inline constexpr typename Iterator::template rebind<const typename Iterator::value_type>::other
		to_const_iterator(Iterator itr) {
		using result_type = typename Iterator::template rebind<const typename Iterator::value_type>::other;
		return *reinterpret_cast<result_type*>(&itr);
	}
	//const_iteratorからiteratorに変換
	template <class ConstIterator>
	inline constexpr typename ConstIterator::template rebind<typename remove_cv<typename ConstIterator::value_type>::type>::other
		to_iterator(ConstIterator itr) {
		using result_type = typename ConstIterator::template rebind<typename remove_cv<typename ConstIterator::value_type>::type>::other;
		return *reinterpret_cast<result_type*>(&itr);
	}
}

namespace iml {

	//変数リストを代入するためのクラス
	template <class ForwardIterator>
	class comma_input {
		//前方向イテレータでないとき
		static_assert(is_base_of<forward_iterator_tag, typename iterator_traits<ForwardIterator>::iterator_category>::value
			, "The type of iterator is different.");

		ForwardIterator	itr;
		ForwardIterator	last;

		using value_type = typename iterator_traits<ForwardIterator>::value_type;
	public:
		constexpr comma_input(ForwardIterator first, ForwardIterator last) :itr(first), last(last) {}

		template <class T, class = typename iml::enable_if<iml::is_inclusion<T, value_type>::value>::type>
		comma_input& operator,(const T& value) {
			//外部に書き込まないようにする
			if (itr != last) {
				*itr = static_cast<value_type>(value);
				++itr;
			}
			return *this;
		}
	};
	template <class ForwardIterator>
	class value_list_input {
		//前方向イテレータでないとき
		static_assert(is_base_of<forward_iterator_tag, typename iterator_traits<ForwardIterator>::iterator_category>::value
			, "The type of iterator is different.");

		using value_type = typename iterator_traits<ForwardIterator>::value_type;
		using iterator = ForwardIterator;
		using const_iterator = typename iterator::template rebind<const value_type>::other;
	public:
		constexpr value_list_input() {}

		virtual iterator begin() noexcept = 0;
		virtual const_iterator begin() const noexcept = 0;
		virtual iterator end() noexcept = 0;
		virtual const_iterator end() const noexcept = 0;

		comma_input<ForwardIterator> operator<<(const value_type& value) {
			ForwardIterator itr = begin();
			*itr = value;
			++itr;
			return comma_input<ForwardIterator>(itr, end());
		}
	};
}

#endif