#ifndef IMATH_UTILITY_SMART_PTR_HPP
#define IMATH_UTILITY_SMART_PTR_HPP

#include "IMathLib/container/container.hpp"

//スマートポインタの実装

namespace iml {

	//スマートポインタの基底クラス
	template <class T>
	class smart_base {
	protected:
		T*	p;
	public:
		using element_type = typename remove_extent<T>::type;

		constexpr smart_base() :p(nullptr) {}
		constexpr smart_base(T* p) :p(p) {}
		template <class S>
		constexpr smart_base(S* p) : p(static_cast<T*>(p)) {}
		virtual ~smart_base() = 0 {};

		//各種参照
		T& operator*() const { return *get(); }
		T* operator->() const { return get(); }
		T* get() const { return p; }

		//リソースの破棄(デストラクタで自動解放等される)
		virtual void reset() = 0;

		//リソースを所持しているかの判定
		operator bool() const { return !!p; }
		bool operator!() const { return !p; }

		const T& operator[](imsize_t index) const { return p[index]; }
		T& operator[](imsize_t index) { return p[index]; }
	};

	//比較演算
	template <class T>
	inline bool operator==(const smart_base<T>& lhs, const smart_base<T>& rhs) {
		return lhs.get() == rhs.get();
	}
	template<class T>
	inline bool operator!=(const smart_base<T>& lhs, const smart_base<T>& rhs) {
		return lhs.get() != rhs.get();
	}
}

//シェアスマートポインタ
namespace iml {

	//参照カウンタ
	template <class T, imsize_t N>
	class shared_count {
		template <class _T, imsize_t _N>
		friend class weak_count;

		//カウンタ実装部
		struct impl {
			//実装とデリーター
			template<class... Args>
			constexpr explicit impl(T* p, Args... args) :shared_cnt(1), weak_cnt(0), dealloc(deallocator<T>::get<N>(p, iml::forward<Args>(args)...)) {}
			template<class S, class... Args>
			constexpr explicit impl(S* p, Args... args) :shared_cnt(1), weak_cnt(0), dealloc(deallocator<T>::get<N>(static_cast<T*>(p), iml::forward<Args>(args)...)) {}
			constexpr impl(const impl& c) :shared_cnt(c.shared_cnt), weak_cnt(c.weak_cnt), dealloc(c.dealloc) {}
			impl(impl&& c) { *this = move(c); }
			~impl() { dealloc->destroy(); }

			imsize_t inc() { return ++shared_cnt; }
			imsize_t dec() { return --shared_cnt; }
			imsize_t weak_inc() { return ++weak_cnt; }
			imsize_t weak_dec() { return --weak_cnt; }
			imsize_t shared_count() { return shared_cnt; }
			imsize_t weak_count() { return weak_cnt; }

			imsize_t			shared_cnt;			//シェアカウンタ
			imsize_t			weak_cnt;			//ウィークカウンタ
			deallocator<T>		*dealloc;

			impl& operator=(const impl&) = delete;
			impl& operator=(impl&& im) noexcept {
				this->shared_cnt = im.shared_cnt;
				this->weak_cnt = im.weak_cnt;
				this->dealloc = im.dealloc;
				im.dealloc = nullptr;
				return *this;
			}
		};
		impl*	pn;
	public:
		constexpr shared_count() :pn(nullptr) {}
		shared_count(const shared_count& r) :pn(r.pn) { if (pn) pn->inc(); }
		shared_count(shared_count&& r) { *this = move(r); }
		template<class... Args>
		constexpr explicit shared_count(T* p, Args... args) :pn(new impl(p, iml::forward<Args>(args)...)) {}
		template<class S, class... Args>
		constexpr explicit shared_count(S* p, Args... args) :pn(new impl(p, iml::forward<Args>(args)...)) {}
		~shared_count() { release(); }

		//弱参照とシェアカウンタが0ならば解放
		void release() {
			if (pn && !pn->dec() && !pn->weak_count()) {
				pn->dealloc->dispose();
				delete pn;
				pn = nullptr;
			}
		}
		shared_count& operator=(const shared_count& r) {
			impl* tmp = r.pn;
			//同じでないならばthisをマイナスしてrをプラスする
			if (tmp != pn) {
				if (tmp != nullptr) tmp->inc();
				release();
				pn = tmp;
			}
			return *this;
		}
		shared_count& operator=(shared_count&& r) {
			//同じでないならばthisをマイナス
			if (r.pn != pn) {
				release();
				pn = r.pn;
			}
			return *this;
		}
		//スワップ
		void swap(shared_count& r) { iml::swap(pn, r.pn); }
		//インスタンスが唯一かの判定
		bool unique() const { return use_count() == 1; }
		//カウントの取得
		imsize_t use_count() const { return pn ? pn->shared_count() : 0; }
	};
	//所有権をシェア(共有)するスマートポインタ(デフォルトは通常変数)
	template <class T, imsize_t N = deallocator<T>::variable>
	class shared_ptr :public smart_base<T> {
		template<class S, imsize_t N>
		friend class shared_ptr;
		template<class S, imsize_t N>
		friend class weak_ptr;

