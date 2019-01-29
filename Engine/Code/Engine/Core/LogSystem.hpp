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

typedef void (*log_cb)( log_t const &log, void* user_args ); 

struct loghook_t
{
	loghook_t(log_cb cb, void* args);
	log_cb callback; 
	void *user_argument; 
};



/************************************************************************/

class LogSystem{
public:
	LogSystem();

	bool IsRunning() const;
	void Stop();
	
	
	void FlushMessages();
	void Flush();

	// FILTERING
	void LogShowAll(); 
	void LogHideAll(); 
	void LogShowTag( char const *tag ); 
	void LogHideTag( char const *tag ); 
	void ToggleWhitelist();
	bool CanWriteTag(std::string tag);

	//HOOKS
	void LogHook( log_cb cb, void *user_arg ) ;

	Thread* m_thread;
	bool m_isRunning = true;
	ThreadSafeQueue<log_t*> m_logQueue;
	std::fstream* m_file;
	std::fstream* m_timeStampFile;
	bool m_isFlushing = true;

	ThreadSafeVector<loghook_t*> m_hooks;
	ThreadSafeVector<std::string> m_filters;	// = ThreadSafeVector<std::string>();  // default empty
	bool m_filtersAreWhitelist = false;          // default false

	static LogSystem* GetInstance();
	static LogSystem* s_instance;
};

void LogSystemStartup();
void LogSystemShutdown();
void LogSystemFlush();

void LogThreadWorker( void* ) ;

// Logging Call (Master call - most functions will feed through this)
// Notice we take a va_list, so we can forward other functions that
// take variable arguments to this; 
void LogTaggedPrintv( char const *tag, 
	char const *format, va_list args ); 

// Full
void LogTaggedPrintf( char const *tag, 
	char const *format, ... );

// HELPERS
// Default ("log")
void LogPrintf( char const *format, ... ); 

// Wranings ("warning");
void LogWarningf( char const *format, ... ); 

// Errors ("error"), Flush and Assert at the end
// if we add callstack support - attaching a callstack is 
// really useful here.
void LogErrorf( char const *format, ... ); 

void LogThreadTest(const char* fileName);
void LogTest( unsigned int thread_count = 1 , std::string fileName = "Data/big.txt") ;

void LogShowTag(std::string tagName);
void LogHideTag(std::string tagName);
void LogShowAllTags();
void LogHideAllTags();
void LogToggleWhitelist();


void WriteLogToFile(log_t *data, void* user_args );
void WriteLogToConsole(log_t *data, void* user_args );
void WriteLogToOutput(log_t * data, void* user_args );

