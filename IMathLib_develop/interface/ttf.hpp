#ifndef _TTF_HPP
#define _TTF_HPP

#ifdef _IMATH_INTERFACE_FONT_


#include "IMathLib/image/color.hpp"
#include "IMathLib/string/string.hpp"
#include "IMathLib/utility/smart_ptr.hpp"
#include "IMathLib/interface/graphic.hpp"

namespace iml {
	namespace i_f {

		//TrueTypeフォントクラス
		class ttf {
			TTF_Font*		font_ptr;
			imsize_t		point_size;			//フォントのポイントサイズ
			imsize_t		line_width;			//改行幅
			imsize_t		font_height;		//フォントの高さ
		public:
			ttf(const char* str, imsize_t size) :font_ptr(TTF_OpenFont(str, size)), point_size(size) {
				//インスタンスの存在確認をしてフォントハンドルが存在するかを確認する
				if (!font_ptr) {
					interface_control::inst()->log_wrrite(log_output::error, TTF_GetError(), log_output::file_read_error, str);
					return;
				}
				this->line_width = 0;
				this->font_height = TTF_FontHeight(font_ptr);
			}
			~ttf() {}

			//オーディオの参照
			TTF_Font* operator()() { return font_ptr; }
			TTF_Font* operator()() const { return font_ptr; }

		};

		//一つあたりの文字列テクスチャ
		class unit_string_texture :public texture {
			bool indention;				//次のテクスチャを改行するかのフラグ

			//normal_colorをSDL_Colorに変換する
			static SDL_Color color_trans(const color_32bit& color) {
				SDL_Color c;
				c.r = color.c.r;
				c.g = color.c.g;
				c.b = color.c.b;
				c.a = color.c.a;
				return c;
			}
			//UTF-16
			static SDL_Surface* create_string_texture(const ttf& font, wchar_t* str, const color_32bit &color) {
				SDL_Surface	*img;
				img = TTF_RenderUNICODE_Blended(font(), reinterpret_cast<Uint16*>(str), color_trans(color));
				if (!img) {
					interface_control::inst()->log_wrrite(log_output::error, TTF_GetError(), log_output::function_error, str);
					return nullptr;
				}
				return img;
			}
			//UTF-8
			static SDL_Surface* create_string_texture(const ttf& font, char* str, const color_32bit &color) {
				SDL_Surface	*img;
				img = TTF_RenderUTF8_Blended(font(), str, color_trans(color));
				if (!img) {
					interface_control::inst()->log_wrrite(log_output::error, TTF_GetError(), log_output::function_error, str);
					return nullptr;
				}
				return img;
			}
		public:
			unit_string_texture() :indention(true) {}
			unit_string_texture(const SDL_Surface *img, bool i) :indention(i), texture(img) {}
			template <class CharT>
			unit_string_texture(const ttf& font, const CharT* str, const color_32bit &color, bool i): indention(i)
				, texture(unique_ptr<SDL_Surface, deallocator<SDL_Surface>::function>(create_string_texture(font, const_cast<CharT*>(str), color), SDL_FreeSurface).get()) {}

			const bool flag() const { return indention; }
			void set_flag(bool i) { indention = i; }
		};
		//文字列用のテクスチャクラス
		class string_texture {
			list<unit_string_texture*> str_array;

			//pの位置から改行記号を探索する
			static char* find_indention(const char* p) {
				if (p == nullptr) return nullptr;
				for (char* i = const_cast<char*>(p); *i != '\0'; ++i)
					if (*i == '\n') return i;
				return nullptr;
			}
			static wchar_t* find_indention(const wchar_t* p) {
				if (p == nullptr) return nullptr;
				for (wchar_t* i = const_cast<wchar_t*>(p); *i != L'\0'; ++i)
					if (*i == L'\n') return i;
				return nullptr;
			}
			//文字列の横幅を取得
			//UTF-16
			imsize_t get_text_width(const ttf& font, wchar_t* str) {
				int temp;
				TTF_SizeUNICODE(font(), reinterpret_cast<Uint16*>(str), &temp, nullptr);
				return temp;
			}
			//UTF-8
			imsize_t get_text_width(const ttf& font, char* str) {
				int temp;
				TTF_SizeUTF8(font(), str, &temp, nullptr);
				return temp;
			}
		public:
			//横幅の制限の指定可能な文字列テクスチャの作成
			template <class CharT>
			string_texture(const ttf& font, const CharT* str, const color_32bit &color, imsize_t width = -1) {
				push_back(font, str, color, width);
			}
			~string_texture() {
				for (auto itr = str_array.begin(); itr != str_array.end(); ++itr)
					delete *itr;
			}

			//文字列データの追加(null文字は0共通のため文字の種類に依存しない)
			template <class CharT>
			void push_back(const ttf& font, const CharT* str, const color_32bit &color, imsize_t width = -1) {
				CharT *p = const_cast<CharT*>(str), *buf_p = 0;
				CharT c[2];					//文字列置換用のバッファ

				if (str == nullptr) return;
				if (str[0] == 0) return;

				while (1) {
					//わざとnull文字を置くことで一行分のデータの構築
					buf_p = find_indention(p);
					if (buf_p != nullptr) {
						c[0] = *buf_p;
						*buf_p = '\0';
					}

					//一番最後の行
					if (buf_p == nullptr) {
						str_array.push_back(new unit_string_texture(font, p, color, false));
						return;
					}
					//widthが最大値ならばそのままデータの追加
					if (width == static_cast<imsize_t>(-1)) str_array.push_back(new unit_string_texture(font, p, color, true));
					else {
						imsize_t i, j = 0;
						for (i = 0; p[i] != '\0'; ++i) {
							//わざと終端文字を置くことでメモリ効率と高速化を図る
							c[1] = p[i];
							p[i] = '\0';

							if (get_text_width(font, p + j) > width) {
								//折り返しの追加
								str_array.push_back(new unit_string_texture(font, p + j, color, true));
								j = i - 1;
							}
							p[i] = c[1];					//元に戻す
						}
						//折り返しの一番最後の追加
						str_array.push_back(new unit_string_texture(font, p + j, color, true));
					}

					*buf_p = c[0];				//元に戻す
					p = buf_p + 1;				//次の位置から探索する
				}

			}
			//一行分の文字列データの削除
			void pop_back() { str_array.pop_back(); }

			unit_string_texture* operator[](imsize_t n) {
				return str_array[n];
			}
			unit_string_texture* operator[](imsize_t n) const {
				return str_array[n];
			}
			imsize_t size() const { return str_array.size(); }
		};

		//文字列テクスチャの描画
		inline void draw_string_texture2(double x, double y, const string_texture& tex) {
			double height = y, width = x;				//現在の左上の座標
			for (imsize_t i = 0; i < tex.size(); ++i) {
				draw_texture2(vertex2<double>(width, height, width + tex[i]->width(), height + tex[i]->height()), tex[i]->id());
				//改行フラグに応じて次に座標の移動
				if (tex[i]->flag()) { height += tex[i]->height(); width = x; }
				else width += tex[i]->width();
			}
		}
		template <class CharT>
		inline void draw_string_texture2(double x, double y, const ttf& font, const CharT* str, const color_32bit &color, imsize_t width = -1) {
			string_texture tex(font, str, color, width);
			draw_string_texture2(x, y, tex);
		}

	}

}


#endif
#endif