#ifndef _IMATH_LINER_ARALGEBRA_HPP
#define _IMATH_LINER_ARALGEBRA_HPP

//線型代数演算をする関数群(右手系)


#include "IMathLib/math/math.hpp"

#include "IMathLib/math/liner_algebra/vector.hpp"
#include "IMathLib/math/liner_algebra/matrix.hpp"

#include "IMathLib/math/liner_algebra/determinant.hpp"
#include "IMathLib/math/liner_algebra/exp.hpp"
#include "IMathLib/math/liner_algebra/identity_matrix.hpp"
#include "IMathLib/math/liner_algebra/inverse_matrix.hpp"
#include "IMathLib/math/liner_algebra/norm.hpp"
#include "IMathLib/math/liner_algebra/product_matrix.hpp"
#include "IMathLib/math/liner_algebra/projection.hpp"
#include "IMathLib/math/liner_algebra/rank.hpp"
#include "IMathLib/math/liner_algebra/rotation_matrix.hpp"


namespace iml {

	//一般正方行列の対角化の結果の構造体(pairの方がいいか検討中)
	template <class T, size_t N>
	struct diagonalization_struct {
		matrix<T, N, N>	p;				//対角化行列
		matrix<T, N, N>	eigenvalue;		//固有値のみの行列
		diagonalization_struct() {}
		diagonalization_struct(const matrix<T, N, N>& p, const matrix<T, N, N>& e) :p(p), eigenvalue(e) {}

		diagonalization_struct& operator=(const diagonalization_struct& d) {
			p = d.p; eigenvalue = d.eigenvalue;
			return *this;
		}
		
	};
	template <class T>
	using diagonalization_struct2 = diagonalization_struct<T, 2>;
	template <class T>
	using diagonalization_struct3 = diagonalization_struct<T, 3>;
	template <class T>
	using diagonalization_struct4 = diagonalization_struct<T, 4>;

	//衝突判定用の構造体
	template <class T>
	struct hit_polygon {
		bool			flag;
		vector3<T>		pos;
		constexpr hit_polygon() :flag(false) {}
		hit_polygon(bool flag, const vector3<T>& pos = vector3<T>()) {
			this->flag = flag; this->pos = pos;
		}
	};

}

namespace iml {

	template <class T, size_t N>
	struct math_function<vector<T, N>> {
		using type = vector<T, N>;

		//正規化をする(単位ベクトルの取得)
		static type __unit(const type& v) {
			T size = abs(v);
			return ((size == 0) ? type() : v / size);
		}
		//クロネッカー積
		template <size_t P>
		static matrix<T, N, P> __kronecker_product(const type& v1, const vector<T, P>& v2) {
			matrix<T, N, P> result;
			for (size_t i = 0; i < N; ++i)
				for (size_t j = 0; j < P; ++j)
					result[i][j] = v1[i] * v2[j];
			return result;
		}
		//アダマール積
		static type __hadamard_product(const type& v1, const type& v2) {
			type result;
			for (size_t i = 0; i < N; ++i)
				result[i] = v1[i] * v2[i];
			return result;
		}

		//三角形の面積
		static auto __triangle_area(const type& v1, const type& v2, const type& v3) {
			return __triangle_area(v1 - v2, v1 - v3);
		}
		static auto __triangle_area(const type& v1, const type& v2) {
			return sqrt(pow(__abs(v1), 2)*pow(__abs(v2), 2) - pow(v1*v2, 2)) / 2;
		}

