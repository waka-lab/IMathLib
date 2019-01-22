#ifndef _IMATH_MATH_RIEMANN_ZETA_HPP
#define _IMATH_MATH_RIEMANN_ZETA_HPP

#include "IMathLib/math/math/math_traits.hpp"
#include "IMathLib/math/math/dirichlet_eta.hpp"
#include "IMathLib/math/math/pow.hpp"
#include "IMathLib/math/math/pi.hpp"
#include "IMathLib/math/math/gamma.hpp"

namespace iml {

	//リーマンのゼータ関数
	template <class T>
	struct _Riemann_zeta {
		using result_type = typename math_function_type<T>::type;

		static constexpr result_type __riemann_zeta(const T& x) {
			return dirichlet_eta(x) / (1 - pow<imsize_t, T>(2, 1 - x));
		}
	};
	template <class T>
	inline constexpr auto riemann_zeta(const T& x) { return _Riemann_zeta<T>::__riemann_zeta(x); }


	//リーマンのクシー関数
	template <class T>
	struct _Riemann_xi {
		using result_type = typename math_function_type<T>::type;

		static constexpr result_type __riemann_xi(const T& x) {
			return x * (x - 1)*pow(_Pi_<result_type>::pi, -result_type(x) / 2)*gamma(result_type(x) / 2)*riemann_zeta(x) / 2;
		}
	};
	template <class T>
	inline constexpr auto riemann_xi(const T& x) { return _Riemann_zeta<T>::__riemann_zeta(x); }

}


#endif