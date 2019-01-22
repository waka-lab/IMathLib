#ifndef _INTERPOLATION_HPP
#define _INTERPOLATION_HPP

#include "IMathLib/math/liner_algebra.hpp"
#include "IMathLib/math/math.hpp"
#include "IMathLib/utility/algorithm.hpp"

//補間や近似を行うクラス(1価関数であることが前提)

namespace iml {
	/*
	//サンプリングデータの構造体の基底(型,データの次元)
	template <class T, size_t N>
	struct DiscreteDataBase {
	private:
		T		data[N];
	public:
		DiscreteDataBase() {}
		~DiscreteDataBase() {}

		//代入演算
		DiscreteDataBase& operator=(const DiscreteDataBase& n) {
			for (size_t i = 0; i < N; ++i)
				this->data[i] = n.data[i];
		}
		//比較演算
		bool operator==(const DiscreteDataBase& n) const { return (this->data[0] == n.data[0]); }
		bool operator!=(const DiscreteDataBase& n) const { return (this->data[0] != n.data[0]); }
		bool operator>(const DiscreteDataBase& n) const { return (this->data[0] > n.data[0]); }
		bool operator>=(const DiscreteDataBase& n) const { return (this->data[0] >= n.data[0]); }
		bool operator<(const DiscreteDataBase& n) const { return (this->data[0] < n.data[0]); }
		bool operator<=(const DiscreteDataBase& n) const { return (this->data[0] <= n.data[0]); }
	};
	//サンプリングデータの構造体
	template <class T, size_t N, class _Allocator = Allocator<T>>
	struct DiscreteData : public DynamicContainer<DiscreteDataBase<T, N>, _Allocator> {

	public:
		//イテレータの取得
		Iterator<T> begin() noexcept { return _p; }
		Const_iterator<T> begin() const noexcept { return _p; }
		Iterator<T> end() noexcept { return &_p[_use_size]; }
		Const_iterator<T> end() const noexcept { return &_p[_use_size]; }

		//データセット
		void set(const InitializerList<T>& data) {

		}
	};
	*/

	//補間曲線の類の名前空間
	namespace inter {

		//ニュートン補間
		template <class T>
		class newton {
			T*		x = nullptr;		//xのサンプリングデータ
			T*		y = nullptr;		//yのサンプリングデータ
			T*		a = nullptr;		//係数のデータ
			size_t	size = 0;			//現在のデータサイズ

			//係数配列の取得(算出するデータ位置,サイズ)
			void coefficient() {
				T* w = new T[size];			//係数算出用のバッファ
				for (size_t i = 0; i < size; ++i) {
					w[i] = y[i];
					//for (int j = i - 1; j >= 0; j--)
					//	w[j] = (w[j + 1] - w[j]) / (x[i] - x[j]);
					for (size_t j = 1; j <= i; ++j)
						w[i - j] = (w[i - j + 1] - w[i - j]) / (x[i] - x[i - j]);
					a[i] = w[0];
				}
				delete[] w;
			}
		public:
			newton() {}
			~newton() { delete[] x; delete[] y; delete[] a; }

			//データの設定
			void set(const T* x_data, const T* y_data, size_t size) {
				if (x != nullptr) { delete[] x; delete[] y; delete[] a; }
				this->size = size;
				x = new T[size];
				y = new T[size];
				a = new T[size];
				std::memcpy(x, x_data, sizeof(T)*size);
				std::memcpy(y, y_data, sizeof(T)*size);
				//係数の算出
				coefficient();
			}
			//補間された曲線のデータの取得
			T get(const T& _x) {
				T sum = a[size - 1];
				//for (int i = size - 2; i >= 0; i--)
				//	sum = sum * (_x - x[i]) + a[i];
				for (size_t i = 1; i <= size - 1; ++i)
					sum = sum * (_x - x[size - 1 - i]) + a[size - 1 - i];
				return sum;
			}
		};
		//ラグランジュ補間
		template <class T>
		class lagrange {
			T*		x = nullptr;		//xのサンプリングデータ
			T*		y = nullptr;		//yのサンプリングデータ
			size_t	size = 0;			//現在のデータサイズ
		public:
			lagrange() {}
			~lagrange() { delete[] x; delete[] y; }

			//データの設定
			void set(const T* x_data, const T* y_data, size_t size) {
				if (x != nullptr) { delete[] x; delete[] y; }
				this->size = size;
				x = new T[size];
				y = new T[size];
				std::memcpy(x, x_data, sizeof(T)*size);
				std::memcpy(y, y_data, sizeof(T)*size);
			}
			//補間された曲線のデータの取得
			T get(const T& _x) {
				T sum = 0, buf;
				for (size_t i = 0; i < size; ++i) {
					buf = y[i];
					for (size_t j = 0; j < size; ++j) {
						if (i == j) continue;
						buf *= (_x - x[j]) / (x[i] - x[j]);
					}
					sum += buf;
				}
				return sum;
			}
		};