		//面法線ベクトルの取得(v1,v2,v3で右ねじ方向)
		static type __surface_normal(const type& v1, const type& v2, const type& v3) {
			static_assert(N == 3, "The size of a vector must be 3.");
			return (v2 - v1) % (v3 - v1);
		}
	public:
		//線分と平面の衝突
		static hit_polygon<T> __hit_position_line_surface(const type& start, const type& end, const type& x1, const type& x2, const type& x3) {
			static_assert(N == 3, "The size of a vector must be 3.");
			type n = __surface_normal(x1, x2, x3);			//法線ベクトル
			type v = end - start;							//方向ベクトル
			T t = n*v;
			if (t == 0) return hit_polygon<T>(false);			//平面と平行かの判定
			t = n*(x1 - start) / t;
			if ((t < 0) || (t > 1)) return hit_polygon<T>(false);		//線分の延長線上に存在するかの判定
			return hit_polygon<T>(true, start + t*v);
		}
		//三角平面と線分の衝突
		static hit_polygon<T> __hit_check_line_triangle(const type& start, const type& end, const type& x1, const type& x2, const type& x3) {
			static_assert(N == 3, "The size of a vector must be 3.");
			type n = __surface_normal(x1, x2, x3);			//法線ベクトル
			type v = end - start;							//方向ベクトル
			T t = n * v;
			if (t == 0) return hit_polygon<T>(false);			//平面と平行かの判定
			t = n * (x1 - start) / t;
			if ((t < 0) || (t > 1)) return hit_polygon<T>(false);		//線分の延長線上に存在するかの判定
			type p = start + t * v;
			//外積の方向ベクトルが皆等しい
			return hit_polygon<T>((((p - x1) % (x3 - x1))*n > 0) && (((p - x2) % (x1 - x2))*n > 0) && ((p - x3) % (x2 - x3)*n > 0), p);
		}
	};
}

namespace iml {

	template <class T, size_t M, size_t N>
	struct math_function<matrix<T, M, N>> {
		using type = matrix<T, M, N>;

