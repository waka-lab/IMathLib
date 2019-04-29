#ifndef _IMATH_MULTI_MULTI_FLOAT_HPP
#define _IMATH_MULTI_MULTI_FLOAT_HPP

#include "IMathLib/IMathLib_config.hpp"
#include "IMathLib/math/math.hpp"

namespace iml {

	//固定長任意精度浮動小数点(N:仮数部のビット数)
	template <size_t N>
	class multi_float {
		template <size_t> friend class multi_float;

		//確保される配列数
		static constexpr size_t size_m = N / numeric_traits<size_t>::digits + (N % numeric_traits<size_t>::digits != 0);

		bool sign_m;					//符号部
		size_t x_m[size_m];			//仮数部
		int_t index_m;				//指数部

		//加算(型の符号は無視)
		multi_float multi_float_add(const multi_float& n) {
			multi_float result;
			bool carry_out = false;

			if (this->index_m > n.index_m) {
				//指数の差
				int_t index_imu_range = this->index_m - n.index_m;
				result.index_m = this->index_m;
				//キャリーアウト
				carry_out = n.x_m[size_m - 1 - index_imu_range] > (numeric_traits<size_t>::max() / 2);
				//キャリーしながら加算
				for (size_t i = size_m - 1; i != index_imu_range; --i) {
					result.x_m[i] = this->x_m[i] + n.x_m[i - index_imu_range] + carry_out;
					carry_out = (this->x_m[i] > (numeric_traits<size_t>::max() - carry_out))
						|| (n.x_m[i - index_imu_range] > (numeric_traits<size_t>::max() - carry_out))
						|| (this->x_m[i] > (numeric_traits<size_t>::max() - n.x_m[i - index_imu_range]));
				}
				//(--0) == numeric_traits<size_t>::max()
				for (size_t i = index_imu_range; (i != numeric_traits<size_t>::max()) && (carry_out); --i) {
					result.x_m[i] = this->x_m[i] + carry_out;
					carry_out = this->x_m[i] > (numeric_traits<size_t>::max() - carry_out);
				}
			}
			else {
				//指数の差
				int_t index_imu_range = n.index_m - this->index_m;
				result.index_m = n.index_m;
				//キャリーアウト(キャリーしない場合も考慮)
				carry_out = (index_imu_range == 0) ? 0 : (this->x_m[size_m - 1 - index_imu_range] > (numeric_traits<size_t>::max() / 2));
				//キャリーしながら加算
				for (size_t i = size_m - 1; i != index_imu_range; --i) {
					result.x_m[i] = this->x_m[i - index_imu_range] + n.x_m[i] + carry_out;
					carry_out = (this->x_m[i - index_imu_range] > (numeric_traits<size_t>::max() - carry_out))
						|| (n.x_m[i] > (numeric_traits<size_t>::max() - carry_out))
						|| (this->x_m[i - index_imu_range] > (numeric_traits<size_t>::max() - n.x_m[i]));
				}
				//(--0) == numeric_traits<size_t>::max()
				for (size_t i = index_imu_range; (i != numeric_traits<size_t>::max()) && (carry_out); --i) {
					result.x_m[i] = n.x_m[i] + carry_out;
					carry_out = n.x_m[i] > (numeric_traits<size_t>::max() - carry_out);
				}
			}
			//シフト処理
			if (carry_out) {
				++result.index_m;
				carry_out = result.x_m[size_m - 1] > (numeric_traits<size_t>::max() / 2);
				for (size_t i = size_m - 1; i != 0; --i) {
					result.x_m[i] = result.x_m[i - 1] + carry_out;
					carry_out = result.x_m[i - 1] > (numeric_traits<size_t>::max() - carry_out);
				}
				result.x_m[0] = 1 + carry_out;
			}
			return result;
		}
		//減算(型の符号は無視)
		multi_float multi_float_sub(const multi_float& n) {
			multi_float result;
			bool carry_out = false;

			if (this->index_m > n.index_m) {
				//指数の差
				int_t index_imu_range = this->index_m - n.index_m;
				result.index_m = this->index_m;
				//キャリーアウト
				carry_out = (numeric_traits<size_t>::max() / 2 + 1) > n.x_m[size_m - 1 - index_imu_range];
				//キャリーしながら加算
				for (size_t i = size_m - 1; i != index_imu_range; --i) {
					result.x_m[i] = this->x_m[i] + numeric_traits<size_t>::max() - n.x_m[i - index_imu_range] + carry_out;
					carry_out = ((this->x_m[i] + carry_out) > n.x_m[i - index_imu_range])
						|| (this->x_m[i] > (n.x_m[i - index_imu_range] - carry_out))
						|| (this->x_m[i] > (n.x_m[i - index_imu_range]));
				}
				//(--0) == numeric_traits<size_t>::max()
				for (size_t i = index_imu_range; i != numeric_traits<size_t>::max(); --i) {
					result.x_m[i] = this->x_m[i] + numeric_traits<size_t>::max() + carry_out;
					carry_out = (this->x_m[i] > 0) || (carry_out);
				}
			}
			else {
				//指数の差
				int_t index_imu_range = n.index_m - this->index_m;
				result.index_m = n.index_m;
				//キャリーアウト(キャリーしない場合も考慮)
				carry_out = (index_imu_range == 0) ? 1 : ((numeric_traits<size_t>::max() / 2 + 1) > this->x_m[size_m - 1 - index_imu_range]);
				//キャリーしながら加算
				for (size_t i = size_m - 1; i != index_imu_range; --i) {
					result.x_m[i] = this->x_m[i - index_imu_range] + numeric_traits<size_t>::max() - n.x_m[i] + carry_out;
					carry_out = ((this->x_m[i - index_imu_range] + carry_out) > n.x_m[i])
						|| (this->x_m[i - index_imu_range] > (n.x_m[i] - carry_out))
						|| (this->x_m[i - index_imu_range] > (n.x_m[i]));
				}
				//(--0) == numeric_traits<size_t>::max()
				for (size_t i = index_imu_range; (i != numeric_traits<size_t>::max()) && (carry_out); --i) {
					result.x_m[i] = numeric_traits<size_t>::max() - n.x_m[i] + carry_out;
					carry_out = carry_out > n.x_m[i];
				}
			}
			//補数表現を元に戻す(この処理の後で理論上はcarry_out==falseとなる)
			if (!carry_out) {
				result.sign_m = true;
				carry_out = true;
				for (size_t i = size_m - 1; i != numeric_traits<size_t>::max(); --i) {
					bool temp = carry_out > result.x_m[i];
					result.x_m[i] = numeric_traits<size_t>::max() - result.x_m[i] + carry_out;
					carry_out = temp;
				}
			}
			//シフト処理
			if (result.x_m[0] == 0) {
				//シフト量のカウント
				size_t cnt = 1;
				for (; (cnt < size_m) && (result.x_m[cnt] == 0); ++cnt);
				result.index_m -= cnt;
				//演算結果が0
				if (cnt == size_m) {
					result.index_m = 0;
					result.sign_m = false;
				}
				else {
					//シフト実行
					for (size_t i = 0, end = size_m - cnt; i < end; ++i) result.x_m[i] = result.x_m[i + cnt];
					for (size_t i = this->size_m - cnt; i < size_m; ++i) result.x_m[i] = 0;
				}
			}
			return result;
		}
	public:
		constexpr multi_float(size_t n) : x_m{ n }, sign_m(false), index_m(0) {}
		multi_float(const multi_float& n) {
			this->sign_m = n.sign_m;
			this->index_m = n.index_m;
			for (size_t i = 0; i < size_m; ++i) this->x_m[i] = n.x_m[i];
		}
		template <size_t _N>
		explicit multi_float(const multi_float<_N>& n) {
			this->sign_m = n.sign_m;
			this->index_m = n.index_m;
			for (size_t i = 0, m = min(this->size_m, n.size_m); i < m; ++i) this->x_m[i] = n.x_m[i];
		}

