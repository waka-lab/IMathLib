#ifndef _LZSS_HPP
#define _LZSS_HPP

#include "IMathLib/utility/utility.hpp"
#include "IMathLib/math/extra_math.hpp"
#include "IMathLib/container/array.hpp"


namespace iml {
	
	//lzss
	class lzss {
	public:
		// 圧縮データの情報構造体
		struct LZSS_ENCODE_INFO {
			size_t		EncodeCode;			//圧縮情報の開始を示す数値
		};

	private:
		LZSS_ENCODE_INFO			_encode_info;	//エンコード情報
		std::vector<unsigned char>	_original;		//オリジナルデータ
		std::vector<unsigned char>	_press;			//圧縮データ

		size_t						_p_original;	//オリジナルデータの位置(データ取り出し時に使用)
		size_t						_p_press;		//圧縮データの位置(データ取り出し時に使用)

	public:

		lzss() :_p_original(0), _p_press(0) {}

		//圧縮データの情報構造体を代入する(解凍のとき必要)
		void encode_info_set(const LZSS_ENCODE_INFO& _info) { _encode_info = _info; }

		//オリジナルデータを追加する
		void original_add(void* data, size_t size) {
			unsigned char* p = (unsigned char*)data;
			for (size_t i = 0; i < size; ++i, ++p)
				_original.push_back(*p);
		}
		//圧縮データを追加する
		void press_add(void* data, size_t size) {
			unsigned char* p = (unsigned char*)data;
			for (size_t i = 0; i < size; ++i, ++p) {
				_press.push_back(*p);
			}
		}

		//オリジナルデータの現在の位置の文字
		unsigned char original_type() {
			if ((_original.size() - 1) < _p_original) return -1;
			return _original[_p_original];
		}
		//圧縮データの現在の位置の文字
		unsigned char press_type() {
			if ((_press.size() - 1) < _p_press) return -1;
			return _press[_p_original];
		}

		//オリジナルデータの現在位置の変更
		bool original_pos(size_t p) { if ((_original.size() - 1) < p) return false; _p_original = p; return true; }
		//圧縮データの現在位置の変更
		bool press_pos(size_t p) { if ((_press.size() - 1) < p) return false; _p_press = p; return true; }

		//オリジナルデータの取り出し
		void original_get(void* data, size_t size);
		//圧縮データの取り出し
		void press_get(void* data, size_t size);

		//オリジナルデータのコンバート
		size_t original_convert();
		//圧縮データのコンバート
		size_t press_convert();

		//オリジナルデータのサイズ
		size_t original_size() { return _original.size(); }
		//圧縮データのサイズ
		size_t press_size() { return _press.size(); }
		
		/*//オリジナルデータの全出力
		bool original_output(tstring fname) {
			FILE* fp;
			errno_t error;
			error = _tfopen_s(&fp, fname.c_str(), _T("wb"));
			if (error != 0) return false;
			fwrite(&_original[0], _original.size(), 1, fp);

			return true;
		}
		//圧縮データの全出力
		bool press_output(tstring fname) {
			FILE* fp;
			errno_t error;
			error = _tfopen_s(&fp, fname.c_str(), _T("wb"));
			if (error != 0) return false;
			fwrite(&_encode_info, sizeof(LZSS_ENCODE_INFO), 1, fp);
			fwrite(&_press[0], _press.size(), 1, fp);

			return true;
		}*/


		//エンコード
		static shared_array<unsigned char> encode(const unsigned char* binary, size_t byte) {
			__int32 encode_code;
		}

		//デコード
		static shared_array<unsigned char> decode(const unsigned char* binary, size_t byte) {
			
		}
	};

