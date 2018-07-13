#include "LogSystem.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/DevConsole.hpp"
#include <stdarg.h>
#include <corecrt_io.h>
#include <iosfwd>

LogSystem* LogSystem::s_instance;

void LogSystemStartup()
{
	LogSystem::GetInstance()->m_thread = ThreadCreate(LogThreadWorker);
	
}

void LogSystemShutdown()
{
	LogSystem::GetInstance()->Stop(); 
	ThreadJoin( LogSystem::GetInstance()->m_thread );


	LogSystem::GetInstance()->m_thread = nullptr; 
}

void LogThreadWorker(void *)
{
	while (LogSystem::GetInstance()->IsRunning()) {
		LogSystem::GetInstance()->Flush(); 
		ThreadSleep(10);  
	}

	// do the inner side of the loop again right before exiting
	LogSystem::GetInstance()->Flush(); 
}

void LogTaggedPrintv(char const * tag, char const * format, va_list args)
{
	log_t *log = new log_t(); 
	log->tag = tag; 
	log->text = Stringf( format, args );

	LogSystem::GetInstance()->m_logQueue.enqueue(log); 
}

void LogTaggedPrintf(char const * tag, char const * format, ...)
{
	va_list args;
	va_start( args, format );
	LogTaggedPrintv(tag, format, args);
	va_end( args );
}

void LogThreadTest()
{
	FILE* file = nullptr;
	fopen_s(&file, "Data/little.txt", "r");

	std::string str;
	char lineStr[1000];
	while (fgets( lineStr, 1000, file) != NULL) {
		LogTaggedPrintf("test", lineStr );
	}

	fclose(file);
	ConsolePrintf("Big.txt saved to system log");
}

void LogTest(unsigned int thread_count)
{
	// spin up thread_count threads.
	// each thread will open the src file (read-only mode so as not to have conflicts)
	// and read line by line, and log as follows
	// LogPrintf( "[%u:%u] %s", thread_no, line_no, line_text );
	for (unsigned int i = 0; i < thread_count; i++){
		ThreadCreateAndDetach((thread_cb) LogThreadTest);
	}
}

LogSystem::LogSystem()
{
	m_file = new std::fstream();
	char errorString[512];
	errno = 0;
	//_chmod("Logs/system_log.txt", _S_IREAD | _S_IWRITE);
	//fopen_s(&m_file, "Logs/system_log.txt", "w+");
	m_file->open("Logs/system_log.txt", std::fstream::in | std::fstream::out | std::fstream::app);
	if (m_file == nullptr){	 
		ConsolePrintf("Failed to open log system file. error %d\n", errno);
		perror(errorString);
		ConsolePrintf("%s\n", errorString);
	} 


}

bool LogSystem::IsRunning() const
{
	return m_isRunning;
}

void LogSystem::Stop()
{
	m_isRunning = false;
	//fclose(m_file);
	m_file->close();
}

void LogSystem::WriteLogToFile(log_t * data)
{
	std::string logText = data->tag + ":: " + data->text;
	//fputs( logText.c_str(), m_file); 
	m_file->write(logText.c_str(), logText.size());
}

void LogSystem::Flush()
{
	log_t *data; 
	while (m_logQueue.dequeue(&data)) {
		// do something with the data
		WriteLogToFile( data ); 
		delete data; 
	}
}

LogSystem * LogSystem::GetInstance()
{
	if (s_instance == nullptr){
		s_instance = new LogSystem();
	}
	return s_instance;
}