		//単位行列の取得
		static type __identity_matrix() {
			return multiplication_traits<type>::identity_element();
		}
		//トレースの取得
		static T __trace(const type& ma) {
			static_assert(N == M, "The size of a matrix must be N==M.");
			T tr = 0;
			for (size_t i = 0; i < N; ++i)
				tr += ma[i][i];
			return tr;
		}
		//転置行列の取得
		static matrix<T, N, M> __transpose_matrix(const type& ma) {
			matrix<T, N, M> buf;
			for (size_t i = 0; i < N; ++i)
				for (size_t j = 0; j < M; ++j)
					buf[i][j] = ma[j][i];
			return buf;
		}
		//クロネッカー積
		template <size_t P, size_t Q>
		static matrix<T, M*P, N*Q> __kronecker_product(const type& ma1, const matrix<T, P, Q>& ma2) {
			matrix<T, M*P, N*Q> result;
			for (size_t i = 0; i < M; ++i)
				for (size_t j = 0; j < N; ++j)
					for (size_t k = 0; k < P; ++k)
						for (size_t l = 0; l < Q; ++l)
							result[P*i + k][Q*j + l] = ma1[i][j] * ma2[k][l];
			return result;
		}
		//アダマール積
		static type __hadamard_product(const type& ma1, const type& ma2) {
			type result;
			for (size_t i = 0; i < M; ++i)
				for (size_t j = 0; j < N; ++j)
				result[i][j] = ma1[i][j] * ma2[i][j];
			return result;
		}
	private:
		//ハウスホルダー変換(戻り値は変換後の第一成分)
		static T __householder(T* v, size_t dim) {
			T	x_0 = 0;		//第一成分
			T	a;				//定数a

			//ベクトルの内積の取得
			for (size_t i = 0; i < dim; ++i)
				x_0 += v[i] * v[i];
			x_0 = -((v[0] >= 0) ? 1 : -1)*sqrt(x_0);
			if ((x_0 == 0) || ((v[0] + x_0) == 0)) return 0;

			//aの値の算出
			v[0] += x_0;
			a = 1 / (sqrt(2 * x_0 * v[0]));
			//単位法線ベクトルuの作成
			for (size_t i = 0; i < dim; i++)
				v[i] *= a;

			return -x_0;
		}
		//右下の2*2の小行列で右下隅に一番近い固有値を算出
		static T __eigenvalue2_2(T _00, T _01, T _10, T _11) {
			T b = _00 + _11;
			T det = sqrt(b*b - 4 * (_00*_11 - _01*_10));
			return ((abs(_11 - ((b - det) / 2)) < abs(_11 - ((b + det) / 2))) ? (b - det) / 2 : (b + det) / 2);
		}
		//法線ベクトルに対する行列の更新
		static void __mult(matrix<T, M, N>* x, T *v, size_t dim) {
			size_t	offset = M - dim;			//行列とベクトルの次元のオフセット
			T		temp = 0;
			T*		g = new T[dim];

			//dの取得
			for (size_t i = offset; i < M; ++i) {
				g[i - offset] = 0;
				for (size_t j = offset; j < M; ++j)
					g[i - offset] += (*x)[i][j] * v[j - offset];
			}
			//dとvの内積の取得
			for (size_t i = 0; i < dim; ++i)
				temp += g[i] * v[i];
			//gの取得
			for (size_t j = 0; j < dim; ++j)
				g[j] = 2 * (g[j] - v[j] * temp);
			//行列の更新
			for (size_t i = offset; i < M; ++i)
				for (size_t j = offset; j < M; j++)
					(*x)[i][j] -= (v[i - offset] * g[j - offset] + g[i - offset] * v[j - offset]);

			delete[] g;
		}
		//三重対角化(戻り値は変換行列)
		static matrix<T, M, N> __tridiagonalization(matrix<T, M, N>* ma) {
			vector<T, M>		v;
			size_t				v_dim = M;
			T					temp;
			matrix<T, M, N>		trans(__identity_matrix());		//変換行列

			for (size_t i = 0; i < M - 2; ++i) {
				v = vector<T, M>();
				//ハウスホルダー変換をする行の部分を取得
				--v_dim;
				for (size_t j = i + 1; j < M; ++j)
					v[j] = (*ma)[i][j];
				if (!(temp = __householder(&v.begin()[M - v_dim], v_dim))) continue;
				//ハウスホルダー変換による小行列の更新
				__mult(ma, &v.begin()[M - v_dim], v_dim);
				//小行列以外の更新
				(*ma)[i][i + 1] = (*ma)[i + 1][i] = temp;
				for (size_t j = i + 2; j < M; ++j)
					(*ma)[i][j] = (*ma)[j][i] = 0;
				//対角化行列
				trans *= (__identity_matrix() - 2 * math_function<vector<T, M>>::__kronecker_product(v, v));
			}
			return trans;
		}
		//上三角行列と直交行列に変換
		static void __change_AQ(matrix<T, M, N>* a, matrix<T, M, N>* q, size_t dim) {
			T alpha, s, c;
			T temp;

			for (size_t i = 0; i < dim - 1; i++) {
				alpha = sqrt((*a)[i][i] * (*a)[i][i] + (*a)[i + 1][i] * (*a)[i + 1][i]);
				s = alpha ? (*a)[i + 1][i] / alpha : alpha;
				c = alpha ? (*a)[i][i] / alpha : alpha;

				for (size_t j = i + 1; j < dim; j++) {
					temp = -(*a)[i][j] * s + (*a)[i+1][j] * c;
					(*a)[i][j] = (*a)[i][j] * c + (*a)[i+1][j] * s;
					(*a)[i + 1][j] = temp;
				}

				for (size_t j = 0; j < dim; j++) {
					temp = -(*q)[j][i] * s + (*q)[j][i+1] * c;
					(*q)[j][i] = (*q)[j][i] * c + (*q)[j][i+1] * s;
					(*q)[j][i + 1] = temp;
				}
				(*a)[i][i] = alpha;
				(*a)[i + 1][i] = 0;

			}
		}
	public:
		//対角化行列と対応する固有値行列の取得
		static diagonalization_struct<T, N> __diagonalization(const type& ma, const T& epsilon) {
			static_assert(N == M, "The size of a matrix must be N==M.");
			size_t				dim = M;				//現在扱っている小行列の大きさ
			matrix<T, M, N>		q;
			vector<T, M>		v;
			diagonalization_struct<T, M>	result(__identity_matrix(), ma);

			//三重対角化をする
			result.p = __tridiagonalization(&result.eigenvalue);

			while (dim > 1) {
				T u;

				//誤差がEpsilon以下ならば次元を下げて次のステップに行く
				if (abs(result.eigenvalue[dim - 1][dim - 2]) < epsilon) {
					--dim; continue;
				}
				//右下の2*2の小行列で右下隅に一番近い固有値を算出
				u = __eigenvalue2_2(result.eigenvalue[dim - 2][dim - 2], result.eigenvalue[dim - 2][dim - 1]
					, result.eigenvalue[dim - 1][dim - 2], result.eigenvalue[dim - 1][dim - 1]);
				//対角成分から値を引く
				for (size_t i = 0; i < dim; ++i)
					result.eigenvalue[i][i] -= u;
				//maを上三角行列に、qを直交行列にする
				q = __identity_matrix();
				__change_AQ(&result.eigenvalue, &q, dim);
				//ma*q(RQ)の行列演算をして新しいma(A)を作る
				for (size_t i = 0; i < dim; ++i) {
					T sum = 0;
					for (size_t j = 0; j < dim; ++j) {
						for (size_t k = i; k < dim; k++)
							sum += result.eigenvalue[i][k]*q[k][j];
						v[j] = sum;
						sum = 0;
					}
					for (size_t j = 0; j < dim; ++j)
						result.eigenvalue[i][j] = v[j];
				}
				//対角成分を戻す
				for (size_t i = 0; i < dim; ++i)
					result.eigenvalue[i][i] += u;
				//対角化行列
				result.p *= q;
			}
			return result;
		}
		//行列指数関数
		static type __exp(const type& ma) {
			static_assert(N == M, "The size of a matrix must be N==M.");
			type x1 = __identity_matrix(), x2 = __identity_matrix(), buf;
			for (size_t i = 1; x2 != buf; ++i) {
				buf = x2;
				x1 *= (x / i);
				x2 += x1;
			}
			return x2;
		}
	};

}

