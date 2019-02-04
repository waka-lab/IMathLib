#ifndef IMATH_UTILITY_TYPE_TRAITS_WRAPPER_HPP
#define IMATH_UTILITY_TYPE_TRAITS_WRAPPER_HPP

#include "IMathLib/utility/type_traits/integral_constant.hpp"


//型や推論に対するラッパー

namespace iml {

	//Tを変更しないでoperator()をなし(任意の整数テンプレートに対して型Tを与える)
	//要はラップなし
	template<class T, imsize_t = 0>
	struct identity_type {
		using type = T;
	};


	//int型引数が_Wrap_intより優先されるようにラップ
	struct _Wrap_int { _Wrap_int(int) {} };


	//参照変数ののためのラッパー(vector<T&>とvector<reference_wrapper<T>>とするみたいな)
	template <class T>
	class reference_wrapper {
		static_assert(is_object<T>::value || is_function<T>::value, "T must be object type or function type.");

		T *ptr;
	public:
		reference_wrapper(T& val) noexcept : ptr(addressof(val)) {}
		reference_wrapper(T&&) = delete;

		using type = T;

		operator T&() const noexcept { return *ptr; }
		T& get() const noexcept { return *ptr; }
		//オブジェクトの呼び出し
		template<class... Types>
		auto operator()(Types&&... args) const { return invoke(get(), forward<Types>(args)...); }
	};

	//reference_wrapperの構築
	template<class T>
	inline reference_wrapper<T> ref(T& val) noexcept { return reference_wrapper<T>(val); }
	template<class T>
	inline reference_wrapper<T> ref(reference_wrapper<T> val) noexcept { return ref(val.get()); }
	template<class T>
	void ref(const T&&) = delete;
	template<class T>
	inline reference_wrapper<const T> cref(const T& val) noexcept { return reference_wrapper<const T>(val); }
	template<class T>
	inline reference_wrapper<const T> cref(reference_wrapper<T> val) noexcept { return cref(val.get()); }
	template<class T>
	void cref(const T&&) = delete;


	template<class T>
	struct reference_unwrapper_impl {
		using type = T;
		static constexpr bool is_refwrap_value = false;
	};
	template<class T>
	struct reference_unwrapper_impl<reference_wrapper<T>> {
		using type = T&;
		static constexpr bool is_refwrap_value = true;
	};
	//reference_wrapperのラップの解除
	template<class T>
	struct reference_unwrapper {
		using T1 = typename decay<T>::type;
		using type = typename reference_unwrapper_impl<T1>::type;
		static constexpr bool is_refwrap_value = reference_unwrapper_impl<T1>::is_refwrap_value;
	};
}


#endif