		//N次スプライン補間
		template <class T, size_t N>
		class spline {
			T*      x = nullptr;        //xのサンプリングデータ
			T*      y = nullptr;        //yのサンプリングデータ
			T*		a = nullptr;		//係数配列
			size_t  size = 0;           //現在のデータサイズ

			//階乗モドキ
			static T fact_d(size_t n, size_t size) {
				if (n == 0) return 1;
				if (size == 0 || size == 1) return n;
				return n*fact_d(n - 1, size);
			}
			//ポインタの二次配列の逆行列
			static void matrix_inverse(T** ma, size_t size) {
				T buf;
				const size_t n = size;
				T** inv;

				inv = new T*[size];
				for (size_t i = 0; i < size; ++i)
					inv[i] = new T[size];
				//単位行列生成
				for (size_t i = 0; i < size; ++i)
					for (size_t j = 0; j < size; ++j)
						inv[i][j] = (i == j);

				for (size_t i = 0; i < n; i++) {			//上三角行列に変換
					//成分を0にする
					for (size_t j = 0; j < i; j++) {
						if ((buf = ma[i][j]) == 0) continue;
						buf /= ma[j][j];
						for (size_t l = 0; l < n; ++l) {
							ma[i][l] -= ma[j][l] * buf;
							inv[i][l] -= inv[j][l] * buf;
						}
					}
					for (size_t k = i + 1; (ma[i][i] == 0) && (k < n); ++k) {
						//行を交換して対角成分を0でなくする
						for (size_t l = 0; l < n; ++l) {
							buf = ma[i][l];
							ma[i][l] = ma[k][l];
							ma[k][l] = buf;
							buf = inv[i][l];
							inv[i][l] = inv[k][l];
							inv[k][l] = buf;
						}
						//成分を0にする
						for (size_t j = 0; j < i; j++) {
							buf = ma[i][j] / ma[j][j];
							for (size_t k = 0; k < n; k++) {
								ma[i][k] -= ma[j][k] * buf;
								inv[i][k] -= inv[j][k] * buf;
							}
						}
					}
					if (ma[i][i] == 0) goto FINISH;		//正則判定
					//対角成分を正規化
					buf = 1 / ma[i][i];
					for (size_t k = 0; k < n; k++) {
						ma[i][k] *= buf;
						inv[i][k] *= buf;
					}
				}
				for (size_t i = 0; i < n; ++i) {		//対角成分以外を1にする
					for (size_t j = i + 1; j < n; ++j) {
						buf = ma[i][j];
						for (size_t k = 0; k < n; k++) {
							ma[i][k] -= ma[j][k] * buf;
							inv[i][k] -= inv[j][k] * buf;
						}
					}
				}
				//代入
				for (size_t i = 0; i < size; ++i)
					for (size_t j = 0; j < size; ++j)
						ma[i][j] = inv[i][j];
			FINISH:
				for (size_t i = 0; i < size; ++i)
					delete[] inv[i];
				delete[] inv;
			}

