#ifndef _HUFFMAN_HPP
#define _HUFFMAN_HPP

#include <bitset>
#include <array>
#include "IMathLib/container/array.hpp"

namespace iml {

	using ChildIndexArray = std::array<size_t, 2>;

	//ビットストリーム(メモリは管理せず参照するのみ)
	class _Bit_stream {
		const unsigned char*	binary;
		size_t				size;

		size_t				_next_bit = 0;
		size_t				_next_byte = 0;
	public:
		constexpr _Bit_stream(const unsigned char* binary, size_t byte) :binary(binary), size(byte) {}

		//終端
		bool eof() { return _next_byte >= size; }

		//1ビットロード(戻り値に値を返す)
		size_t get() {
			size_t bit = top();
			next_bit(1);
			return bit;
		}

		//ビット列をロード
		//符号ではないデータは 要素の最下位ビットから順にパックされている
		size_t get_range(size_t n) {
			size_t bit = 0;
			for (size_t i = 0; i < n; ++i) bit |= (get() << i);
			return bit;
		}
		//ビット列をロード
		//符号化されたデータは最上位ビットから順にパックされている
		size_t get_coded_range(size_t numbit) {
			size_t bit = 0;
			for (size_t i = 0; i < numbit; ++i) {
				bit <<= 1;
				bit |= get();
			}
			return bit;
		}

		//次のbyte境界までスキップ
		void skip_byte() {
			if (_next_bit == 0) return;
			_next_bit = 0;
			++_next_byte;
		}

		//現在見ているbit
		int top() const {
			return (binary[_next_byte] & (1 << _next_bit)) != 0;
		}

		//ビットシフト
		void next_bit(size_t n) {
			_next_bit += n;
			if (_next_bit >= 8) {
				_next_byte += _next_bit >> 3;
				_next_bit &= 8 - 1;
			}
		}
		//バイトシフト
		void next_byte(size_t n) {
			_next_byte += n;
		}

		//! 1ビットロードする
		_Bit_stream& operator >> (int& out) {
			out = get();
			return *this;
		}

		size_t now_bit() const { return _next_bit; }
		size_t now_byte() const { return _next_byte; }

		//参照ストリームのためconstイテレータのみを定義
		using const_iterator = array_iterator<const unsigned char>;
		const_iterator begin() const noexcept { return binary; }
		const_iterator end() const noexcept { return binary + size; }
	};

	//接頭符号ツリー
	template<typename T>
	class _Basic_prefix_tree {
		//無効なインデックス
		static constexpr size_t invalid_index = -1;
	public:

		//ノードクラス
		class Node {
			friend class _Basic_prefix_tree<T>;

			T				m_value = T{};
			bool			m_hasvalue = false;
			ChildIndexArray	m_childIndex{ invalid_index ,invalid_index };
		public:
			T GetValue() const { return m_value; }
			void SetValue(T val) {
				m_value = val;
				m_hasvalue = true;
			}

			//このノードが値を持っているか
			bool HasValue() const noexcept { return m_hasvalue; }

			//子へのIndexを返す(値は_Basic_prefix_treeの持つvectorへのIndexを指す)
			size_t ChildIndex(int bit) const {
				return m_childIndex.at(bit);
			}
		};

		//ツリーをたどるクラス
		class _Tree_walker {
			friend class _Basic_prefix_tree<T>;

		public:

			//現在指しているノードを返す
			const typename _Basic_prefix_tree<T>::Node& Get() const {
				//return m_tree.m_nodeList.at(m_current);
				return m_tree.m_nodeList[m_current];
			}

			//現在指しているノードへアクセス
			const typename _Basic_prefix_tree<T>::Node* operator->() const { return &Get(); }

			//指定した分岐先に子が存在するか
			bool HasChild(int bit) const {
				return GetNextIndex(bit) != invalid_index;
			}

			//移動
			bool Next(int nextBit) {
				m_current = GetNextIndex(nextBit);
				return m_current != invalid_index;
			}

			_Tree_walker(const _Basic_prefix_tree& tree)
				:m_tree(tree)
				, m_current(tree.m_nodeTop)
			{}

		private:

			const _Basic_prefix_tree&	m_tree;
			size_t					m_current;

