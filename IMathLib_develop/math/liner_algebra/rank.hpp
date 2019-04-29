#ifndef _IMATH_LINER_ALGEBRA_RANK_HPP
#define _IMATH_LINER_ALGEBRA_RANK_HPP

#include "IMathLib/math/liner_algebra/matrix.hpp"


namespace iml {

	//ランク
	template <class T, size_t M, size_t N>
	inline size_t rank(matrix<T, M, N> ma) {
		size_t L = min(M, N);
		size_t dim = 0;				//退化次元

		//行列式ベースで求める
		for (size_t i = 0; i < L; ++i) {
			//対角成分が0であれば0でない行と交換する
			if (ma[i][i] == 0) {
				size_t j = i + 1;
				for (; j < L; ++j) {
					//0でない項が存在するとき行を交換
					if (ma[j][i] != 0) {
						for (size_t k = i; k < L; ++k) swap(ma[i][k], ma[j][k]);
						break;
					}
				}
				//交換できなかった時は退化次元を増やす
				if (j == L) { ++dim; continue; }
			}

			//第i列の対角成分より下を0にする
			for (size_t j = i + 1; j < L; ++j) {
				if (ma[j][i] != 0) {
					//i列目から順に引いていく(i列より左は全て0であるため計算しない)
					//実際はi列は以後0となるため計算する必要がない
					T temp = ma[j][i] / ma[i][i];
					for (size_t k = i + 1; k < L; ++k) ma[j][k] -= ma[i][k] * temp;
				}
			}
		}

		return L - dim;
	}
}

#endif