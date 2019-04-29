#ifndef _GRAPHIC_HPP
#define _GRAPHIC_HPP

#ifdef _IMATH_INTERFACE_IMAGE_


#include "IMathLib/interface/interface.hpp"
#include "IMathLib/image/color.hpp"
#include "IMathLib/image/image.hpp"
#include "IMathLib/utility/smart_ptr.hpp"
#include "IMathLib/utility/tuple.hpp"

//シェーダープログラムをソース内に組み込むためのマクロ
#define sharder_string(str)	#str

//画像処理(ビューポート領域の左上を(0,0)とする)

namespace iml {
	namespace i_f {

		//シェアポインタとして管理するためのテクスチャクラス
		class texture {
			size_t	tex_id = 0;
			size_t	w = 0, h = 0;
		public:
			//テクスチャ拡大縮小時の補間
			static const size_t nearest = GL_NEAREST;			//最近傍補間
			static const size_t linear = GL_LINEAR;			//双線形補間

			constexpr texture() {}
			texture(const SDL_Surface *img, size_t interpolation = nearest) {
				int				internal_format;
				int				pixel_format;

				if (img == nullptr) return;
				w = img->w; h = img->h;

				//OpenGL形式に変換
				if (img->format->BitsPerPixel == 32) {
					internal_format = GL_RGBA;
					pixel_format = (img->format->Bshift > img->format->Rshift) ? GL_RGBA : GL_BGRA;
				}
				else if (img->format->BitsPerPixel == 24) {
					internal_format = GL_RGB;
					pixel_format = (img->format->Bshift > img->format->Rshift) ? GL_RGB : GL_BGR;
				}
				else {
					//ビットレートが24か32でない
					interface_control::inst()->log_wrrite(log_output::error, "The bit rate is not 24 or 32.", log_output::data_error, "bit rate error.");
					tex_id = 0;
					return;
				}

				//OpenGLテクスチャの作成
				glGenTextures(1, &tex_id);
				//テクスチャを選択
				glBindTexture(GL_TEXTURE_2D, tex_id);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, interpolation);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, interpolation);
				//テクスチャにPNGファイルから読み込んだピクセルを書き込む
				gluBuild2DMipmaps(GL_TEXTURE_2D, internal_format, img->w, img->h, pixel_format, GL_UNSIGNED_BYTE, img->pixels);
				//バインドの解除
				glBindTexture(GL_TEXTURE_2D, 0);

			}
			texture(const img::image *img, size_t interpolation = nearest) {
				int				internal_format;
				int				pixel_format;

				if (img == nullptr) return;
				w = img->w; h = img->h;

				//OpenGL形式に変換
				switch (img->color) {
				case 3:
					internal_format = GL_RGB;
					pixel_format = GL_RGB;
					break;
				case 4:
					internal_format = GL_RGBA;
					pixel_format = GL_RGBA;
					break;
				default:
					//ビットレートが24か32でない
					interface_control::inst()->log_wrrite(log_output::error, "The bit rate is not 24 or 32.", log_output::data_error, "bit rate error.");
					tex_id = 0;
					return;
				}

				//OpenGLテクスチャの作成
				glGenTextures(1, &tex_id);
				//テクスチャを選択
				glBindTexture(GL_TEXTURE_2D, tex_id);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, interpolation);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, interpolation);
				//テクスチャにPNGファイルから読み込んだピクセルを書き込む
				gluBuild2DMipmaps(GL_TEXTURE_2D, internal_format, img->w, img->h, pixel_format, GL_UNSIGNED_BYTE, img->pixels);
				//バインドの解除
				glBindTexture(GL_TEXTURE_2D, 0);

			}
			~texture() { glDeleteTextures(1, &tex_id); }

			size_t id() const { return tex_id; }
			size_t width() const { return w; }
			size_t height() const { return h; }
		};

		//シェーダークラス
		class shader {
			size_t shader_id;			//シェーダーの管理id
		public:
			static constexpr size_t vertex = 0;		//バーテクスシェーダー
			static constexpr size_t fragment = 1;		//フラグメントシェーダー
			static constexpr size_t geometry = 2;		//ジオメトリシェーダー

			shader(const char* str, size_t ident) {
				int result = GL_FALSE;					//コンパイル結果

				shader_id = (ident == vertex) ? glCreateShader(GL_VERTEX_SHADER) :
					((ident == fragment) ? glCreateShader(GL_FRAGMENT_SHADER) :
					((ident == geometry) ? glCreateShader(GL_GEOMETRY_SHADER) : 0));

				glShaderSource(shader_id, 1, &str, nullptr);		//シェーダオブジェクトに変換
				glCompileShader(shader_id);							//シェーダのコンパイル
				//エラーチェック
				glGetShaderiv(shader_id, GL_COMPILE_STATUS, &result);
				if (result == GL_FALSE) {
					char* error_mes;
					int log_length;
					glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_length);
					error_mes = new char[log_length + 1];
					glGetShaderInfoLog(shader_id, log_length, NULL, error_mes);
					interface_control::inst()->log_wrrite(log_output::error, error_mes, log_output::data_error, str);
					delete[] error_mes;
				}
			}
			~shader() { glDeleteShader(shader_id); }

			const size_t id() const { return shader_id; }

		};

		//シェアポインタとして管理するためのプログラムオブジェクトクラス
		class program_object {
			size_t po_id;			//プログラムオブジェクトのid

			//変数の位置の取得
			int get_variable(const char* str) {
				return glGetUniformLocation(po_id, str);
			}
		public:
			template <class... Args>
			program_object(const Args&... shaders) {
				po_id = glCreateProgram();
				attach(shaders...);
			}
			~program_object() { glDeleteProgram(po_id); }

			//アタッチ
			void attach() {}
			void attach(const shader& first) { glAttachShader(po_id, first.id()); }
			template <class... Args>
			void attach(const shader& first, const Args&... shaders) { glAttachShader(po_id, first.id()); attach(shaders...); }
			void attach(size_t first) { glAttachShader(po_id, first); }
			template <class... Args>
			void attach(size_t first, const Args&... shaders) { glAttachShader(po_id, first); attach(shaders...); }
			//デタッチ
			void dettach() {}
			void dettach(const shader& first) { glDetachShader(po_id, first.id()); }
			template <class... Args>
			void dettach(const shader& first, const Args&... shaders) { glDetachShader(po_id, first.id()); dettach(shaders...); }
			void dettach(size_t first) { glDetachShader(po_id, first); }
			template <class... Args>
			void dettach(size_t first, const Args&... shaders) { glDetachShader(po_id, first); dettach(shaders...); }
			//シェーダのリンク
			void link() {
				int result = GL_FALSE;
				glLinkProgram(po_id);
				// プログラムをチェックする
				glGetProgramiv(po_id, GL_LINK_STATUS, &result);
				if (result == GL_FALSE) {
					char* error_mes;
					int log_length;
					glGetProgramiv(po_id, GL_INFO_LOG_LENGTH, &log_length);
					error_mes = new char[log_length + 1];
					glGetProgramInfoLog(po_id, log_length, nullptr, error_mes);
					interface_control::inst()->log_wrrite(log_output::error, error_mes, log_output::data_error, "glLinkProgram");
					delete[] error_mes;
				}
			}

			//変数値の設定
			void set_variable(const char* str, int v) { glUniform1i(get_variable(str), v); }
			void set_variable(const char* str, int v1, int v2) { glUniform2i(get_variable(str), v1, v2); }
			void set_variable(const char* str, int v1, int v2, int v3) { glUniform3i(get_variable(str), v1, v2, v3); }
			void set_variable(const char* str, float value) { glUniform1f(get_variable(str), value); }
			void set_variable(const char* str, float v1, float v2) { glUniform2f(get_variable(str), v1, v2); }
			void set_variable(const char* str, float v1, float v2, float v3) { glUniform3f(get_variable(str), v1, v2, v3); }
			void set_variable(const char* str, size_t size, int* v) { glUniform1iv(get_variable(str), size, v); }
			void set_variable(const char* str, size_t size, float* v) { glUniform1fv(get_variable(str), size, v); }
			void set_variable(const char* str, const matrix2<float>& ma) { glUniformMatrix2fv(get_variable(str), 1, GL_TRUE, ma[0]); }
			void set_variable(const char* str, const matrix3<float>& ma) { glUniformMatrix3fv(get_variable(str), 1, GL_TRUE, ma[0]); }
			void set_variable(const char* str, const matrix4<float>& ma) { glUniformMatrix4fv(get_variable(str), 1, GL_TRUE, ma[0]); }

			//プログラムオブジェクトの使用
			void shader_start() { glUseProgram(po_id); }
			//プログラムオブジェクトの使用の解除
			void shader_end() { glUseProgram(0); }

		};

		//フレームバッファの基底
		class frame_buffer_object_base {
		protected:
			size_t	buffer_handle = 0;		//バッファハンドル
			size_t	obj_id = 0;				//フレームバッファオブジェクトid

			size_t	w, h;
		public:
			constexpr frame_buffer_object_base(size_t w, size_t h) :w(w), h(h) {}
			virtual ~frame_buffer_object_base() = 0 {}

			size_t handle() const { return buffer_handle; }
			size_t id() const { return obj_id; }

			size_t width() const { return w; }
			size_t height() const { return h; }
		};
		//カラーフレームバッファ
		class color_frame_buffer_object :public frame_buffer_object_base {
			size_t render_buffer;			//zバッファ
		public:
			color_frame_buffer_object(size_t w, size_t h, bool alpha = true, bool depth = true) :frame_buffer_object_base(w, h) {

				// カラーバッファ用のテクスチャ作成
				glGenTextures(1, &this->buffer_handle);
				glBindTexture(GL_TEXTURE_2D, this->buffer_handle);
				glTexImage2D(GL_TEXTURE_2D, 0, (alpha) ? GL_RGBA : GL_RGB, w, h, 0, (alpha) ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, nullptr);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glBindTexture(GL_TEXTURE_2D, 0);
				//レンダーバッファを用意
				glGenRenderbuffersEXT(1, &this->render_buffer);
				glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, this->render_buffer);
				glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, w, h);
				glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
				//フレームバッファオブジェクトを作成
				glGenFramebuffersEXT(1, &this->obj_id);
				glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->obj_id);

				//フレームバッファオブジェクトにカラーバッファとしてテクスチャを結合
				glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, this->buffer_handle, 0);

				//フレームバッファオブジェクトにデプスバッファとしてレンダーバッファを結合
				glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, this->render_buffer);

				//フレームバッファオブジェクトの結合を解除
				glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
			}
		};
		//レンダーバッファを描画するためのフレームバッファ
		class render_frame_buffer_object :public frame_buffer_object_base {
		public:
			render_frame_buffer_object(size_t w, size_t h) :frame_buffer_object_base(w,h){

				// カラーバッファ用のテクスチャ作成
				glGenTextures(1, &this->buffer_handle);
				glBindTexture(GL_TEXTURE_2D, this->buffer_handle);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, w, h, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				//ボーダーの色を設定しておく
				glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, normal_color<float>(1, 1, 1, 1).v);
				glBindTexture(GL_TEXTURE_2D, 0);
				//フレームバッファオブジェクトを作成
				glGenFramebuffersEXT(1, &this->obj_id);
				glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->obj_id);

				//フレームバッファオブジェクトにデプステクスチャを結合
				glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, this->buffer_handle, 0);

				//カラーバッファを読み書きしない
				glDrawBuffer(GL_NONE);
				glReadBuffer(GL_NONE);

				//フレームバッファオブジェクトの結合を解除
				glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
			}
		};

		//描画先のスクリーン(フレームバッファの指定)(nullptrで裏画面)
		inline void set_draw_framebuffer_object(const frame_buffer_object_base* fbo = nullptr) {
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, (fbo == nullptr) ? 0 : fbo->id());
		}

		//色の設定
		template <class T>
		inline void set_color(const normal_color<T>& color) { glColor4f(color.r, color.g, color.b, color.a); }

		//1つのテクスチャのための頂点配列の構造体
		template <class T>
		struct vertex2 {
			T vtx[8];
			vertex2() :vtx{} {}
			vertex2(const T& left_up_x, const T& left_up_y, const T& right_down_x, const T& right_down_y) {
				vtx[0] = left_up_x; vtx[1] = left_up_y;			//左上
				vtx[2] = left_up_x; vtx[3] = right_down_y;		//左下
				vtx[4] = right_down_x; vtx[5] = right_down_y;	//右下
				vtx[6] = right_down_x; vtx[7] = left_up_y;		//右上
			}
			vertex2(const T& _1, const T& _2, const T& _3, const T& _4, const T& _5, const T& _6, const T& _7, const T& _8) {
				vtx[0] = _1; vtx[1] = _2;		//左上
				vtx[2] = _3; vtx[3] = _4;		//左下
				vtx[4] = _5; vtx[5] = _6;		//右下
				vtx[6] = _7; vtx[7] = _8;		//右上
				/*glTexCoord2f(0, 0);
				glTexCoord2f(0, 1);
				glTexCoord2f(1, 1);
				glTexCoord2f(1, 0);*/
			}
			vertex2(const T* p) {
				for (size_t i = 0; i < 8; ++i)
					vtx[i] = p[i];
			}
			template <class S>
			vertex2(const vertex2<S>& vtx) {
				for (size_t i = 0; i < 8; ++i)
					this->vtx[i] = static_cast<T>(vtx.vtx[i]);
			}
		};

		//各テクスチャのテクスチャ座標配列の登録(何故かシェーダーではこの情報が無効になる)
		template <size_t N = 0>
		inline void set_coord_pointer(size_t tex) {
			static const vertex2<double> tex_coord(0, 0, 1, 1);
			glMultiTexCoordPointerEXT(GL_TEXTURE0 + N, 2, GL_DOUBLE, 0, tex_coord.vtx);
		}
		template <size_t N = 0, class... Args>
		inline void set_coord_pointer(size_t tex, const Args&... args) {
			static const vertex2<double> tex_coord(0, 0, 1, 1);
			glMultiTexCoordPointerEXT(GL_TEXTURE0 + N, 2, GL_DOUBLE, 0, tex_coord.vtx);
			set_coord_pointer<N + 1>(args...);
		}
		template <size_t N = 0>
		inline void set_coord_pointer(const frame_buffer_object_base* tex) {
			//フレームバッファオブジェクトのときは上下反転させる
			static const vertex2<double> tex_coord(0, 1, 1, 0);
			glMultiTexCoordPointerEXT(GL_TEXTURE0 + N, 2, GL_DOUBLE, 0, tex_coord.vtx);
		}
		template <size_t N = 0, class... Args>
		inline void set_coord_pointer(const frame_buffer_object_base* tex, const Args&... args) {
			static const vertex2<double> tex_coord(0, 1, 1, 0);
			glMultiTexCoordPointerEXT(GL_TEXTURE0 + N, 2, GL_DOUBLE, 0, tex_coord.vtx);
			set_coord_pointer<N + 1>(args...);
		}
		template <size_t N = 0>
		inline void set_coord_pointer(const texture* tex) {
			static const vertex2<double> tex_coord(0, 0, 1, 1);
			glMultiTexCoordPointerEXT(GL_TEXTURE0 + N, 2, GL_DOUBLE, 0, tex_coord.vtx);
		}
		template <size_t N = 0, class... Args>
		inline void set_coord_pointer(const texture* tex, const Args&... args) {
			static const vertex2<double> tex_coord(0, 0, 1, 1);
			glMultiTexCoordPointerEXT(GL_TEXTURE0 + N, 2, GL_DOUBLE, 0, tex_coord.vtx);
			set_coord_pointer<N + 1>(args...);
		}
		//各テクスチャをバインドして描画
		template <size_t N = 0>
		inline void draw_bind_texture(size_t n, size_t tex) {
			glActiveTexture(GL_TEXTURE0 + N);
			glBindTexture(GL_TEXTURE_2D, tex);
			glDrawArrays(GL_QUADS, 0, 4 * n);
		}
		template <size_t N = 0, class... Args>
		inline void draw_bind_texture(size_t n, size_t tex, const Args&... args) {
			glActiveTexture(GL_TEXTURE0 + N);
			glBindTexture(GL_TEXTURE_2D, tex);
			glDrawArrays(GL_QUADS, 0, 4 * n);
			draw_bind_texture(args...);
		}
		template <size_t N = 0>
		inline void draw_bind_texture(size_t n, const frame_buffer_object_base* tex) {
			glActiveTexture(GL_TEXTURE0 + N);
			glBindTexture(GL_TEXTURE_2D, tex->handle());
			glDrawArrays(GL_QUADS, 0, 4 * n);
		}
		template <size_t N = 0, class... Args>
		inline void draw_bind_texture(size_t n, const frame_buffer_object_base* tex, const Args&... args) {
			glActiveTexture(GL_TEXTURE0 + N);
			glBindTexture(GL_TEXTURE_2D, tex->handle());
			glDrawArrays(GL_QUADS, 0, 4 * n);
			draw_bind_texture(args...);
		}
		template <size_t N = 0>
		inline void draw_bind_texture(size_t n, const texture* tex) {
			glActiveTexture(GL_TEXTURE0 + N);
			glBindTexture(GL_TEXTURE_2D, tex->id());
			glDrawArrays(GL_QUADS, 0, 4 * n);
		}
		template <size_t N = 0, class... Args>
		inline void draw_bind_texture(size_t n, const texture* tex, const Args&... args) {
			glActiveTexture(GL_TEXTURE0 + N);
			glBindTexture(GL_TEXTURE_2D, tex->id());
			glDrawArrays(GL_QUADS, 0, 4 * n);
			draw_bind_texture(args...);
		}
		//同じ頂点上にテクスチャ(マルチテクスチャ)を描画(複数テクスチャの場合はシェーダーを用いるべき)
		template <class... Args>
		inline void draw_texture2(const vertex2<double>& vtx, const Args&... args) {
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glVertexPointer(2, GL_DOUBLE, 0, vtx.vtx);			//頂点の登録
			set_coord_pointer<0>(args...);						//テクスチャ座標配列の登録

			//テクスチャの有効
			glEnable(GL_TEXTURE_2D);
			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);

			//各テクスチャをバインドして描画
			draw_bind_texture<0>(1, args...);

			//0に戻してテクスチャの終了
			glActiveTexture(GL_TEXTURE0);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			glDisableClientState(GL_VERTEX_ARRAY);
			glDisable(GL_TEXTURE_2D);

			glDisable(GL_BLEND);
		}

		//アルファブレンド有効
		inline void enable_alpha_state() {
			enable_state<GL_BLEND>();
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
		//アルファブレンド無効
		inline void disable_alpha_state() {
			disable_state<GL_BLEND>();
		}

		//テクスチャのバインド(tex_unitは描画に用いるテクスチャ番号)
		inline void bind_texture(size_t tex_id, size_t tex_unit = 0) {
			glActiveTexture(GL_TEXTURE0 + tex_unit);
			glBindTexture(GL_TEXTURE_2D, tex_id);
		}
		inline void bind_texture(const texture* tex, size_t tex_unit = 0) {
			glActiveTexture(GL_TEXTURE0 + tex_unit);
			glBindTexture(GL_TEXTURE_2D, tex->id());
		}
		inline void bind_texture(const frame_buffer_object_base* tex, size_t tex_unit = 0) {
			glActiveTexture(GL_TEXTURE0 + tex_unit);
			glBindTexture(GL_TEXTURE_2D, tex->handle());
		}
		//テクスチャ座標配列のセット
		template <size_t Byte = 0>
		inline void set_texture_coord_array(const float* coord, size_t tex_unit = 0) {
			glMultiTexCoordPointerEXT(GL_TEXTURE0 + tex_unit, 2, GL_FLOAT, Byte, coord);
		}
		template <size_t Byte = 0>
		inline void set_texture_coord_array(const double* coord, size_t tex_unit = 0) {
			glMultiTexCoordPointerEXT(GL_TEXTURE0 + tex_unit, 2, GL_DOUBLE, Byte, coord);
		}
		//頂点座標配列のセット(dimは次元)
		template <size_t Byte = 0>
		inline void set_vertex_array(size_t dim, const float* vtx) {
			glVertexPointer(dim, GL_FLOAT, Byte, vtx);
		}
		template <size_t Byte = 0>
		inline void set_vertex_array(size_t dim, const double* vtx) {
			glVertexPointer(dim, GL_DOUBLE, Byte, vtx);
		}

		//配列のステート有効
		template <size_t FirstVtxIndex>
		inline void enable_array_state() {
			glEnableClientState(FirstVtxIndex);
		}
		template <size_t FirstVtxIndex, size_t SecondVtxIndex, size_t... VtxIndex>
		inline void enable_array_state() {
			glEnableClientState(FirstVtxIndex);
			enable_array_state<SecondVtxIndex, VtxIndex...>();
		}
		template <size_t... VtxIndex>
		inline void _Enable_array_state(index_tuple<size_t, VtxIndex...>) { enable_array_state<VtxIndex...>(); }
		//配列のステート無効
		template <size_t FirstVtxIndex>
		inline void disable_array_state() {
			glDisableClientState(FirstVtxIndex);
		}
		template <size_t FirstVtxIndex, size_t SecondVtxIndex, size_t... VtxIndex>
		inline void disable_array_state() {
			glDisableClientState(FirstVtxIndex);
			disable_array_state<SecondVtxIndex, VtxIndex...>();
		}
		template <size_t... VtxIndex>
		inline void _Disable_array_state(index_tuple<size_t, VtxIndex...>) { disable_array_state<VtxIndex...>(); }

		//テクスチャの描画(VtxIndexTupleは有効にするステート)
		template <size_t Mode, class VtxIndexTuple>
		inline void draw_texture(size_t first, size_t cnt, size_t tex_unit = 0) {
			_Enable_array_state(VtxIndexTuple());

			enable_state<GL_TEXTURE_2D>();
			glActiveTexture(GL_TEXTURE0 + tex_unit);
			glDrawArrays(Mode, first, cnt);
			disable_state<GL_TEXTURE_2D>();

			_Disable_array_state(VtxIndexTuple());
		}
		template <size_t Mode, class VtxIndexTuple, class Texture>
		inline void draw_texture(Texture tex, size_t first, size_t cnt, size_t tex_unit = 0) {
			bind_texture(tex, tex_unit);
			draw_texture<Mode, VtxIndexTuple>(first, cnt, tex_unit);
		}
		template <size_t Mode, class VtxIndexTuple, size_t VtxByte, size_t CoordByte, class Texture, class Float>
		inline void draw_texture(size_t dim, Texture tex, const Float* vtx, const Float* coord, size_t first, size_t cnt, size_t tex_unit = 0) {
			set_vertex_array<VtxByte>(dim, vtx);
			set_texture_coord_array<CoordByte>(coord, tex_unit);

			draw_texture<Mode, VtxIndexTuple>(tex, first, cnt, tex_unit);
		}

		//インデックス指定verのテクスチャ描画
		template <size_t Mode, class VtxIndexTuple>
		inline void draw_texture(const unsigned int* index, size_t cnt, size_t tex_unit = 0) {
			_Enable_array_state(VtxIndexTuple());

			enable_state<GL_TEXTURE_2D>();
			glActiveTexture(GL_TEXTURE0 + tex_unit);
			glDrawElements(Mode, cnt, GL_UNSIGNED_INT, index);
			disable_state<GL_TEXTURE_2D>();

			_Disable_array_state(VtxIndexTuple());
		}
		template <size_t Mode, class VtxIndexTuple>
		inline void draw_texture(const unsigned short* index, size_t cnt, size_t tex_unit = 0) {
			_Enable_array_state(VtxIndexTuple());

			enable_state<GL_TEXTURE_2D>();
			glActiveTexture(GL_TEXTURE0 + tex_unit);
			glDrawElements(Mode, cnt, GL_UNSIGNED_SHORT, index);
			disable_state<GL_TEXTURE_2D>();

			_Disable_array_state(VtxIndexTuple());
		}
		template <size_t Mode, class VtxIndexTuple>
		inline void draw_texture(const unsigned char* index, size_t cnt, size_t tex_unit = 0) {
			_Enable_array_state(VtxIndexTuple());

			enable_state<GL_TEXTURE_2D>();
			glActiveTexture(GL_TEXTURE0 + tex_unit);
			glDrawElements(Mode, cnt, GL_UNSIGNED_BYTE, index);
			disable_state<GL_TEXTURE_2D>();

			_Disable_array_state(VtxIndexTuple());
		}
		template <size_t Mode, class VtxIndexTuple, class Texture, class IndexType>
		inline void draw_texture(Texture tex, const IndexType* index, size_t cnt, size_t tex_unit = 0) {
			bind_texture(tex, tex_unit);
			draw_texture<Mode, VtxIndexTuple>(index, cnt, tex_unit);
		}

		//図形描画
		template <size_t Mode, class VtxIndexTuple>
		inline void draw_figure(size_t first, size_t cnt) {
			_Enable_array_state(VtxIndexTuple());

			glDrawArrays(Mode, first, cnt);

			_Disable_array_state(VtxIndexTuple());
		}
		template <size_t Mode, class VtxIndexTuple, size_t VtxByte, class Float>
		inline void draw_figure(size_t dim, const Float* vtx, size_t first, size_t cnt) {
			set_vertex_array<VtxByte>(dim, vtx);
			draw_figure<Mode, VtxIndexTuple>(first, cnt);
		}
		//インデックス指定verの図形描画
		template <size_t Mode, class VtxIndexTuple>
		inline void draw_figure(const unsigned int* index, size_t cnt) {
			_Enable_array_state(VtxIndexTuple());

			glDrawElements(Mode, cnt, GL_UNSIGNED_INT, index);

			_Disable_array_state(VtxIndexTuple());
		}
		template <size_t Mode, class VtxIndexTuple>
		inline void draw_figure(const unsigned short* index, size_t cnt) {
			_Enable_array_state(VtxIndexTuple());

			glDrawElements(Mode, cnt, GL_UNSIGNED_SHORT, index);

			_Disable_array_state(VtxIndexTuple());
		}
		template <size_t Mode, class VtxIndexTuple>
		inline void draw_figure(const unsigned char* index, size_t cnt) {
			_Enable_array_state(VtxIndexTuple());

			glDrawElements(Mode, cnt, GL_UNSIGNED_BYTE, index);

			_Disable_array_state(VtxIndexTuple());
		}


		//以後基本図形

		//点の描画
		template <class T, size_t N>
		inline void draw_point(const vector<T, N>& p, size_t size = 1) {
			glPointSize(size);
			draw_figure<GL_POINTS, index_tuple<size_t, GL_VERTEX_ARRAY>, 0>(N, &p[0], 0, 1);
		}
		//線の描画
		template <class T, size_t N>
		inline void draw_line(const vector<T, N>& start, const vector<T, N>& end) {
			vector<T, N> temp[2] = { start,end };
			draw_figure<GL_LINES, index_tuple<size_t, GL_VERTEX_ARRAY>, sizeof(vector<T, N>)>(N, &temp[0][0], 0, 2);
		}
		//三角形ポリゴンの描画
		template <class T, size_t N>
		inline void draw_triangle(const vector<T, N>& v1, const vector<T, N>& v2, const vector<T, N>& v3) {
			vector<T, N> temp[3] = { v1,v2,v3 };
			draw_figure<GL_TRIANGLES, index_tuple<size_t, GL_VERTEX_ARRAY>, sizeof(vector<T, N>)>(N, &temp[0][0], 0, 3);
		}
	}
}


#endif
#endif