//
//
// shared_ptrもどき。スレッドセーフでない、例外安全でないので注意！ boost:;shared_ptrを使うべき
//
// Created by 2007 ikeda
//

#ifndef TSL_SHARED_PTR_HPP
#define TSL_SHARED_PTR_HPP

namespace tsl {

template <typename T_>
class shared_ptr {
	mutable struct Body {
		T_* parent;
		int ref;
		Body() : parent(0), ref(0) {}
		void deRef() { 
			if (--ref <= 0) {
				if (parent != 0) delete parent;
				delete this;
			}
		}
		void addRef() { ++ref; }
	}* m_body;
public:
	shared_ptr() : m_body(0) {}
	shared_ptr(T_* parent) {
		m_body = new Body;
		m_body->parent = parent;
		m_body->ref = 1;
	}
	shared_ptr(const shared_ptr& src) : m_body(0) {
		*this = src;
	}

	~shared_ptr() {
		if (m_body != 0) m_body->deRef();
	}

	shared_ptr& operator = (const shared_ptr& src) {
		if (m_body != 0) m_body->deRef();
		if (src.m_body != 0) src.m_body->addRef();
		m_body = src.m_body;
		return *this;
	}

	T_ & operator*() const {
		if (m_body == 0) return 0;
		return *m_body->parent;
	}

	T_* operator->() const {
		if (m_body == 0) return 0;
		return m_body->parent;
	}

	T_* get() const { 
		if (m_body == 0) return 0;
		return m_body->parent;
	}
};

} // namespace

#endif
