#ifndef _IMATH_LINER_ALGEBRA_INVERSE_MATRIX_HPP
#define _IMATH_LINER_ALGEBRA_INVERSE_MATRIX_HPP

#include "IMathLib/math/liner_algebra/matrix.hpp"
#include "IMathLib/math/liner_algebra/identity_matrix.hpp"


namespace iml {

	//逆行列
	template <class T, imsize_t N>
	inline matrix<T, N, N> inverse_matrix(matrix<T, N, N> ma) {
		matrix<T, N, N> result = identity_matrix<T, N>();

		//行列式ベースで計算
		for (imsize_t i = 0; i < N; ++i) {
			//対角成分が0であれば0でない行と交換する
			if (ma[i][i] == 0) {
				imsize_t j = i + 1;
				for (; j < N; ++j) {
					//0でない項が存在するとき行を交換
					if (ma[j][i] != 0) {
						//for (imsize_t k = i; k < N; ++k) swap(ma[i][k], ma[j][k]);
						//for (imsize_t k = 0; k < N; ++k) swap(result[i][k], result[j][k]);
						imsize_t k = 0;
						for (; k < i; ++k) swap(result[i][k], result[j][k]);
						for (; k < N; ++k) { swap(ma[i][k], ma[j][k]); swap(result[i][k], result[j][k]); }
						break;
					}
				}
				//交換できなかった時は非正則
				if (j == N) return matrix<T, N, N>();
			}

			//i行を対角成で正規化(対角成分は正規化する必要はない)
			//for (imsize_t j = i + 1; j < N; ++j) ma[i][j] /= ma[i][i];
			//for (imsize_t j = 0; j < N; ++j) result[i][j] /= ma[i][i];
			imsize_t l = 0;
			for (; l < i + 1; ++l) result[i][l] /= ma[i][i];
			for (; l < N; ++l) { ma[i][l] /= ma[i][i]; result[i][l] /= ma[i][i]; }

			//第i列の対角成分より上を0にする
			for (imsize_t j = 0; j < i; ++j) {
				if (ma[j][i] != 0) {
					//i列目から順に引いていく(i列より左は全て0であるため計算しない)
					//実際はi列は以後0となるため計算する必要がない
					//for (imsize_t k = i + 1; k < N; ++k) ma[j][k] -= ma[i][k] * ma[j][i];
					//for (imsize_t k = 0; k < N; ++k) result[j][k] -= result[i][k] * ma[j][i];
					imsize_t k = 0;
					for (; k < i + 1; ++k) result[j][k] -= result[i][k] * ma[j][i];
					for (; k < N; ++k) { ma[j][k] -= ma[i][k] * ma[j][i]; result[j][k] -= result[i][k] * ma[j][i]; }
				}
			}
			//第i列の対角成分より下を0にする
			for (imsize_t j = i + 1; j < N; ++j) {
				if (ma[j][i] != 0) {
					//i列目から順に引いていく(i列より左は全て0であるため計算しない)
					//実際はi列は以後0となるため計算する必要がない
					//for (imsize_t k = i + 1; k < N; ++k) ma[j][k] -= ma[i][k] * ma[j][i];
					//for (imsize_t k = 0; k < N; ++k) result[j][k] -= result[i][k] * ma[j][i];
					imsize_t k = 0;
					for (; k < i + 1; ++k) result[j][k] -= result[i][k] * ma[j][i];
					for (; k < N; ++k) { ma[j][k] -= ma[i][k] * ma[j][i]; result[j][k] -= result[i][k] * ma[j][i]; }
				}
			}
		}

		return result;
	}

}

#endif