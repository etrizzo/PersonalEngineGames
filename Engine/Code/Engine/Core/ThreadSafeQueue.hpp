#pragma once
#include "Engine/Core/SpinLock.hpp"
#include <queue>
template <typename T>
class ThreadSafeQueue
{
public:
	void enqueue( T const &v )
	{
		m_lock.enter();

		// I AM THE ONLY PERSON HERE
		m_data.push( v ); 

		m_lock.leave();
		// no longer true...
	}


	// return if it succeeds
	bool dequeue( T *out_v ) 
	{
		m_lock.enter();

		bool has_item = !m_data.empty();
		if (has_item) {
			*out_v = m_data.front();
			m_data.pop(); 
		}

		m_lock.leave();
		return has_item; 
	}

public:
	std::queue<T> m_data; 
	SpinLock m_lock; 
};