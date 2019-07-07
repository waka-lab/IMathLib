#ifndef IMATH_STRING_STRING_HPP
#define IMATH_STRING_STRING_HPP

#include "IMathLib/container/array.hpp"
#include "IMathLib/utility/smart_ptr.hpp"
#include "IMathLib/utility/utility.hpp"
#include "IMathLib/utility/algorithm.hpp"

namespace iml {

	//文字列の長さの取得
	template <class CharT>
	inline size_t string_length(const CharT* str) {
		size_t temp = 0;
		for (; *str != CharT(0); ++str, ++temp);
		return temp;
	}
	//文字列の探索(ないときはnullptrを返す)
	template <class CharT>
	inline const CharT* string_find(const CharT* str1, size_t len1, const CharT* str2, size_t len2) {
		if (len1 < len2) return nullptr;
		// 探索文字列の文字数が1のときは単純なfind
		const CharT* result = (len2 == 1) ? find(str1, str1 + len1, *str2) : find(str1, str1 + len1, str2, str2 + len2);
		// ないときはnullptrを返す
		return (result == str1 + len1) ? nullptr : result;
	}
	template <class CharT>
	inline const CharT* string_find(const CharT* str1, const CharT* str2) {
		return string_find(str1, string_length(str1), str2, string_length(str2));
	}
	template <class CharT>
	inline const CharT* string_find(const CharT* str1, size_t len1, const CharT* str2) {
		return string_find(str1, len1, str2, string_length(str2));
	}
	template <class CharT>
	inline const CharT* string_find(const CharT* str1, const CharT* str2, size_t len2) {
		return string_find(str1, string_length(str1), str2, len2);
	}
	//後方から文字列の探索
	template <class CharT>
	inline const CharT* string_rfind(const CharT* str1, size_t len1, const CharT* str2, size_t len2) {
		if (len1 < len2) return nullptr;
		//探索文字列の文字数が1のときは単純なrfind
		const CharT* result = (len2 == 1) ? rfind(str1, str1 + len1, *str2) : rfind(str1, str1 + len1, str2, str2 + len2);
		return (result == str1 + len1) ? nullptr : result;
	}
	template <class CharT>
	inline const CharT* string_rfind(const CharT* str1, const CharT* str2) {
		return string_rfind(str1, string_length(str1), str2, string_length(str2));
	}
	template <class CharT>
	inline const CharT* string_rfind(const CharT* str1, size_t len1, const CharT* str2) {
		return string_rfind(str1, len1, str2, string_length(str2));
	}
	template <class CharT>
	inline const CharT* string_rfind(const CharT* str1, const CharT* str2, size_t len2) {
		return string_rfind(str1, string_length(str1), str2, len2);
	}


	// コンテナを1本のひものように連結・切り取り等ができるようにする
	// 文字列ライクな設計をとる
	// Container : push_backが可能でなければならない
	template <class CharT, class Container = dynamic_array<CharT>, class Predicate = type_comparison<CharT>>
	class string : public Container {
	public:

		using typename Container::value_type;
		using typename Container::reference;
		using typename Container::const_reference;
		using typename Container::iterator;
		using typename Container::const_iterator;
		using typename Container::allocator_type;
		using typename Container::size_type;

		static constexpr size_type npos = (numeric_traits<size_type>::max)();

		using Container::Container;
		string() : Container() {}
		// 文字列ライクな初期化
		string(const CharT* s) : Container() {
			size_type size = string_length(s);
			switch (size) {
			case 0:
				break;
			case 1:
				push_back(*s);
				break;
			default:
				// 一番最後の要素を挿入してからその位置に全部挿入する
				push_back(s[size - 1]);
				insert(begin(), s, s + (size - 1));
				break;
			}
		}
		string(const CharT* s, size_type size) : Container(s, s + size) {}

		// ひもの長さの取得
		size_type length() const { return size(); }

