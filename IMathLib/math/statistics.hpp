#ifndef _STATISTICS_HPP
#define _STATISTICS_HPP

#include "IMathLib/math/math.hpp"
#include "IMathLib/utility/algorithm.hpp"
#include "IMathLib/math//random.hpp"

//統計演算関係

namespace iml {
	namespace stats {

		//統計演算の定義(データの順序が保証されない)
		template <class BidirectionalIterator>
		struct statistice_function {
			//双方向イテレータでなければならない
			static_assert(is_base_of<bidirectional_iterator_tag, typename iterator_traits<BidirectionalIterator>::iterator_category>::value
				, "The type of iterator is different.");

			using value_type = typename iterator_traits<BidirectionalIterator>::value_type;
			using iterator = BidirectionalIterator;

			//最大値(可変長要素)
			static value_type (__max)(const value_type& first) {
				return first;
			}
			template <class... Args>
			static value_type (__max)(const value_type& first, Args... args) {
				value_type temp = (__max)(iml::forward<Args>(args)...);
				return (first < temp ? temp : first);
			}
			//最大値(ポインタ)
			static value_type (__max)(iterator first, iterator last) {
				iterator p = first++;
				if (first == last) return first;
				while (first != last) {
					if (*p < *first) p = first;
					++first;
				}
				return *p;
			}

			//最小値(可変長要素)
			static value_type (__min)(const value_type& first) {
				return first;
			}
			template <class... Args>
			static value_type (__min)(const value_type& first, Args... args) {
				value_type temp = (__min)(iml::forward<Args>(args)...);
				return (first < temp ? first : temp);
			}
			//最小値(ポインタ)
			static value_type (__min)(iterator first, iterator last) {
				iterator p = first++;
				if (first == last) return first;
				while (first != last) {
					if (*p > *first) p = first;
					++first;
				}
				return *p;
			}

			//総和(可変長要素)
			static value_type __accumulate(const value_type& first) {
				return first;
			}
			template <class... Args>
			static value_type __accumulate(const value_type& first, Args... args) {
				value_type temp = __accumulate(iml::forward<Args>(args)...);
				return first + temp;
			}
			//総和(ポインタ)
			static value_type __accumulate(iterator first, iterator last) {
				value_type result = addition_traits<value_type>::identity_element();
				while (first != last) {
					result += *first;
					++first;
				}
				return result;
			}

			//平均値(可変長要素)
			template <class... Args>
			static value_type __average(const value_type& first, Args... args) {
				return __accumulate(first, iml::forward<Args>(args)...) / (sizeof...(Args)+1);
			}
			//平均値(ポインタ)
			static value_type __average(iterator first, iterator last) {
				return __accumulate(first, last) / (last - first);
			}

		private:
			//中央値探索の補助構造体
			template <class IteratorTag = typename iterator_traits<BidirectionalIterator>::iterator_category>
			struct __median_struct {
				//ランダムアクセスイテレータ用

				//3つの値の中央値
				static value_type __median3(value_type& _1, value_type& _2, value_type& _3) {
					return ((_2 > _1) ? ((_3 > _2) ? _2 : ((_3 > _1) ? _3 : _1)) : ((_3 > _1) ? _1 : ((_3 > _2) ? _3 : _2)));
				}
				//クイックソートを利用した中央値探索(実際はn番目に小さい数の探索)
				static value_type __quick_sort_median(iterator first, iterator last, size_t n) {
					if (last - first <= 1) return first[n];
					iterator i = first, j = last - 1;
					value_type pivot = __median3(*i, *(i + (j - i) / 2), *j);
					while (true) {
						while (*i < pivot) ++i;
						while (pivot < *j) --j;
						if (i >= j) break;
						iml::swap(*i, *j);
						++i; --j;
					}
					//中央値の位置を評価する(右半分か左半分に存在する)
					return ((i - first) < (last - (j + 1))) ? __quick_sort_median(++j, last, n - (i - first)) : __quick_sort_median(first, i, n);
				}

				//間接アクセス的な役割の関数
				static value_type __find(iterator first, iterator last, size_t n) {
					return __quick_sort_median(first, last, n);
				}
			};
			template <>
			struct __median_struct<bidirectional_iterator_tag> {
				//双方向イテレータ用

