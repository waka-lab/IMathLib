/*#include <IMathLib/IMathLib.hpp>

//ローレンツ方程式の定義
static double p = 10, r = 28, b = 1;
double __f1(double t, double x, double y, double z) { return -p * x + p * y; }
double __f2(double t, double x, double y, double z) { return -x * z + r * x - y; }
double __f3(double t, double x, double y, double z) { return  x * y - b * z; }

iml::ode::runge_kutta_method<double, double, double, double> ode(__f1, __f2, __f3);

iml::vector3<double> __f4(double t, const iml::vector3<double>& x) {
	return iml::vector3<double>(-p * x[0] + p * x[1], -x[0] * x[2] + r * x[0] - x[1], x[0] * x[1] - b * x[2]);
}
iml::ode::runge_kutta_method<double, iml::vector3<double>> ode2(__f4);

int main() {

	//初期値の設定(t=0,x=y=z=1)
	ode.set_ini_value(0, 1, 1, 1);

	std::ofstream ofs("C:\\Users\\Yoshiki\\Documents\\test.csv");

	for (size_t i = 0; i < 10000; ++i) {
		//現在の値の参照
		ofs << ode.get_l_value<0>()
			<< ',' << ode.get_l_value<1>()
			<< ',' << ode.get_l_value<2>() << std::endl;
		//値の更新
		ode.update(1 / 60.);
	}

	iml::sin(0.5);

	ofs.close();

	return 0;
}*/

//#ifdef ___________________

#define _IMATH_LOG_OUTPUT_
//#define _IMATH_INTERFACE_
#define _IMATH_INTERFACE_IMAGE_
#define _IMATH_INTERFACE_FONT_
#define _IMATH_INTERFACE_INPUT_
#define _IMATH_OPENGL_AND_SDL2_
#define _IMATH_GLEW_


#include <IMathLib/IMathLib.hpp>


#include "../resource.h"
#include "win_api.hpp"

//ボーンモデルのテスト(xyz:基底,pos:絶対位置,length:ボーンの長さ)
template <class T>
inline void draw_bone(const iml::vector3<T>& x, const iml::vector3<T>& y, const iml::vector3<T>& z, const iml::vector3<T>& pos, const T& length) {
	//基底情報から頂点配列を作成
	static const iml::vector3<T>	bone_vtx[6] = { {0,0,0},{0,1,0},{-0.1f,0.2f,0.1f},{0.1f,0.2f,0.1f},{0.1f,0.2f,-0.1f},{-0.1f,0.2f,-0.1f} };
	iml::vector3<T> vtx[6] = { bone_vtx[0] ,bone_vtx[1] ,bone_vtx[2] ,bone_vtx[3] ,bone_vtx[4] ,bone_vtx[5] };
	unsigned int index[] = { 2,3,1,2,0,3,4,5,1,4,0,5,2 };
	iml::matrix3<T>	tm;
	for (size_t i = 0; i < 3; ++i) {
		tm[i][0] = x[i];
		tm[i][1] = y[i];
		tm[i][2] = z[i];
	}
	tm *= length;
	vtx[0] = tm*vtx[0];
	vtx[1] = tm*vtx[1];
	vtx[2] = tm*vtx[2];
	vtx[3] = tm*vtx[3];
	vtx[4] = tm*vtx[4];
	vtx[5] = tm*vtx[5];

	glPushMatrix();

	glEnableClientState(GL_VERTEX_ARRAY);
	//アルファブレンドをする
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glVertexPointer(3, GL_FLOAT, sizeof(iml::vector3<T>), &vtx[0][0]);

	glTranslatef(pos[0], pos[1], pos[2]);
	//glRotatef(angle, axis_x, axis_y, asix_z);
	glDrawElements(GL_LINE_LOOP, 13, GL_UNSIGNED_INT, index);

	glDisable(GL_BLEND);
	glDisableClientState(GL_VERTEX_ARRAY);

	glPopMatrix();
}

namespace iml {

	//共役
	template <class T>
	struct _Conj<quaternion<T>> {
		static constexpr quaternion<T> __conj(const quaternion<T>& x) { return quaternion<T>(x[0], -x[1], -x[2], -x[3]); }
	};
}


//マップダイアログのコールバック
BOOL CALLBACK callback(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	//汎用
	UINT wmId;
	char buf_str[256];

	switch (msg) {
		//ダイアログ生成時の初期化処理
	case WM_INITDIALOG:
		InitCommonControls();
		break;
	}
	return FALSE;
}

static double p = 10, r = 28, b = 1;
double __f1(double t, double x, double y, double z) { return -p * x + p * y; }
double __f2(double t, double x, double y, double z) { return -x * z + r * x - y; }
double __f3(double t, double x, double y, double z) { return  x * y - b * z; }

union test_union {};
struct __struct {};
struct __struct2 :__struct {};

namespace iml {

	template <class T, bool>
	struct _Is_union {
	private:
		template <class _T>
		struct tester_struct : _T {};
		template <template<class> class _T1, class _T2> static auto tester(_T2*) -> decltype(declval<_T1<_T2>>(), false_type());
		template <template<class> class _T1, class _T2> static true_type tester(...);
	public:
		static constexpr bool value = decltype(tester<tester_struct, T>(nullptr))::value;
	};
	template <class T>
	struct _Is_union<T, false> {
		static constexpr bool value = false;
	};
	template <class T>
	struct _is_union : _Is_union<T, is_class_or_union<T>::value> {};
}

enum class test_enum {
	_A
	,_B
	,_C
};

struct test_te {
	static constexpr bool pred(size_t x) { return x < 10; }
};

#include <IMathLib/physics/unit.hpp>

