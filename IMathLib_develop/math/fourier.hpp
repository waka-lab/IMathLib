#ifndef _FOURIER_HPP
#define _FOURIER_HPP

#include <cmath>
#include <complex>
#include <vector>
#include <algorithm>
#include "IMathLib/math/math.hpp"
#include "IMathLib/math/hypercomplex.hpp"
#include "IMathLib/utility/tuple.hpp"

//フーリエ変換に関するヘッダ

/*namespace iml {

	//n次元データ配列の型
	template <size_t N, class T, class... Types>
	struct _Dimensional_data :_Dimensional_data<N - 1, T*, Types..., size_t> {};
	template <class T, class... Types>
	struct _Dimensional_data<0, T, Types...> {
		using type = tuple<T, Types...>;
	};
	template <class T, size_t N>
	using dimensional_data_type = typename _Dimensional_data<N, T>::type;

	//フーリエ変換関連の関数の定義
	template <class T, size_t N>
	struct fourier {
		//データは一次元以上でなければならない
		static_assert(N != 1, "N must be greater than or equal to 1.");

		using type = dimensional_data_type<complex<T>, N>;

	private:
		//一次元の場合の定義

		//離散フーリエ変換
		static complex<T>* __DFT1(const complex<T>* data, size_t size, bool inv) {
			complex<T>* result = new complex<T>[size];
			static const T _pi = pi<T>();

			for (size_t i = 0; i < size; ++i) {
				complex<T> buf_exp = exp(-complex<T>(2 * _pi*i / size));
				complex<T> buf = 0;
				//逆変換ならデータの共役を取ってかける
				for (size_t j = 0; j < size; ++j) buf += ((inv) ? conj(data[j]) : data[j]) * pow(buf_exp, j);
				//逆変換なら共役を取る
				result[i] = (inv) ? conj(buf) : buf;
			}
			return result;
		}
		//逆離散フーリエ変換
		static complex<T>* __IDFT1(const complex<T>* data, size_t size) {
			complex<T>* result = __DFT1(data, size, true);
			for_each(result, result + size, [size](complex<T>& val) {val /= size; });
			return result;
		}

		//高速フーリエ変換
		static complex<T>* __FFT1(const complex<T>* data, size_t size, bool inv) {
			//!(size&(size-1))を満たさないときエラーを投げる(今はまだ)

		}
		//逆高速フーリエ変換
		static complex<T>* __IFFT1(const complex<T>* data, size_t size) {
			//!(size&(size-1))を満たさないときエラーを投げる(今はまだ)
			complex<T>* result = __DFT(data, size, true);
			for_each(result, result + size, [size](complex<T>& val) {val /= size; });
			return result;
		}

		//多次元用の補助構造体
		struct __DFT_struct {

		};

	public:
		//多次元の場合の定義
		type __DFT(const type& data) {
			complex<T>* conplex_buf;			//一次元フーリエ変換のための一時計算バッファ

		}

	};

	//高速フーリエ変換クラス
	class FFT {
	private:
		size_t N;							//入力信号のサイズ(2のべき乗数)
		int level;							//2のべき乗の指数部
		std::vector<size_t> ids;			//ビット反転用の配列
	public:
		//コンストラクタ(Nは2のべき乗)
		FFT(const int n);
		~FFT() {}
		//高速フーリエ変換
		template <class T>
		void fft(const std::vector<std::complex<T>>& a, std::vector<std::complex<T>>* pout, bool is_inverse = 0);
		//逆変換
		template <class T>
		void ifft(const std::vector<std::complex<T>>& a, std::vector<std::complex<T>>* pout);
	};

	inline FFT::FFT(const int n) {
		//値渡し
		N = n;
		//2のべき乗の指数部の計算
		auto& i = level;
		for (i = 0; i < 64; ++i)
			if ((N >> i) == 1) break;
		//ビット反転
		ids.reserve(N);             //サンプリング数分データ確保
		ids.push_back(0);
		ids.push_back(1);
		for (int i = 0; i < level - 1; ++i) {
			auto sz = ids.size();
			iml::for_each(ids.begin(), ids.end(), [](size_t& x) { x *= 2; });
			ids.insert(ids.end(), ids.begin(), ids.end());      //idsの後ろにそのままidsを結合
			auto it = ids.begin();
			std::advance(it, sz);           //イテレータitをsz回進める
			iml::for_each(it, ids.end(), [](size_t& x) { x += 1; });
		}
		return;
	}

	template <class T>
	inline void FFT::fft(const std::vector<std::complex<T>>& a, std::vector<std::complex<T>>* pout, bool is_inverse) {

		auto& F = *pout;
		//データを確保してビット反転した配列順データを代入する
		F.resize(N);
		for (size_t i = 0; i < N; ++i)
			F[i] = a[ids[i]];
		unsigned int po2 = 1;
		//バタフライ演算の回数分ループ
		for (int i_level = 0; i_level < level; ++i_level) {
			po2 <<= 1;                            //毎回シフト演算をすることで2のべき乗で変化
			const int po2m = po2 >> 1;            //ビット反転したものとどれだけ離れているか
			//exp()は最小限に
			auto w = std::exp(std::complex<T>(.0, 2 * M_PI / (T)po2));
			//逆変換の場合はwの複素共役にする
			w = is_inverse ? conj(w) : w;
			auto ws = std::complex<T>(1, 0);
			//バタフライ演算本体：Wを共有する項をまとめて計算
			//バタフライ分ループ
			for (int k = 0; k < po2m; ++k) {
				//全ての要素についてループ
				for (size_t j = 0; j < N; j += po2) {
					auto pa = &F[j + k];
					auto pb = &F[j + k + po2m];
					auto wfb = ws**pb;
					*pb = *pa - wfb;
					*pa += wfb;
				}
				ws *= w;                //指数関数の指数の増加
			}
		}
		return;
	}
	template <class T>
	inline void FFT::ifft(const std::vector<std::complex<T>>& a, std::vector<std::complex<T>>* pout) {
		fft(a, pout, !0);
		auto N = this->N;
		//Nで割る
		for_each(pout->begin(), pout->end(), [N](std::complex<T>& val) {val /= N; });
		return;
	}
}*/

#endif