				//3つの値の中央値
				static value_type __median3(value_type& _1, value_type& _2, value_type& _3) {
					return ((_2 > _1) ? ((_3 > _2) ? _2 : ((_3 > _1) ? _3 : _1)) : ((_3 > _1) ? _1 : ((_3 > _2) ? _3 : _2)));
				}
				//クイックソートを利用した中央値探索(実際はn番目に小さい数の探索)
				static value_type __quick_sort_median(iterator first, iterator last, size_t n, size_t dis) {
					if (dis <= 1) return first[n];
					iterator i = first, j = last - 1;
					--dis;
					value_type pivot = __median3(*i, *next(i, dis / 2), *j);
					size_t cnt = 0;							//イテレータの変化のカウンタ
					while (true) {
						while (*i < pivot) { ++i; ++cnt; }
						while (pivot < *j) { --j; ++cnt; }
						if (dis <= cnt) break;
						iml::swap(*i, *j);
						++i; --j; cnt += 2;
					}
					//次のデータの長さを算出
					dis = distance(first, i);
					//中央値の位置を評価する(右半分か左半分に存在する)
					return (dis < cnt - dis) ? __quick_sort_median(++j, last, n - dis, cnt - dis) : __quick_sort_median(first, i, n, dis);
					//return ((i - first) < (last - (j + 1))) ? __quick_sort_median(++j, last, n - (i - first)) : __quick_sort_median(first, i, n);
				}

				//間接アクセス的な役割の関数
				static value_type __find(iterator first, iterator last, size_t n) {
					return __quick_sort_median(first, last, n, distance(first, last));
				}
			};
		public:
			//中央値(可変長要素)
			template <class... Args>
			static value_type __median(const value_type& first, Args... args) {
				value_type temp[sizeof...(Args)+1] = { first,static_cast<value_type>(args)... };
				return __median(temp, &temp[sizeof...(Args)+1]);
			}
			//中央値(ポインタ)
			static value_type __median(iterator first, iterator last) {
				//偶数なら2つの平均(奇数の方が高速)
				size_t dis = distance(first, last);
				return (((last - first) & 1) == 0) ?
					(__median_struct<>::__find(first, last, dis >> 1) + __median_struct<>::__find(first, last, (dis >> 1) - 1)) / 2
					: __median_struct<>::__find(first, last, dis >> 1);
			}
			//n番目に大きい値(可変長要素)
			template <class... Args>
			static value_type __nth_biggest(size_t n, const value_type& first, Args... args) {
				value_type temp[sizeof...(Args)+1] = { first,static_cast<value_type>(args)... };
				return __nth_smallest(n, temp, &temp[sizeof...(Args)+1]);
			}
			//n番目に大きい値(ポインタ)
			static value_type __nth_biggest(size_t n, iterator first, iterator last) {
				//偶数なら2つの平均(奇数の方が高速)
				return __median_struct<>::__find(first, last, distance(first, last) - n - 1);
			}
			//n番目に小さい値(可変長要素)
			template <class... Args>
			static value_type __nth_smallest(size_t n, const value_type& first, Args... args) {
				value_type temp[sizeof...(Args)+1] = { first,static_cast<value_type>(args)... };
				return __nth_smallest(n, temp, &temp[sizeof...(Args)+1]);
			}
			//n番目に小さい値(ポインタ)
			static value_type __nth_smallest(size_t n, iterator first, iterator last) {
				//偶数なら2つの平均(奇数の方が高速)
				return __median_struct<>::__find(first, last, n);
			}

		private:
			//最頻値テーブルを評価
			static value_type __mode_table_evaluation(value_type* table, size_t size) {
				//ソート済みテーブルで最も連続して同じ数が出現するものの探索
				value_type result = table[0], sum = 0, temp = 0;
				for (size_t i = 1; i < size; ++i, ++temp) {
					if (table[i] != table[i - 1]) {
						if (temp > sum) {
							sum = temp;
							result = table[i - 1];
						}
						temp = 0;
					}
				}
				//一番最後の評価
				if (temp > sum) {
					sum = temp;
					result = table[size - 1];
				}
				return result;
			}
		public:

