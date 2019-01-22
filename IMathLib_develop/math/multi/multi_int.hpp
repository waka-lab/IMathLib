#ifndef _IMATH_MULTI_MULTI_INT_HPP
#define _IMATH_MULTI_MULTI_INT_HPP

#include "IMathLib/IMathLib_config.hpp"
#include "IMathLib/math/math.hpp"

namespace iml {

	//多倍長整数(N:ビット数)
	template <imsize_t N>
	class multi_int {
		template <imsize_t> friend class multi_int;

		//確保される配列数
		static constexpr imsize_t size_m = N / limits<imsize_t>::digits + (N % limits<imsize_t>::digits != 0);

		bool sign_m;					//符号部
		imsize_t x_m[size_m];			//仮数部(負数なら補数の状態)

	public:
		constexpr multi_int() :x_m{}, sign_m(false) {}
		multi_int(const multi_int& n) {
			this->sign_m = n.sign_m;
			for (imsize_t i = 0; i < size_m; ++i) this->x_m[i] = n.x_m[i];
		}
		template <imsize_t _N>
		explicit multi_int(const multi_int<_N>& n) : x_m{} {
			this->sign_m = n.sign_m;
			for (imsize_t i = 1, m = min(this->size_m, n.size_m) + 1; i < m; ++i) this->x_m[this->size_m - i] = n.x_m[n.size_m - i];
		}
		~multi_int() {}

		//単項演算子
		multi_int operator+() const { return *this; }
		multi_int operator-() const {
			multi_int temp(*this);
			temp.sign_m = !temp.sign_m;
			return temp;
		}
		//代入演算
		multi_int& operator+=(const multi_int& n) {
			multi_int result;
			result.sign_m = this->sign_m^n.sign_m;
			bool carry_out = false;
			//キャリーしながら加算
			for (imsize_t i = size_m - 1; i != limits<imsize_t>::max(); --i) {
				result.x_m[i] = this->x_m[i] + n.x_m[i] + carry_out;
				carry_out = (this->x_m[i] > (limits<imsize_t>::max() - carry_out))
					|| (n.x_m[i] > (limits<imsize_t>::max() - carry_out))
					|| (this->x_m[i] > (limits<imsize_t>::max() - n.x_m[i]));
			}
			result.sign_m ^= carry_out;
			return result;
		}
		multi_int& operator-=(const multi_int& n) {
			multi_int result;
			result.sign_m = this->sign_m ^ (!n.sign_m);
			bool carry_out = true;
			//キャリーしながら加算
			for (imsize_t i = size_m - 1; i != limits<imsize_t>::max(); --i) {
				result.x_m[i] = this->x_m[i] + limits<imsize_t>::max() - n.x_m[i] + carry_out;
				carry_out = ((this->x_m[i] + carry_out) > n.x_m[i])
					|| (this->x_m[i] > (n.x_m[i] - carry_out))
					|| (this->x_m[i] > (n.x_m[i]));
			}
			result.sign_m ^= carry_out;
		}
		multi_int& operator*=(const multi_int& n) {
			multi_int result;
		}
		multi_int& operator=(const multi_int& n) {
			for (imsize_t i = 0; i < size_m; ++i) this->x_m[i] = n.x_m[i];
		}
	};

}


#endif