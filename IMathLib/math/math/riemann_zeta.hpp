#ifndef IMATH_MATH_MATH_RIEMANN_ZETA_HPP
#define IMATH_MATH_MATH_RIEMANN_ZETA_HPP

#include "IMathLib/math/math/math_traits.hpp"
#include "IMathLib/math/math/dirichlet_eta.hpp"
#include "IMathLib/math/math/pow.hpp"
#include "IMathLib/math/math/pi.hpp"
#include "IMathLib/math/math/gamma.hpp"

namespace iml {

	//リーマンのゼータ関数
	template <class T>
	struct Riemann_zeta {
		using result_type = typename math_function_type<T>::type;

		static constexpr result_type _riemann_zeta_(const T& x) {
			return dirichlet_eta(x) / (1 - pow<size_t, T>(2, 1 - x));
		}
	};
	template <class T>
	inline constexpr auto riemann_zeta(const T& x) { return Riemann_zeta<T>::_riemann_zeta_(x); }


	//リーマンのクシー関数
	template <class T>
	struct Riemann_xi {
		using result_type = typename math_function_type<T>::type;

		static constexpr result_type _riemann_xi_(const T& x) {
			return x * (x - 1)*pow(pi<result_type>, -result_type(x) / 2)*gamma(result_type(x) / 2)*riemann_zeta(x) / 2;
		}
	};
	template <class T>
	inline constexpr auto riemann_xi(const T& x) { return Riemann_xi<T>::_riemann_xi_(x); }

}


#endif