namespace iml {
	//テスト出力
	template <class>
	struct test_output;
	template <class T, class... Types>
	struct test_output<type_tuple<T, Types...>> {
		static void f() {
			std::cout << T::value << std::endl;
			test_output<type_tuple<Types...>>::f();
		}
	};
	template <>
	struct test_output<type_tuple<>> {
		static void f() {
			std::cout << "finish" << std::endl;
		}
	};
}

namespace iml {

	//多次元配列の先頭ポインタの取得
	template <class T>
	constexpr auto addressof_multi_array(T &val) {
		return reinterpret_cast<typename remove_all_extents<T>::type*>(addressof(val));
	}
	template <class T>
	constexpr T* addressof_multi_array(const T&&) = delete;
}

namespace iml {

	//テンプレート引数から最大値を得る
	template <size_t First, size_t... Indices>
	struct template_max {
		static constexpr size_t value = First;
	};
	template <size_t First, size_t Second, size_t... Indices>
	struct template_max<First, Second, Indices...> : template_max<((First < Second) ? Second : First), Indices...> {};


	//自動微分型の基底
	template <class, class, class>
	class ad_base;
	template <class T, size_t... Dims, size_t... Indices>
	class ad_base<T, index_tuple<Dims...>, index_tuple<Indices...>> {
		template <class, size_t...> friend class ad;

		typename multi_array<T, Dims...>::type x;

		//乗算用の多重ループ(bool:終了条件)
		template <class S, bool = (!is_same<typename remove_extent<S>::type, typename remove_all_extents<S>::type>::value)>
		struct multi_loop_multiplicative;
		template <class S, size_t N>
		struct multi_loop_multiplicative<S[N], true> {
			//result:結果, val1・val2:演算対象, c:係数
			static void loop(S(&result)[N], const S(&val1)[N], const S(&val2)[N], const T& c) {
				multi_loop_multiplicative<S>::loop(result[0], val1[0], val2[0], c);
				for (size_t i = 1; i < N; ++i) {
					T temp = c;
					multi_loop_multiplicative<S>::loop(result[i], val1[0], val2[i], temp);
					for (size_t j = 1; j <= i; ++j) {
						temp *= T(i + 1 - j) / j;
						multi_loop_multiplicative<S>::loop(result[i], val1[j], val2[i - j], temp);
					}
				}
			}
			template <class U, size_t M, class = typename enable_if<!is_same<S, U>::value>::type>
			static void loop(S(&result)[N], S(&val1)[N], const U(val2)[M], const T& c) {
				loop(result, val1, val2[0], c);
			}
		};
		template <class S, size_t N>
		struct multi_loop_multiplicative<S[N], false> {
			static void loop(S(&result)[N], const S(&val1)[N], const S(&val2)[N], const T& c) {
				result[0] += c * val1[0] * val2[0];
				for (size_t i = 1; i < N; ++i) {
					T temp = c;
					result[i] += temp * val1[0] * val2[i];
					for (size_t j = 1; j <= i; ++j) {
						temp *= T(i + 1 - j) / j;
						result[i] += temp * val1[j] * val2[i - j];
					}
				}
			}
			template <class U, size_t M, class = typename enable_if<!is_same<S, U>::value>::type>
			static void loop(S(&result)[N], S(&val1)[N], const U(val2)[M], const T& c) {
				loop(result, val1, val2[0], c);
			}
		};
		//除算用の多重ループ(bool:終了条件)
		template <class S, bool = (!is_same<typename remove_extent<S>::type, typename remove_all_extents<S>::type>::value)>
		struct multi_loop_division;
		template <class S, size_t N>
		struct multi_loop_division<S[N], true> {
			//result:結果, x:元データ, table:xの冪乗テーブル, p:xの逆数の冪
			template <size_t M>
			static void loop(S(&result)[N], const S(&x)[N], const ad_base(&table)[M], size_t p) {
				T temp1 = 1 / pow(addressof_multi_array(x)[0], p);
				for (size_t i = 1; i < N; ++i) {
					T temp2 = p * binom(p + i, i);
					for (size_t j = 1; j <= i; ++j) {
						//逆数の導関数の計算
						S val = { temp1 };
						multi_loop_division<S>::loop(val, x[0], table, j + p);

						temp2 *= -T(i + 1 - j) / j;
						multi_loop_multiplicative<S>::loop(result[i], table[j].x[i], val, temp2 / (p + j));
					}
				}
			}
		};
		template <class S, size_t N>
		struct multi_loop_division<S[N], false> {
			//tableとresultが同じ型になるまで添え字アクセス
			static void loop_impl(size_t i, S(&result)[N], const S(&table)[N], const T& c) {
				result[i] += c * table[i];
			}
			template <class U, size_t M, class = typename enable_if<!is_same<S, U>::value>::type>
			static void loop_impl(size_t i, S(&result)[N], const U(&table)[M], const T& c) {
				loop_impl(i, result, table[0], c);
			}
			template <size_t M>
			static void loop(S(&result)[N], const S(&x)[N], const ad_base(&table)[M], size_t p) {
				//p*f^(-p)部の構築
				T temp1 = p / pow(addressof_multi_array(x)[0], p);

				for (size_t i = 1; i < N; ++i) {
					T temp2 = temp1 * binom(p + i, i);
					for (size_t j = 1; j <= i; ++j) {
						temp2 *= (i + 1 - j) / (-addressof_multi_array(x)[0] * j);
						loop_impl(i, result, table[j].x, temp2 / (p + j));
					}
				}
			}
		};
		//逆数の導関数の取得
		static ad_base inverse_element(const ad_base& x) {
			ad_base result(1 / addressof_multi_array(x.x)[0]);
			ad_base table[template_max<Dims...>::value] = { 1 };		//xの冪乗のテーブル(1,x,x^2,...)
			//テーブルの構築
			for (size_t i = 1; i < template_max<Dims...>::value; ++i) table[i] = table[i - 1] * x;

			multi_loop_division<typename multi_array<T, Dims...>::type>::loop(result.x, x.x, table, 1);
			return result;
		}
		//任意のテンプレートに対してTを返す補助
		template <size_t>
		struct _Get {
			using type = T;
		};
	public:
		constexpr ad_base() : x{} {}
		constexpr ad_base(const T& x) : x{ x } {}
		constexpr ad_base(const T& x, const typename _Get<Indices>::type&... y) : x{ x,y... } {}
		constexpr ad_base(const ad_base& a) : x{ addressof_multi_array(a.x)[0],addressof_multi_array(a.x)[Indices]... } {}


