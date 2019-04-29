#ifndef _FPS_HPP
#define _FPS_HPP

#include "IMathLib/interface/timer.hpp"
#include <thread>

//FPS管理

namespace iml {
	namespace i_f {

		template <class T>
		class fps_control {
			size_t	cnt;			//フレームカウンタ
			T			now_fps;		//現在のfps
			size_t	N;				//サンプル数
			T			fps;			//設定したfps
			timer<std::chrono::milliseconds>	time;					//時間計測

		public:
			constexpr fps_control() :cnt(1), now_fps(0), fps(60), N(20) {}
			constexpr fps_control(const T& fps, size_t sample) : cnt(1), now_fps(0), fps(fps), N(sample) {}

			//フレーム記憶
			void update() {
				if (cnt == 1) time.start();			//1フレーム目は時間リセット
				if (cnt == N) {						//サンプル数目は平均を計算
					long long t = time.now_time();
					now_fps = 1000 / (static_cast<T>(t) / static_cast<T>(N));
					cnt = 0;
				}
				++cnt;
			}
			//ウェイト
			void wait() {
				long long took_time = time.now_time();													//かかった時間
				long long wait_time = static_cast<long long>(cnt * 1000 / fps - took_time);			//待つべき時間
				//待機
				if (wait_time > 0) std::this_thread::sleep_for(std::chrono::milliseconds(wait_time));
			}
			//fps取得
			T get_fps() { return now_fps; }
			//フレームカウント取得
			size_t frame() { return cnt; }

			//fpsと計測のサンプル数の変更
			void set_fps(const T& _fps, size_t _sample) {
				fps = _fps; N = _sample;
			}
		};
	}
}


#endif