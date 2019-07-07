#ifndef IMATHLIB_UTILITY_TIMER_HPP
#define IMATHLIB_UTILITY_TIMER_HPP

#include <chrono>


// ミリ秒制度の時間計測処理
namespace iml {

	template <class T>
	class timer {
		std::chrono::high_resolution_clock::time_point start_m;
		std::chrono::high_resolution_clock::time_point pause_m;
	public:
		constexpr timer() : start_m(), pause_m() {}
		// 時間計測開始
		void start() { start_m = std::chrono::high_resolution_clock::now(); }
		// 現在の計測開始からの時間の取得
		long long now_time() {
			std::chrono::high_resolution_clock::time_point temp(std::chrono::high_resolution_clock::now());
			return std::chrono::duration_cast<T>(temp - start_m).count();
		}
		// 一時停止
		void pause() { pause_m = std::chrono::high_resolution_clock::now(); }
		// 一時停止解除(戻り値はポーズ時間)
		long long resume() {
			std::chrono::high_resolution_clock::time_point temp(std::chrono::high_resolution_clock::now());
			start_m += temp - pause_m;
			return std::chrono::duration_cast<T>(temp - pause_m).count();
		}
	};
}



#endif