		multi_float operator+() const { return *this; }
		multi_float operator-() const {
			multi_float temp(*this);
			temp.sign_m = !temp.sign_m;
			return temp;
		}
		multi_float& operator+=(const multi_float& n) {
			if (this->sign_m) {
				if (n.sign_m) {
					//(-n1) + (-n2)=-(n1 + n2)
					multi_float temp = multi_float_add(n);
					temp.sign_m = !temp.sign_m;
					return *this = temp;
				}
				else {
					//(-n1) + n2=-(n1 - n2)
					multi_float temp = multi_float_sub(n);
					temp.sign_m = !temp.sign_m;
					return *this = temp;
				}
			}
			else {
				//n1 + (-n2)=n1 - n2 / n1 + n2
				return *this = (n.sign_m ? multi_float_sub(n) : multi_float_add(n));
			}
		}
		multi_float& operator-=(const multi_float& n) {
			if (this->sign_m) {
				if (n.sign_m) {
					//(-n1) - (-n2)=-(n1 - n2)
					multi_float temp = multi_float_sub(n);
					temp.sign_m = !temp.sign_m;
					return *this = temp;
				}
				else {
					//(-n1) - n2=-(n1 + n2)
					multi_float temp = multi_float_add(n);
					temp.sign_m = !temp.sign_m;
					return *this = temp;
				}
			}
			else {
				//n1 - (-n2)=n1 + n2 / n1 - n2
				return *this = (n.sign_m ? multi_float_add(n) : multi_float_sub(n));
			}
		}
		multi_float& operator*=(const multi_float& n) {
			multi_float result;
			result.index_m = this->index_m + n.index_m;
		}
		multi_float& operator=(const multi_float& n) {
			for (size_t i = 0; i < size_m; ++i) this->x_m[i] = n.x_m[i];
			this->index_m = n.index_m;
			this->sign_m = n.sign_m;
		}
	};

}


#endif