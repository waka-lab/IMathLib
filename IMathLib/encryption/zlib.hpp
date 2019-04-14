#ifndef _ZLIB_HPP
#define _ZLIB_HPP

#include "IMathLib/encryption/deflate.hpp"
#include "IMathLib/container/array.hpp"

namespace iml {

	//zlib
	class zlib {
		//reinterpret_castではメモリ配置が逆になるため独自実装のuintへのキャスト
		static unsigned __int32 unsigned_int_cast(const unsigned char* p) {
			return (p[0] << 24) + (p[1] << 16) + (p[2] << 8) + (p[3]);
		}
		//reinterpret_castではメモリ配置が逆になるため独自実装のushortへのキャスト
		static unsigned __int16 unsigned_short_cast(const unsigned char* p) {
			return (p[0] << 8) + (p[1]);
		}

		//Adler-32のチェックサムを返す(65521は2^{16}未満の最大素数)
		static unsigned int adler_32(const shared_array<unsigned char>& data){
			unsigned int a = 1, b = 0;

			for (auto itr = data.begin(); itr != data.end(); ++itr) {
				a = (a + *itr) % 65521;
				b = (b + a) % 65521;
			}
			return (b << 16) + a;
		}
	public:
		//エンコード

		//デコード
		static shared_array<unsigned char> decode(const unsigned char* binary, size_t byte) {

			//圧縮方式がdeflateであることのチェック
			if ((binary[0] & 0x0F) != 0x08) throw std::runtime_error("deflate圧縮以外のタイプが指定されました。非対応につきDecodeできません。");
			//スライド窓サイズ(256(2^8) ～ 32768(2^15))
			const unsigned int slide_window = (1 << (((binary[0] & 0xF0) >> 4) + 8));
			if (slide_window > 32768) throw std::runtime_error("要求されたスライド窓のサイズが大きすぎます。");

			//zlibのヘッダチェック
			//結果が31の倍数にならなければならない
			if (unsigned_short_cast(&binary[0]) % 31 != 0) throw std::runtime_error("zlibのヘッダの整合性チェックに失敗しました。");
			//プリセット辞書には非対応
			if (binary[1] & 0x20) throw std::runtime_error("プリセット辞書非対応です。decodeできません。");

			//解凍とデータに壊れがないかのチェック(ヘッダーとフッターの和の6byte分引く)
			auto result = deflate::decode(&binary[2], byte - 6, slide_window);

			//フッターでのAdler-32のチェックサム
			if (unsigned_int_cast(&binary[byte - 4]) != adler_32(result)) throw std::runtime_error("圧縮データが壊れています。decodeに失敗しました。");
			
			return result;
		}
	};
}

#endif