#ifndef _INTERFACE_HPP
#define _INTERFACE_HPP

//インターフェース構築に必要なもの

#ifdef _IMATH_INTERFACE_
#define _IMATH_INTERFACE_INPUT_						//入力
#define _IMATH_INTERFACE_AUDIO_						//オーディオ
#define _IMATH_INTERFACE_IMAGE_						//画像
#define _IMATH_INTERFACE_FONT_						//フォント
#endif

//オーディオ関連の設定
#ifdef _IMATH_INTERFACE_AUDIO_
//SEの種類の数
#ifndef _IMATH_INTERFACE_SE_NAM_
#define _IMATH_INTERFACE_SE_NAM_	2
#endif
//SEのチャネルの数(SEの種類に対してチャネルは等分される)
#ifndef _IMATH_INTERFACE_CHANNEL_NUM_
#define _IMATH_INTERFACE_CHANNEL_NUM_			16
#endif
#endif

//Zバッファの深度
#ifndef _IMATH_INTERFACE_Z_BUFFER_BITS_
#define _IMATH_INTERFACE_Z_BUFFER_BITS_			24
#endif

#include <GL/glew.h>
#pragma comment(lib, "glew32.lib")
#include <GL/gl.h>
#include <GL/glu.h>
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "opengl32.lib")
//#pragma comment (lib, "gdi32.lib")    /* link Windows GDI lib        */
//#pragma comment (lib, "winmm.lib")    /* link Windows MultiMedia lib */
//#pragma comment (lib, "user32.lib")   /* link Windows user lib       */
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <SDL.h>
#include <SDL_syswm.h>
#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "SDL2main.lib")
#pragma comment(lib, "SDL2_mixer.lib")
#pragma comment(lib, "SDL2_ttf.lib")


#include "IMathLib/utility/functional.hpp"
#include "IMathLib/math/liner_algebra.hpp"
#include "IMathLib/interface/log.hpp"
#include "IMathLib/container/map.hpp"

namespace iml {
	namespace i_f {

		//汎用ステートの有効
		template <size_t FirstIndex>
		inline void enable_state() {
			glEnable(FirstIndex);
		}
		template <size_t FirstIndex, size_t SecondIndex, size_t... Index>
		inline void enable_state() {
			glEnable(FirstIndex);
			enable_state<SecondIndex, Index...>();
		}
		//汎用ステートの無効
		template <size_t FirstIndex>
		inline void disable_state() {
			glDisable(FirstIndex);
		}
		template <size_t FirstIndex, size_t SecondIndex, size_t... Index>
		inline void disable_state() {
			glDisable(FirstIndex);
			disable_state<SecondIndex, Index...>();
		}

		//矩形領域
		template <class T>
		struct rect {
			T left, right, bottom, top;
			constexpr rect() :left{}, right{}, bottom{}, top{} {}
			constexpr rect(const T& l, const T& r, const T& b, const T& t) : left(l), right(r), bottom(b), top(t) {}

			template <class S>
			rect& operator=(const rect<S>& r) {
				left = r.left;
				right = r.right;
				bottom = r.bottom;
				top = r.top;
			}
		};

		//インターフェースの統括管理
		class interface_control :public singleton<interface_control> {
			friend class singleton<interface_control>;
			friend class window;

			log_output		log_text;
			SDL_GLContext	context = nullptr;		//OpenGLのコンテキスト
			rect<int_t>		client_area;			//仮想クライアント領域(ビューポート領域)

			tree_map<Uint32, window*>	wnd_id;		//ウィンドウIDのための探索木

			interface_control() {}
		public:
			~interface_control() {
				SDL_GL_DeleteContext(context);

				//各種終了
#ifdef _IMATH_INTERFACE_AUDIO_
				Mix_CloseAudio();
				Mix_Quit();
#endif
/*#ifdef _IMATH_INTERFACE_IMAGE_
				IMG_Quit();
#endif*/
#ifdef _IMATH_INTERFACE_FONT_
				TTF_Quit();
#endif
			}

