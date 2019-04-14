#ifndef _DEFLATE_HPP
#define _DEFLATE_HPP

//-------------------------------------------------------------
// include
//-------------------------------------------------------------
#include <vector>
#include <assert.h>
#include "IMathLib/container/array.hpp"
#include "IMathLib/encryption/huffman.hpp"

namespace iml {

	//スライドウィンドウ
	class _Slide_window {
		unsigned					m_top = 0;
		const size_t				m_maxbufferSize;
		std::vector<unsigned char>	m_buffer;
	public:
		_Slide_window(size_t buffersize) :m_maxbufferSize(buffersize) {
			m_buffer.reserve(buffersize);
			assert(m_buffer.size() == 0);
		}

		void push_back(unsigned char value) {
			// 1周目
			if (m_buffer.size() < m_maxbufferSize)
				m_buffer.push_back(value);
			// 2周目以降
			else {
				m_buffer.at(m_top) = value;
				m_top = (m_top + 1) % m_maxbufferSize;
			}
		}
		unsigned char& at(size_t index) {
			//本当はバッファサイズを２の乗数に固定することで
			//「%」を「&」の計算に置き換えられる
			return m_buffer.at((index + m_top) % m_maxbufferSize);
		}
		const unsigned char& at(size_t index) const {
			return m_buffer.at((index + m_top) % m_maxbufferSize);
		}
		size_t size() const { return m_buffer.size(); }
	};

	//長さ/距離に該当するデータパターンを返す
	//長さ > 距離に対応できるようにする
	//新しくpush_back()された値ほど、距離は短い
	inline shared_array<unsigned char> get_pattern(const _Slide_window& slide, size_t length, size_t distance) {
		shared_array<unsigned char> vec;
		vec.resize(length);
		//スライド窓の後ろから距離分の長さを循環させて代入
		for (size_t i = 0; i < length; ++i) vec[i] = slide.at(slide.size() - (distance - (i % distance)));
		return vec;
	}

	//データパターンを記録する
	inline void push_back_pattern(_Slide_window* slide, const shared_array<unsigned char> substr) {
		for (auto itr = substr.begin(); itr != substr.end(); ++itr) slide->push_back(*itr);
	}


	//deflate
	class deflate {

		//拡張ビット分の読み出し
		static size_t read_extra_value(_Bit_stream& bitstream, size_t base, size_t extra_bit) {
			if (extra_bit == 0) return base;
			return base + bitstream.get_range(extra_bit);
		}

		//スライド窓から拝借するパターンの長さ情報を読み出す
		static size_t read_length_code(size_t code, _Bit_stream& bitstream){
			constexpr size_t code_begin = 257;
			constexpr size_t code_end = 286;
			assert(code >= code_begin && code < code_end);

			//テーブルの宣言
			static pair<size_t, size_t> code_table[] = {
				//最小の長さ/拡張ビット数
				{ 3,0 },
				{ 4,0 },
				{ 5,0 },
				{ 6,0 },
				{ 7,0 },
				{ 8,0 },
				{ 9,0 },
				{ 10,0 },
				{ 11,1 },
				{ 13,1 },
				{ 15,1 },
				{ 17,1 },
				{ 19,2 },
				{ 23,2 },
				{ 27,2 },
				{ 31,2 },
				{ 35,3 },
				{ 43,3 },
				{ 51,3 },
				{ 59,3 },
				{ 67,4 },
				{ 83,4 },
				{ 99,4 },
				{ 115,4 },
				{ 131,5 },
				{ 163,5 },
				{ 195,5 },
				{ 227,5 },
				{ 258,0 },
			};
			pair<size_t, size_t>& info = code_table[code - code_begin];
			return read_extra_value(bitstream, info.first, info.second);
		}

