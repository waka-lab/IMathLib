#ifndef IMATH_MATH_MATH_MIN_HPP
#define IMATH_MATH_MATH_MIN_HPP

#undef min

namespace iml {

	//最小値
	template <class T>
	struct Min {
		static constexpr T _min_(const T& x, const T& y) { return (x < y) ? x : y; }
		template <class... Types>
		static constexpr T _min_(const T& x1, const T& x2, const T& x3, const Types&... args) {
			T temp = _min_(x2, x3, args...);
			return (x1 < temp) ? x1 : temp;
		}
	};
	template <class T>
	inline constexpr auto min(const T& x, const T& y) { return Min<T>::_min_(x, y); }
	template <class T, class... Types>
	inline constexpr auto min(const T& x1, const T& x2, const T& x3, const Types&... args) { return Min<T>::_min_(x1, x2, x3, args...); }
}


#endif