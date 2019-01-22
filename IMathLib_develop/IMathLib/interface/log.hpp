#ifndef _LOG_HPP
#define _LOG_HPP

#include <filesystem>
#include "IMathLib/interface/timer.hpp"
#include "IMathLib/string/string.hpp"

//ログを出力する

namespace iml {
	namespace i_f {

		//エラーの出力(初期化さえしなければ出力されない)
		class log_output {
			timer<std::chrono::milliseconds>		time;			//時間の記録
			char*		file_name = nullptr;		//出力ファイル名
			imsize_t	log_level = 0;				//ログの出力レベル
			//ログレベルの文字列テーブルから取得
			static const char* log_level_str(imsize_t n) {
				static const char * const table[] = {
					"Lv.0 Debug:"
					,"Lv.1 Warn:"
					,"Lv.2 Error:"
					,"Lv.3 Fatal:"
				};
				return table[n];
			}
			//エラーコードの文字列テーブル
			static const char* error_code_str(imsize_t n) {
				static const char * const table[] = {
					""
					,"code\"1\":variable error.->"
					,"code\"2\":function error.->"
					,"code\"3\":file read error.->"
					,"code\"4\":file format error.->"
					,"code\"5\":file data error.->"
				};
				return table[n];
			}

			//ミリ秒に対して各時間数値の取得
			static long long get_ms(long long ms) {
				return ms % 10000;
			}
			static long long get_sec(long long ms) {
				return (ms / 10000) % 60;
			}
			static long long get_min(long long ms) {
				return ((ms / 10000 - get_sec(ms)) / 60) % 60;
			}
			static long long get_hour(long long ms) {
				return (ms / 10000 - get_sec(ms) - get_min(ms) * 60) / 3600;
			}
		public:
			constexpr log_output() {}
			~log_output() { delete[] file_name; }

			//ログレベルの識別定数
			static const imsize_t debug = 0;
			static const imsize_t warn = 1;
			static const imsize_t error = 2;
			static const imsize_t fatal = 3;
			//エラーコードの識別定数
			static const imsize_t	none = 0;						//なし
			static const imsize_t	variable_error = 1;				//変数
			static const imsize_t	function_error = 2;				//関数
			static const imsize_t	file_read_error = 3;			//ファイル読み込み
			static const imsize_t	file_format_error = 4;			//ファイル形式
			static const imsize_t data_error = 5;					//一般的なデータ

			//ログレベルの設定
			void set_log_level(imsize_t level) { log_level = level; }
			//ログレベルの取得
			imsize_t get_log_level() { return log_level; }

			//初期化
			void init() {
#ifdef _IMATH_LOG_OUTPUT_
				namespace sys = std::experimental::filesystem;
				sys::path p("log.txt");
				//ログの出力ファイル名の決定
				if (sys::exists(p)) {
					for (imsize_t i = 2;; ++i) {
						p = "log" + std::to_string(i) + ".txt";
						if (!sys::exists(p)) break;
					}
				}
				file_name = new char[p.string().size() + 1]{};
				for (imsize_t i = 0; i < p.string().size(); ++i)
					file_name[i] = p.string()[i];
				time.start();
#endif
			}

			//ログの書き込み
			template <class T, class S = char*>
			void wrrite(imsize_t level, const T& str1, imsize_t code = 0, const S& str2 = nullptr) {
#ifdef _IMATH_LOG_OUTPUT_
				if (level >= log_level) {
					long long ms = time.now_time();
					std::fstream ofs(file_name, std::ios::app | std::ios::out);
					ofs.imbue(std::locale(std::locale(), new std::codecvt_utf8<char, 0x10ffff, std::codecvt_mode::generate_header>));
					ofs << get_hour(ms) << ':' << get_min(ms) << ':' << get_sec(ms) << ':' << get_ms(ms) << "->"
						<< log_level_str(level) << error_code_str(code) << str2
						<< std::endl << "msg:" << str1 << std::endl;
					ofs.close();
				}
				//ログレベルが3以上であるとき強制終了
				if (level >= fatal) std::exit(1);
#endif
			}
		};

	}
}


#endif