			void init() {

				//SDL2の初期化
				if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
					log_text.wrrite(log_output::fatal, SDL_GetError(), log_output::function_error, "SDL_Init");
				SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);									//ダブルバッファリングの有効
				SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, _IMATH_INTERFACE_Z_BUFFER_BITS_);		//デプスバッファは32ビット精度
				
				//ログの初期化
				log_text.init();
				//ログレベルの設定
#ifdef _DEBUG
				log_text.set_log_level(log_output::debug);
#else
				log_text.set_log_level(log_output::error);
#endif

				//各種インターフェースの初期化
#ifdef _IMATH_INTERFACE_AUDIO_
				//サウンドミキシングの初期化
				if (Mix_Init(MIX_INIT_OGG | MIX_INIT_MOD | MIX_INIT_MP3) == -1)
					log_text.wrrite(log_output::fatal, Mix_GetError(), log_output::function_error, "Mix_Init");
				//44.1KHz, 16bit符号あり, システムのバイト順, ステレオ, 4096byteチャンクでサウンドミキサーを開く
				if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) == -1)
					log_text.wrrite(log_output::fatal, Mix_GetError(), log_output::function_error, "Mix_OpenAudio");
#endif
				//TrueTypeフォント
#ifdef _IMATH_INTERFACE_FONT_
				if (TTF_Init() == -1)
					log_text.wrrite(log_output::fatal, TTF_GetError(), log_output::function_error, "TTF_Init");
#endif

#ifdef _IMATH_INTERFACE_AUDIO_
				{
					//チャネル数の補正(チャネル数がSEの数で等分できる必要がある)
					size_t channel_sum = _IMATH_INTERFACE_CHANNEL_NUM_;
					if (channel_sum%_IMATH_INTERFACE_SE_NAM_ != 0)
						channel_sum += _IMATH_INTERFACE_SE_NAM_ - channel_sum % _IMATH_INTERFACE_SE_NAM_;
					//一つあたりのチャネルの個数
					size_t channel_1 = channel_sum / _IMATH_INTERFACE_SE_NAM_;
					//ミキシングチャネルを確保する
					Mix_AllocateChannels(channel_sum);
					//各チャンネルを登録
					for (size_t i = 0; i < _IMATH_INTERFACE_SE_NAM_; ++i)
						Mix_GroupChannels(i*channel_1, (i + 1)*channel_1 - 1, i);
				}
