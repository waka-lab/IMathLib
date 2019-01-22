#ifndef _MULTCOMPLEX_MATH_HPP
#define _MULTCOMPLEX_MATH_HPP

#include "IMathLib/math/complex.hpp"
#include "IMathLib/math/math.hpp"

//複素数の数学関数

namespace iml {

	//デフォルトの数値評価関数の定義
	template <class T>
	struct numerical_evaluation_function<complex<T>> {
		using type = complex<T>;

		//実部取得
		static T __real(const type& x) {
			return x.real();
		}
		//虚部取得
		static T __imag(const type& x) {
			return x.imag();
		}
		//偏角取得
		static T __arg(const type& x) {
			return atan2<T>(x.imag(), x.real());
		}
		//共役
		static type __conj(const type& x) {
			return type(x.real(), -x.imag());
		}
		//極形式で複素数取得
		template <class T>
		type __polar(const T& r, const T& theta) {
			return (r*__exp(theta));
		}
		//リーマン球面への射影
		static type __proj(const type& x) {
			//無限大を判定するものがないためそのまま
			return x;
		}
	};

	template <class T>
	struct math_function<complex<T>> {
		using type = complex<T>;
		using result_type = typename conditional<is_integral<T>::value, complex<double>, complex<T>>::type;		//ほとんどの場合はTが整数型ならば演算結果はcomplex<double>

		//絶対値
		static auto __abs(const type& x) {
			return sqrt<T>(x.real()*x.real() + x.imag()*x.imag());
		}
		//平方根
		static type __sqrt(type x) {
			return (sqrt<T>(__abs(x))*__exp(type(0, __arg(x) / 2)));
		}
		//正弦
		static type __sin(const type& x) {
			type temp = __exp(type(0, 1)*x);
			return (temp - 1 / temp) / type(0, 2);
		}
		//余弦
		static type __cos(const type& x) {
			type temp = __exp(type(0, 1)*x);
			return (temp + 1 / temp) / 2;
		}
		//正接
		static type __tan(const type& x) {
			type temp = __exp(type(0, 1)*x);
			type temp2 = 1 / temp;
			return(temp - temp2) / ((temp + temp2)*type(0, 1));
		}
		//逆正弦
		static type __asin(const type& x) {
			return type(0, -1)*__asinh(type(0, 1)*x);
		}
		//逆余弦
		static type __acos(const type& x) {
			static const T _pi = pi<T>();
			return (_pi / 2 - __asin(x));
		}
		//逆正接
		static type __atan(const type& x) {
			return type(0, -1)*__atanh(type(0, 1)*x);
		}
		//指数関数
		static type __exp(const type& x) {
			return (exp<T>(x.real())*type(cos<T>(x.imag()), sin<T>(x.imag())));
		}
		//対数関数
		static type __log(const type& x) {
			return type(log<T>(__abs(x)), __arg(x));
		}
	private:
		//冪乗の分岐のための構造体
		//一般
		template <class S, bool flag1 = is_same<S, imint_t>::value, bool flag2 = is_same<S, imsize_t>::value>
		struct __pow_struct {
			static type __pow(const type& x, const type& y) {
				return __exp(y*__log(x));
			}
		};
		//imint_t
		template <class S>
		struct __pow_struct<S, true, false> {
			static type __pow(const type& x, imint_t y) {
				if (y == 0) return 1;
				if (y == 1) return x;
				if (y == 2) return x*x;
				if (y < 0) {
					if (((-y) & 1) == 0) return __pow(__pow(x, y >> 1), 2);
					else return 1 / x*__pow(__pow(x, (y + 1) >> 1), 2);
				}
				if ((y & 1) == 0) return __pow(__pow(x, y >> 1), 2);
				else return x*__pow(__pow(x, (y - 1) >> 1), 2);
			}
		};
		//imsize_t
		template <class S>
		struct __pow_struct<S, false, true> {
			static type __pow(const type& x, imsize_t y) {
				if (y == 0) return 1;
				if (y == 1) return x;
				if (y == 2) return x*x;
				if ((y & 1) == 0) return __pow(__pow(x, y >> 1), 2);
				else return x*__pow(__pow(x, (y - 1) >> 1), 2);
			}
		};
	public:
		//冪乗
		template <class S>
		static type __pow(const type& x, const S& y) {
			return __pow_struct<S>::__pow(x, y);
		}
		//双曲線正弦
		static type __sinh(const type& x) {
			return ((__exp(x) - __exp(-x)) / 2);
		}
		static type __cosh(const type& x) {
			return ((__exp(x) + __exp(-x)) / 2);
		}
		static type __tanh(const type& x) {
			type x1 = __exp(x), x2 = __exp(-x);
			return ((x1 - x2) / (x1 + x2));
		}
		//逆双曲線正弦
		static type __asinh(type x) {
			bool flag = x.real() < 0;
			if (flag) x.real(-x.real());
			type x1(__log(x + __sqrt(x*x + 1)));
			return (flag ? type(-x1.real(), x1.imag()) : x1);
		}
		//逆双曲線余弦
		static type __acosh(const type& x) {
			type x1(__log(x + __sqrt(x*x - 1)));
			type buf(-x.real(), x.imag());
			type x2(__log(buf + __sqrt(buf*buf - 1)));
			return (x.real() < 0) ? type(x2.real(), -x1.imag()) : __log(x + __sqrt(x*x - 1));
		}
		//逆双曲線正接
		static type __atanh(type x) {
			bool flag = x.real() < 0;
			if (flag) x.real(-x.real());
			type x1(__log((1 + x) / (1 - x)) / 2);
			return (flag ? type(-x1.real(), x1.imag()) : x1);
		}
	};
}

//複素数での数学関数の新しいオーバーロードと関数
namespace iml {

	//実部取得
	template <class T>
	inline auto real(const T& x) { return numerical_evaluation_function<T>::__real(x); }
	//虚部取得
	template <class T>
	inline auto imag(const T& x) { return numerical_evaluation_function<T>::__imag(x); }
	//偏角取得
	template <class T>
	inline T arg(const complex<T>& x) { return numerical_evaluation_function<complex<T>>::__arg(x); }
	//極形式で複素数取得
	template <class T>
	inline complex<T> polar(const T& r, const T& theta) { return numerical_evaluation_function<complex<T>>::__polar(x); }
	//リーマン球面への射影
	template <class T>
	inline complex<T> proj(const complex<T>& x) { return numerical_evaluation_function<complex<T>>::__proj(x); }

}



#endif