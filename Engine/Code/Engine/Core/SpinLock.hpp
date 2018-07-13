#include <mutex>
class SpinLock
{
public:
	void enter() { m_lock.lock(); } // blocking operation
	bool try_enter() { return m_lock.try_lock(); }
	void leave() { m_lock.unlock(); } // must be called if entered

public: 
	std::mutex m_lock; 
};
