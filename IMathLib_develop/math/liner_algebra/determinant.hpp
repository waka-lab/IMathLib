#ifndef _IMATH_LINER_ALGEBRA_DETERMINANT_HPP
#define _IMATH_LINER_ALGEBRA_DETERMINANT_HPP

#include "IMathLib/math/liner_algebra/matrix.hpp"


namespace iml {

	//行列式
	template <class T>
	inline auto determinant(const matrix<T, 1, 1>& ma) {
		return ma[0][0];
	}
	template <class T>
	inline auto determinant(const matrix2<T>& ma) {
		return ma[0][0] * ma[1][1] - ma[0][1] * ma[1][0];
	}
	template <class T>
	inline auto determinant(const matrix3<T>& ma) {
		return (ma[0][0] * ma[1][1] * ma[2][2]) + (ma[1][0] * ma[2][1] * ma[0][2]) + (ma[2][0] * ma[0][1] * ma[1][2])
			- (ma[2][0] * ma[1][1] * ma[0][2]) - (ma[1][0] * ma[0][1] * ma[2][2]) - (ma[0][0] * ma[2][1] * ma[1][2]);
	}
	//アルゴリズム的都合上浮動小数点でなければならない
	template <class T, imsize_t N>
	inline T determinant(matrix<T, N, N> ma) {
		T result = 1;

		//上三角行列にに変形しながら対角成分の積をとる
		for (imsize_t i = 0; i < N; ++i) {
			//対角成分が0であれば0でない行と交換する
			if (ma[i][i] == 0) {
				imsize_t j = i + 1;
				for (; j < N; ++j) {
					//0でない項が存在するとき行を交換
					if (ma[j][i] != 0) {
						for (imsize_t k = i; k < N; ++k) swap(ma[i][k], ma[j][k]);
						result *= -1;
						break;
					}
				}
				//交換できなかった時は行列式は0
				if (j == N) return 0;
			}

			//第i列の対角成分より下を0にする
			for (imsize_t j = i + 1; j < N; ++j) {
				if (ma[j][i] != 0) {
					//i列目から順に引いていく(i列より左は全て0であるため計算しない)
					//実際はi列は以後0となるため計算する必要がない
					T temp = ma[j][i] / ma[i][i];
					for (imsize_t k = i + 1; k < N; ++k) ma[j][k] -= ma[i][k] * temp;
				}
			}

			result *= ma[i][i];
		}
	
		return result;
	}

}

#endif