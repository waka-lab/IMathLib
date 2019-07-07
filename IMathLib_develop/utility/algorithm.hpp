#ifndef IMATH_UTILITY_ALGORITHM_HPP
#define IMATH_UTILITY_ALGORITHM_HPP

//イテレータアルゴリズム

#include "IMathLib/utility/type_traits.hpp"
#include "IMathLib/utility/iterator.hpp"
#include "IMathLib/utility/utility.hpp"


//ソート
namespace iml {

	//イテレータの種類に応じたソートアルゴリズム
	template <class, class, class>
	struct Sort;
	template <class RandomAccessIterator, class Predicate>
	struct Sort<RandomAccessIterator, Predicate, random_access_iterator_tag>{

		using value_type = typename iterator_traits<RandomAccessIterator>::value_type;
		using iterator = RandomAccessIterator;

		static constexpr bool pred_swap(iterator a, iterator b) {
			//イテレータ位置はa < bとなっている
			return Predicate::lt(*b, *a) ? (iml::swap(*a, *b), true) : false;
		}

		//整数の自然対数の整数近似
		static constexpr size_t _log_int_(size_t x) {
			size_t index = 0;			//冪指数
			double c = x;
			if (x <= 1) return 0;			//1未満ならば負数となる

			//xを1に近くする
			//1< x < 2^8 = 256
			while (c > 256) { c /= 256; index += 8; }
			//1 < x < 2^6 = 64
			while (c > 64) { c /= 64; index += 6; }
			//1 < x < 2^4 = 16
			while (c > 16) { c /= 16; index += 4; }
			//1 < x < 2^2 = 4
			while (c > 4) { c /= 4; index += 2; }
			//1 < x < 2
			while (c > 2) { c /= 2; ++index; }
			if (c >= 1.5) ++index;
			return static_cast<size_t>(index*0.69314718056);
		}

		//3つの点の中央値
		static constexpr value_type __median3(const value_type& _1, const value_type& _2, const value_type& _3) {
			//return ((_1 < _2) ? ((_2 < _3) ? _2 : ((_1 < _3) ? _3 : _1)) : ((_1 < _3) ? _1 : ((_2 < _3) ? _3 : _2)));
			return (Predicate::lt(_1, _2)
				? (Predicate::lt(_2, _3)
					? _2 : (Predicate::lt(_1, _3)
						? _3 : _1)) : (Predicate::lt(_1, _3)
							? _1 : (Predicate::lt(_2, _3)
								? _3 : _2)));
		}

		//挿入ソート
		static constexpr void _insert_sort_(iterator first, iterator last) {
			pred_swap(first, first + 1);
			for (iterator i = first + 1; i != last; ++i)
				for (iterator j = i; j != first; --j)
					if (!pred_swap(j - 1, j)) break;
		}
		//イントロソート用のクイックソート
		static constexpr void _quick_sort_(iterator first, iterator last, size_t size) {
			if (distance(first, last) <= size) return;
			iterator i = first, j = last - 1;
			value_type pivot = __median3(*i, *(i + (j - i) / 2), *j);
			while (true) {
				while (Predicate::lt(*i, pivot)) ++i;
				while (Predicate::lt(pivot, *j)) --j;
				if (i >= j) break;
				iml::swap(*i, *j);
				++i; --j;
			}
			_quick_sort_(first, i, size);
			_quick_sort_(++j, last, size);
		}

		//イントロソート
		static constexpr void _sort_(iterator first, iterator last) {
			size_t dis = last - first;
			if (dis <= 1) return;
			_quick_sort_(first, last, _log_int_(dis));
			_insert_sort_(first, last);
		}
	};
	template <class BidirectionalIterator, class Predicate>
	struct Sort<BidirectionalIterator, Predicate, bidirectional_iterator_tag> {

		using value_type = typename iterator_traits<BidirectionalIterator>::value_type;
		using iterator = BidirectionalIterator;

		static constexpr bool pred_swap(iterator a, iterator b) {
			//イテレータ位置はb < aとなっている
			return Predicate::lt(*b, *a) ? iml::swap(*a, *b), true : false;
		}

