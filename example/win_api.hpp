#ifndef _WIN_API_HPP
#define _WIN_API_HPP

#define _IMATH_OPENGL_AND_SDL2_
#include "IMathLib/IMathLib.hpp"

#include <Windows.h>
#include <commctrl.h>
#pragma comment(lib, "comctl32.lib")


//WindowsのAPIを使いやすくする


namespace win_api {


	//メニューバー

	//ウィンドウ
	class window {
		HWND			hwnd = nullptr;			//ウィンドウハンドル
		HMENU			hmenu = nullptr;		//メニューハンドル
		WNDPROC			proc = nullptr;			//プロージャハンドル
		//HWND			parent;			//親ウィンドウ
		//iml::dynamic_array<window*>	child;			//子ウィンドウ


		//ウィンドウ処理のコールバック
		BOOL CALLBACK window_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

			switch (msg) {
				//初期化
			case WM_INITDIALOG:
				InitCommonControls();
				break;
				//メニューコマンド
			case WM_COMMAND:
				// 選択されたメニューの解析:
				switch (LOWORD(wParam)){
				case 1:
					if (MessageBox(hWnd, "終了しますか？", "Model Viewer", MB_OKCANCEL) == IDOK) DestroyWindow(hWnd);
					break;
				default:
					return (proc) ? CallWindowProc(proc, hWnd, msg, wParam, lParam) : DefWindowProc(hWnd, msg, wParam, lParam);
				}
				break;
				//ウィンドウ削除
			case WM_DESTROY:
				PostQuitMessage(0);
				break;
			default:
				return (proc) ? CallWindowProc(proc, hWnd, msg, wParam, lParam) : DefWindowProc(hWnd, msg, wParam, lParam);
			}

			return FALSE;
		}

		//ウィンドウ処理のコールバックの補助
		static BOOL CALLBACK _Window_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
			window* temp = (window*)(GetWindowLongPtr(hWnd, GWLP_USERDATA));

			if (temp == nullptr) {
				switch (msg) {
				case WM_CREATE:
					//CreateWindowのパラメータから取得する
					temp = (window*)(((LPCREATESTRUCT)lParam)->lpCreateParams);
					break;
				}
				//リンクしておく
				if (temp != nullptr) SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)(temp));
			}
			//ハンドルを持つならばプロージャの実行
			if (temp != nullptr) return temp->window_proc(hWnd, msg, wParam, lParam);
			
			return DefWindowProc(hWnd, msg, wParam, lParam);
		}

		//ダイアログ作成用のコンストラクタ
		window(HWND hdlg) {
			hwnd = hdlg;
			proc = WNDPROC(GetWindowLong(hwnd, GWLP_WNDPROC));

			SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)(_Window_proc));
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)(this));
		}
	public:
		window(iml::i_f::window& wnd) {
			SDL_SysWMinfo wnd_info;

			//バージョンで初期化
			SDL_VERSION(&wnd_info.version);
			//ウィンドウ情報の取得
			if (!SDL_GetWindowWMInfo(wnd.handle(), &wnd_info))
				iml::i_f::interface_control::inst()->log_wrrite(iml::i_f::log_output::fatal, SDL_GetError()
					, iml::i_f::log_output::function_error, "SDL_GetWindowWMInfo");
			hwnd = wnd_info.info.win.window;
			proc = WNDPROC(GetWindowLong(hwnd, GWLP_WNDPROC));

			SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)(_Window_proc));
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)(this));

			//メニューバーのセット
			//hmenu = LoadMenu(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDR_MAIN_MENU));
			//SetMenu(hwnd, hmenu);
		}
		~window() {}


		//ウィンドウの表示非表示
		void show() const { ShowWindow(hwnd, SW_SHOW); }
		void hide() const { ShowWindow(hwnd, SW_HIDE); }

		//ダイアログの作成
		template <int ID, class F>
		window* create_dialog(F call_back) {
			HWND	hdlg;
			//ダイアログの作成
			if ((hdlg = CreateDialog(nullptr, MAKEINTRESOURCE(ID), hwnd, call_back)) == nullptr) {
				iml::i_f::interface_control::inst()->log_wrrite(iml::i_f::log_output::warn, ""
					, iml::i_f::log_output::function_error, "CreateDialog");
				return nullptr;
			}
			return new window(hdlg);
		}
		template <int ID, class F>
		friend window* create_dialog(F call_back);
	};

	//親無しのダイアログの作成
	template <int ID, class F>
	inline window* create_dialog(F call_back) {
		HWND	hdlg;
		//ダイアログの作成
		if ((hdlg = CreateDialog(nullptr, MAKEINTRESOURCE(ID), nullptr, call_back)) == nullptr) {
			iml::i_f::interface_control::inst()->log_wrrite(iml::i_f::log_output::warn, ""
				, iml::i_f::log_output::function_error, "CreateDialog");
			return nullptr;
		}
		return new window(hdlg);
	}

}


#endif