			//最頻値(可変長要素)
			template <class... Args>
			static value_type __mode(const value_type& first, Args... args) {
				value_type temp[sizeof...(Args)+1] = { first,static_cast<value_type>(args)... };
				sort(temp, &temp[sizeof...(Args)+1]);
				return __mode_table_evaluation(temp, sizeof...(Args)+1);
			}
			//最頻値(ポインタ)
			static value_type __mode(iterator first, iterator last) {
				sort(first, last);
				return __mode_table_evaluation(first, distance(first, last));
			}

			//標本分散(可変長要素)
			template <class... Args>
			static value_type __variance(const value_type& first, Args... args) {
				return __average(pow(first, 2), pow(iml::forward<Args>(args), 2)...) - pow(__average(first, iml::forward<Args>(args)...), 2);
			}
			//標本分散(ポインタ)
			static value_type __variance(iterator first, iterator last) {
				//2乗テーブルの作成
				iterator temp = new value_type[last - first];
				for (iterator i = first, *j = temp; i != last; ++i, ++j)
					*j = (*i)*(*i);
				return __average(temp, &temp[last - first]) - pow(__average(first, last), 2);
			}

			//標準偏差(可変長要素)
			template <class... Args>
			static value_type __standard_deviation(const value_type& first, Args... args) {
				return sqrt(__variance(first, iml::forward<Args>(args)...));
			}
			//標準偏差(ポインタ)
			static value_type __standard_deviation(iterator first, iterator last) {
				return sqrt(__variance(first, last));
			}

			//正規化
			static void __normalization(iterator first, iterator last) {
				value_type sd = __standard_deviation(first, last);
				for (; first != last; ++first) *first /= sd;
			}
		};

		//最大値
		template <class First, class... Rest>
		inline First (max)(First first, Rest... rest) {
			return (statistice_function<First*>::__max)(first, iml::forward<Rest>(rest)...);
		}
		template <class BidirectionalIterator>
		inline typename iterator_traits<BidirectionalIterator>::value_type (max)(BidirectionalIterator first, BidirectionalIterator last) {
			return (statistice_function<BidirectionalIterator>::__max)(first, last);
		}
		//最小値
		template <class First, class... Rest>
		inline First (min)(First first, Rest... rest) {
			return (statistice_function<First*>::__min)(first, iml::forward<Rest>(rest)...);
		}
		template <class BidirectionalIterator>
		inline typename iterator_traits<BidirectionalIterator>::value_type (min)(BidirectionalIterator first, BidirectionalIterator last) {
			return (statistice_function<BidirectionalIterator>::__min)(first, last);
		}
		//総和
		template <class First, class... Rest>
		inline First accumulate(First first, Rest... rest) {
			return statistice_function<First*>::__accumulate(first, iml::forward<Rest>(rest)...);
		}
		template <class BidirectionalIterator>
		inline typename iterator_traits<BidirectionalIterator>::value_type accumulate(BidirectionalIterator first, BidirectionalIterator last) {
			return statistice_function<BidirectionalIterator>::__accumulate(first, last);
		}
		//平均値
		template <class First, class... Rest>
		inline First average(First first, Rest... rest) {
			return statistice_function<First*>::__average(first, iml::forward<Rest>(rest)...);
		}
		template <class BidirectionalIterator>
		inline typename iterator_traits<BidirectionalIterator>::value_type average(BidirectionalIterator first, BidirectionalIterator last) {
			return statistice_function<BidirectionalIterator>::__average(first, last);
		}
		//中央値
		template <class First, class... Rest>
		inline First median(First first, Rest... rest) {
			return statistice_function<First*>::__median(first, iml::forward<Rest>(rest)...);
		}
		template <class BidirectionalIterator>
		inline typename iterator_traits<BidirectionalIterator>::value_type median(BidirectionalIterator first, BidirectionalIterator last) {
			return statistice_function<BidirectionalIterator>::__median(first, last);
		}
		//n番目に大きい値
		template <class First, class... Rest>
		inline First nth_biggest(size_t n, First first, Rest... rest) {
			return statistice_function<First*>::__nth_biggest(n, first, iml::forward<Rest>(rest)...);
		}
		template <class BidirectionalIterator>
		inline typename iterator_traits<BidirectionalIterator>::value_type nth_biggest(size_t n, BidirectionalIterator first, BidirectionalIterator last) {
			return statistice_function<BidirectionalIterator>::__nth_biggest(n, first, last);
		}
		//n番目に小さい値
		template <class First, class... Rest>
		inline First nth_smallest(size_t n, First first, Rest... rest) {
			return statistice_function<First*>::__nth_smallest(n, first, iml::forward<Rest>(rest)...);
		}
		template <class BidirectionalIterator>
		inline typename iterator_traits<BidirectionalIterator>::value_type nth_smallest(size_t n, BidirectionalIterator first, BidirectionalIterator last) {
			return statistice_function<BidirectionalIterator>::__nth_smallest(n, first, last);
		}
		//最頻値
		template <class First, class... Rest>
		inline First mode(First first, Rest... rest) {
			return statistice_function<First*>::__mode(first, iml::forward<Rest>(rest)...);
		}
		template <class BidirectionalIterator>
		inline typename iterator_traits<BidirectionalIterator>::value_type mode(BidirectionalIterator first, BidirectionalIterator last) {
			return statistice_function<BidirectionalIterator>::__mode(first, last);
		}
		//標本分散
		template <class First, class... Rest>
		inline First variance(First first, Rest... rest) {
			return statistice_function<First*>::__variance(first, iml::forward<Rest>(rest)...);
		}
		template <class BidirectionalIterator>
		inline typename iterator_traits<BidirectionalIterator>::value_type variance(BidirectionalIterator first, BidirectionalIterator last) {
			return statistice_function<BidirectionalIterator>::__variance(first, last);
		}
		//標本偏差
		template <class First, class... Rest>
		inline First standard_deviation(First first, Rest... rest) {
			return statistice_function<First*>::__standard_deviation(first, iml::forward<Rest>(rest)...);
		}
		template <class BidirectionalIterator>
		inline typename iterator_traits<BidirectionalIterator>::value_type standard_deviation(BidirectionalIterator first, BidirectionalIterator last) {
			return statistice_function<BidirectionalIterator>::__standard_deviation(first, last);
		}
		//正規化
		template <class BidirectionalIterator>
		inline typename iterator_traits<BidirectionalIterator>::value_type normalization(BidirectionalIterator first, BidirectionalIterator last) {
			return statistice_function<BidirectionalIterator>::__normalization(first, last);
		}

	}

