#ifndef _TIMER_HPP
#define _TIMER_HPP

#include <chrono>

//ミリ秒制度の時間計測処理

namespace iml {
	namespace i_f {

		template <class T>
		class timer {
			std::chrono::high_resolution_clock::time_point _start;
			std::chrono::high_resolution_clock::time_point _pause;
		public:
			constexpr timer() {}
			//時間計測開始
			void start() {
				_start = std::chrono::high_resolution_clock::now();
			}
			//現在の時間の取得
			long long now_time() {
				std::chrono::high_resolution_clock::time_point _route(std::chrono::high_resolution_clock::now());
				return std::chrono::duration_cast<T>(_route - _start).count();
			}
			//一時停止
			void pause() {
				_pause = std::chrono::high_resolution_clock::now();
			}
			//一時停止解除(戻り値はポーズ時間)
			long long resume() {
				std::chrono::high_resolution_clock::time_point _route(std::chrono::high_resolution_clock::now());
				_start += _route - _pause;
				return std::chrono::duration_cast<T>(_route - _pause).count();
			}
		};
	}

}



#endif