		using dimension_type = index_tuple<Dims...>;


		ad_base operator-() const { return ad_base(-addressof_multi_array(this->x)[0], (-addressof_multi_array(this->x)[Indices])...); }
		ad_base operator+() const { return ad_base(*this); }

		ad_base& operator=(const ad_base& n) {
			for (size_t i = 0; i < dimension<Dims...>::value; ++i)
				addressof_multi_array(this->x)[i] = addressof_multi_array(n.x)[i];
			return *this;
		}
		ad_base& operator+=(const ad_base& n) {
			for (size_t i = 0; i < dimension<Dims...>::value; ++i)
				addressof_multi_array(this->x)[i] += addressof_multi_array(n.x)[i];
			return *this;
		}
		ad_base& operator+=(const T& n) {
			addressof_multi_array(this->x)[0] += n;
			return *this;
		}
		ad_base& operator-=(const ad_base& n) {
			for (size_t i = 0; i < dimension<Dims...>::value; ++i)
				addressof_multi_array(this->x)[i] -= addressof_multi_array(n.x)[i];
			return *this;
		}
		ad_base& operator-=(const T& n) {
			addressof_multi_array(this->x)[0] -= n;
			return *this;
		}
		ad_base& operator*=(const ad_base& n) {
			typename multi_array<T, Dims...>::type result = {};
			multi_loop_multiplicative<typename multi_array<T, Dims...>::type>::loop(result, this->x, n.x, 1);
			for (size_t i = 0; i < dimension<Dims...>::value; ++i)
				addressof_multi_array(this->x)[i] = addressof_multi_array(result)[i];
			return *this;
		}
		ad_base& operator*=(const T& k) {
			for (size_t i = 0; i < dimension<Dims...>::value; ++i) addressof_multi_array(this->x)[i] *= k;
			return *this;
		}
		ad_base& operator/=(const ad_base& n) {
			return *this *= inverse_element(n);
		}
		ad_base& operator/=(const T& k) {
			for (imsize_t i = 0; i < dimension<Dims...>::value; ++i) addressof_multi_array(this->x)[i] /= k;
			return *this;
		}

		//二項演算
		friend ad_base operator+(const ad_base& n1, const ad_base& n2) {
			return ad_base(addressof_multi_array(n1.x)[0] + addressof_multi_array(n2.x)[0]
				, (addressof_multi_array(n1.x)[Indices] + addressof_multi_array(n2.x)[Indices])...);
		}
		friend ad_base operator+(const ad_base& n, const T& k) {
			return ad_base(addressof_multi_array(n.x)[0] + k, addressof_multi_array(n.x)[Indices]...);
		}
		friend ad_base operator+(const T& k, const ad_base& n) {
			return ad_base(k + addressof_multi_array(n.x)[0], addressof_multi_array(n.x)[Indices]...);
		}
		friend ad_base operator-(const ad_base& n1, const ad_base& n2) {
			return ad_base(addressof_multi_array(n1.x)[0] - addressof_multi_array(n2.x)[0]
				, (addressof_multi_array(n1.x)[Indices] - addressof_multi_array(n2.x)[Indices])...);
		}
		friend ad_base operator-(const ad_base& n, const T& k) {
			return ad_base(addressof_multi_array(n.x)[0] - k, addressof_multi_array(n.x)[Indices]...);
		}
		friend ad_base operator-(const T& k, const ad_base& n) {
			return ad_base(k - addressof_multi_array(n.x)[0], (-addressof_multi_array(n.x)[Indices])...);
		}
		friend ad_base operator*(const ad_base& n1, const ad_base& n2) {
			ad_base result;
			multi_loop_multiplicative<typename multi_array<T, Dims...>::type>::loop(result.x, n1.x, n2.x, 1);
			return result;
		}
		friend ad_base operator*(const ad_base& n, const T& k) {
			return ad_base(addressof_multi_array(n.x)[0] * k, (addressof_multi_array(n.x)[Indices] * k)...);
		}
		friend ad_base operator*(const T& k, const ad_base& n) {
			return ad_base(k*addressof_multi_array(n.x)[0], (k*addressof_multi_array(n.x)[Indices])...);
		}
		friend ad_base operator/(const ad_base& n1, const ad_base& n2) {
			return n1 * inverse_element(n2);
		}
		friend ad_base operator/(const ad_base& n, const T& k) {
			return ad_base(addressof_multi_array(n.x)[0] / k, (addressof_multi_array(n.x)[Indices] / k)...);
		}
		friend ad_base operator/(const T& k, const ad_base& n) {
			return k * inverse_element(n);
		}

