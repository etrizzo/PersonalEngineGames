#pragma once
#include "Engine/Core/Thread.hpp"
#include "Engine/Core/ThreadSafeQueue.hpp"
#include <fstream>  

/************************************************************************/
/* TYPES                                                                */
/************************************************************************/
struct log_t
{
	std::string tag; 
	std::string text; 
};

typedef void (*log_cb)( log_t const &log ); 

struct loghook_t
{
	log_cb callback; 
	void *user_argument; 
};



/************************************************************************/

class LogSystem{
public:
	LogSystem();

	bool IsRunning() const;
	void Stop();
	
	void WriteLogToFile(log_t *data);
	void Flush();

	Thread* m_thread;
	bool m_isRunning = true;
	ThreadSafeQueue<log_t*> m_logQueue;
	std::fstream* m_file;
	//FILE* m_file;

	static LogSystem* GetInstance();
	static LogSystem* s_instance;
};

void LogSystemStartup();
void LogSystemShutdown();

void LogThreadWorker( void* ) ;

// Logging Call (Master call - most functions will feed through this)
// Notice we take a va_list, so we can forward other functions that
// take variable arguments to this; 
void LogTaggedPrintv( char const *tag, 
	char const *format, va_list args ); 

// Full
void LogTaggedPrintf( char const *tag, 
	char const *format, ... );



void LogThreadTest();
void LogTest( unsigned int thread_count = 1 ) ;