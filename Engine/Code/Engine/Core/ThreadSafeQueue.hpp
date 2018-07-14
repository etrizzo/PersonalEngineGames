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


template <typename T>
class ThreadSafeVector
{
public:
	ThreadSafeVector<T>()
	{
		m_data = std::vector<T>();
	}

	void push_back( T const &v )
	{
		m_lock.enter();

		// I AM THE ONLY PERSON HERE
		m_data.push_back( v ); 

		m_lock.leave();
		// no longer true...
	}

	void erase(int index)
	{
		m_lock.enter();

		m_data.erase(m_data.begin() + index);

		m_data.leave();
	}

	void remove(T const& v)
	{
		m_lock.enter();

		for (int i = m_data.size() - 1; i >=0 ; i++ ){
			if (m_data[i] == v){
				if (m_data.size() > 1){
					m_data[i] = m_data[m_data.size() - 1];
				}
				m_data.pop_back();
				break;
			}
		}

		m_lock.leave();
	}

	void lock()
	{
		m_lock.enter();
	}

	void leave()
	{
		m_lock.leave();
	}

	T at(int index)
	{
		//lock();
		T data = m_data[index];
		//leave();
		return data;
	}

	unsigned int size()
	{
		return m_data.size();
	}

	void clear()
	{
		m_lock.enter();
		m_data.clear();
		m_lock.leave();
	}

	bool contains(T const& v)
	{
		m_lock.enter();

		for (int i = m_data.size() - 1; i >=0 ; i-- ){
			if (m_data[i] == v){
				m_lock.leave();
				return true;
			}
		}
		
		m_lock.leave();
		return false;
	}

private:
	std::vector<T> m_data; 
	SpinLock m_lock; 
};