		//整数の自然対数の整数近似
		static constexpr size_t _log_int_(size_t x) {
			size_t index = 0;			//冪指数
			double c = x;
			if (x <= 1) return 0;			//1未満ならば負数となる

			//xを1に近くする
			//1< x < 2^8 = 256
			while (c > 256) { c /= 256; index += 8; }
			//1 < x < 2^6 = 64
			while (c > 64) { c /= 64; index += 6; }
			//1 < x < 2^4 = 16
			while (c > 16) { c /= 16; index += 4; }
			//1 < x < 2^2 = 4
			while (c > 4) { c /= 4; index += 2; }
			//1 < x < 2
			while (c > 2) { c /= 2; ++index; }
			if (c >= 1.5) ++index;
			return static_cast<size_t>(index*0.69314718056);
		}

		//3つの点の中央値
		static constexpr value_type __median3(const value_type& _1, const value_type& _2, const value_type& _3) {
			//return ((_1 < _2) ? ((_2 < _3) ? _2 : ((_1 < _3) ? _3 : _1)) : ((_1 < _3) ? _1 : ((_2 < _3) ? _3 : _2)));
			return (Predicate::lt(_1, _2)
				? (Predicate::lt(_2, _3)
					? _2 : (Predicate::lt(_1, _3)
						? _3 : _1)) : (Predicate::lt(_1, _3)
							? _1 : (Predicate::lt(_2, _3)
								? _3 : _2)));
		}

		//挿入ソート
		static constexpr void _insert_sort_(iterator first, iterator last) {
			pred_swap(first, next(first));
			for (iterator i = next(first); i != last; ++i)
				for (iterator j = i; j != first; --j)
					if (!pred_swap(prev(j), j)) break;
		}
		//イントロソート用のクイックソート(予め距離を計算しておいて高速化をする)
		static constexpr void _quick_sort_(iterator first, iterator last, size_t size, size_t dis) {
			if (dis <= size) return;
			iterator i = first, j = prev(last);
			--dis;
			value_type pivot = __median3(*i, *next(i, dis / 2), *j);
			size_t cnt = 0;							//イテレータの変化のカウンタ
			while (true) {
				while (Predicate::lt(*i, pivot)) { ++i; ++cnt; }
				while (Predicate::lt(pivot, *j)) { --j; ++cnt; }
				if (dis <= cnt) break;
				iml::swap(*i, *j);
				++i; --j; cnt += 2;
			}
			//次のデータの長さを算出
			dis = distance(first, i);
			_quick_sort_(first, i, size, dis);
			_quick_sort_(++j, last, size, cnt - dis);
		}

		//イントロソート
		static constexpr void _sort_(iterator first, iterator last) {
			size_t dis = distance(first, last);
			if (dis <= 1) return;
			_quick_sort_(first, last, _log_int_(dis), dis);
			_insert_sort_(first, last);
		}
	};
	template <class ForwardIterator, class Predicate>
	struct Sort<ForwardIterator, Predicate, forward_iterator_tag> {

		using value_type = typename iterator_traits<ForwardIterator>::value_type;
		using iterator = ForwardIterator;

		static constexpr void pred_swap(iterator a, iterator b) {
			if (Predicate::lt(*b, *a)) iml::swap(*a, *b);
		}

		//コームソート
		static constexpr void _sort_(iterator first, iterator last) {
			size_t dis = distance(first, last);
			if (dis <= 1) return;
			for (size_t interval = dis * 10 / 13;; interval = interval * 10 / 13) {
				for (iterator i = first, k = next(i, interval); (k = next(i, interval)) != last; ++i)
					pred_swap(i, k);
				if (interval <= 1) break;
			}
		}
	};

	//ソート
	template <class Iterator, class Predicate = type_comparison<typename iterator_traits<Iterator>::value_type>>
	inline constexpr void sort(Iterator first, Iterator last) {
		Sort<Iterator, Predicate, typename iterator_traits<Iterator>::iterator_category>::_sort_(first, last);
	}

}

// 線型探索
namespace iml {

