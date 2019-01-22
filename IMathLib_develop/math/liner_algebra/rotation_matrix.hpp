#ifndef _IMATH_LINER_ALGEBRA_ROTATION_MATRIX_HPP
#define _IMATH_LINER_ALGEBRA_ROTATION_MATRIX_HPP

#include "IMathLib/math/math.hpp"
#include "IMathLib/math/liner_algebra/vector.hpp"
#include "IMathLib/math/liner_algebra/matrix.hpp"


namespace iml {

	//2次元回転行列
	template <class T>
	inline auto rotation_matrix2(const T& radian) {
		using result_type = typename math_function_type<T>::type;
		result_type temp_c = cos(radian), temp_s = sin(radian);
	
		return matrix2<result_type>(
			temp_c, -temp_s
			, temp_s, temp_c
			);
	}
	template <class T>
	inline auto rotation_matrix2(const T& radian, const vector2<T>& shift) {
		using result_type = typename math_function_type<T>::type;
		result_type temp_c = cos(radian), temp_s = sin(radian);

		return matrix3<result_type>(
			temp_c, -temp_s, shift[0]
			, temp_s, temp_c, shift[1]
			, 0, 0, 1
			);
	}


	//3次元回転行列
	template <class T>
	inline auto rotation_matrix3_1(const T& radian) {
		using result_type = typename math_function_type<T>::type;
		result_type temp_c = cos(radian), temp_s = sin(radian);

		return matrix3<result_type>(
			1, 0, 0
			, 0, temp_c, -temp_s
			, 0, temp_s, temp_c
			);
	}
	template <class T>
	inline auto rotation_matrix3_1(const T& radian, const vector3<T>& shift) {
		using result_type = typename math_function_type<T>::type;
		result_type temp_c = cos(radian), temp_s = sin(radian);

		return matrix4<result_type>(
			1, 0, 0, shift[0]
			, 0, temp_c, -temp_s, shift[1]
			, 0, temp_s, temp_c, shift[2]
			, 0, 0, 0, 1
			);
	}
	template <class T>
	inline auto rotation_matrix3_2(const T& radian) {
		using result_type = typename math_function_type<T>::type;
		result_type temp_c = cos(radian), temp_s = sin(radian);

		return matrix3<result_type>(
			temp_c, 0, temp_s
			, 0, 1, 0
			, -temp_s, 0, temp_c
			);
	}
	template <class T>
	inline auto rotation_matrix3_2(const T& radian, const vector3<T>& shift) {
		using result_type = typename math_function_type<T>::type;
		result_type temp_c = cos(radian), temp_s = sin(radian);

		return matrix4<result_type>(
			temp_c, 0, temp_s, shift[0]
			, 0, 1, 0, shift[1]
			, -temp_s, 0, temp_c, shift[2]
			, 0, 0, 0, 1
			);
	}
	template <class T>
	inline auto rotation_matrix3_3(const T& radian) {
		using result_type = typename math_function_type<T>::type;
		result_type temp_c = cos(radian), temp_s = sin(radian);

		return matrix3<result_type>(
			temp_c, -temp_s, 0
			, temp_s, temp_c, 0
			, 0, 0, 1
			);
	}
	template <class T>
	inline auto rotation_matrix3_3(const T& radian, const vector3<T>& shift) {
		using result_type = typename math_function_type<T>::type;
		result_type temp_c = cos(radian), temp_s = sin(radian);

		return matrix4<result_type>(
			temp_c, -temp_s, 0, shift[0]
			, temp_s, temp_c, 0, shift[1]
			, 0, 0, 1, shift[2]
			, 0, 0, 0, 1
			);
	}


	//任意の軸による回転(v:回転するベクトル,ax:回転軸の単位ベクトル,center:回転の中心,rad:回転角)
	template <class T>
	inline static auto rotation_vector(const vector3<T>& v, const vector3<T>& ax, const vector3<T>& center, T rad) {
		using result_type = typename math_function_type<T>::type;
		result_type s = sin(rad / 2), c = cos(rad / 2);

		vector3<result_type> temp1 = v - center;
		vector3<result_type> temp2 = s * ax;
		vector3<result_type> temp3 = c * temp1 + temp2 % temp1;
		return c * temp3 + (temp1*temp2) * temp2 - temp3 % temp2 + center;
	};


	//任意の回転面によるベクトルの回転
	template <class T, imsize_t N>
	inline auto rotation_vector(const vector<T, N>& v, const vector<T, N>& x, const vector<T, N>& y, const T& radian) {
		using result_type = typename math_function_type<T>::type;
		result_type temp_c = cos(radian), temp_s = sin(radian);
		vector2<T> temp_v(v*x, v*y), result(v);
		matrix2<T> temp_m(
			temp_c - 1, -temp_s
			, temp_s, temp_c - 1
		);
		temp_v = temp_m * temp_v;
		for (imsize_t i = 0; i < N; ++i) result[i] += x[i] * temp_v[0] + y[i] * temp_v[1];

		return result;
	}

}

#endif