			//指定した分岐先の子を指すIndexを返す
			size_t GetNextIndex(int bit) const {
				return Get().ChildIndex(bit);
			}
		};

		//ランダムアクセス
		//template<unsigned numBit>
		//Node& operator[] (std::bitset<numBit> key);

		//符号とそれに対応する値を登録する
		bool Entry(unsigned bitset, T value, unsigned numRead) {
			constexpr auto CAPACITY_LENGTH = std::numeric_limits<unsigned>::digits;
			if (numRead > CAPACITY_LENGTH) throw std::runtime_error("ビット長さの指定が不正です");

			using Bitset = std::bitset<CAPACITY_LENGTH>;
			return Entry(Bitset(bitset), value, numRead);
		}
		//符号とそれに対応する値を登録する
		template<unsigned numBit>
		bool Entry(std::bitset<numBit> bitset, T value, unsigned numRead = numBit) {
			auto result = CreateNodeIfNotFound(bitset, numRead);
			if (result.second == false)
				return false;

			result.first.SetValue(value);
			return true;
		}

		//! コンストラクタ
		_Basic_prefix_tree() {
			//! 先頭ノードの作成
			m_nodeList.push_back(Node{});
			m_nodeTop = 0;
		}

	private:
		dynamic_array<Node>		m_nodeList;
		size_t				m_nodeTop = invalid_index;

		//指定された接頭符号に対応するノードを返す
		template<unsigned numBit>
		std::pair<Node&, bool> CreateNodeIfNotFound(std::bitset<numBit> bits, unsigned numRead) {
			_Tree_walker walker(*this);

			// 先頭ビットから順に読む
			int i = static_cast<int>(numRead) - 1;

			// ツリーをたどって、指定されたkeyにすでにノードが存在するかを確認
			for (; i >= 0; --i) {
				if (walker.HasChild(bits[i]) == false) break;
				walker.Next(bits[i]);
			}
			//if (i < 0) return std::pair<Node&, bool>(m_nodeList.at(walker.m_current), false);
			if (i < 0) return std::pair<Node&, bool>(m_nodeList[walker.m_current], false);

			// (ノードが作られていなかった続きから) 新しくノードを作成
			for (; i >= 0; --i) {
				size_t newIndex = static_cast<size_t>(m_nodeList.size());
				m_nodeList.push_back(Node{});

				//auto& node = m_nodeList.at(walker.m_current);
				auto& node = m_nodeList[walker.m_current];
				node.m_childIndex[(bits[i])] = newIndex;

				walker.Next(bits[i]);
			}

			//return std::pair<Node&, bool>(m_nodeList.at(walker.m_current), true);
			return std::pair<Node&, bool>(m_nodeList[walker.m_current], true);
		}
	};

	//現在のビットストリームと木から復号化
	template<typename T>
	inline bool _Bit_tree_decode(_Bit_stream& stream, const _Basic_prefix_tree<T>& tree, T* out) {
		auto walker = typename _Basic_prefix_tree<T>::_Tree_walker(tree);
		while (!stream.eof()) {
			int bit = stream.get();

			//対応する符号が見つからない
			if (walker.HasChild(bit) == false) break;
			//次のビットに移動
			walker.Next(bit);

			//値を持つノードが見つかったらそれが復号データ
			if (walker->HasValue()){
				*out = walker->GetValue();
				return true;
			}
		}
		return false;
	}
}

namespace iml {

	using _Prefix_code_tree = _Basic_prefix_tree<size_t>;

	//ハフマン符号
	class huffman {

	public:
		//エンコード
		static dynamic_array<unsigned char> encode(const unsigned char* binary, size_t byte) {
			dynamic_array<unsigned char> result;
			size_t sum_table[256] = {};				//ASCIIコードの和テーブル

			//和のテーブルの作成
			for (size_t i = 0; i < byte; ++i) ++sum_table[binary[i]];

		}

		//デコード
		static dynamic_array<unsigned char> decode(const unsigned char* binary, size_t byte) {
			dynamic_array<unsigned char> result;
			//unsigned char huffman_table[256];
			//最初の4バイトはサイズ情報
			result.resize(*reinterpret_cast<const unsigned __int32*>(binary));
			binary += 4;
			
		}
	};
}

#endif