	//ベルヌーイ分布
	namespace stats {

		//一様分布
		template <class T>
		class uniform_distribution {
		public:
			using type = T;
		private:
			type				range[2];		//区間[range[0], range[1]]
		public:
			constexpr uniform_distribution() :range{} {}
			constexpr uniform_distribution(type a, type b) : range{ a,b } {}

			void reset(type a, type b) { range[0] = a; range[1] = b; }

			//一様分布な疑似乱数の取得
			template <class S>
			type get(rnd::random_base<S>* handle) { return (*this)(handle); }
			template <class S>
			type operator()(rnd::random_base<S>* handle) {
				return range[0] + (range[1] - range[0])*handle->unit<type>();
			}
		};

		//ベルヌーイ分布
		namespace stats {

			//二項分布
			template <class IntT, class FloatT>
			class binomial_distribution {
			public:
				using n_type = IntT;			//試行回数をあらわす型
				using p_type = FloatT;			//確率をあらわす型
			private:
				n_type	n;
				p_type	p;
			public:
				constexpr binomial_distribution() :n(1), p(1) {}
				constexpr binomial_distribution(n_type n, p_type p) : n(n), p(p) {}

				void reset(n_type n, p_type p) { this->n = n; this->p = p; }

				//二項分布な疑似乱数の取得
				template <class S>
				n_type get(rnd::random_base<S>* handle) { return (*this)(handle); }
				template <class S>
				n_type operator()(rnd::random_base<S>* handle) {
					n_type result = 0;
					for (size_t i = 0; i < n; ++i)
						result += (handle->unit<p_type>() <= p);
					return result;
				}
			};
		}

		//ポアソン分布
		namespace stats {

			//ポアソン分布
			template <class IntT, class FloatT>
			class poisson_distribution {
			public:
				using n_type = IntT;			//試行回数をあらわす型
				using p_type = FloatT;			//確率をあらわす型
			private:
				p_type	lambda;
			public:
				constexpr poisson_distribution() :lambda(1) {}
				constexpr poisson_distribution(p_type l) : lambda(l) {}

