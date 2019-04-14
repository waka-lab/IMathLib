#ifndef IMATH_STRING_STATIC_STRING_HPP
#define IMATH_STRING_STATIC_STRING_HPP


#include "IMathLib/utility/utility.hpp"
#include "IMathLib/utility/algorithm.hpp"


namespace iml {

	//static文字列(とりあえず最低限だけ)
	template <class T, size_t N>
	struct static_string {
		static_assert(N > 0, "N must be greater than or equal to 1.");

		T str_m[N];

	private:
	public:
		template <size_t M>
		constexpr static_string<T, N + M - 1> operator+(const static_string<T, M>& str) const {
			static_string<T, N + M - 1> result = {};
			size_t i = 0;
			for (; i < N - 1; ++i) {
				if (this->str_m[i] == T(0)) break;
				result.str_m[i] = this->str_m[i];
			}
			for (size_t j = 0; j < M - 1; ++i, ++j) {
				if (str.str_m[j] == T(0)) break;
				result.str_m[i] = str.str_m[j];
			}
			return result;
		}

		constexpr size_t size() const {
			for (size_t i = 0; i < N; ++i) if (this->str_m[i] == T(0)) return i;
			return size_t(-1);
		}
	};

	template <class T, size_t N>
	constexpr static_string<T, N> to_static_string(const T(&str)[N]) {
		static_string<T, N> result = {};
		for (size_t i = 0; i < N; ++i) result.str_m[i] = str[i];
		return result;
	}

}

#endif