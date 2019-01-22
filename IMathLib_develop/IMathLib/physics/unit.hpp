#ifndef _IMATH_PHYSICS_UNIT_HPP
#define _IMATH_PHYSICS_UNIT_HPP

#include "IMathLib/math/math.hpp"

//SI単位系の付与(次元解析)


namespace iml {
	namespace phy {

		//長さ
		struct meter {
			static constexpr imsize_t value = 0;
		};
		struct rmeter {
			static constexpr imsize_t value = 1;
		};
		//質量
		struct kilogram {
			static constexpr imsize_t value = 2;
		};
		struct rkilogram {
			static constexpr imsize_t value = 3;
		};
		//時間
		struct second {
			static constexpr imsize_t value = 4;
		};
		struct rsecond {
			static constexpr imsize_t value = 5;
		};
		//電流
		struct ampere {
			static constexpr imsize_t value = 6;
		};
		struct rampere {
			static constexpr imsize_t value = 7;
		};
		//熱力学温度
		struct kelvin {
			static constexpr imsize_t value = 8;
		};
		struct rkelvin {
			static constexpr imsize_t value = 9;
		};
		//物質量
		struct mole {
			static constexpr imsize_t value = 10;
		};
		struct rmole {
			static constexpr imsize_t value = 11;
		};
		//光度
		struct candela {
			static constexpr imsize_t value = 12;
		};
		struct rcandela {
			static constexpr imsize_t value = 13;
		};

		//SI単位の組み立て
		//ニュートンN(kg m / s^2)
		using newton = arg_tuple<kilogram, meter, rsecond, rsecond>;
		//速度v(m / s)
		using velocity = arg_tuple<meter, rsecond>;
		//加速度a(m / s^2)
		using acceleration = arg_tuple<meter, rsecond, rsecond>;
		//運動量p(N s)
		using momentum = arg_tuple<newton, second>;
		//仕事J(N m)
		using joule = arg_tuple<newton, meter>;
		using energy = arg_tuple<newton, meter>;
		//仕事率W(J / s)
		using watt = arg_tuple<joule, rsecond>;
		using power = arg_tuple<joule, rsecond>;


		//型がSI単位系であるか
		template <class T>
		struct is_si_unit : _Cat_base<
			is_same<T, meter>::value
			|| is_same<T, rmeter>::value
			|| is_same<T, kilogram>::value
			|| is_same<T, rkilogram>::value
			|| is_same<T, second>::value
			|| is_same<T, rsecond>::value
			|| is_same<T, kelvin>::value
			|| is_same<T, rkelvin>::value
			|| is_same<T, mole>::value
			|| is_same<T, rmole>::value
			|| is_same<T, candela>::value
			|| is_same<T, rcandela>::value> {};
		//組み立てによるSI単位か(深層に対しても再帰的に走査)
		template <class, bool>
		struct _Is_si_unit;
		template <bool f>
		struct _Is_si_unit<arg_tuple<>, f> : _Cat_base<f> {};
		template <class First, class... Types, bool f>
		struct _Is_si_unit<arg_tuple<First, Types...>, f>
			: _Is_si_unit<arg_tuple<Types...>, is_si_unit<First>::value && f> {};
		template <class... Types>
		struct is_si_unit<arg_tuple<Types...>> : _Is_si_unit<arg_tuple<Types...>, sizeof...(Types) != 0> {};


		//SI単位系本体
		template <class T, class = typename enable_if<is_si_unit<T>::value>::type>
		class _Si_unit;
		template <class... Types>
		class _Si_unit<arg_tuple<Types...>> {

		public:

		};
		template <class... Types>
		class si_unit : _Si_unit<arg_tuple<Types...>> {};


		//リテラルの列挙と演算の定義
		namespace si {
			static const meter m;
			static const kilogram kg;
			static const ampere A;
			static const kelvin K;
			static const mole mol;
			static const candela cd;
		}


		//2つの単位系の次元が一致する
		template <class Unit1, class Unit2>
		class is_same_dimension {

		};

		//SI単位系生成の呼び出し本体
		template <class T, class... Types>
		class quantity {
			template <class, class...>
			friend class quantity;

			T x_m;				//ホールディングしてる変数
		public:
			constexpr quantity(const T& x) : x_m(x) {}
			template <class _T>
			explicit constexpr quantity(const _T& x) : x_m(x) {}

			//using dimension_type = _Unit<Types...>;

			//単項演算
			template <class = typename enable_if<is_exist_add_inverse_element<T>::value>::type>
			quantity operator-() const { return quantity(-this->x_m); }
			quantity operator+() const { return *this; }
			//代入演算
			template <class _T, class = typename enable_if<is_calcable<T, _T>::add_eq_value>::type>
			quantity& operator+=(const quantity<_T, Types...>& x) { return quantity(this->x_m += x.x_m); }
			template <class _T, class = typename enable_if<is_calcable<T, _T>::sub_eq_value>::type>
			quantity& operator-=(const quantity<_T, Types...>& x) { return quantity(this->x_m -= x.x_m); }


		};
	}
}


#endif