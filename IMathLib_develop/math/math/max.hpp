#ifndef IMATH_MATH_MATH_MAX_HPP
#define IMATH_MATH_MATH_MAX_HPP

#undef max

namespace iml {

	//最大値
	template <class T>
	struct Max {
		static constexpr T _max_(const T& x, const T& y) { return (x < y) ? y : x; }
		template <class... Types>
		static constexpr T _max_(const T& x1, const T& x2, const T& x3, const Types&... args) {
			T temp = _min_(x2, x3, args...);
			return (x1 < temp) ? temp : x1;
		}
	};
	template <class T>
	inline constexpr auto max(const T& x, const T& y) { return Max<T>::_max_(x, y); }
	template <class T, class... Types>
	inline constexpr auto max(const T& x1, const T& x2, const T& x3, const Types&... args) { return Max<T>::_max_(x1, x2, x3, args...); }
}


#endif