			//係数配列の取得(算出するデータ位置,サイズ)
			void coefficient() {
				T **ma;				//行列
				T *b;				//定数成分
				T buf;
				size_t k;			//現在構成中の行
									//演算用のメモリ確保
				ma = new T*[(size - 1)*N];
				for (size_t i = 0; i < (size - 1)*N; ++i) {
					ma[i] = new T[(size - 1)*N];
					std::memset(ma[i], 0, sizeof(T)*(size - 1)*N);		//ほとんどの成分は0のため
				}
				b = new T[size - 1];			//ほとんどの成分は0のため最小限の配列
												//定数成分の構成
				for (size_t i = 0; i < size - 1; ++i)
					b[i] = y[i + 1] - y[i];
				//行列の構成
				//点を通る
				for (k = 0; k < size - 1; ++k) {
					buf = x[k + 1] - x[k];
					for (size_t j = 0; j < N; ++j) {
						ma[k][k*N + j] = buf;
						buf *= (x[k + 1] - x[k]);
					}
				}
				//微分係数の一致
				//曲線の数の分-1
				for (size_t i = 0; i < size - 1 - 1; ++i) {
					//n階微分の分
					for (size_t j = 0; j < N - 1; ++j, ++k) {
						buf = x[i + 1] - x[i];
						//各成分(最初の1は別のところで処理)
						for (size_t l = j + 1; l < N; ++l) {
							ma[k][i*N + l] = fact_d(l + 1, j)*buf;
							buf *= (x[i + 1] - x[i]);
						}
						ma[k][i*N + j] = 1;				//1の成分
						ma[k][(i + 1)*N + j] = -1;		//-1の成分
					}
				}
				//次数が深い方から両端の微分係数が0
				for (size_t i = 0; i < N - 1; ++i, ++k) {
					//偶数奇数で処理を分ける(N - 1 - (i >> 1)次導関数の微分係数の代入)
					if (i & 1) {
						buf = x[size - 1] - x[size - 2];
						//各成分(最初の1は別のところで処理)(jは(i >> 1)となる)
						for (size_t l = (N - 1) - (i >> 1); l < N; ++l) {
							ma[k][(size - 2)*N + l] = fact_d(l + 1, (N - 1) - (i >> 1) - 1)*buf;
							buf *= (x[size - 1] - x[size - 2]);
						}
						ma[k][(size - 2)*N + (N - 1) - ((i >> 1) + 1)] = 1;				//1の成分
					}
					else ma[k][(N - 1) - ((i >> 1) + 1)] = 1;				//1の成分
				}
				//ma[k][N - 1 - 1] = 1; ma[k][N - 1] = N*(x[0] - x[0]); ++k;
				//ma[k][(size - 1)*N - 1 - 1] = 1; ma[k][(size - 1)*N - 1] = N*(x[size - 1] - x[size - 2]); ++k;
				//逆行列
				matrix_inverse(ma, (size - 1)*N);
				//係数成分の格納
				for (size_t i = 0; i < size - 1; ++i) {
					a[i*(N + 1)] = y[i];
					for (size_t j = 0; j < N; ++j) {
						//行列とベクトルで変換する
						buf = 0;
						for (size_t l = 0; l < size - 1; ++l)
							buf += ma[i*N + j][l] * b[l];
						a[i*(N + 1) + j + 1] = buf;
					}
				}
				for (size_t i = 0; i < (size - 1)*N; ++i)
					delete[] ma[i];
				delete[] ma;
				delete[] b;
			}
		public:
			spline() {}
			~spline() { delete[] x; delete[] y; delete[] a; }

			//データの設定
			void set(const T* x_data, const T* y_data, size_t size) {
				if (x != nullptr) { delete[] x; delete[] y; delete[] a; }
				this->size = size;
				x = new T[size];
				y = new T[size];
				a = new T[(size - 1)*(N + 1)];
				std::memcpy(x, x_data, sizeof(T)*size);
				std::memcpy(y, y_data, sizeof(T)*size);
				//係数の取得
				coefficient();
			}
			//補間された曲線のデータの取得
			T get(const T& _x) {
				T sum = 0, buf;
				size_t index = 0;
				//配列(区分領域)のインデックスの取得
				while (_x >= x[index]) { ++index; if (index >= size) break; } --index;
				if (index >= size - 1) index = size - 2;
				for (size_t i = 0; i <= N; ++i) {
					buf = a[index*(N + 1) + i];
					for (size_t j = 0; j < i; ++j)
						buf *= (_x - x[index]);
					sum += buf;
				}
				return sum;
			}
		};

		//n次多項式による最小二乗法
		template <class T, size_t N>
		class least_squares {
			vector<T, N + 1>	a;			//曲線の次数

		public:
			least_squares() {}
			~least_squares() {}

			//データの設定
			void set(const T* x_data, const T* y_data, size_t size) {
				matrix<T, N + 1, N + 1> X;
				vector<T, N + 1> Y;
				T buf;
				//冪乗和の部分の算出
				for (size_t i = 0; i <= 2 * N; ++i) {
					//和の部分
					buf = 0;
					for (size_t j = 0; j < size; ++j)
						buf += pow(x_data[j], i);
					//折り返しを考慮して代入
					if (i < N + 1) for (size_t j = 0; j <= i; ++j) X[j][i - j] = buf;
					else for (size_t j = 0; j <= 2 * N - i; ++j) X[N - j][i - (N - j)] = buf;
				}
				X = MInverse(X);			//逆行列に変換
											//ベクトルの構成
				for (size_t i = 0; i <= N; ++i) {
					//和の部分
					buf = 0;
					for (size_t j = 0; j < size; ++j)
						buf += y_data[j] * pow(x_data[j], i);
					Y[i] = buf;				//代入
				}
				a = X*Y;
			}
			//補間された曲線のデータの取得
			T get(const T& _x) {
				T sum = 0, buf;
				for (size_t i = 0; i <= N; ++i) {
					buf = a[i];
					for (size_t j = 0; j < i; ++j)
						buf *= _x;
					sum += buf;
				}
				return sum;
			}
		};

	}
}

#endif