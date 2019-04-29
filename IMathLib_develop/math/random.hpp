#ifndef _IMATH_RANDOM_HPP
#define _IMATH_RANDOM_HPP

#include "IMathLib/utility/type_traits.hpp"
#include "IMathLib/math/math.hpp"

//疑似乱数

namespace iml {
	namespace rnd {

		//乱数の基底クラス(テンプレート引数は符号無し整数型)
		template <class UIntT, class = typename enable_if<!is_unsigned<UIntT>::value>::type>
		class random_base {
			//整数型でなければならない
			static_assert(is_integral<UIntT>::value, "Template must be integer type.");
		protected:
			size_t	cnt = 0;			//カウンタ
		public:
			constexpr random_base() {}
			virtual ~random_base() = 0 {}

			virtual constexpr UIntT(min)() const = 0;
			virtual constexpr UIntT(max)() const = 0;

			//シード値のセット
			virtual void seed(UIntT s) = 0;

			//疑似乱数の取得
			virtual UIntT get() = 0;

			//乱数を進める
			void discard(size_t k) { for (size_t i = 0; i < k; ++i) this->get(); }

			//計算回数
			size_t times() { return cnt; }

			//正規化した疑似乱数の取得
			template <class S>
			S unit() { return static_cast<S>(this->get()) / ((this->max)() - (this->min)()); }
		};

		//線型合同法
		template <class T, T A, T B, T M>
		class linear_congruential :public random_base<T> {
		public:
			using type = T;
		private:
			type		x;		//現在の生成された値
		public:
			constexpr linear_congruential() :x(1) {}
			constexpr linear_congruential(type s) : x(s) {}

			constexpr type(min)() const { return B == 0 ? 1 : 0; }
			constexpr type(max)() const { return M - 1; }

			//シード値のセット
			void seed(type s) { x = s; cnt = 0; }

			//疑似乱数の取得
			type get() { return (*this)(); }
			type operator()() { ++cnt; return x = (A*x + B) % M; }
		};
		//MINSTD乱数
		using linear_congruential_minstd = linear_congruential<unsigned __int32, 48271, 0, 2147483647>;

		//Xorshift
		template <class T, size_t Shift1, size_t Shift2, size_t Shift3>
		class Xorshift32 :public random_base<T> {
		public:
			using type = T;
			using inner_type = unsigned __int32;
		private:
			static_assert((Shift1 <= (1 << numeric_traits<type>::digits) - 1) || (Shift2 <= (1 << numeric_traits<type>::digits) - 1)
				|| (Shift3 <= (1 << numeric_traits<type>::digits) - 1) || (numeric_traits<type>::digits <= numeric_traits<inner_type>::digits)
				, "The parameter is incorrect.");

			inner_type	x;		//現在の生成された値

		public:
			constexpr Xorshift32() :x(1) {}
			constexpr Xorshift32(type s) : x(s) {}

			constexpr type(min)() const { return 0; }
			constexpr type(max)() const { return (numeric_traits<type>::max)(); }

			//シード値のセット
			void seed(type s) { x = s; cnt = 0; }

			//疑似乱数の取得
			type get() { return (*this)(); }
			type operator()() {
				++cnt;
				x ^= (x << Shift1);
				x ^= (x >> Shift2);
				return x ^= (x << Shift3);
			}
		};
		template <class T, size_t Shift1, size_t Shift2, size_t Shift3>
		class Xorshift64 :public random_base<T> {
		public:
			using type = T;
			using inner_type = unsigned __int64;
		private:
			static_assert((Shift1 <= (1 << numeric_traits<type>::digits) - 1) || (Shift2 <= (1 << numeric_traits<type>::digits) - 1)
				|| (Shift3 <= (1 << numeric_traits<type>::digits) - 1) || (numeric_traits<type>::digits <= numeric_traits<inner_type>::digits)
				, "The parameter is incorrect.");

			inner_type	x;		//現在の生成された値

		public:
			constexpr Xorshift64() :x(1) {}
			constexpr Xorshift64(type s) : x(s) {}

			constexpr type(min)() const { return 0; }
			constexpr type(max)() const { return (numeric_traits<type>::max)(); }

			//シード値のセット
			void seed(type s) { x = s; cnt = 0; }