		//比較演算
		friend bool operator==(const ad_base& n1, const ad_base& n2) { return addressof_multi_array(n1.x)[0] == addressof_multi_array(n2.x)[0]; }
		friend bool operator==(const ad_base& n1, const T& n2) { return addressof_multi_array(n1.x)[0] == n2; }
		friend bool operator==(const T& n1, const ad_base& n2) { return n1 == addressof_multi_array(n2.x)[0]; }
		friend bool operator!=(const ad_base& n1, const ad_base& n2) { return addressof_multi_array(n1.x)[0] != addressof_multi_array(n2.x)[0]; }
		friend bool operator!=(const ad_base& n1, const T& n2) { return addressof_multi_array(n1.x)[0] != n2; }
		friend bool operator!=(const T& n1, const ad_base& n2) { return n1 != addressof_multi_array(n2.x)[0]; }
		friend bool operator<(const ad_base& n1, const ad_base& n2) { return addressof_multi_array(n1.x)[0] < addressof_multi_array(n2.x)[0]; }
		friend bool operator<(const ad_base& n1, const T& n2) { return addressof_multi_array(n1.x)[0] < n2; }
		friend bool operator<(const T& n1, const ad_base& n2) { return n1 < addressof_multi_array(n2.x)[0]; }
		friend bool operator<=(const ad_base& n1, const ad_base& n2) { return addressof_multi_array(n1.x)[0] <= addressof_multi_array(n2.x)[0]; }
		friend bool operator<=(const ad_base& n1, const T& n2) { return addressof_multi_array(n1.x)[0] <= n2; }
		friend bool operator<=(const T& n1, const ad_base& n2) { return n1 <= addressof_multi_array(n2.x)[0]; }
		friend bool operator>(const ad_base& n1, const ad_base& n2) { return addressof_multi_array(n1.x)[0] > addressof_multi_array(n2.x)[0]; }
		friend bool operator>(const ad_base& n1, const T& n2) { return addressof_multi_array(n1.x)[0] > n2; }
		friend bool operator>(const T& n1, const ad_base& n2) { return n1 > addressof_multi_array(n2.x)[0]; }
		friend bool operator>=(const ad_base& n1, const ad_base& n2) { return addressof_multi_array(n1.x)[0] >= addressof_multi_array(n2.x)[0]; }
		friend bool operator>=(const ad_base& n1, const T& n2) { return addressof_multi_array(n1.x)[0] >= n2; }
		friend bool operator>=(const T& n1, const ad_base& n2) { return n1 >= addressof_multi_array(n2.x)[0]; }

		//アクセサ
		const auto& operator[](size_t index) const { return this->x[index]; }
		auto& operator[](size_t index) { return this->x[index]; }

		//ストリーム
		friend std::ostream& operator<<(std::ostream& os, const ad_base& n) {
			os << addressof_multi_array(n.x)[0];
			for (imsize_t i = 1; i < dimension<Dims...>::value; ++i) os << ',' << addressof_multi_array(n.x)[i];
			return os;
		}
	};
	//自動微分型(Dims:変数の数の分だけ引数が存在し、各変数の最大微分回数)
	template <class T, size_t... Dims>
	class ad : public ad_base<T, index_tuple<Dims...>, typename index_range<1, dimension<Dims...>::value>::type> {
	public:
		//コンストラクタの継承と定義
		using ad_base<T, index_tuple<Dims...>, typename index_range<1, dimension<Dims...>::value>::type>::ad_base;
		constexpr ad() : ad_base<T, index_tuple<Dims...>, typename index_range<1, dimension<Dims...>::value>::type>() {}
		constexpr ad(const ad_base<T, index_tuple<Dims...>, typename index_range<1, dimension<Dims...>::value>::type>& n)
			: ad_base<T, index_tuple<Dims...>, typename index_range<1, dimension<Dims...>::value>::type>(n) {}
	};


	//N番目の変数を設定するための添え字の取得
	template <size_t N, size_t, class, bool = (N == 0)>
	struct variable_suffix_impl;
	template <size_t Result, size_t... Indices>
	struct variable_suffix_impl<0, Result, index_tuple<Indices...>, true> {
		static constexpr size_t value = Result;
	};
	template <size_t N, size_t Result, size_t First, size_t... Indices>
	struct variable_suffix_impl<N, Result, index_tuple<First, Indices...>, false>
		: variable_suffix_impl<N - 1, Result*First, index_tuple<Indices...>> {};
	template <size_t, class>
	struct variable_suffix;
	template <size_t N, size_t... Dims>
	struct variable_suffix<N, index_tuple<Dims...>>
		: variable_suffix_impl<sizeof...(Dims) - 1 - N, 1, typename reverse_index_tuple<index_tuple<Dims...>>::type> {};
	//自動微分をするための変数
	template <class AD, size_t N, class T>
	AD variable(const T& x) {
		AD result(x);
		addressof_multi_array(result[0])[variable_suffix<N, typename AD::dimension_type>::value] = 1;
		return result;
	}
}

namespace _iml {

	//平方根
	template <class T>
	constexpr T sqrt(T x) {

		if ((x == 0) || (x == 1)) return x;

		T temp = 1;

		//1< x < 2^8 = 256
		while (x > 256) { x /= 256; temp *= 16; }
		while (x < 1) { x *= 256; temp /= 16; }
		//1 < x < 2^6 = 64
		while (x > 64) { x /= 64; temp *= 8; }
		//1 < x < 2^4 = 16
		while (x > 16) { x /= 16; temp *= 4; }
		//1 < x < 2^2 = 4
		while (x > 4) { x /= 4; temp *= 2; }
		T x1 = 1 / x, x2 = 0;
		T h = 1 - x * x1*x1;            //0 < h < 0.75
		while (x2 < x1) {
			x2 = x1;
			x1 *= (1 + (64 * h + h * h*(48 + 40 * h + 35 * h*h)) / 128);
			h = 1 - x * x1 * x1;
		}
		return temp * x1 * x;
	}

}

