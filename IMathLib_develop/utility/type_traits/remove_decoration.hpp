#ifndef IMATH_UTILITY_TYPE_TRAITS_REMOVE_DECORATION_HPP
#define IMATH_UTILITY_TYPE_TRAITS_REMOVE_DECORATION_HPP


//修飾の除去

namespace iml {

	//型のconst修飾の除去
	template<class T>
	struct remove_const {
		using type = T;
	};
	template<class T>
	struct remove_const<const T> {
		using type = T;
	};


	//型のvolatileの除去
	template<class T>
	struct remove_volatile {
		using type = T;
	};
	template<class T>
	struct remove_volatile<volatile T> {
		using type = T;
	};


	//型のconst-volatile修飾の除去
	template<class T>
	struct remove_cv {
		using type = typename remove_const<typename remove_volatile<T>::type>::type;
	};


	//参照の削除
	template <class T>
	struct remove_reference {
		using type = T;
	};
	template <class T>
	struct remove_reference<T&> {
		using type = T;
	};
	template <class T>
	struct remove_reference<T&&> {
		using type = T;
	};


	//const-volatile修飾とreferenceの除去
	template <class T>
	struct remove_cvr {
		using type = typename remove_reference<typename remove_cv<T>::type>::type;
	};


	//配列から次数の削除
	template<class T>
	struct remove_extent {
		using type = T;
	};
	template<class T, imsize_t N>
	struct remove_extent<T[N]> {
		using type = T;
	};
	template<class T>
	struct remove_extent<T[]> {
		using type = T;
	};


	//配列から全ての次数の削除
	template<class T>
	struct remove_all_extents {
		using type = T;
	};
	template<class T, imsize_t N>
	struct remove_all_extents<T[N]> {
		using type = typename remove_all_extents<T>::type;
	};
	template<class T>
	struct remove_all_extents<T[]> {
		using type = typename remove_all_extents<T>::type;
	};


	//ポインタの除去
	template<class T>
	struct remove_pointer {
		using type = T;
	};
	template<class T>
	struct remove_pointer<T*> {
		using type = T;
	};
	template<class T>
	struct remove_pointer<T* const> {
		using type = T;
	};
	template<class T>
	struct remove_pointer<T* volatile> {
		using type = T;
	};
	template<class T>
	struct remove_pointer<T* const volatile> {
		using type = T;
	};
}

#endif