		//スライド窓の参照開始地点(距離)の情報を読み出す
		static size_t read_distance_code(size_t code, _Bit_stream& bitstream){
			constexpr size_t code_end = 30;
			assert(code < code_end);

			// テーブルの宣言
			static pair<size_t, size_t> code_table[] ={
				//最短の距離/拡張ビット数
				{ 1,0 },
				{ 2,0 },
				{ 3,0 },
				{ 4,0 },
				{ 5,1 },
				{ 7,1 },
				{ 9,2 },
				{ 13,2 },
				{ 17,3 },
				{ 25,3 },
				{ 33,4 },
				{ 49,4 },
				{ 65,5 },
				{ 97,5 },
				{ 129,6 },
				{ 193,6 },
				{ 257,7 },
				{ 385,7 },
				{ 513,8 },
				{ 769,8 },
				{ 1025,9 },
				{ 1537,9 },
				{ 2049,10 },
				{ 3073,10 },
				{ 4097,11 },
				{ 6145,11 },
				{ 8193,12 },
				{ 12289,12 },
				{ 16385,13 },
				{ 24577,13 },
			};
			auto info = code_table[code];
			return read_extra_value(bitstream, info.first, info.second);
		}

		//固定リテラルハフマンツリーを作成
		static _Prefix_code_tree make_fixed_huffman_tree(){
			_Prefix_code_tree fixedTree;

			//0 - 143 -> 8bit
			//[0011 0000] ～ [10111111]
			for (size_t i = 0; i <= 143; ++i) {
				int key = 0x30 + i;
				fixedTree.Entry<8>(key, i);
			}
			//144 - 255 -> 9bit
			//[110010000] ～ [111111111]
			for (size_t i = 0; i <= (255 - 144); ++i) {
				int key = 0x190 + i;
				fixedTree.Entry<9>(key, i + 144);
			}
			//256 - 279 -> 7bit
			//[0000000] ～ [0010111]
			for (size_t i = 0; i <= (279 - 256); ++i) {
				int key = 0x00 + i;
				fixedTree.Entry<7>(key, i + 256);
			}
			//280 - 287 -> 8bit
			//[11000000] ～ [11000111]
			for (size_t i = 0; i <= (287 - 280); ++i) {
				int key = 0xC0 + i;
				fixedTree.Entry<8>(key, i + 280);
			}
			return fixedTree;
		}

		//固定ハフマン符号によるパース処理
		static void _Decode_fixed_huffman(_Bit_stream& bitstream, _Slide_window& slide, shared_array<unsigned char>& resultbuffer){
			//固定ハフマンツリー作成
			static const auto tree = make_fixed_huffman_tree();

			//ハフマン符号(0 ～ 286)
			unsigned val;
			while (iml::_Bit_tree_decode(bitstream, tree, &val)) {
				//終端
				if (val == 256) break;
				//値そのまま
				if (val <= 255) {
					resultbuffer.push_back(static_cast<unsigned char>(val));
					slide.push_back(static_cast<unsigned char>(val));
					continue;
				}

				//if (val > 256)

				//長さ情報
				size_t length = read_length_code(val, bitstream);
				//距離情報(5bit固定)
				size_t extra_value = bitstream.get_coded_range(5);
				size_t distance = read_distance_code(extra_value, bitstream);

				//一致した値パターンを抽出
				shared_array<unsigned char> pattern = get_pattern(slide, length, distance);

				resultbuffer.insert(resultbuffer.end(), pattern.begin(), pattern.end());
				push_back_pattern(&slide, pattern);
			}
		}

		// @brief 正規化されたハフマンツリーを作成
		//-------------------------------------------------------------
		template<unsigned NUM_CODE>
		static _Prefix_code_tree MakeNormalizedHuffmanTree(std::array<size_t, NUM_CODE>& codeLanArray){
			static_assert(NUM_CODE > 0, "不正な配列サイズ");

			// 事前に最大の長さを調べておく
			constexpr auto CAPACITY_LENGTH = numeric_traits<size_t>::digits;
			const size_t   maxCodeLength = *std::max_element(codeLanArray.begin(), codeLanArray.end());
			assert(maxCodeLength <= CAPACITY_LENGTH);

			// 符号長別に出現する符号個数をカウント
			std::array<size_t, CAPACITY_LENGTH> codeLenCount{};
			for (size_t i = 0; i < NUM_CODE; ++i){
				auto length = codeLanArray[i];
				if (length > 0) codeLenCount[length - 1] += 1;
			}

			// 符号長別の最小の符号(最初に割り当てられる符号)を準備
			std::array<size_t, CAPACITY_LENGTH> allocateCodes{ 0 };
			for (size_t i = 1; i < maxCodeLength; ++i){
				// note:
				// 符号長が一つ小さいグループのなかで
				// 未割当てになる符号を１bit++したものを
				// この符号長グループの符号割り当てに使用する
				// ---> 瞬時複合可能な条件を満たす
				size_t prevEndCode = allocateCodes[i - 1] + codeLenCount[i - 1];
				allocateCodes[i] = prevEndCode << 1;
			}

			// 正規化したコードで置き換えた結果でツリーを作成
			_Prefix_code_tree tree;
			for (size_t i = 0; i < NUM_CODE; ++i){
				auto length = codeLanArray[i];
				if (length > 0){
					auto newCode = allocateCodes[length - 1];
					tree.Entry(newCode, i, length);

					allocateCodes[length - 1] += 1;
				}
			}
			return tree;
		}