	// イテレータ範囲からvを探索
	template <class InputIterator, class T, class Predicate = type_comparison<typename iterator_traits<InputIterator>::value_type>>
	inline constexpr InputIterator find(InputIterator first, InputIterator last, const T& v) {
		// 入力イテレータでなければならない
		static_assert(is_iterator_v<InputIterator, input_iterator_tag>, "The type of iterator is different.");

		for (; first != last; ++first) if (Predicate::eq(*first, v)) return first;
		return last;
	}
	//イテレータ範囲1からイテレータ範囲2を探索
	template <class InputIterator1, class InputIterator2, class Predicate = type_comparison<void>>
	inline constexpr InputIterator1 find(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2) {
		//入力イテレータでなければならない
		static_assert(is_iterator_v<InputIterator1, input_iterator_tag>, "The type of iterator is different.");
		static_assert(is_iterator_v<InputIterator2, input_iterator_tag>, "The type of iterator is different.");

		InputIterator1 temp1 = first1;
		InputIterator2 temp2 = first2;
		while (first1 != last1) {
			InputIterator1 temp3 = find(next(temp1), last1, *first2);		//平行して探索するためのバッファ
			InputIterator1 temp4 = temp3;									//temp3が次に探索すべき位置の記録
			InputIterator2 temp5 = first2;									//平行して探索する要素の次に探索する文字(temp3==last1対策に2文字目にはしない)
			for (; (temp1 != last1) && (temp2 != last2); ++temp1, ++temp2) {
				if (!Predicate::eq(*temp1, *temp2)) break;

				//平行して探索
				if (temp3 == last1) continue;
				if (Predicate::eq(*temp3, *temp5)) { ++temp3; ++temp5; }
				else {
					//次の探索に変移
					temp4 = find(++temp4, last1, *first2);
					temp3 = temp4;
					temp5 = first2;			//探索位置のリセット
				}
			}
			if (temp2 == last2) return first1;
			//各種更新
			first1 = temp4;
			temp1 = temp3;
			temp2 = temp5;
		}
		return last1;
	}

	//イテレータ範囲からvを後方探索
	template <class BidirectionalIterator, class T, class Predicate = type_comparison<void>>
	inline constexpr BidirectionalIterator rfind(BidirectionalIterator first, BidirectionalIterator last, const T& v) {
		//双方向イテレータでなければならない
		static_assert(is_iterator_v<BidirectionalIterator, bidirectional_iterator_tag>, "The type of iterator is different.");

		for (auto itr = prev(last); first != itr; --itr) if (Predicate::eq(*itr, v)) return itr;
		return (Predicate::eq(*first, v)) ? first : last;
	}
	//イテレータ範囲1からイテレータ範囲2を後方探索
	template <class BidirectionalIterator1, class BidirectionalIterator2, class Predicate = type_comparison<void>>
	inline constexpr BidirectionalIterator1 rfind(BidirectionalIterator1 first1, BidirectionalIterator1 last1, BidirectionalIterator2 first2, BidirectionalIterator2 last2) {
		//双方向イテレータでなければならない
		static_assert(is_iterator_v<BidirectionalIterator1, bidirectional_iterator_tag>, "The type of iterator is different.");
		static_assert(is_iterator_v<BidirectionalIterator2, bidirectional_iterator_tag>, "The type of iterator is different.");

		BidirectionalIterator1 itr = last1;
		BidirectionalIterator1 temp1 = rfind(first1, last1, *first2);
		BidirectionalIterator2 temp2 = first2;
		BidirectionalIterator1 temp3 = rfind(first1, prev(temp1), *first2);		//平行して探索するためのバッファ
		BidirectionalIterator1 temp4 = temp3;									//temp3が次に探索すべき位置の記録
		BidirectionalIterator2 temp5 = first2;									//平行して探索する要素の次に探索する文字(temp3==last1対策に2文字目にはしない)
		last1 = temp4;
		while (itr != temp1) {
			for (; (temp1 != itr) && (temp2 != last2); ++temp1, ++temp2) {
				if (!Predicate::eq(*temp1, *temp2)) break;

				//平行して探索
				if (temp3 == itr) break;
				if (Predicate::eq(*temp3, *temp5)) { ++temp3; ++temp5; }
				else {
					//次の探索に変移
					temp4 = rfind(first1, --temp4, *first2);
					temp3 = temp4;
					temp5 = first2;			//探索位置のリセット
				}
			}
			if (temp2 == last2) return last1;
			//各種更新
			last1 = temp4;
			temp1 = temp3;
			temp2 = temp5;
			temp3 = rfind(first1, prev(temp1), *first2);
			temp4 = temp3;
			temp5 = first2;
		}
		return itr;
	}

