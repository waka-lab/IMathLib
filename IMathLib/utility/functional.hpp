#ifndef IMATH_UTILITY_FUNCTIONAL_HPP
#define IMATH_UTILITY_FUNCTIONAL_HPP

#include "IMathLib/utility/smart_ptr.hpp"
#include "IMathLib/utility/utility.hpp"

namespace iml {

	//関数を保持するクラス
	template<class T>
	class function;
	template<class R, class... Args>
	class function<R(Args...)> {
	public:
		using result_type = R;
	private:
		//関数ホルダーの基底クラス
		struct holder_base {
			virtual ~holder_base() = 0 {}
			virtual holder_base* clone() const = 0;
			virtual const std::type_info& target_type() const = 0;
			virtual void* target() = 0;
			//関数呼び出し
			virtual result_type invoke(Args...) = 0;
		};
		//一般の関数
		template <class F>
		struct holder : public holder_base {
			holder(F f) : f(f) {}
			virtual ~holder() {}

			virtual holder_base* clone() const { return allocate_holder(f); }
			virtual const std::type_info& target_type() const { return typeid(F); }
			virtual void* target() { return &f; }

			virtual result_type invoke(Args... args) { return f(iml::forward<Args>(args)...); }
		private:
			F f;
		};
		//メンバ関数
		template <class holder_R, class T, class... Types>
		struct member_holder : public holder_base {
			member_holder(holder_R(T::* const f)(Types...)) : f(f) {}
			virtual ~member_holder() {}

			virtual holder_base * clone() const { return allocate_holder(f); }
			virtual const std::type_info& target_type() const { return typeid(f); }
			virtual void* target() { return &f; }

			result_type invoke_impl(T t1, Types... args) { return (t1.*f)(iml::forward<Types>(args)...); }
			result_type invoke_impl(T* t1, Types... args) { return ((*t1).*f)(iml::forward<Types>(args)...); }
			virtual result_type invoke(Args ... args) { return invoke_impl(iml::forward<Args>(args)...); }

		private:
			holder_R(T::*f)(Types...);
		};
		//データメンバ(メンバ変数ポインタ)
		template <class T, class DATA>
		struct data_member_holder : holder_base {
			data_member_holder(DATA T::* const f) : f(f) {}
			virtual ~data_member_holder() {}

			virtual holder_base* clone() const { return allocate_holder(f); }
			virtual const std::type_info& target_type() const { return typeid(f); }
			virtual void* target() { return &f; }

			result_type invoke_impl(T & t1) { return t1.*f; }
			result_type invoke_impl(T * const t1) { return (*t1).*f; }
			virtual result_type invoke(Args ... args) { return invoke_impl(args...); }

		private:
			DATA T::*f;
		};

		holder_base * ptr;

		//オーバーロードによるホルダーの作成
		template <class F>
		static holder_base* allocate_holder(F f) {
			return new holder<F>(f);
		}
		template <class holder_R, class T, class... Types >
		static holder_base* allocate_holder(holder_R(T::* f)(Types...)) {
			return new member_holder<holder_R, T, Types...>(f);
		}
		template <class T, class DATA >
		static holder_base* allocate_holder(DATA T::* f) {
			return new data_member_holder<T, DATA>(f);
		}
		//ホルダーのデリート
		void deallocate_holder() {
			delete ptr;
			ptr = nullptr;
		}
		//ホルダーの複製
		holder_base* clone() const {
			return (ptr != nullptr) ? ptr->clone() : nullptr;
		}
	public:
		explicit function() : ptr(nullptr) {}
		function(nullptr_t) : ptr(nullptr) {}
		template <class F>
		function(F f) : ptr(allocate_holder(f)) {}
		function(const function& f) : ptr(f.clone()) {}
		function(function&& f) : ptr(f.ptr) { f.ptr = nullptr; }
		~function() { deallocate_holder(); }

		function& operator=(const function& f){
			if (this == &f) return *this;
			deallocate_holder();
			this->ptr = f.clone();
			return *this;
		}
		function& operator=(function&& f){
			if (this == &f) return *this;

			deallocate_holder();
			this->ptr = f.ptr;
			f.ptr = nullptr;

			return *this;
		}
		function& operator=(nullptr_t){
			deallocate_holder();
			return *this;
		}
		template <class F>
		function& operator=(F f) {
			delete ptr;
			ptr = allocate_holder(f);
			return *this;
		}

		//リソースを所持しているかの判定
		explicit operator bool() const { return ptr != nullptr; }
		bool operator!() const { return ptr == nullptr; }
		//関数呼び出し
		result_type operator()(Args... args) const {
			if (ptr != nullptr) return ptr->invoke(iml::forward<Args>(args)...);
			throw;
		}

		//型情報の取得
		const std::type_info& target_type() const{
			return (ptr != nullptr) ? ptr->target_type() : typeid(void);
		}
		//関数の取得
		template <class T>
		T* target() {
			return (target_type() == typeid(T)) ? static_cast<T*>(ptr->target()) : nullptr;
		}
		template <class T>
		const T* target() const {
			return (target_type() == typeid(T)) ? static_cast<const T*>(ptr->target()) : nullptr;
		}

		void swap(function& f) {
			iml::swap(this->ptr, f.ptr);
		}
	};


	//シングルトンを実装するための基底クラス
	template <class T>
	class singleton {
		singleton(const singleton &) = delete;
		singleton(singleton &&) = delete;
		singleton& operator=(const singleton &) = delete;
		singleton& operator=(singleton &&) = delete;
	public:
		virtual ~singleton() = 0 {}

		//インスタンスの取得
		static T* inst() {
			static unique_ptr<T> instance(new T());
			return instance.get();
		}
	protected:
		constexpr singleton() {}
	};
}

namespace iml {

	//関数型の戻り値型の取得
	template <class F>
	struct function_result { using type = typename is_function<F>::result_type; };
	template <class R, class... Types>
	struct function_result<function<R(Types...)>> { using type = R; };
	//関数型の任意の位置の引数型の取得
	template <size_t N, class F>
	struct function_args_type { using type = at_type_tuple<N, typename is_function<F>::arg_types>; };
	template <size_t N, class R, class... Types>
	struct function_args_type<N, function<R(Types...)>> { using type = typename at_type<N, Types...>::type; };

	//可変長引数の任意の位置の値の取得
	template <size_t M, size_t N, bool flag = (M == N)>
	struct get_argument_struct {
		template <class First, class... Args>
		static constexpr First __get_argument(First first, Args... args) {
			return first;
		}
	};
	template <size_t M, size_t N>
	struct get_argument_struct<M, N, false> {
		template <class First, class... Args>
		static constexpr auto __get_argument(First first, Args... args) {
			return get_argument_struct<M + 1, N>::__get_argument(args...);
		}
	};
	template <size_t N, class First, class... Args>
	inline constexpr typename at_type<N, First, Args...>::type get_argument(First first, Args... args) {
		static_assert((sizeof...(Args)+1) > N, "N is too big.");
		return get_argument_struct<0, N>::__get_argument(first, args...);
	}
}

#endif