		using smart_base<T>::p;

		shared_count<T, N>	sc;			//シェアカウンタ
	public:
		constexpr shared_ptr() noexcept : smart_base<T>(nullptr), sc() {}
		shared_ptr(const shared_ptr& s) noexcept : smart_base<T>(s.p), sc(s.sc) {}
		shared_ptr(shared_ptr&& s) noexcept { *this = move(s); }
		template<class S>
		shared_ptr(const shared_ptr<S, N>& s) noexcept : smart_base<T>(s.p), sc(s.sc) {}
		template<class S>
		shared_ptr(shared_ptr<S, N>&& s) noexcept { *this = move(s); }
		template<class S, class... Args>
		explicit shared_ptr(S* p, Args... args) :smart_base<T>(p), sc(p, iml::forward<Args>(args)...) {}
		template <class S, imsize_t N>
		shared_ptr(const weak_ptr<S, N>& r) : sc(*r.wc.pn) {}
		~shared_ptr() {}

		//所有権の破棄
		void reset() { shared_ptr().swap(*this); }
		void reset(shared_ptr& s) { shared_ptr(s).swap(*this); }
		template<class S>
		void reset(S* p) { shared_ptr(p).swap(*this); }
		template<class S, class D>
		void reset(S* p, D d) { shared_ptr(p, d).swap(*this); }
		//スワップ
		void swap(shared_ptr& other) noexcept {
			iml::swap(p, other.p);
			sc.swap(other.sc);
		}

		//インスタンスが唯一かの判定
		bool unique() const { return sc.unique(); }
		//カウントの取得
		imsize_t use_count() const { return sc.use_count(); }

		//代入演算
		shared_ptr& operator=(const shared_ptr& s) {
			this->sc = s.sc; this->p = s.p;
			return *this;
		}
		template<class S>
		shared_ptr& operator=(const shared_ptr<S, N>& s) {
			this->sc = s.sc; this->p = s.p;
			return *this;
		}
		shared_ptr& operator=(shared_ptr&& s) noexcept {
			this->sc = s.sc; this->p = s.p;
			return *this;
		}
		template<class S>
		shared_ptr& operator=(shared_ptr<S, N>&& s) noexcept {
			this->sc = s.sc; this->p = s.p;
			return *this;
		}
	};
}

//shared_ptrの弱参照を保持するスマートポインタ(shared_ptrのオブサーバー)
namespace iml {

	//ウィークカウンタ
	template <class T, imsize_t N>
	class weak_count {
		typename shared_count<T, N>::impl* pn;
	public:
		constexpr weak_count() :pn(nullptr) {}
		weak_count(const shared_count<T, N>& r) :pn(r.pn) { if (pn) pn->weak_inc(); }
		weak_count(const weak_count& r) :pn(r.pn) { if (pn) pn->weak_inc(); }
		weak_count(const weak_count&& r) :pn(r.pn) {}
		~weak_count() { release(); }

		//弱参照とシェアカウントが0ならば解放
		void release() {
			if (pn && !pn->weak_dec() && !pn->shared_count()) {
				pn->dealloc->dispose();
				delete pn;
				pn = nullptr;
			}
		}

		weak_count& operator=(const shared_count<T, N>& r) {
			typename shared_count<T, N>::impl* tmp = r.pn;
			//同じでないならばthisをマイナスしてrをプラスする
			if (tmp != pn) {
				if (tmp != nullptr) tmp->weak_inc();
				release();
				pn = tmp;
			}
			return *this;
		}
		weak_count& operator=(shared_count<T, N>&& r) {
			//同じでないならばthisをマイナス
			if (r.pn != pn) {
				pn = r.pn;
				r.pn = nullptr;
			}
			return *this;
		}
		weak_count& operator=(const weak_count& r) {
			typename shared_count<T, N>::impl* tmp = r.pn;
			//同じでないならばthisをマイナスしてrをプラスする
			if (tmp != pn) {
				if (tmp) tmp->weak_inc();
				release();
				pn = tmp;
			}
			return *this;
		}
		weak_count& operator=(weak_count&& r) {
			//同じでないならばthisをマイナス
			if (r.pn != pn) {
				pn = r.pn;
				r.pn = nullptr;
			}
			return *this;
		}
		//スワップ
		void swap(weak_count& r) { iml::swap(pn, r.pn); }
		//インスタンスが唯一かの判定
		bool unique() const { return use_count() == 1; }
		//カウントの取得
		imsize_t use_count() const { return pn ? pn->shared_count() : 0; }
	};
	//shared_ptrの弱参照を保持して循環参照を解決するスマートポインタ
	template <class T, imsize_t N = deallocator<T>::variable>
	class weak_ptr : public smart_base<T> {
		template<class S, imsize_t N>
		friend class weak_ptr;
		template<class S, imsize_t N>
		friend class shared_ptr;

