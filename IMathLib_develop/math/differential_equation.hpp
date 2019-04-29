#ifndef IMATH_MATH_DIFFERENTIAL_EQUATION_HPP
#define IMATH_MATH_DIFFERENTIAL_EQUATION_HPP

#include "IMathLib/IMathLib_config.hpp"
#include "IMathLib/utility/tuple.hpp"

//微分方程式の数値解析

namespace iml {

	//微分方程式用導関数ホルダー
	template <size_t N, class... Types>
	class ode_holder {
		template <class, class...>
		friend class ode_tuple_base;

		using result_type = typename decay<typename at_type_tuple<N, Types...>::type>::type;
		using type = function<result_type(Types...)>;

		type value;
	public:
		constexpr explicit ode_holder() : value(nullptr) {}
		template <class F>
		constexpr explicit ode_holder(F&& f) : value(f) {}
	};
	//方程式の基底クラス
	template <class Indices, class... Types>
	class ode_tuple_base;
	template <size_t... Indices, class... Types>
	class ode_tuple_base<index_tuple<size_t, Indices...>, Types...> : ode_holder<Indices, Types...>... {

		//要素取得のため
		template <size_t N>
		static constexpr typename ode_holder<N, Types...>::type get_impl(const ode_holder<N, Types...>& t) {
			return t.value;
		}
		template <size_t N>
		static typename ode_holder<N, Types...>::type& get_impl(ode_holder<N, Types...>& t) {
			return t.value;
		}

	public:
		constexpr ode_tuple_base() : ode_holder<Indices, Types...>()... {}
		template <class... Fs>
		constexpr explicit ode_tuple_base(Fs... args) : ode_holder<Indices, Types...>(args)... {}
		ode_tuple_base(const ode_tuple_base&) = default;
		ode_tuple_base(ode_tuple_base&&) = default;

		using sequence_type = index_tuple<size_t, Indices...>;
		using type_tuple_type = type_tuple<Types...>;

		//データの取得
		template <size_t N>
		constexpr auto get() const { return get_impl<N>(*this); }
		template <size_t N>
		auto& get() { return get_impl<N>(*this); }

		ode_tuple_base& operator=(const ode_tuple_base&) = default;
		ode_tuple_base& operator=(ode_tuple_base&& im) noexcept = default;

		//スワップ
		void swap(ode_tuple_base& t) { iml::swap(*this, t); }
	};
	//方程式クラス本体(DN:独立変数の数)
	template <class... Types>
	class ode_tuple : public ode_tuple_base<typename index_range<size_t, 1, sizeof...(Types)>::type, Types...> {
	public:
		using ode_tuple_base<typename index_range<size_t, 1, sizeof...(Types)>::type, Types...>::ode_tuple_base;
	};

	template <class... Types1, class... Types2>
	inline constexpr auto apply(ode_tuple<Types1...>&& f, tuple<Types2...>&& t) {
		return Apply2<typename ode_tuple<Types1...>::sequence_type, typename tuple<Types2...>::sequence_type>::_apply_(forward<ode_tuple<Types1...>>(f), forward<tuple<Types2...>>(t));
	}

	//常微分方程式
	namespace ode {

		//常微分方程式の基底クラス(最大微分回数,独立変数,関数ポインタの引数型)
		template <class First, class Second, class... Types>
		class ode_base {
		public:
			//方程式の独立変数のtuple
			using d_value_t = tuple<typename decay<First>::type>;
			//方程式の従属変数のtuple
			using l_value_t = tuple<typename decay<Second>::type, typename decay<Types>::type...>;

		protected:
			d_value_t	d_value_m;				//現在の独立変数の値
			l_value_t	l_value_m;				//現在の従属変数の値

			ode_tuple<First, Second, Types...>	ode_f_m;		//方程式の関数ポインタ
		public:
			constexpr ode_base() : l_value_m(), d_value_m(), ode_f_m() {}
			template <class... Fs>
			constexpr ode_base(Fs&&... fs) : l_value_m(), d_value_m(), ode_f_m(forward<Fs>(fs)...) {}

			//初期値の設定
			void set_ini_value(const First& first, const Second& second, const Types&... args) {
				d_value_m.swap(d_value_t(first));
				l_value_m.swap(l_value_t(second, args...));
			}
			//関数の設定
			template <class... Fs>
			void set_function(Fs... fs) {
				ode_f_m.swap(ode_tuple<First, Second, Types...>(fs...));
			}
			//更新
			virtual void update(const First& t) = 0;

			//独立変数の取得
			constexpr First get_d_value() const { return d_value_m.get<0>(); }
			First& get_d_value() { return d_value_m.get<0>(); }
			//従属変数の取得
			template <size_t N>
			constexpr auto get_l_value() const { return l_value_m.get<N>(); }
			template <size_t N>
			auto& get_l_value() { return l_value_m.get<N>(); }

			//変数の数
			static constexpr size_t size() { return 2 + sizeof...(Types); }
		};

		//オイラー法
		template <class First, class... Types>
		class euler_method :public ode_base<First, Types...> {
		public:
			using ode_base<First, Types...>::ode_base;

			//更新
			void update(const First& dt) {
				auto result = apply(move(ode_f_m), tuple_cat(d_value_m, l_value_m))*dt;
				l_value_m = l_value_m + apply(move(ode_f_m), tuple_cat(d_value_m, l_value_m))*dt;
				d_value_m.get<0>() += dt;
			}
		};

		//4次のルンゲクッタ法(古典的ルンゲクッタ法)
		template <class First, class... Types>
		class runge_kutta_method :public ode_base<First, Types...> {
		public:
			using ode_base<First, Types...>::ode_base;

			//更新
			void update(const First& dt) {
				l_value_t k1 = dt*apply(move(ode_f_m), tuple_cat(d_value_m, l_value_m));
				l_value_t k2 = dt*apply(move(ode_f_m), tuple_cat(d_value_m + d_value_t(dt / 2), l_value_m + k1 / 2));
				l_value_t k3 = dt*apply(move(ode_f_m), tuple_cat(d_value_m + d_value_t(dt / 2), l_value_m + k2 / 2));
				l_value_t k4 = dt*apply(move(ode_f_m), tuple_cat(d_value_m + d_value_t(dt), l_value_m + k3));
				
				l_value_m = l_value_m + (k1 + 2 * k2 + 2 * k3 + k4) / 6;

				d_value_m.get<0>() += dt;
			}
		};

	}

	//偏微分方程式
	namespace pde {

	}

}
#endif