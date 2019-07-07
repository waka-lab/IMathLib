#ifndef IMATHLIB_INTERFACE_INPUT_HPP
#define IMATHLIB_INTERFACE_INPUT_HPP
#ifdef IMATHLIB_INTERFACE_INPUT

#include "IMathLib/interface/interface.hpp"
#include "IMathLib/math/liner_algebra/vector.hpp"

namespace iml {
	namespace i_f {

		// キーボード入力の状態管理クラス
		class keyboard_input : public singleton<keyboard_input> {
			friend class singleton<keyboard_input>;

			bool now_m[SDL_NUM_SCANCODES];			// キーボード入力情報
			bool last_m[SDL_NUM_SCANCODES];			// 前回の入力

			constexpr keyboard_input() :now_m{}, last_m{}{}
		public:
			~keyboard_input() {}

			// 更新
			void update() {
				const Uint8 *state = SDL_GetKeyboardState(nullptr);
				for (size_t i = 0; i < SDL_NUM_SCANCODES; ++i) {
					last_m[i] = now_m[i];
					now_m[i] = state[i] == 1;
				}
			}
			// 押している
			bool push(int_t code) const { return now_m[code]; }
			// 押した瞬間
			bool trigger(int_t code) const { return (now_m[code] && !last_m[code]); }
			// 離した瞬間
			bool release(int_t code) const { return (!now_m[code] && last_m[code]); }
		};

		// マウス入力の状態管理クラス
		class mouse_input : public singleton<mouse_input> {
			friend class singleton<mouse_input>;

			vector2<int_t>	now_m;		// 現在の座標
			vector2<int_t>	last_m;		// 前回の座標
			bool	now_l_m, last_l_m;		// 左ボタン
			bool	now_r_m, last_r_m;		// 右ボタン
			bool	now_m_m, last_m_m;		// 中央ボタン

			constexpr mouse_input() :now_l_m(false), last_l_m(false), now_r_m(false), last_r_m(false), now_m_m(false), last_m_m(false) {}
		public:
			~mouse_input() {}

			// 更新(マウス座標は現在のフォーカスのあるウィンドウの座標)
			void update() {
				last_m = now_m;
				last_l_m = now_l_m; last_r_m = now_r_m; last_m_m = now_m_m;
				Uint32 temp = SDL_GetMouseState(&now_m[0], &now_m[1]);
				now_l_m = (temp&SDL_BUTTON(SDL_BUTTON_LEFT)) == SDL_BUTTON(SDL_BUTTON_LEFT);
				now_r_m = (temp&SDL_BUTTON(SDL_BUTTON_RIGHT)) == SDL_BUTTON(SDL_BUTTON_RIGHT);
				now_m_m = (temp&SDL_BUTTON(SDL_BUTTON_MIDDLE)) == SDL_BUTTON(SDL_BUTTON_MIDDLE);
			}
			// 現在のマウス座標
			const vector2<int_t>& now_mouse() const { return now_m; }
			// 前回からの相対マウス座標(移動距離)
			vector2<int_t> move_mouse() const { return now_m - last_m; }
			// クリックしている
			bool push_l() const { return now_l_m; }
			bool push_r() const { return now_r_m; }
			bool push_m() const { return now_m_m; }
			// クリックした瞬間
			bool trigger_l() const { return now_l_m && !last_l_m; }
			bool trigger_r() const { return now_r_m && !last_r_m; }
			bool trigger_m() const { return now_m_m && !last_m_m; }
			// 離した瞬間
			bool release_l() const { return !now_l_m && last_l_m; }
			bool release_r() const { return !now_r_m && last_r_m; }
			bool release_m() const { return !now_m_m && last_m_m; }
		};
	}
}


#endif
#endif