		using smart_base<T>::p;

		weak_count<T, N> wc;			//ウィークカウンタ
	public:
		constexpr weak_ptr() noexcept : smart_base<T>(nullptr), wc() {}
		weak_ptr(const weak_ptr& r) noexcept : smart_base<T>(r.p), wc(r.wc) {}
		weak_ptr(const weak_ptr&& r) noexcept { *this = move(r); }
		template <class S>
		weak_ptr(const weak_ptr<S, N>& r) noexcept : smart_base<T>(r.p), wc(r.wc) {}
		template <class S>
		weak_ptr(const weak_ptr<S, N>&& r) noexcept { *this = move(r); }
		template <class S>
		weak_ptr(const shared_ptr<S, N>& r) noexcept : smart_base<T>(r.p), wc(r.sc) {}

		//所有権の破棄
		void reset() { weak_ptr().swap(*this); }
		void reset(const weak_ptr& s) { weak_ptr(s).swap(*this); }
		//スワップ
		void swap(weak_ptr& other) noexcept {
			iml::swap(p, other.p);
			wc.swap(other.wc);
		}
		//監視しているリソースのインスタンスの取得
		shared_ptr<T, N> lock() const{
			return shared_ptr<T, N>(*this);
		}

		//インスタンスが唯一かの判定
		bool unique() const { return wc.unique(); }
		//カウントの取得
		imsize_t use_count() const { return wc.use_count(); }

		//代入演算
		template <class S>
		weak_ptr& operator=(const shared_ptr<S, N>& s) {
			this->p = s.p; this->wc = s.sc;
			return *this;
		}
		weak_ptr& operator=(const weak_ptr& s) {
			this->p = s.p; this->wc = s.wc;
			return *this;
		}
		weak_ptr& operator=(const weak_ptr&& s) {
			this->p = s.p; this->wc = s.wc;
			return *this;
		}
		template<class S>
		weak_ptr& operator=(const weak_ptr<S, N>& s) {
			this->p = s.p; this->wc = s.wc;
			return *this;
		}
		template<class S>
		weak_ptr& operator=(const weak_ptr<S, N>&& s) {
			this->p = s.p; this->wc = s.wc;
			return *this;
		}
	};
}

//インスタンスの所有権が唯一(ユニーク)なスマートポインタ
namespace iml {

	template <class T, imsize_t N = deallocator<T>::variable>
	class unique_ptr : public smart_base<T> {
		template<class _T, imsize_t _N>
		friend class unique_ptr;

		using smart_base<T>::p;

		deallocator<T> *dealloc;		//ディアロケーター
	public:
		constexpr unique_ptr() noexcept : smart_base<T>(nullptr), dealloc(deallocator<T>::get<N>(static_cast<T*>(nullptr))) {}
		unique_ptr(unique_ptr&& r) noexcept { *this = move(r); }
		template<class S>
		unique_ptr(unique_ptr<S, N>&& r) noexcept { *this = move(r); }
		template<class S, class... Args>
		explicit unique_ptr(S* p, Args... args) :smart_base<T>(p), dealloc(deallocator<T>::get<N>(static_cast<T*>(p), iml::forward<Args>(args)...)) {}
		~unique_ptr() { dealloc->dispose(); dealloc->destroy(); }

		//リソースの所有権を破棄(解放はしない)
		void release() noexcept { this->p = nullptr; }
		//リソースの破棄
		void reset() {
			dealloc->dispose();
			this->p = nullptr;
		}
		//リソースを破棄して再設定
		void reset(T* p) {
			if (this->p == p) return;
			dealloc->dispose(p);
			this->p = p;
		}
		void swap(unique_ptr& x) noexcept {
			iml::swap(p, x.p);
			iml::swap(dealloc, x.dealloc);
		}

		//コピー禁止
		unique_ptr& operator=(const unique_ptr&) = delete;
		unique_ptr& operator=(unique_ptr&& s) noexcept {
			//同じポインタでないとき
			if (this != &s) {
				p = s.p; s.p = nullptr;
				dealloc = s.dealloc; s.dealloc = nullptr;
			}
			return *this;
		}
	};
}

#endif