		// 代入
		using Container::operator=;
		// 文字列ライクな代入
		string& operator=(const CharT* s) {
			size_type len = string_length(s);
			clear();
			switch (len) {
			case 0:
				break;
			case 1:
				push_back(*s);
				break;
			default:
				// 一番最後の要素を挿入してからその位置に全部挿入する
				push_back(s[len - 1]);
				insert(begin(), s, s + (len - 1));
				break;
			}
			return *this;
		}
		string& operator=(const CharT& s) {
			clear(); push_back(s);
			return *this;
		}
		// ひもの連結
		string& operator+=(const string& s) {
			if (empty()) return *this = s;
			if (s.empty()) return *this;
			else {
				insert(end(), s.begin(), s.end());
				return *this;
			}
		}
		string& operator+=(const CharT& s) {
			push_back(s);
			return *this;
		}
		// 文字列ライクな連結
		string& operator+=(const CharT* s) {
			if (empty()) return *this = s;

			size_type len = string_length(s);
			if (len == 0) return *this;
			else {
				insert(end(), s, s + len);
				return *this;
			}
			return *this;
		}

		// 前方探索(nposのときなし)
		size_type find(const string& key, size_type pos = 0) {
			if (pos > length()) return npos;
			size_type temp = distance(begin(), iml::find<iterator, CharT, Predicate>(next(begin(), pos), end(), key.begin(), key.end()));
			return temp == length() ? npos : temp;
		}
		size_type find(const CharT& key, size_type pos = 0) {
			if (pos > length()) return npos;
			size_type temp = distance(begin(), iml::find<iterator, CharT, Predicate>(next(begin(), pos), end(), key));
			return temp == length() ? npos : temp;
		}
		size_type find(const CharT* key, size_type pos = 0) {
			if (pos > length()) return npos;
			size_type len = string_length(key);
			size_type temp = distance(begin(), iml::find<iterator, const CharT*, Predicate>(next(begin(), pos), end(), key, key + len));
			return temp == length() ? npos : temp;
		}
		// 後方探索(nposのときなし)
		size_type rfind(const string& key, size_type pos = npos) {
			size_type temp = distance(begin(), iml::rfind<iterator, CharT, Predicate>(begin(), next(begin(), (iml::min)(pos, length())), key.begin(), key.end()));
			return temp == length() ? npos : temp;
		}
		size_type rfind(const CharT& key, size_type pos = npos) {
			size_type temp = distance(begin(), iml::rfind<iterator, CharT, Predicate>(begin(), next(begin(), (iml::min)(pos, length())), key));
			return temp == length() ? npos : temp;
		}
		size_type rfind(const CharT* key, size_type pos = npos) {
			size_type len = string_length(key);
			size_type temp = distance(begin(), iml::rfind<iterator, CharT, Predicate>(begin(), next(begin(), (iml::min)(pos, length())), key, key + len));
			return temp == length() ? npos : temp;
		}

		// ひもの一部の取得
		string substr(const_iterator first, const_iterator last) {
			return string(first, last);
		}
		// pos <= length()でなければならない
		string substr(size_type pos, size_type size = npos) {
			if (pos > length()) throw std::length_error("pos is larger than length().");
			iterator first = next(begin(), pos);
			return string(first, next(first, (iml::min)(length() - pos, size)));
		}
	};

	//2項演算
	template <class CharT, class Container, class Predicate>
	inline string<CharT, Container, Predicate> operator+(const string<CharT, Container, Predicate>& s1, const string<CharT, Container, Predicate>& s2) {
		string<CharT, Container, Predicate> str(s1);
		return (str += s2);
	}
	template <class CharT, class Container, class Predicate>
	inline string<CharT, Container, Predicate> operator+(const CharT* s1, const string<CharT, Container, Predicate>& s2) {
		string<CharT, Container, Predicate> str(s1);
		return (str += s2);
	}
	template <class CharT, class Container, class Predicate>
	inline string<CharT, Container, Predicate> operator+(const CharT& s1, const string<CharT, Container, Predicate>& s2) {
		string<CharT, Container, Predicate> str(s1);
		return (str += s2);
	}
	template <class CharT, class Container, class Predicate>
	inline string<CharT, Container, Predicate> operator+(const string<CharT, Container, Predicate>& s1, const CharT* s2) {
		string<CharT, Container, Predicate> str(s1);
		return (str += s2);
	}
	template <class CharT, class Container, class Predicate>
	inline string<CharT, Container, Predicate> operator+(const string<CharT, Container, Predicate>& s1, const CharT& s2) {
		string<CharT, Container, Predicate> str(s1);
		return (str += s2);
	}
	//char型のstring
	using cstring = string<char>;
	//wchar_t型のstring
	using wstring = string<wchar_t>;

}

#endif