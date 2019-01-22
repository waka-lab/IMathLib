#ifndef _IMATH_INPUT_HPP
#define _IMATH_INPUT_HPP

#ifdef _IMATH_INTERFACE_INPUT_

#include "IMathLib/interface/interface.hpp"
#include "IMathLib/math/liner_algebra/vector.hpp"

namespace iml {
	namespace i_f {

		//キーボード入力の状態管理クラス
		class keyboard_input : public singleton<keyboard_input> {
			friend class singleton<keyboard_input>;

			bool m_now[SDL_NUM_SCANCODES];			//キーボード入力情報
			bool m_last[SDL_NUM_SCANCODES];			//前回の入力

			constexpr keyboard_input() :m_now{}, m_last{}{}
		public:
			~keyboard_input() {}

			//更新
			void update() {
				const Uint8 *state = SDL_GetKeyboardState(nullptr);
				for (size_t i = 0; i < SDL_NUM_SCANCODES; ++i) {
					m_last[i] = m_now[i];
					m_now[i] = state[i] == 1;
				}
			}
			//押している
			bool push(imint_t code) const { return m_now[code]; }
			//押した瞬間
			bool trigger(imint_t code) const { return (m_now[code] && !m_last[code]); }
			//離した瞬間
			bool release(imint_t code) const { return (!m_now[code] && m_last[code]); }
		};

		//マウス入力の状態管理クラス
		class mouse_input : public singleton<mouse_input> {
			friend class singleton<mouse_input>;

			vector2<imint_t>	m_now;		//現在の座標
			vector2<imint_t>	m_last;		//前回の座標
			bool	m_now_l, m_last_l;		//左ボタン
			bool	m_now_r, m_last_r;		//右ボタン
			bool	m_now_m, m_last_m;		//中央ボタン

			constexpr mouse_input() :m_now_l(false), m_last_l(false), m_now_r(false), m_last_r(false), m_now_m(false), m_last_m(false) {}
		public:
			~mouse_input() {}

			//更新
			void update() {
				m_last = m_now;
				m_last_l = m_now_l; m_last_r = m_now_r; m_last_m = m_now_m;
				Uint32 temp = SDL_GetMouseState(&m_now[0], &m_now[1]);
				m_now_l = (temp&SDL_BUTTON(SDL_BUTTON_LEFT)) == SDL_BUTTON(SDL_BUTTON_LEFT);
				m_now_r = (temp&SDL_BUTTON(SDL_BUTTON_RIGHT)) == SDL_BUTTON(SDL_BUTTON_RIGHT);
				m_now_m = (temp&SDL_BUTTON(SDL_BUTTON_MIDDLE)) == SDL_BUTTON(SDL_BUTTON_MIDDLE);
			}
			//現在のマウス座標
			const vector2<imint_t>& now_mouse() { return m_now; }
			//前回からの相対マウス座標(移動距離)
			vector2<imint_t> move_mouse() { return m_now - m_last; }
			//クリックしている
			bool push_l() { return m_now_l; }
			bool push_r() { return m_now_r; }
			bool push_m() { return m_now_m; }
			//クリックした瞬間
			bool trigger_l() { return m_now_l && !m_last_l; }
			bool trigger_r() { return m_now_r && !m_last_r; }
			bool trigger_m() { return m_now_m && !m_last_m; }
			//離した瞬間
			bool release_l() { return !m_now_l && m_last_l; }
			bool release_r() { return !m_now_r && m_last_r; }
			bool release_m() { return !m_now_m && m_last_m; }
		};
	}
}


#endif
#endif