	//イテレータ範囲から条件を満たすものを探索
	template <class InputIterator, class Predicate>
	inline constexpr InputIterator find_if(InputIterator first, InputIterator last, Predicate pred) {
		//入力イテレータでなければならない
		static_assert(is_iterator_v<InputIterator, input_iterator_tag>, "The type of iterator is different.");

		for (; first != last; ++first)
			if (pred(*first)) return first;
		return last;
	}
	//イテレータ範囲から条件を満たすものを後方探索
	template <class BidirectionalIterator, class Predicate>
	inline constexpr BidirectionalIterator rfind_if(BidirectionalIterator first, BidirectionalIterator last, Predicate pred) {
		//双方向イテレータでなければならない
		static_assert(is_iterator_v<BidirectionalIterator, bidirectional_iterator_tag>, "The type of iterator is different.");

		for (BidirectionalIterator itr = prev(last); first != itr; --itr)
			if (pred(*itr)) return itr;
		return (pred(*first)) ? first : last;
	}

	//イテレータから最大要素のイテレータを返す
	template <class ForwardIterator>
	inline constexpr ForwardIterator max_element(ForwardIterator first, ForwardIterator last) {
		//前方向イテレータでなければならない
		static_assert(is_iterator_v<ForwardIterator, forward_iterator_tag>, "The type of iterator is different.");

		ForwardIterator p = first++;
		if (first == last) return first;
		while (first != last) {
			if (*p < *first) p = first;
			++first;
		}
		return p;
	}
	//イテレータから最小要素のイテレータを返す
	template <class ForwardIterator>
	inline constexpr ForwardIterator min_element(ForwardIterator first, ForwardIterator last) {
		//前方向イテレータでなければならない
		static_assert(is_iterator_v<ForwardIterator, forward_iterator_tag>, "The type of iterator is different.");

		ForwardIterator p = first++;
		if (first == last) return first;
		while (first != last) {
			if (*p > *first) p = first;
			++first;
		}
		return p;
	}
}


namespace iml {

	// データのコピー(順)
	template <class OutputIterator, class InputIterator>
	inline constexpr OutputIterator copy_order(OutputIterator first1, InputIterator first2, size_t n) {
		static_assert(is_iterator_v<OutputIterator, output_iterator_tag>, "The type of iterator is different.");
		static_assert(is_iterator_v<InputIterator, input_iterator_tag>, "The type of iterator is different.");

		for (size_t i = 0; i < n; ++i, ++first1, ++first2) *first1 = *first2;
		return first1;
	}
	template <class OutputIterator, class InputIterator>
	inline constexpr OutputIterator copy_order(OutputIterator first1, InputIterator first2, InputIterator last2) {
		static_assert(is_iterator_v<OutputIterator, output_iterator_tag>, "The type of iterator is different.");
		static_assert(is_iterator_v<InputIterator, input_iterator_tag>, "The type of iterator is different.");

		for (; first2 != last2; ++first1, ++first2) *first1 = *first2;
		return first1;
	}
	// データのムーブ(順)
	template <class OutputIterator, class ForwardIterator>
	inline constexpr OutputIterator move_order(OutputIterator first1, ForwardIterator first2, size_t n) {
		static_assert(is_iterator_v<OutputIterator, output_iterator_tag>, "The type of iterator is different.");
		static_assert(is_iterator_v<ForwardIterator, forward_iterator_tag>, "The type of iterator is different.");

		for (size_t i = 0; i < n; ++i, ++first1, ++first2) *first1 = move(*first2);
		return first1;
	}
	template <class OutputIterator, class ForwardIterator>
	inline constexpr OutputIterator move_order(OutputIterator first1, ForwardIterator first2, ForwardIterator last2) {
		static_assert(is_iterator_v<OutputIterator, output_iterator_tag>, "The type of iterator is different.");
		static_assert(is_iterator_v<ForwardIterator, forward_iterator_tag>, "The type of iterator is different.");

		for (; first2 != last2; ++first1, ++first2) *first1 = move(*first2);
		return first1;
	}
	// データのコピー(逆順→first2の順序を逆にして挿入するのではなく後ろから挿入するという意)
	template <class BidirectionalIterator1, class BidirectionalIterator2>
	inline constexpr BidirectionalIterator1 copy_reverse_order(BidirectionalIterator1 first1, BidirectionalIterator2 first2, size_t n) {
		static_assert(is_iterator_v<BidirectionalIterator1, bidirectional_iterator_tag>, "The type of iterator is different.");
		static_assert(is_iterator_v<BidirectionalIterator2, bidirectional_iterator_tag>, "The type of iterator is different.");
		//判定をしなければならない
		if (n == 0) return first1;

		advance(first1, n - 1);
		BidirectionalIterator1 result = next(first1);
		BidirectionalIterator2 itr = next(first2, n - 1);
		for (; itr != first2; --itr, --first1) *first1 = *itr;
		*first1 = *first2;

		return result;
	}
	template <class BidirectionalIterator1, class BidirectionalIterator2>
	inline constexpr BidirectionalIterator1 copy_reverse_order(BidirectionalIterator1 first1, BidirectionalIterator2 first2, BidirectionalIterator2 last2) {
		static_assert(is_iterator_v<BidirectionalIterator1, bidirectional_iterator_tag>, "The type of iterator is different.");
		static_assert(is_iterator_v<BidirectionalIterator2, bidirectional_iterator_tag>, "The type of iterator is different.");
		// 判定をしなければならない
		if (first2 == last2) return first1;

		advance(first1, distance(first2, last2) - 1);
		BidirectionalIterator1 result = next(first1);
		--last2;
		for (; last2 != first2; --last2, --first1) *first1 = *last2;
		*first1 = *first2;

		return result;
	}
	// データのムーブ(逆順→first2の順序を逆にして挿入するのではなく後ろから挿入するという意)
	template <class BidirectionalIterator1, class BidirectionalIterator2>
	inline constexpr BidirectionalIterator1 move_reverse_order(BidirectionalIterator1 first1, BidirectionalIterator2 first2, size_t n) {
		static_assert(is_iterator_v<BidirectionalIterator1, bidirectional_iterator_tag>, "The type of iterator is different.");
		static_assert(is_iterator_v<BidirectionalIterator2, bidirectional_iterator_tag>, "The type of iterator is different.");
		//判定をしなければならない
		if (n == 0) return first1;

		advance(first1, n - 1);
		BidirectionalIterator1 result = next(first1);
		BidirectionalIterator2 itr = next(first2, n - 1);
		for (; itr != first2; --itr, --first1) *first1 = move(*itr);
		*first1 = *first2;

		return result;
	}
	template <class BidirectionalIterator1, class BidirectionalIterator2>
	inline constexpr BidirectionalIterator1 move_reverse_order(BidirectionalIterator1 first1, BidirectionalIterator2 first2, BidirectionalIterator2 last2) {
		static_assert(is_iterator_v<BidirectionalIterator1, bidirectional_iterator_tag>, "The type of iterator is different.");
		static_assert(is_iterator_v<BidirectionalIterator2, bidirectional_iterator_tag>, "The type of iterator is different.");
		// 判定をしなければならない
		if (first2 == last2) return first1;

		advance(first1, distance(first2, last2) - 1);
		BidirectionalIterator1 result = next(first1);
		--last2;
		for (; last2 != first2; --last2, --first1) *first1 = move(*last2);
		*first1 = *first2;

		return result;
	}

