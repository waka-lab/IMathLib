#ifndef IMATH_UTILITY_TYPE_TRAITS_ADDRESSOF_HPP
#define IMATH_UTILITY_TYPE_TRAITS_ADDRESSOF_HPP

#include "IMathLib/utility/type_traits/is_type.hpp"


namespace iml {

	//アドレスを返す
	template <class T, bool>
	struct Addressof {
		//関数のアドレス
		static constexpr T* _addressof_(T& val) noexcept { return val; }
	};
	template <class T>
	struct Addressof<T, false> {
		//オブジェクトのアドレス(operator&()対策)
		static constexpr T* _addressof_(T& val) noexcept {
			return (reinterpret_cast<T*>(&const_cast<char&>(reinterpret_cast<const volatile char&>(val))));
		}
	};
	template<class T>
	inline constexpr T* addressof(T& val) noexcept {
		return Addressof<T, is_function<T>::value>::_addressof_(val);
	}
	template <class T>
	constexpr T* addressof(const T&&) = delete;


	//多次元配列の先頭ポインタの取得
	template <class T>
	constexpr auto addressof_multi_array(T& val) {
		return reinterpret_cast<typename remove_all_extents<T>::type*>(addressof(val));
	}
	template <class T>
	constexpr T* addressof_multi_array(const T&&) = delete;
}

#endif