template <class T>
T function(T x, T y) { return _iml::sqrt(x*x*y + y); }

constexpr iml::vector2<int> fff() { return iml::vector2<int>(3, 5); }
template <class T>
struct vector_const {
	static constexpr T x = fff();
};

int main(int argc, char* argv[]) {
	constexpr auto temp____ = vector_const<iml::vector2<int>>::x;
	std::cout << temp____ << std::endl;

	int temp_a;
	iml::reference_wrapper<int> f = iml::ref(temp_a);
	f.get() = 42;
	std::cout << temp_a << std::endl;

	std::invoke(function<int>, 123, 123);

	//2.48125, 0.506377, 1.214508, 0.2478589 となるはず
	std::cout << "test:" << function(iml::variable<iml::ad<double, 2, 2>, 0>(1.23)
		, iml::variable<iml::ad<double, 2, 2>, 1>(2.45)) << std::endl;

	iml::ad<double, 5, 4, 2> temp_ad;
	temp_ad[1][0][0] = 10;
	std::cout << temp_ad << std::endl;

	iml::at_index<4, 4, 5, 6, 7, 8>::value;
	double temp_double[1][2][3][4][5] = {1};
	iml::remove_extent<decltype(temp_double)>::type;

	auto temp_double_p = reinterpret_cast<double*>(std::addressof(temp_double));
	std::cout <<"check:"<< temp_double_p[0] << std::endl;

	/*double temp_[4][5] = {
		1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20
	};
	for (size_t i = 0; i < 5; ++i) std::cout << "check:" << temp_[2][i] << std::endl;
	auto temp_p = iml::addressof_multi_array(temp_);
	std::cout << temp_p[0] << std::endl;

	iml::_test_<iml::vector2<int>>::value;

	std::void_t<>;
	std::is_null_pointer<int>::value;
	*/

	//1.10905, 0.45083, -0.18327, 0.22350, -0.45426 となるはず
	std::cout << "test:" << _iml::sqrt(iml::ad<double, 5>(1.23, 1, 0, 0, 0)) << std::endl;

	iml::phy::si_unit<iml::phy::watt, iml::phy::joule> test_test;

	using type_phy = iml::unit_type_tuple<iml::phy::watt>::type;
	iml::test_output<type_phy>::f();
	using type_phy2 = iml::unit_type_tuple<iml::phy::joule>::type;
	iml::test_output<type_phy2>::f();

	iml::template_sort<iml::index_tuple<1, 456, 78, 32, 1, 2, 78, 6, 4>>::type;
	
	/*エラーなやつ
	int temp_int = 12;
	int *const temp_int_p = &temp_int;
	temp_int_p = 0;*/

	iml::is_inclusion<iml::vector3<double>, iml::vector3<iml::vector2<float>>>::value;

	iml::is_enum<test_enum>::value;
	iml::is_convertible<const int*, int*>::value;
	iml::is_convertible<int&, float>::value;

	/*//ローレンツ方程式の定義
	iml::ode::runge_kutta_method<double, double, double, double> ode(__f1, __f2, __f3);
	//初期値の設定(t=0,x=y=z=1)
	ode.set_ini_value(0, 1, 1, 1);

	std::ofstream ofs("C:\\Users\\yoshiki\\Documents\\test.csv");

	for (size_t i = 0; i < 10000; ++i) {
		//現在の値の参照
		ofs << ode.get_l_value<0>()
			<< ',' << ode.get_l_value<1>()
			<< ',' << ode.get_l_value<2>() << std::endl;
		//値の更新
		ode.update(1 / 60.);
	}

	ofs.close();
	*/
	//return 0;


	double temp = 0;

	/*std::cout << std::fixed;

	//ベンチマーク
	iml::i_f::timer<std::chrono::milliseconds> timer;
	timer.start();
	for (size_t i = 0; i < 10000000; ++i) temp = std::cos((double)i);
	std::cout << "std::cos:" << timer.now_time() << "ms" << std::endl;
	std::cout << "check:" << std::setprecision(20) << temp << std::endl;
	timer.start();
	for (size_t i = 0; i < 10000000; ++i) temp = std::exp((double)i);
	std::cout << "std::exp:" << timer.now_time() << "ms" << std::endl;
	std::cout << "check:" << std::setprecision(20) << temp << std::endl;
	timer.start();
	for (size_t i = 0; i < 10000000; ++i) temp = std::log((double)i);
	std::cout << "std::log:" << timer.now_time() << "ms" << std::endl;
	std::cout << "check:" << std::setprecision(20) << temp << std::endl;
	timer.start();
	for (size_t i = 0; i < 10000000; ++i) temp = std::sqrt((double)i);
	std::cout << "std::sqrt:" << timer.now_time() << "ms" << std::endl;
	std::cout << "check:" << std::setprecision(20) << temp << std::endl;
	timer.start();
	for (size_t i = 0; i < 10000000; ++i) temp = iml::cos((double)i);
	std::cout << "iml::cos:" << timer.now_time() << "ms" << std::endl;
	std::cout << "check:" << std::setprecision(20) << temp << std::endl;
	timer.start();
	for (size_t i = 0; i < 10000000; ++i) temp = iml::sqrt((double)i);
	std::cout << "iml::sqrt:" << timer.now_time() << "ms" << std::endl;
	std::cout << "check:" << std::setprecision(20) << temp << std::endl;
	timer.start();
	for (size_t i = 0; i < 10000000; ++i) temp = iml::log((double)i);
	std::cout << "iml::log:" << timer.now_time() << "ms" << std::endl;
	std::cout << "check:" << std::setprecision(20) << temp << std::endl;
	timer.start();
	for (size_t i = 0; i < 10000000; ++i) temp = iml::exp((double)i);
	std::cout << "iml::exp:" << timer.now_time() << "ms" << std::endl;
	std::cout << "check:" << std::setprecision(20) << temp << std::endl;
	*/


	/*std::cout << iml::determinant(iml::matrix4<double>(
		3, 2, 4, 1
		, 1, 1, 3, 2
		, 2, 2, 3, -1
		, -2, 1, -2, 1
		)) << std::endl;
	std::cout << iml::determinant(iml::matrix4<double>(
		0, 1, 0, 0
		, 0, 0, 0, 3
		, 2, 0, 0, 0
		, 0, 0, 1, 0
		)) << std::endl;
	std::cout << iml::determinant(iml::matrix4<double>(
		0, 0, 0, 1
		, 0, 0, 1, 0
		, 0, 1, 0, 0
		, 1, 0, 0, 0
		)) << std::endl;
	std::cout << iml::inverse_matrix(iml::matrix<double, 5, 5>(
		3, 1, 1, 2, -7
		, 5, 1, 3, 4, 0
		, 2, 0, 0, 0, 1
		, 1, 3, 2, 1, 3
		, 0, 0, 2, 8, -5
		)) << std::endl;
	std::cout << iml::rank(iml::matrix<double, 3, 4>(
		1, 2, 1, 2
		, 3, 6, 3, 6
		, -1, -2, -1, -2
		)) << std::endl;
	std::cout << iml::rank(iml::matrix3<double>(
		1, -1, 1
		, 2, -3, 1
		, -1, 4, 2
		)) << std::endl;
	*/


	//出力では正しいからvisual studioのバグ
	std::cout << "iml::vector2<size_t>:" << iml::is_exist_add_inverse_element<iml::vector2<size_t>>::value << std::endl;
	iml::is_calcable<size_t, size_t>::sub_value;

	std::cout << iml::math_constant<float>::pi << std::endl;
	iml::abs(iml::vector2<float>(1, 2));

	std::cout << "微分テスト:" << iml::sqrt(iml::ad_variable<5>(1.23)) << std::endl;
	std::cout << "微分テスト:" << iml::bottomup_ad<double, 5>(10) / iml::ad_variable<5>(1.23) / 10 << std::endl;

	iml::vector3<double> vector;
	vector << 1, 2, 3;

	std::cout << "check" << std::endl;

	std::locale::global(std::locale("Japanese"));

	//初期化
	iml::i_f::interface_control::inst()->init();
	//カメラの設定
	iml::i_f::camera<float> camera;
	//camera.set_viewing_angle(camera.pi() / 4, camera.pi() * 3 / 16);
	camera.set_viewing_angle_magnific(camera.pi() / 4, 600 / 800.);
	//入力処理のポインタの取得
	iml::i_f::keyboard_input *key = iml::i_f::keyboard_input::inst();
	iml::i_f::mouse_input *mouse = iml::i_f::mouse_input::inst();
	//FPS制御用
	iml::i_f::fps_control<float> fps;
	fps.set_fps(60, 20);
	//ウィンドウの生成
	iml::i_f::window wnd(u8"モデルビューワー", 800, 600);
	//メインウィンドウのクローズイベントのコールバックの設定
	wnd.close_ev([]() {std::exit(0); });

	//フォント
	auto font = iml::i_f::load<iml::i_f::ttf>("mgenplus-1c-regular.ttf", 16);

	//ウィンドウの表示
	wnd.show();

	iml::i_f::shadow_mapping shadow_map(4096, 4096, "shadow.vert", "shadow.frag");
	shadow_map.set_target({ 0, 50, 0 });
	shadow_map.set_camera_pos({ 0, 300, 100 });
	shadow_map.set_viewing_angle_magnific(camera.pi() / 4, shadow_map.height() / float(shadow_map.width()));
	shadow_map.set_shadow_ambient(0.3);




	//座標平面軸用の配列データ
	iml::vector3<float> plane_axis_vtx[84];
	{
		size_t cnt = 0;
		for (int i = -10; i < 11; ++i, cnt += 2) {
			plane_axis_vtx[cnt][0] = -100; plane_axis_vtx[cnt][1] = 0; plane_axis_vtx[cnt][2] = 10 * i;
			plane_axis_vtx[cnt + 1][0] = 100; plane_axis_vtx[cnt + 1][1] = 0; plane_axis_vtx[cnt + 1][2] = 10 * i;
		}
		for (int i = -10; i < 11; ++i, cnt += 2) {
			plane_axis_vtx[cnt][0] = 10 * i; plane_axis_vtx[cnt][1] = 0; plane_axis_vtx[cnt][2] = -100;
			plane_axis_vtx[cnt + 1][0] = 10 * i; plane_axis_vtx[cnt + 1][1] = 0; plane_axis_vtx[cnt + 1][2] = 100;
		}
	}
	iml::vector3<float> plane_vtx[4] = { {-100,-0.1f,-100},{ -100,-0.1f,100 },{ 100,-0.1f,100 },{ 100,-0.1f,-100 } };

	//ライト
	auto light = iml::i_f::light<float, 0>::inst();
	light->set_position(iml::vector4<float>(0, 300, 100, 1));
	light->set_direction(iml::vector3<float>(0, -1, -1));
	light->set_ambient({ 0.3f,0.3f,0.3f });
	light->set_specular({ 1,1,1 });
	light->set_diffuse({ 1,1,1 });


	iml::img::image* im_buf = iml::img::load_bmp(iml::i_f::binary_load<char>("org_back.bmp"));
	iml::img::image* im_buf_ = iml::img::load_png(iml::i_f::binary_load<char>("org_back.png"));
	//auto image_ = iml::i_f::load<iml::i_f::texture>(im_buf);
	auto image_ = iml::i_f::load<iml::i_f::texture>(im_buf_);
	auto model_ = iml::i_f::load<iml::m3d::mqo_model<float>>(iml::i_f::text_line_load<char>("iroha.mqo"), .1);

	win_api::window main_window(wnd);
	auto dlg = main_window.create_dialog<IDD_BONE_MAP>(callback);
	dlg->show();
	

	

	while (1) {
		//イベント処理はSDL2の機能をそのまま呼び出す
		SDL_Event ev;
		while (SDL_PollEvent(&ev)) {
			switch (ev.type) {
			case SDL_WINDOWEVENT:
				iml::i_f::window_event(ev.window);
				break;
			}
		}

		//メインウィンドウに対する描画
		wnd.current();

		//入力の更新
		key->update();
		mouse->update();
		//ESCSAPを押したら終了
		if (key->trigger(SDL_SCANCODE_ESCAPE)) std::exit(1);

		//Aで入力開始
		if (key->trigger(SDL_SCANCODE_A)) {
			printf("start\n");
			SDL_StartTextInput();
		}
		//Sで入力終了
		if (key->trigger(SDL_SCANCODE_S)) {
			printf("end\n");
			SDL_StopTextInput();
		}

		iml::i_f::interface_control::inst()->set_client_area(iml::i_f::rect<iml::imint_t>(0, 800, 600, 0));
		//射影行列の更新
		iml::i_f::projection_update(&camera);
		//マウスの操作によってカメラの視点を切り替える
		{
			auto relative = mouse->move_mouse();
			//右クリックのみならば回転
			if (mouse->push_r() && !mouse->push_l())
				camera.move_angle(iml::vector2<float>(-relative[0], relative[1])*0.01);
			//中央クリックならばビューボート面に対する平行移動(ターゲットに近いほど移動距離は減少)
			else if (mouse->push_m())
				camera.move_target(iml::vector2<float>(-relative[0], relative[1])*camera.get_length()*0.001);
			//左右両方クリックしているとき注視点からの距離を調整
			if (mouse->push_l() && mouse->push_r())
				camera.move_length(relative[0] * 2);
		}
		//ビュー行列の更新
		iml::i_f::view_update(&camera);

		glEnable(GL_DEPTH_TEST);
		//シャドウマップのレンダリング
		static auto draw_f = [&]() {
			iml::i_f::draw_model(model_);
		};
		shadow_map.create_shadow(draw_f);
		//シャドウを適応させる空間の描画
		shadow_map.start();
		draw_f();
		glNormal3d(0, 1, 0);
		iml::i_f::set_color(iml::normal_color<float>(1, 1, 1));
		iml::i_f::draw_figure<GL_QUADS, iml::index_tuple<GL_VERTEX_ARRAY>, sizeof(iml::vector3<float>)>(3, &plane_vtx[0][0], 0, 4);
		shadow_map.end();
		//draw_f();
		//iml::i_f::draw_texture2(iml::i_f::vertex2<double>(0, 0, 200, 200), 3);
		glDisable(GL_DEPTH_TEST);

		//light->disable();
		//iml::i_f::disable_light();
		glEnable(GL_DEPTH_TEST);
		//ワールド座標軸
		iml::i_f::set_color(iml::normal_color<float>(1, 0, 0));
		iml::i_f::draw_line(iml::vector3<float>(0, 0, 0), iml::vector3<float>(100, 0, 0));
		iml::i_f::set_color(iml::normal_color<float>(0, 1, 0));
		iml::i_f::draw_line(iml::vector3<float>(0, 0, 0), iml::vector3<float>(0, 100, 0));
		iml::i_f::set_color(iml::normal_color<float>(0, 0, 1));
		iml::i_f::draw_line(iml::vector3<float>(0, 0, 0), iml::vector3<float>(0, 0, 100));
		iml::i_f::set_color(iml::normal_color<float>(0.8, 0.8, 0.8));
		iml::i_f::draw_figure<GL_LINES, iml::index_tuple<GL_VERTEX_ARRAY>, sizeof(iml::vector3<float>)>(3, &plane_axis_vtx[0][0], 0, 84);
		glDisable(GL_DEPTH_TEST);

		iml::vector3<float> bone_screen[2] = { iml::i_f::world_to_screen(camera,iml::vector3<float>(0, 100, 0)),iml::i_f::world_to_screen(camera,iml::vector3<float>(0, 80, 0)) };
		iml::vector2<int> mouse_pos = mouse->now_mouse();
		iml::i_f::set_color(iml::normal_color<float>(0, 0, 1));
		draw_bone(iml::vector3<float>(1, 0, 0), iml::vector3<float>(0, 1, 0), iml::vector3<float>(0, 0, 1), iml::vector3<float>(0, 100, 0), 20.f);
		draw_bone(iml::vector3<float>(1, 0, 0), iml::vector3<float>(0, 1, 0), iml::vector3<float>(0, 0, 1), iml::vector3<float>(0, 80, 0), 20.f);


		if (mouse_pos[0] + 5 > bone_screen[0][0] && mouse_pos[0] - 5 < bone_screen[0][0] && mouse_pos[1] + 5 > bone_screen[0][1] && mouse_pos[1] - 5 < bone_screen[0][1])
			iml::i_f::set_color(iml::normal_color<float>(0, 1, 1));
		else iml::i_f::set_color(iml::normal_color<float>(0, 0, 1));
		iml::i_f::draw_point(iml::vector3<float>(0, 100, 0), 10);
		if (mouse_pos[0] + 5 > bone_screen[1][0] && mouse_pos[0] - 5 < bone_screen[1][0] && mouse_pos[1] + 5 > bone_screen[1][1] && mouse_pos[1] - 5 < bone_screen[1][1])
			iml::i_f::set_color(iml::normal_color<float>(0, 1, 1));
		else iml::i_f::set_color(iml::normal_color<float>(0, 0, 1));
		iml::i_f::draw_point(iml::vector3<float>(0, 80, 0), 10);

		//クォータニオン回転テスト
		iml::vector3<double> _ax = iml::unit(iml::vector3<double>(1, 1, 1));
		iml::vector3<double> _center(50, 40, 30);
		static iml::vector3<double> _v(100, 30, 20);
		_v = iml::rotation_vector(_v, _ax, _center, 0.05);
		iml::i_f::set_color(iml::normal_color<float>(1, 0, 1));
		iml::i_f::draw_line(iml::vector3<double>(_center - _ax * 100), iml::vector3<double>(_center + _ax * 100));
		iml::i_f::draw_point(_center, 10);
		iml::i_f::draw_point(_v, 10);

		//スクリーンモード
		iml::i_f::screen_start();
		iml::i_f::set_color(iml::normal_color<float>(1, 1, 1));
		iml::i_f::draw_texture<GL_QUADS, iml::index_tuple<GL_VERTEX_ARRAY, GL_TEXTURE_COORD_ARRAY>, 0, 0>(
			2, image_, iml::i_f::vertex2<float>(0, 0, 300, 300).vtx, iml::i_f::vertex2<float>(0, 0, 1, 1).vtx, 0, 4);
		//iml::i_f::draw_texture2(iml::i_f::vertex2<double>(0, 0, 200, 200), image_);
		iml::i_f::draw_string_texture2(10, 500, *font, (L"てすとFPS\n" + std::to_wstring(fps.get_fps())).c_str(), iml::color_32bit(0, 0, 0));
		iml::i_f::draw_texture2(iml::i_f::vertex2<double>(100, 300, 300, 500), shadow_map.get_fbo());

		//y=0平面の取得(y-up系だから)
		{
			auto start_sc = iml::i_f::screen_to_world(camera, iml::vector3<float>(mouse->now_mouse()[0], mouse->now_mouse()[1], 0));
			auto end_sc = iml::i_f::screen_to_world(camera, iml::vector3<float>(mouse->now_mouse()[0], mouse->now_mouse()[1], 1));

			auto plane_p = iml::hit_position_line_surface(start_sc, end_sc, iml::vector3<float>(0, 0, 0), iml::vector3<float>(0, 0, 1), iml::vector3<float>(1, 0, 0));
			iml::i_f::draw_string_texture2(150, 500, *font, (L"hit:" + std::to_wstring(plane_p.flag)+ L',' +std::to_wstring(plane_p.pos[0]) + L',' + std::to_wstring(plane_p.pos[1]) + L',' + std::to_wstring(plane_p.pos[2])).c_str(), iml::color_32bit(0, 0, 0));
			iml::i_f::draw_string_texture2(200, 550, *font, (std::to_wstring(mouse->now_mouse()[0]) + L',' + std::to_wstring(mouse->now_mouse()[1])).c_str(), iml::color_32bit(0, 0, 0));

			if (key->trigger(SDL_SCANCODE_A)) {
				iml::i_f::draw_string_texture2(200, 400, *font, L"A", iml::color_32bit(0, 0, 0));
				std::cout << "check" << std::endl;
			}
			if (key->push(SDL_SCANCODE_B)) iml::i_f::draw_string_texture2(200, 410, *font, L"B", iml::color_32bit(0, 0, 0));
			if (key->push(SDL_SCANCODE_C)) iml::i_f::draw_string_texture2(200, 420, *font, L"C", iml::color_32bit(0, 0, 0));

			iml::i_f::draw_string_texture2(150, 300, *font, (std::to_wstring(start_sc[0]) + L',' + std::to_wstring(start_sc[1]) + L',' + std::to_wstring(start_sc[2])).c_str(), iml::color_32bit(0, 0, 0));
			iml::i_f::draw_string_texture2(150, 340, *font, (std::to_wstring(end_sc[0]) + L',' + std::to_wstring(end_sc[1]) + L',' + std::to_wstring(end_sc[2])).c_str(), iml::color_32bit(0, 0, 0));

			//3角形との衝突
			auto plane_p2 = iml::hit_check_line_triangle(start_sc, end_sc, iml::vector3<float>(0, 0, 0), iml::vector3<float>(0, 0, 20), iml::vector3<float>(20, 0, 0));
			if (plane_p2.flag) iml::i_f::draw_string_texture2(200, 250, *font, L"check", iml::color_32bit(0, 0, 0));

			iml::i_f::screen_end();

			iml::i_f::set_color(iml::normal_color<float>(0, 0, 0));
			iml::i_f::draw_line(start_sc, end_sc);
			iml::i_f::draw_line(iml::vector3<float>(), start_sc);
			iml::i_f::draw_line(iml::vector3<float>(), end_sc);
		}

		//メインウィンドウの更新とバッファのクリア
		wnd.update();
		wnd.clear();

		//FPS安定化処理
		fps.update();
		fps.wait();
	}

	


	std::cout << "check" << std::endl;

	return 0;

}
//#endif