				void reset(p_type l) { lambda = l; }

				//指数分布な疑似乱数の取得
				template <class S>
				n_type get(rnd::random_base<S>* handle) { return (*this)(handle); }
				template <class S>
				n_type operator()(rnd::random_base<S>* handle) {
					n_type	k = 0;
					p_type	_exp = exp<p_type>(-lambda);
					p_type	xp = handle->unit<p_type>();
					while (xp >= _exp) {
						xp *= handle->unit<p_type>();
						++k;
					}
					return k;
				}
			};

			//指数分布
			template <class T>
			class exponential_distribution {
			public:
				using type = T;
			private:
				type	lambda;
			public:
				constexpr exponential_distribution() :lambda(1) {}
				constexpr exponential_distribution(type l) : lambda(l) {}

				void reset(type l) { lambda = l; }

				//指数分布な疑似乱数の取得
				template <class S>
				type get(rnd::random_base<S>* handle) { return (*this)(handle); }
				template <class S>
				type operator()(rnd::random_base<S>* handle) {
					return -log(1 - handle->unit<type>()) / lambda;
				}
			};

			//ガンマ分布
			template <class T>
			class gamma_distribution {
			public:
				using type = T;
			private:
				type	alpha;				//形状母数(0 < alpha)
				type	beta;				//尺度母数(0 < beta)
			public:
				constexpr gamma_distribution() :alpha(1), beta(1) {}
				constexpr gamma_distribution(type a, type b) : alpha(b), beta(b) {}

				void reset(type a, type b) { alpha = a; beta = b; }

				//ガンマ分布な疑似乱数の取得
				template <class S>
				type get(rnd::random_base<S>* handle) { return (*this)(handle); }
				template <class S>
				type operator()(rnd::random_base<S>* handle) {
					static type _e = e<type>();
					int int_beta = static_cast<int>(beta);
					double frac_beta = beta - int_beta;

					double u, uu;
					double b, p, x_frac, x_int;

					//整数部
					x_int = 0;
					for (size_t i = 0; i < int_beta; ++i) x_int += -log(handle->unit<type>());		//平均が1の指数分布乱数の加算

					//小数部
					if (abs(frac_beta) < 0.01) x_frac = 0;
					else {
						b = (_e + frac_beta) / _e;
						while (1) {

							u = handle->unit<type>();
							p = b*u;

							uu = handle->unit<type>();

							if (p <= 1) {
								x_frac = pow(p, 1 / frac_beta);
								if (uu <= exp(-x_frac)) break;
							}
							else {
								x_frac = -log((b - p) / frac_beta);
								if (uu <= pow(x_frac, frac_beta - 1)) break;
							}

						}
					}
					return (x_int + x_frac)*alpha;
				}
			};
		}

		//正規分布
		namespace stats {

			//正規分布
			template <class FloatT>
			class normal_distribution {
			public:
				using type = FloatT;
			private:
				type	sigma;			//標準偏差
				type	mu;				//平均
			public:
				constexpr normal_distribution() :n(1), p(1) {}
				constexpr normal_distribution(type s, type m) : sigma(s), mu(m) {}

				void reset(type s, type m) { sigma = s; mu = m; }

				//正規分布な疑似乱数の取得
				template <class S>
				type get(rnd::random_base<S>* handle) { return (*this)(handle); }
				template <class S>
				type operator()(rnd::random_base<S>* handle) {
					static type _2pi = 2 * pi<type>();
					return sigma*sqrt(-2 * log(handle->unit<type>()))*sin(_2pi*handle->unit<type>()) + mu;
				}
			};

			//対数正規分布
			template <class FloatT>
			class lognormal_distribution {
			public:
				using type = FloatT;
			private:
				type	sigma;			//標準偏差
				type	mu;				//平均
			public:
				constexpr lognormal_distribution() :n(1), p(1) {}
				constexpr lognormal_distribution(type s, type m) : sigma(s), mu(m) {}

				void reset(type s, type m) { sigma = s; mu = m; }