		//@brief "符号の長さ"を表す符号を連ねるハフマンツリーを読み込む
		//-------------------------------------------------------------
		static _Prefix_code_tree ReadCodeLenCodeTree(_Bit_stream& bitstream, int numCodeLenCode){
			// note:
			// コードの長さを示す符号は 変則的な並びで記録されている
			// 普段利用されない符号長ほど、末尾に配置される並びにすることで、
			// 実際利用されなかった符号長の記録を省略し
			// 全体のデータ数を減らす最適化のため?
			static const size_t indexSequence[] ={
				16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15
			};

			// 読み出されなかった要素は「0」になる
			std::array<size_t, 19> codeLenCodeLens{};
			for (size_t i = 0; i < numCodeLenCode; ++i){
				auto index = indexSequence[i];
				codeLenCodeLens[index] = bitstream.get_range(3);
			}
			// この符号長配列からハフマンツリーを作る
			return MakeNormalizedHuffmanTree(codeLenCodeLens);
		}

		//@brief 繰り返し長さを読み出す
		//-------------------------------------------------------------
		static unsigned ReadRunLength(size_t code, _Bit_stream& bitstream){
			const size_t code_begin = 16;
			const size_t code_end = 19;
			assert(code >= code_begin && code < code_end);

			// テーブルの宣言
			static const std::pair<size_t, size_t> TABLE[] = {
				// 最小の繰返し回数 / 拡張ビット数
				std::make_pair(3, 2),
				std::make_pair(3, 3),
				std::make_pair(11, 7),
			};

			const auto info = TABLE[code - code_begin];
			return read_extra_value(bitstream, info.first, info.second);
		}

		//@brief "符号の長さ"ハフマンツリーを使って 符号ツリーを読み出す
		//-------------------------------------------------------------
		template<unsigned CAPACITY_LENGTH>
		static _Prefix_code_tree ReadCustomHuffmanTree(_Bit_stream& bitstream, size_t numRead, _Prefix_code_tree codeLenCodeTree){
			assert(numRead <= CAPACITY_LENGTH);
			std::array<size_t, CAPACITY_LENGTH> codeLenArray{};

			for (size_t index = 0; index < numRead; ++index) {
				// ビット読み出し -> "符号の長さ"ハフマンツリーでパース
				size_t val;
				if (iml::_Bit_tree_decode(bitstream, codeLenCodeTree, &val) == false) throw std::runtime_error("読み出し中にエラーが発生しました");
				// 15 以下はそのまま記録
				if (val <= 15) {
					codeLenArray[index] = val;
					continue;
				}
				// 16は直前の値を、
				// 17, 18は「0」を一定回数繰り返す(ランレングス)
				auto runLength = ReadRunLength(val, bitstream);
				auto repeatVal = (val == 16) ? codeLenArray[index - 1] : 0;

				for (size_t j = 0; j < runLength; ++j) codeLenArray[index + j] = repeatVal;
				index += (runLength - 1);
			}
			// この符号長配列からハフマンツリーを作る
			return MakeNormalizedHuffmanTree(codeLenArray);
		}

