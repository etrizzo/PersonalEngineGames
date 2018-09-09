#include "Thread.hpp"


Thread* ThreadCreate(thread_cb cb, void * user_data)
{
	Thread* t = new Thread(cb, user_data);
	return t;
//	thread_handle_t id = (thread_handle_t)  (t->get_id());
	/*thread_handle_t handle = (thread_handle_t)  (t->native_handle());
	s_threads.push_back(t);
	return handle;*/
}

void ThreadJoin(Thread* th)
{
	if (th != nullptr){
		th->join();
	}
	//Thread* thread = GetThreadByID(th);
	//if (thread != nullptr){
	//	thread->join();
	//}
}

void ThreadDetach(Thread* th)
{
	th->detach();
	//Thread* thread = GetThreadByID(th);
	//if (thread != nullptr){
	//	thread->detach();
	//}
}

//Thread* GetThreadByID(Thread th)
//{
//	for(Thread* thread : s_threads){
//		if (thread->native_handle() == th){
//			return thread;
//		}
//	}
//	return nullptr;
//}

void ThreadCreateAndDetach(thread_cb cb, void * user_data)
{
	ThreadDetach( ThreadCreate(cb, user_data) );
}

void ThreadSleep(unsigned int ms)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

void ThreadYield()
{
	std::this_thread::yield();
}

void ThreadSetName(char const * name)
{
	name;
	//name = x;
	//this is some hacky bullshit to get visual studio to name the thread

	//if (nullptr == name) {
	//	return;
	//}

	//thread_id_t id = ThreadGetCurrentID();	//in std::thread, handle is NOT the ID
	//if (0 != id) {
	//	THREADNAME_INFO info;
	//	info.type = 0x1000;
	//	info.name = name;
	//	info.thread_id =  (DWORD)(uintptr_t)id;
	//	info.flags = 0;

	//	__try 
	//	{
	//		RaiseException( MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)(&info) );
	//	} 
	//	__except (EXCEPTION_CONTINUE_EXECUTION) 
	//	{
	//	}
	//}
}


