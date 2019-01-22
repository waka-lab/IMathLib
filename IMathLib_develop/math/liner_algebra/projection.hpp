#ifndef _IMATH_LINER_ALGEBRA_PROJECTION_HPP
#define _IMATH_LINER_ALGEBRA_PROJECTION_HPP

#include "IMathLib/math/math.hpp"
#include "IMathLib/math/liner_algebra/matrix.hpp"
#include "IMathLib/math/liner_algebra/vector.hpp"

//流石にnearポインタとfarポインタはもうないはず(と思いたい)
#undef near
#undef far


//基本的にy-up系
//ビュー行列に対して座標入れ替え処理を行うことによりx-up系やz-up系を実装

namespace iml {

	//ビュー行列
	template <class T>
	inline matrix4<T> view_matrix(const vector3<T>& shift, const vector3<T>& ex, const vector3<T>& ey, const vector3<T>& ez) {
		//基底による逆行列(転置行列)を構築
		return matrix4<T>(
			ex[0], ex[1], ex[2], -ex * shift
			, ey[0], ey[1], ey[2], -ey * shift
			, ez[0], ez[1], ez[2], -ez * shift
			, 0, 0, 0, 1
			);
	}
	template <class T>
	inline matrix4<T> transpose_view_matrix(const vector3<T>& shift, const vector3<T>& ex, const vector3<T>& ey, const vector3<T>& ez) {
		//基底による逆行列(転置行列)を構築
		return matrix4<T>(
			ex[0], ey[0], ez[0], 0
			, ex[1], ey[1], ez[1], 0
			, ex[2], ey[2], ez[2], 0
			, -ex * shift, -ey * shift, -ez * shift, 1
			);
	}


	//透視投影による射影行列
	template <class T>
	inline matrix4<T> projection_frustum_matrix(const T& left, const T& right, const T& bottom, const T& top, const T& near, const T& far) {
		return matrix4<T>(
			2 * near / (right - left), 0, (right + left) / (right - left), 0
			, 0, 2 * near / (top - bottom), (top + bottom) / (top - bottom), 0
			, 0, 0, -(far + near) / (far - near), -2 * far*near / (far - near)
			, 0, 0, -1, 0
			);
	}
	template <class T>
	inline matrix4<T> transpose_projection_frustum_matrix(const T& left, const T& right, const T& bottom, const T& top, const T& near, const T& far) {
		return matrix4<T>(
			2 * near / (right - left), 0, 0, 0
			, 0, 2 * near / (top - bottom), 0, 0
			, (right + left) / (right - left), (top + bottom) / (top - bottom), -(far + near) / (far - near), -1
			, 0, 0, -2 * far*near / (far - near), 0
			);
	}
	//平行投影による射影行列
	template <class T>
	inline matrix4<T> projection_ortho_matrix(const T& left, const T& right, const T& bottom, const T& top, const T& near, const T& far) {
		return matrix4<T>(
			2 / (right - left), 0, 0, -(right + left) / (right - left)
			, 0, 2 / (top - bottom), 0, -(top + bottom) / (top - bottom)
			, 0, 0, -2 / (far - near), -(far + near) / (far - near)
			, 0, 0, 0, 1
			);
	}
	template <class T>
	inline matrix4<T> transpose_projection_ortho_matrix(const T& left, const T& right, const T& bottom, const T& top, const T& near, const T& far) {
		return matrix4<T>(
			2 / (right - left), 0, 0, 0
			, 0, 2 / (top - bottom), 0, 0
			, 0, 0, -2 / (far - near), 0
			, -(right + left) / (right - left), -(top + bottom) / (top - bottom), -(far + near) / (far - near), 1
			);
	}


	//ビューポート行列
	template <class T>
	inline matrix4<T> viewport_matrix(const T& x0, const T& y0, const T& width, const T& height, const T& near, const T& far) {
		//左上を(0,0)とするため-height/2とする
		return matrix4<T>(
			width / 2, 0, 0, x0 + width / 2
			, 0, -height / 2, 0, y0 + height / 2
			, 0, 0, (far - near) / 2, (far + near) / 2
			, 0, 0, 0, 1
			);
	}
	template <class T>
	inline matrix4<T> transpose_viewport_matrix(const T& x0, const T& y0, const T& width, const T& height, const T& near, const T& far) {
		//左上を(0,0)とするため-height/2とする
		return matrix4<T>(
			width / 2, 0, 0, 0
			, 0, -height / 2, 0, 0
			, 0, 0, (far - near) / 2, 0
			, x0 + width / 2, y0 + height / 2, (far + near) / 2, 1
			);
	}
}


namespace iml {

	//座標入れ替え行列
	template <class T>
	inline matrix4<T> xup_to_yup() {
		return matrix4<T>(
			0, 1, 0, 0
			, 0, 0, 1, 0
			, 1, 0, 0, 0
			, 0, 0, 0, 1
			);
	}
	template <class T>
	inline matrix4<T> transpose_xup_to_yup() {
		return matrix4<T>(
			0, 0, 1, 0
			, 1, 0, 0, 0
			, 0, 1, 0, 0
			, 0, 0, 0, 1
			);
	}
	template <class T>
	inline matrix4<T> zup_to_yup() {
		return matrix4<T>(
			0, 0, 1, 0
			, 1, 0, 0, 0
			, 0, 1, 0, 0
			, 0, 0, 0, 1
			);
	}
	template <class T>
	inline matrix4<T> transpose_zup_to_yup() {
		return matrix4<T>(
			0, 1, 0, 0
			, 0, 0, 1, 0
			, 1, 0, 0, 0
			, 0, 0, 0, 1
			);
	}


	//フリップ行列(主にビューポート行列に対して)
	template <class T>
	inline matrix4<T> flip_x() {
		return matrix4<T>(
			-1, 0, 0, 0
			, 0, 1, 0, 0
			, 0, 0, 1, 0
			, 0, 0, 0, 1
			);
	}
	template <class T>
	inline matrix4<T> flip_y() {
		return matrix4<T>(
			1, 0, 0, 0
			, 0, -1, 0, 0
			, 0, 0, 1, 0
			, 0, 0, 0, 1
			);
	}
	template <class T>
	inline matrix4<T> flip_z() {
		return matrix4<T>(
			1, 0, 0, 0
			, 0, 1, 0, 0
			, 0, 0, -1, 0
			, 0, 0, 0, 1
			);
	}
}

#endif