	//データのコンバート
	inline size_t lzss::original_convert() {

		size_t SrcSizeCounter = 0;
		int EncodeCode;

		//元あった圧縮データのクリア
		//press_free();

		//圧縮元データの中で一番出現頻度が低いコードを算出する
		{
			size_t counter_table[256] = {};		//1バイトで表現できる数値は0~255
			size_t *MinNum = nullptr;

			//出現回数をカウント
			for (size_t i = 0; i < _original.size(); ++i) counter_table[_original[i]] ++;
			//最も出現頻度の低いコードの取得
			MinNum = min_element(counter_table, counter_table + 256);
			EncodeCode = MinNum - counter_table;
		}

		// 圧縮処理
		{
			int EqualNum, MaxEqualNum, MaxIndex;

			// 全てのデータを圧縮するまで繰り返す
			while (SrcSizeCounter < _original.size()) {
				// 今までに同じ数値の羅列がなかったか調べる
				{
					MaxEqualNum = -1;
					MaxIndex = -1;

					// 最高で256バイト前まで調べる
					for (int i = 0; i < 256; ++i) {
						// データの先頭より前を調べようとしていたら抜ける
						if ((int)(SrcSizeCounter - i) < 0) break;

						// 同じ数値が何回続いているか調べる
						for (EqualNum = 0; EqualNum < i; EqualNum++) {
							// 圧縮元データの最後に到達したらループを抜ける
							if (EqualNum + SrcSizeCounter >= _original.size()) break;

							// 数値が違ったらループを抜ける
							if (_original[SrcSizeCounter + EqualNum] != _original[SrcSizeCounter - i + EqualNum]) break;
						}

						// 同じだった回数が４以上(４未満だと逆にサイズが増える)で、且
						// 今まで見つけた回数よりも多い場合に参照アドレスを更新する
						if (EqualNum >= 4 && MaxEqualNum < EqualNum) {
							//最大一致数
							MaxEqualNum = EqualNum;
							//最大一致のインデックス
							MaxIndex = i;
						}
					}
				}

				// 同じ数値の羅列が見つからなかったら普通に出力
				if (MaxIndex == -1) {
					_press.push_back(_original[SrcSizeCounter]);

					// 圧縮情報の開始を示す数値と同じだった場合は２回連続で出力する
					if (_original[SrcSizeCounter] == EncodeCode)
						_press.push_back(_original[SrcSizeCounter]);

					// 圧縮元データの参照アドレスを一つ進める                
					++SrcSizeCounter;
				}
				else {
					// 見つかった場合は見つけた位置と長さを出力する

					// 最初に圧縮情報の開始を示す数値を出力する
					_press.push_back((unsigned char)EncodeCode);

					// 次に『何バイト前からが同じか？』の数値を出力する
					{
						_press.push_back((unsigned char)MaxIndex);

						// もし圧縮情報の開始を示す数値と同じ場合、展開時に
						// 『圧縮情報の開始を示す数値そのもの』と判断されて
						// しまうため、圧縮情報の開始を示す数値と同じかそれ以上の
						// 場合は数値を +1 するというルールを使う。(展開時は逆に -1 にする)
						if (MaxIndex >= EncodeCode) _press[_press.size() - 1]++;
					}

					// 次に『何バイト同じか？』の数値を出力する
					_press.push_back((unsigned char)MaxEqualNum);

					// 同じだったバイト数分だけ圧縮元データの参照アドレスを進める
					SrcSizeCounter += MaxEqualNum;
				}
			}
		}

		// 圧縮データの情報をセットする
		{
			// 圧縮情報の開始を知らせる数値をセット
			_encode_info.EncodeCode = EncodeCode;
		}

		// 圧縮後のデータのサイズを返す
		return _press.size();

	}

	inline size_t lzss::press_convert() {

		size_t PressSizeCounter = 0;

		//元あったオリジナルデータのクリア
		//original_free();

		// 解凍処理
		{
			size_t Index, EqualNum;

			// 全ての圧縮データを解凍するまでループ
			while (PressSizeCounter < _press.size()) {
				// 圧縮情報の開始を示す数値かどうかで処理を分岐
				if (_press[PressSizeCounter] == _encode_info.EncodeCode) {
					++PressSizeCounter;

					// ２バイト連続で圧縮情報の開始を示す数値だった場合、開始を示す
					// 数値そのものを示しているのでそのまま出力する
					if (_press[PressSizeCounter] == _encode_info.EncodeCode) {
						_original.push_back((unsigned char)_encode_info.EncodeCode);

						++PressSizeCounter;
					}
					else {
						// 普通に圧縮情報を示す数値だった場合

						// 『何バイト前から？』の数値を得る
						{
							Index = _press[PressSizeCounter];
							PressSizeCounter++;

							// 『何バイト前から？』の数値が圧縮情報を示す数値より
							// 大きい値の場合は－１する(詳しくはエンコードプログラムを参照してください)
							if (Index > _encode_info.EncodeCode) Index--;
						}

						// 『何バイト同じか？』の数値を得る
						EqualNum = _press[PressSizeCounter];
						++PressSizeCounter;

						// 指定のバイト数分だけ前のアドレスから、指定の
						// バイト数分だけコピー
						{
							std::vector<unsigned char> buf;
							for (size_t i = 0; i < EqualNum; ++i)
								buf.push_back(_original[_original.size() - 1 + 1 - Index + i]);
							for (size_t i = 0; i < EqualNum; ++i)
								_original.push_back(buf[i]);

							buf.clear(); buf.shrink_to_fit();
						}
					}
				}
				else {
					// 普通に数値を出力
					_original.push_back(_press[PressSizeCounter]);

					++PressSizeCounter;
				}
			}
		}

		// 解凍後のサイズを返す
		return _original.size();
	}

}


#endif