	// 条件が真のもののみコピー
	template <class OutputIterator, class InputIterator, class F>
	inline constexpr OutputIterator copy_if(OutputIterator first1, InputIterator first2, size_t n, F f) {
		static_assert(is_iterator_v<OutputIterator, output_iterator_tag>, "The type of iterator is different.");
		static_assert(is_iterator_v<InputIterator, input_iterator_tag>, "The type of iterator is different.");

		for (size_t i = 0; i < n; ++i, ++first2) if (f(*first2)) { *first1 = *first2; ++first1; }
		return first1;
	}
	template <class OutputIterator, class InputIterator, class F>
	inline constexpr OutputIterator copy_if(OutputIterator first1, InputIterator first2, InputIterator last2, F f) {
		static_assert(is_iterator_v<OutputIterator, output_iterator_tag>, "The type of iterator is different.");
		static_assert(is_iterator_v<InputIterator, input_iterator_tag>, "The type of iterator is different.");

		for (; first2 != last2; ++first2) if (f(*first2)) { *first1 = *first2; ++first1; }
		return first1;
	}

	//指定された値でイテレータ範囲を埋める
	template <class ForwardIterator, class T>
	inline constexpr ForwardIterator fill(ForwardIterator first, ForwardIterator last, const T& v) {
		static_assert(is_iterator_v<ForwardIterator, forward_iterator_tag>, "The type of iterator is different.");

		for (; first != last; ++first) *first = v;
		return last;
	}
}

//その他
namespace iml {

	//範囲の全てに関数の適応
	template<class InputIterator, class F>
	inline constexpr F for_each(InputIterator first, InputIterator last, F f) {
		static_assert(is_iterator_v<InputIterator, input_iterator_tag>, "The type of iterator is different.");

		for (; first != last; ++first) f(*first);
		return f;
	}
}


#endif