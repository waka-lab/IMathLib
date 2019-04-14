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
		//探索文字列の文字数が1のときは単純なfind
		const CharT* result = (len2 == 1) ? find(str1, str1 + len1, *str2) : find(str1, str1 + len1, str2, str2 + len2);
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


	//null文字を代入しないことを前提とした文字列クラス
	template <class CharT, class Predicate = type_comparison<CharT>, class Allocator = allocator<CharT, array_iterator<CharT>>>
	class string : public dynamic_array_container<CharT, Allocator> {
		using typename dynamic_array_container<CharT, Allocator>::value_type;
		using typename dynamic_array_container<CharT, Allocator>::iterator;
		using typename dynamic_array_container<CharT, Allocator>::const_iterator;
		using typename dynamic_array_container<CharT, Allocator>::allocator_type;
	public:
		string() { this->reserve(2); this->_p[0] = CharT(0); this->_use_size = 1; }
		string(const CharT& s) { push_back(s); }
		string(const CharT* s) {
			size_t size = string_length(s);
			this->reserve(get_power_of_2(size + 1), s, size + 1);
		}
		string(const CharT* s, size_t size) {
			this->reserve(get_power_of_2(size + 1), s, size + 1);
			this->_p[size] = CharT(0);
		}
		string(const string& s) { this->reserve(s._size, s._p, s._use_size); }
		string(string&& s) { *this = move(s); }
		string(const CharT* first, const CharT* last) {
			if (last == nullptr || first == nullptr) {
				push_back(CharT(0));
				return;
			}
			size_t size = last - first;
			this->reserve(get_power_of_2(size + 1), first, size + 1);
			this->_p[size] = CharT(0);
		}
		~string() {}

		//イテレータの取得
		iterator begin() noexcept { return _p; }
		const_iterator begin() const noexcept { return _p; }
		iterator end() noexcept { return &_p[_use_size - 1]; }
		const_iterator end() const noexcept { return &_p[_use_size - 1]; }

		//文字列の長さ取得
		size_t length() { return _use_size - 1; }

		//要素の削除
		void erase(size_t pos = 0, size_t n = npos) {
			this->dynamic_array_container<CharT, Allocator>::erase(pos, n);
			*this->end() = CharT(0);
		}
		void erase(const CharT* itr) {
			this->dynamic_array_container<CharT, Allocator>::erase(itr);
			*this->end() = CharT(0);
		}
		void erase(const CharT* first, const CharT* last) {
			this->dynamic_array_container<CharT, Allocator>::erase(first, last);
			*this->end() = CharT(0);
		}

		//代入
		string& operator=(const string& s) {
			//確保メモリサイズが足りないときは再確保
			if (this->_size < s._use_size) {
				this->reserve(get_power_of_2(s._use_size), s._p, s._use_size);
				return *this;
			}
			//足りているときはそのまま実行
			this->_use_size = s._use_size;
			array_copy_order(this->_p, s._p, this->_use_size);
			return *this;
		}
		string& operator=(string&& s) noexcept {
			this->_p = s._p; s._p = nullptr;
			this->_use_size = s._use_size;
			this->_size = s._size;
			return *this;
		}
		string& operator=(const CharT* s) {
			size_t size = string_length(s);
			//'\0'を考慮したメモリ確保のチェック
			if (this->_size < size + 1) {
				this->reserve(get_power_of_2(size + 1), s, size + 1);
				return *this;
			}
			this->_use_size = size + 1;
			copy_order(this->_p, s, size + 1);
			return *this;
		}
		string& operator=(const CharT& s) {
			if (this->_size < 2)
				this->reserve(4);
			this->_p[0] = s;
			this->_p[1] = CharT(0);
			this->_use_size = 2;
			return *this;
		}
		string& operator+=(const string& s) {
			if (this->_size <= this->_use_size + s._use_size - 1)
				this->reserve(get_power_of_2(this->_use_size + s._use_size - 1));
			--this->_use_size;				//'\0'の分をバックする
			copy_order(&this->_p[this->_use_size], s._p, s._use_size);
			this->_use_size += s._use_size;
			return *this;
		}
		string& operator+=(const CharT* s) {
			size_t size = string_length(s);
			if (this->_size < this->_use_size + size)
				this->reserve(get_power_of_2(this->_use_size + size));
			--this->_use_size;				//'\0'の分をバックする
			copy_order(&this->_p[this->_use_size], s, size + 1);
			this->_use_size += size + 1;			//'\0'の分を加算
			return *this;
		}
		string& operator+=(const CharT& s) {
			this->push_back(s);
			return *this;
		}

		//後方にデータの挿入
		void push_back(const CharT& s) {
			if (this->_size < this->_use_size + 1)
				this->reserve(get_power_of_2(this->_use_size + 1));
			this->_p[this->_use_size - 1] = s;
			this->_p[this->_use_size] = CharT(0);
			++this->_use_size;
		}
		//後方のデータの削除
		void pop_back() {
			_use_size -= (_use_size != 0) ? 1 : 0;
			_p[_use_size - 1] = CharT(0);
		}
		//文字列の先頭ポインタの取得
		const CharT* c_str() const noexcept { return pointer_traits<CharT*>::pointer_to(*_p); }

		//前方探索(nposのときなし)
		size_t find(const string& key, size_t pos = 0) {
			iterator itr = iml::find<iterator, CharT, Predicate>(begin() + (iml::min)(pos, length()), end(), key.begin(), key.end());
			return (itr - begin()) == length() ? npos : itr - begin();
		}
		size_t find(const CharT& key, size_t pos = 0) {
			iterator itr = iml::find<iterator, CharT, Predicate>(begin() + (iml::min)(pos, length()), end(), key);
			return (itr - begin()) == length() ? npos : itr - begin();
		}
		size_t find(const CharT* key, size_t pos = 0) {
			size_t size = string_length(key);
			iterator itr = iml::find<iterator, const CharT*, Predicate>(begin() + (iml::min)(pos, length()), end(), key, key + size);
			return (itr - begin()) == length() ? npos : itr - begin();
		}
		//後方探索(nposのときなし)
		size_t rfind(const string& key, size_t pos = npos) {
			iterator itr = iml::rfind<iterator, CharT, Predicate>(begin() + (iml::min)(pos, length()), end(), key.begin(), key.end());
			return (itr - begin()) == length() ? npos : itr - begin();
		}
		size_t rfind(const CharT& key, size_t pos = npos) {
			iterator itr = iml::rfind<iterator, CharT, Predicate>(begin(), begin() + (iml::min)(pos, length()), key);
			return (itr - begin()) == length() ? npos : itr - begin();
		}
		size_t rfind(const CharT* key, size_t pos = npos) {
			size_t size = string_length(key);
			iterator itr = iml::rfind<iterator, const CharT*, Predicate>(begin(), begin() + (iml::min)(pos, length()), key, key + size);
			return (itr - begin()) == length() ? npos : itr - begin();
		}

		//文字列の一部の取得
		string substr(size_t pos, size_t size = npos) {
			size_t p = (iml::min)(this->_use_size, pos);
			return string(&this->_p[p], (iml::min)(this->_use_size - p, size));
		}

		//空かの判定
		bool empty() const noexcept { return (this->_p[0] == CharT(0)) || (this->_p == nullptr); }

		CharT& operator[](size_t index) noexcept { return _p[index]; }
		const CharT& operator[](size_t index) const noexcept { return _p[index]; }
	};
	//2項演算
	template <class CharT, class Predicate, class Allocator>
	inline string<CharT, Predicate, Allocator> operator+(const string<CharT, Predicate, Allocator>& s1, const string<CharT, Predicate, Allocator>& s2) {
		string<CharT, Predicate, Allocator> str(s1);
		return (str += s2);
	}
	template <class CharT, class Predicate, class Allocator>
	inline string<CharT, Predicate, Allocator> operator+(const CharT* s1, const string<CharT, Predicate, Allocator>& s2) {
		string<CharT, Predicate, Allocator> str(s1);
		return (str += s2);
	}
	template <class CharT, class Predicate, class Allocator>
	inline string<CharT, Predicate, Allocator> operator+(const CharT& s1, const string<CharT, Predicate, Allocator>& s2) {
		string<CharT, Predicate, Allocator> str(s1);
		return (str += s2);
	}
	template <class CharT, class Predicate, class Allocator>
	inline string<CharT, Predicate, Allocator> operator+(const string<CharT, Predicate, Allocator>& s1, const CharT* s2) {
		string<CharT, Predicate, Allocator> str(s1);
		return (str += s2);
	}
	template <class CharT, class Predicate, class Allocator>
	inline string<CharT, Predicate, Allocator> operator+(const string<CharT, Predicate, Allocator>& s1, const CharT& s2) {
		string<CharT, Predicate, Allocator> str(s1);
		return (str += s2);
	}
	//char型のstring
	using cstring = string<char>;
	//wchar_t型のstring
	using wstring = string<wchar_t>;

}

#endif