			//疑似乱数の取得
			type get() { return (*this)(); }
			type operator()() {
				++cnt;
				x ^= (x << Shift1);
				x ^= (x >> Shift2);
				return x ^= (x << Shift3);
			}
		};
		//周期2^32-1のXorshift
		template <class T>
		using Xorshift32_32 = Xorshift32<T, 13, 17, 5>;
		//周期2^64-1のXorshift
		template <class T>
		using Xorshift64_64 = Xorshift64<T, 13, 7, 17>;

		//メルセンヌ・ツイスタ
		template <class T, size_t w, size_t n, size_t m, size_t r, T a,
			size_t u, size_t s, size_t t, size_t l, T b, T c, T d,
			T f>
		class mersenne_twister :public random_base<T> {
		public:
			using type = T;
		private:
			static_assert((0 > m) || (n >= m) || (2 * u < w) || (r <= w) || (u <= w) || (s <= w) || (t <= w) || (l <= w)
				|| (w <= numeric_traits<type>::digits)
				|| (a <= (1 << w) - 1) || (b <= (1 << w) - 1) || (c <= (1 << w) - 1) || (d <= (1 << w) - 1) || (f <= (1 << w) - 1),
				"The parameter is incorrect.");

			static constexpr type mask_a[2] = { 0,a };						//aのマスク
			static constexpr type upper_mask = ~((~type(0)) >> (w - r));	//上位ビット抽出マスク
			static constexpr type lower_mask = ~upper_mask;					//下位ビット抽出マスク

			size_t	array_cnt;				//配列用のカウンタ
			type		x[n];

			//ビット配列の作成
			void make_bit_array() {
				size_t i;
				T temp;

				//0～n-mまで
				for (i = 0; i < n - m; ++i) {
					//下位ビットと上位ビットを抽出してorをする
					temp = (x[i] & upper_mask) | (x[i + 1] & lower_mask);
					//yの最下位ビットが0ならばaとのxorを実行
					x[i] = x[i + m] ^ (temp >> 1) ^ mask_a[temp & 1];
				}
				//n-mからn-1まで
				for (; i < n - 1; ++i) {
					temp = (x[i] & upper_mask) | (x[i + 1] & lower_mask);
					x[i] = x[i + (m - n)] ^ (temp >> 1) ^ mask_a[temp & 1];
				}
				//一番最後の部分
				temp = (x[n - 1] & upper_mask) | (x[0] & lower_mask);
				x[n - 1] = x[m - 1] ^ (temp >> 1) ^ mask_a[temp & 1];

				//配列カウンタのリセット
				array_cnt = 0;
			}

			//2のw乗の取得(何故かwの値を含めるとインスタンス化に失敗する)
			template <size_t N, size_t X, type Result>
			struct _2_power_of_X :_2_power_of_X<N + 1, X, Result * 2> {};
			template <size_t X, type Result>
			struct _2_power_of_X<X, X, Result> {
				static constexpr type value = Result;
			};
		public:
			mersenne_twister() { seed(type(5489)); }
			mersenne_twister(type s) { seed(s); }

			constexpr type(min)() const { return 0; }
			constexpr type(max)() const { return _2_power_of_X<0, w, 1>::value - 1; }

			//シード値のセット
			void seed(type s) {
				x[0] = s;
				for (size_t i = 1; i < n; ++i) x[i] = (f * (x[i - 1] ^ (x[i - 1] >> 30)) + i);
				cnt = 0;
				make_bit_array();
			}

			//疑似乱数の取得
			type get() { return (*this)(); }
			type operator()() {
				type y;

				++cnt;
				if (array_cnt >= n) make_bit_array();

				y = x[array_cnt++];

				//調律する
				y ^= (y >> 11);
				y ^= (y << 7) & b;
				y ^= (y << 15) & c;
				y ^= (y >> 18) & d;

				return y;
			}
		};
		//mt19937の32ビット用
		using mersenne_twister_19937_32 = mersenne_twister<uint32_t, 32, 624, 397, 31, 0x9908B0DF
			, 11, 7, 15, 18, 0x9D2C5680, 0xEFC60000, 0xFFFFFFFF
			, 1812433253>;
		//mt19937の64ビット用
		using mersenne_twister_19937_64 = mersenne_twister<uint64_t, 64, 312, 156, 31, 0xB5026f5AA96619E9
			, 29, 17, 37, 43, 0x5555555555555555, 0x71D67fffEDA60000, 0xFFF7EEE000000000
			, 6364136223846793005>;
	}

}

#endif