#endif
				//デフォルトでは文字入力を受け付けない
				SDL_StopTextInput();
			}

			//ログの書き込み
			template <class T, class S = char*>
			void log_wrrite(size_t level, const T& str1, size_t code = 0, const S& str2 = nullptr) {
				log_text.wrrite(level, str1, code, str2);
			}
			//現在のウィンドウに対する仮想的なクライアント領域の変更
			void set_client_area(const rect<int_t>& r) {
				client_area = r;
				glViewport(r.left, r.top, r.right - r.left, r.bottom - r.top);
			}
			//現在設定されているクライアント領域の取得
			const rect<int_t>& get_client_area() { return client_area; }

			//コンテキストの破棄
			void destroy() { SDL_GL_DeleteContext(context); context = nullptr; }

			//ビューポート行列の取得
			template <class T>
			matrix4<T> get_viewport_matrix() {
				//flip_y<T>()*
				return viewport_matrix<T>(client_area.left, client_area.top, client_area.right - client_area.left, client_area.bottom - client_area.top, 0, 1);
			}

			friend void window_event(const SDL_WindowEvent& ev);
		};

		//ウィンドウ
		class window {

			SDL_Window*		hwnd = nullptr;				//ウィンドウハンドル
			Uint8			wnd_id;						//ウィンドウID
			SDL_SysWMinfo	wnd_info;					//ウィンドウ情報

			SDL_GLContext	&context = interface_control::inst()->context;			//コンテキストの参照

			function<void(void)>			active_event;		//ウィンドウがアクティブのときの処理
			function<void(Sint32, Sint32)>	move_event;			//ウィンドウが動いたときの処理
			function<void(Sint32, Sint32)>	resize_event;		//ウィンドウのサイズが変わったときの処理
			function<void(void)>			close_event;		//ウィンドウを閉じるときの処理
		public:
			window(size_t w, size_t h, Uint32 flag = SDL_WINDOW_HIDDEN) :window("IMathLib_interface window", w, h) {}
			window(const char* title, size_t w, size_t h, Uint32 flag = SDL_WINDOW_HIDDEN)
				:active_event([]() {})
				, move_event([](Sint32, Sint32) {})
				, resize_event([](Sint32, Sint32) {})
				, close_event([&]() {this->hide(); })
			{
				//描画をOpenGLにするウィンドウの作成
				hwnd = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_OPENGL | flag);
				if (hwnd == nullptr)
					interface_control::inst()->log_wrrite(log_output::fatal, SDL_GetError(), log_output::function_error, "SDL_CreateWindow");
				//OpenGLのコンテキストがないときは作成しカレントにする
				if (context == nullptr) {
					context = SDL_GL_CreateContext(hwnd);
					if (context == nullptr)
						interface_control::inst()->log_wrrite(log_output::fatal, SDL_GetError(), log_output::function_error, "SDL_GL_CreateContext");

					glClearColor(1, 1, 1, 1);		//バッファクリア時の背景色
					glFrontFace(GL_CCW);						//左回りがポリゴンの全面として扱う

					//GLEWの初期化(ウィンドウを作ってからでないとエラーになる)
#ifdef _IMATH_GLEW_
					{
						size_t err = glewInit();
						if (err != GLEW_OK)
							interface_control::inst()->log_text.wrrite(log_output::fatal, reinterpret_cast<char*>(const_cast<GLubyte*>(glewGetErrorString(err))), log_output::function_error, "glewInit");
					}
#endif
				}

				//ウィンドウIDの取得
				if ((wnd_id = SDL_GetWindowID(hwnd)) == 0)
					interface_control::inst()->log_wrrite(log_output::fatal, SDL_GetError(), log_output::function_error, "SDL_GetWindowID");
				interface_control::inst()->wnd_id.insert(wnd_id, this);
			}
			~window() {
				interface_control::inst()->wnd_id.earse(wnd_id);
				SDL_DestroyWindow(hwnd);
			}

			//ウィンドウの表示
			void show() const { SDL_ShowWindow(hwnd); }
			//ウィンドウの非表示
			void hide() const { SDL_HideWindow(hwnd); }

			//ウィンドウハンドルの取得
			SDL_Window* handle() const { return hwnd; }

			//ウィンドウをカレントにする
			void current() const {
				if (SDL_GL_MakeCurrent(hwnd, context) != 0)
					interface_control::inst()->log_wrrite(log_output::error, SDL_GetError(), log_output::function_error, "SDL_GL_MakeCurrent");
			}
			//ウィンドウフレームの更新
			void update() const {
				SDL_GL_SwapWindow(hwnd);
			}
			//カラーバッファとZバッファのクリア
			void clear() const {
				SDL_Window* temp = SDL_GL_GetCurrentWindow();
				current();
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				//カレントを戻す
				if (SDL_GL_MakeCurrent(temp, context) != 0)
					interface_control::inst()->log_wrrite(log_output::error, SDL_GetError(), log_output::function_error, "SDL_GL_MakeCurrent");
			}

			//コールバック関数の設定
			template <class F>
			void active_ev(F f) { active_event = f; }
			template <class F>
			void move_ev(F f) { move_event = f; }
			template <class F>
			void resize_ev(F f) { resize_event = f; }
			template <class F>
			void close_ev(F f) { close_event = f; }

			friend void window_event(const SDL_WindowEvent& ev);
		};

		//ウィンドウイベントの処理
		inline void window_event(const SDL_WindowEvent& ev) {
			window* wnd = interface_control::inst()->wnd_id[ev.windowID];

			switch (ev.event) {
			case SDL_WINDOWEVENT_EXPOSED:
				wnd->active_event();
				break;
			case SDL_WINDOWEVENT_MOVED:
				wnd->move_event(ev.data1, ev.data2);
				break;
			case SDL_WINDOWEVENT_RESIZED:
				wnd->resize_event(ev.data1, ev.data2);
				break;
			case SDL_WINDOWEVENT_CLOSE:
				wnd->close_event();
				break;
			}
		}
	}
}




#endif