		//@brief カスタムリテラルハフマンツリーを作成
		//-------------------------------------------------------------
		static _Prefix_code_tree ReadLiteralTree(_Bit_stream& bitstream, int numRead, _Prefix_code_tree codeLenCodeTree){
			return 	ReadCustomHuffmanTree<286>(bitstream, numRead, codeLenCodeTree);
		}

		//@brief カスタム距離ハフマンツリーを作成
		//-------------------------------------------------------------
		static _Prefix_code_tree ReadDistanceTree(_Bit_stream& bitstream, int numRead, _Prefix_code_tree codeLenCodeTree) {
			return 	ReadCustomHuffmanTree<32>(bitstream, numRead, codeLenCodeTree);
		}

		//@brief カスタムハフマン符号によるパース処理
		//-------------------------------------------------------------
		static void DecodeWithCustomHuffman(_Bit_stream& bitstream, _Slide_window& slide, shared_array<unsigned char>& resultbuffer){
			// HLIT:　記録されたリテラル符号個数(257 ～ 286)
			int numLiteralCode = bitstream.get_range(5) + 257;

			// HDIST: 記録された距離符号個数(1 ～ 32)
			int numDistanceCode = bitstream.get_range(5) + 1;

			// HCLEN: 「符号後の長さ」を表す符号個数(4 ～ 19)
			int numCodeLenCode = bitstream.get_range(4) + 4;

			// 順番に各々のハフマンツリーを作成
			_Prefix_code_tree codeLenCodeTree = ReadCodeLenCodeTree(bitstream, numCodeLenCode);
			_Prefix_code_tree literalTree = ReadLiteralTree(bitstream, numLiteralCode, codeLenCodeTree);
			_Prefix_code_tree distanceTree = ReadDistanceTree(bitstream, numDistanceCode, codeLenCodeTree);

			// あとは固定ハフマンの時とほぼ同じ
			size_t val;
			while (iml::_Bit_tree_decode(bitstream, literalTree, &val)) {
				if (val == 256) break;
				if (val <= 255) {
					resultbuffer.push_back(static_cast<unsigned char>(val));
					slide.push_back(static_cast<unsigned char>(val));
					continue;
				}
				// 長さ情報
				unsigned length = read_length_code(val, bitstream);

				// 距離情報 (距離ハフマンツリーを使って読む)
				unsigned extra_value;
				iml::_Bit_tree_decode(bitstream, distanceTree, &extra_value);
				unsigned distance = read_distance_code(extra_value, bitstream);

				// 一致した値パターンを抽出
				auto pattern = get_pattern(slide, length, distance);
				resultbuffer.insert(resultbuffer.end(), pattern.begin(), pattern.end());
				push_back_pattern(&slide, pattern);
			}
		}

		//@brief 無圧縮データ読み出し
		//-------------------------------------------------------------
		static void _Decode_non_compressed(_Bit_stream& bitstream, _Slide_window& slide, shared_array<unsigned char>& resultbuffer){
			//次のbyte境界位置に移動
			bitstream.skip_byte();
			size_t length = bitstream.get_range(16);
			size_t nlength = bitstream.get_range(16);

			if (length + nlength != 0xffff) throw std::runtime_error("非圧縮データサイズが不正です。");
			for (size_t i = 0; i < length; ++i) {
				unsigned char val = static_cast<unsigned char>(bitstream.get_range(8));
				resultbuffer.push_back(val);
				slide.push_back(val);
			}
		}
	public:
		//デコード
		static shared_array<unsigned char> decode(const unsigned char* binary, size_t numByte, size_t slideWindowSize) {
			_Bit_stream	bitstream(binary, numByte);
			_Slide_window	slide(slideWindowSize);

			shared_array<unsigned char> result;

			while (!bitstream.eof()) {
				bool isLast = (bitstream.get() == 1);
				switch (bitstream.get_range(2)){
				case 0:
					_Decode_non_compressed(bitstream, slide, result);
					break;
				case 1:
					_Decode_fixed_huffman(bitstream, slide, result);
					break;
				case 2:
					DecodeWithCustomHuffman(bitstream, slide, result);
					break;
				case 3:
					throw std::runtime_error("よくわかんないデータが来た");
				}
				if (isLast) break;
			}
			return result;
		}
	};
}


#endif