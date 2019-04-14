#ifndef _LOAD_HPP
#define _LOAD_HPP

//ファイルのロードを中継する(文字コードの問題の回避とソースの簡易化のため)

#include "IMathLib/IMathLib_config.hpp"

namespace iml {
	namespace i_f {

		//最も基本的なローダー
		template <class T, class... Args>
		inline T* load(Args... args) {
			return new T(iml::forward<Args>(args)...);
		}

		//全てのバイナリデータのロードのためのクラス
		template <class CharT>
		class binary_load {
			unsigned char* data = nullptr;
		public:
			binary_load(const CharT* binary_file) {
				std::ifstream ifs(binary_file, std::ios::in | std::ios::binary);
				int len;
				ifs.seekg(0, std::ios::end);
				len = ifs.tellg();
				ifs.seekg(0, std::ios::beg);
				data = new unsigned char[len];
				ifs.read((char*)data, len);
			}
			~binary_load() { delete[] data; }

			operator const unsigned char*() const { return data; }
		};
		//全てのテキストデータのロードのためのクラス
		template <class CharT>
		class text_load {
			std::string str;
		public:
			//flag:コメント除去フラグ(C++準拠)
			text_load(const CharT* text_file, bool flag = false) {
				//ファイル読み込み
				std::fstream ifs(text_file, std::ios::in);
				if (ifs.fail()) {
					interface_control::inst()->log_wrrite(log_output::error, "", log_output::file_read_error, text_file);
					str = nullptr;
					return;
				}
				str = std::string((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

				if (flag) {
					//コメントを全て削除する
					for (size_t c1 = str.find("/*"), c2 = str.find("*/")
						, c3 = str.find("//"), c4 = str.find("\n", c3); (c1 != std::string::npos) || (c3 != std::string::npos); c1 = str.find("/*"), c2 = str.find("*/"), c3 = str.find("//"), c4 = str.find("\n", c3)) {
						if (c1 < c3) { str.erase(c1, c2 - c1 + 2); c3 = c1; c4 = c2; }
						else { str.erase(c3, c4 - c3); c1 = c3; c2 = c4; }
					}
				}
			}
			~text_load() {}

			operator const char*() const { return str.c_str(); }
		};

		//1行づつテキストをロードするためのクラス
		template <class CharT>
		class text_line_load {
			//std::ifstream	ifs;
			//std::string		str;
			FILE			*fp;
			char			str[256];
			string<char>	dir;
		public:
			text_line_load(const CharT* text_file) :dir(text_file, (iml::max)(string_rfind(text_file, "\\", 1), string_rfind(text_file, "/", 1))) {
				errno_t error;
				if ((error = fopen_s(&fp, text_file, "rt")) != 0) {
					fp = nullptr;
					strerror_s(str, 256, error);
					interface_control::inst()->log_wrrite(log_output::error, str, log_output::function_error, "fopen_s");
				}
				dir.push_back('/');
			}
			~text_line_load() { fclose(fp); }

			//1行取得
			const char* get_line() {
				//if (!std::getline(ifs, str)) return nullptr;
				//return str.c_str();
				if (fp == nullptr) return nullptr;
				if (fgets(str, 256, fp) == nullptr) return nullptr;
				return str;
			}
			//ディレクトリパスの取得
			const string<char>& dir_path() const {
				return dir;
			}
		};
	}
}

#endif