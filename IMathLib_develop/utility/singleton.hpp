#ifndef IMATHLIB_UTILITY_SINGLETON_HPP
#define IMATHLIB_UTILITY_SINGLETON_HPP

#include "IMathLib/utility/utility.hpp"
#include "IMathLib/container/allocator.hpp"

namespace iml {

	// シングルトンを実装するための基底クラス
	template <class T>
	class singleton {
		static inline T* inst_m = nullptr;

		singleton(const singleton &) = delete;
		singleton(singleton &&) = delete;
		singleton& operator=(const singleton &) = delete;
		singleton& operator=(singleton &&) = delete;
	protected:
		constexpr singleton() {}
	public:
		virtual ~singleton() = 0 {}

		// インスタンスの生成
		template <class... Types>
		static T* construct(Types... args) {
			return inst_m = new T(forward<Types>(args)...);
		}
		// インスタンスの破棄
		static void destroy() {
			delete inst_m;
			inst_m = nullptr;
		}
		// インスタンスの取得
		static T* inst() {
			return (inst_m == nullptr) ? construct() : inst_m;
		}
		// インスタンスの有無
		static bool is_exists_inst() { return inst_m != nullptr; }
	};
}

#endif