//関数の実装
namespace iml {

	//単位ベクトルの取得
	template <class T, size_t N>
	inline vector<T, N> unit(const vector<T, N>& v) { return math_function<vector<T, N>>::__unit(v); }
	//三角形の面積
	template <class T, size_t N>
	inline auto triangle_area(const vector<T, N>& v1, const vector<T, N>& v2, const vector<T, N>& v3) {
		return math_function<vector<T, N>>::__triangle_area(v1, v2, v3);
	}
	template <class T, size_t N>
	inline auto triangle_area(const vector<T, N>& v1, const vector<T, N>& v2) {
		return math_function<vector<T, N>>::__triangle_area(v1, v2);
	}
	//面法線ベクトルの取得
	template <class T>
	inline vector3<T> face_normal(const vector3<T>& v1, const vector3<T>& v2, const vector3<T>& v3) {
		return math_function<vector3<T>>::__surface_normal(v1, v2, v3);
	}

	//トレースの取得
	template <class T, size_t N>
	inline T trace(const matrix<T, N, N>& ma) { return math_function<matrix<T, N, N>>::__trace(ma); }
	//転置行列の取得
	template <class T, size_t M, size_t N>
	inline matrix<T, N, M> transpose_matrix(const matrix<T, M, N>& ma) { return math_function<matrix<T, M, N>>::__transpose_matrix(ma); }
	//ベクトルの直積(クロネッカー積)
	template <class T, size_t N, size_t P>
	inline matrix<T, N, P> kronecker_product(const vector<T, N>& u, const vector<T, P>& v) {
		return math_function<vector<T, N>>::__kronecker_product(u, v);
	}
	//クロネッカー積
	template <class T, size_t M, size_t N, size_t P, size_t Q>
	inline matrix<T, M*P, N*Q> kronecker_product(const matrix<T, M, N>& ma1, const matrix<T, P, Q>& ma2) {
		return math_function<matrix<T, M, N>>::__kronecker_product(ma1, ma2);
	}
	//アダマール積
	template <class T, size_t N>
	inline vector<T, N> hadamard_product(const vector<T, N>& v1,const vector<T, N>& v2) {
		return math_function<vector<T, N>>::__hadamard_product(v1, v2);
	}
	template <class T, size_t M, size_t N>
	inline matrix<T, M, N> hadamard_product(const matrix<T, M, N>& ma1, const matrix<T, M, N>& ma2) {
		return math_function<matrix<T, M, N>>::__hadamard_product(ma1, ma2);
	}

	//対角化行列と固有値の取得
	template <class T, size_t N>
	inline diagonalization_struct<T, N> diagonalization(const matrix<T, N, N>& ma, const T& epsilon) {
		return math_function<matrix<T, N, N>>::__diagonalization(ma, epsilon);
	}
	//線分と平面の衝突
	template <class T>
	inline hit_polygon<T> hit_position_line_surface(const vector3<T>& start, const vector3<T>& end, const vector3<T>& x1, const vector3<T>& x2, const vector3<T>& x3) {
		return math_function<vector3<T>>::__hit_position_line_surface(start, end, x1, x2, x3);
	}
	//三角平面と線分の衝突
	template <class T>
	inline hit_polygon<T> hit_check_line_triangle(const vector3<T>& start, const vector3<T>& end, const vector3<T>& x1, const vector3<T>& x2, const vector3<T>& x3) {
		return math_function<vector3<T>>::__hit_check_line_triangle(start, end, x1, x2, x3);
	}

}



#endif