				//対数正規分布な疑似乱数の取得(正規分布乱数の指数関数)
				template <class S>
				type get(rnd::random_base<S>* handle) { return (*this)(handle); }
				template <class S>
				type operator()(rnd::random_base<S>* handle) {
					static type _2pi = 2 * pi<type>();
					return exp(sigma*sqrt(-2 * log(handle->unit<type>()))*sin(_2pi*handle->unit<type>()) + mu);
				}
			};

			//逆正規分布
			template <class FloatT>
			class inverse_normal_distribution {
			public:
				using type = FloatT;
			private:
				type	lambda;			//0以上
				type	mu;				//0以上
			public:
				constexpr inverse_normal_distribution() :n(1), p(1) {}
				constexpr inverse_normal_distribution(type l, type m) : lambda(l), mu(m) {}

				void reset(type l, type m) { lambda = l; mu = m; }

				//逆正規分布な疑似乱数の取得(λ(x−μ)^2/μ^2xが自由度1のΧ二乗分布に従う)
				template <class S>
				type get(rnd::random_base<S>* handle) { return (*this)(handle); }
				template <class S>
				type operator()(rnd::random_base<S>* handle) {
					static type _2pi = 2 * pi<type>();
					type x, y, w, z;
					x = sqrt(-2 * log(handle->unit<type>()))*sin(_2pi*handle->unit<type>());	//標準正規
					y = x*x;		//Χ二乗
					w = mu + 0.5*y*mu*mu / lambda - (0.5*mu / lambda)*sqrt(4 * mu*lambda*y + mu*mu*y*y);
					z = handle->unit<type>();

					return (z < mu / (mu + w)) ? w : mu*mu / w;
				}
			};

			//Χ二乗分布
			template <class IntT, class FloatT>
			class chi_squared_distribution {
			public:
				using result_type = FloatT;
				using free_type = IntT;
			private:
				free_type	k;				//自由度
			public:
				constexpr chi_squared_distribution() :k(1) {}
				constexpr chi_squared_distribution(free_type k) : k(k) {}

				void reset(free_type k) { this->k = k; }

				//Χ二乗分布な疑似乱数の取得
				template <class S>
				result_type get(rnd::random_base<S>* handle) { return (*this)(handle); }
				template <class S>
				result_type operator()(rnd::random_base<S>* handle) {
					static result_type _2pi = 2 * pi<result_type>();
					result_type w = 0;

					for (size_t i = 0; i < k; ++i) {
						result_type z = sqrt(-2 * log(handle->unit<result_type>()))*sin(_2pi*handle->unit<result_type>());
						w += z*z;
					}
					return w;
				}
			};

			//コーシー分布
			template <class FloatT>
			class cauchy_distribution {
			public:
				using type = FloatT;
			private:
				type	gamma;			//半値半幅
				type	mu;				//最頻値
			public:
				constexpr cauchy_distribution() :n(1), p(1) {}
				constexpr cauchy_distribution(type g, type m) : gamma(g), mu(m) {}

				void reset(type g, type m) { gamma = g; mu = m; }

				//コーシー分布な疑似乱数の取得
				template <class S>
				type get(rnd::random_base<S>* handle) { return (*this)(handle); }
				template <class S>
				type operator()(rnd::random_base<S>* handle) {
					static type _pi = pi<type>();
					return mu + gamma*tan(_pi*(handle->unit<type>() - 0.5));
				}
			};
		}

		namespace stats {

			//任意の確率密度(質量)関数による疑似乱数生成(ジョン・フォン・ノイマンの棄却法)
			template <class T>
			class rejection_sampling {
			public:
				using type = T;
			private:
				uniform_distribution<type>	uniform;			//任意の範囲で乱数を生成する
			public:
				constexpr rejection_sampling() :uniform() {}
				constexpr rejection_sampling(type a, type b) : uniform(a, b) {}

				void reset(type a, type b) { uniform.reset(a, b); }

				//任意の分布に従う疑似乱数の取得
				template <class S>
				type get(rnd::random_base<S>* handle) { return (*this)(handle); }
				template <class S, class F>
				type operator()(rnd::random_base<S>* handle, F f) {
					type result = uniform(handle);
					while (!(handle->unit<type>() <= f(result))) result = uniform(handle);
					return result;
				}
			};
		}
	}
}

#endif