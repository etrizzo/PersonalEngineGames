#include "LogSystem.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Game/EngineBuildPreferences.hpp"
#include <stdarg.h>
#include <corecrt_io.h>
#include <iosfwd>

LogSystem* LogSystem::s_instance;

#if defined LOGGING_ENABLED
void LogSystemStartup()
{
	LogSystem::GetInstance()->m_thread = ThreadCreate(LogThreadWorker);
	LogSystem::GetInstance()->LogHook((log_cb) WriteLogToFile, nullptr);
	LogSystem::GetInstance()->LogHook((log_cb) WriteLogToConsole, nullptr);
	LogSystem::GetInstance()->LogHook((log_cb) WriteLogToOutput, nullptr);
	
}

void LogSystemShutdown()
{
	LogSystem::GetInstance()->Stop(); 
	ThreadJoin( LogSystem::GetInstance()->m_thread );


	LogSystem::GetInstance()->m_thread = nullptr; 
}
#else
void LogSystemStartup() {};
void LogSystemShutdown() {};
#endif


void LogSystemFlush()
{
	LogSystem::GetInstance()->Flush();
}

void LogThreadWorker(void *)
{
	while (LogSystem::GetInstance()->IsRunning()) {
		LogSystem::GetInstance()->FlushMessages(); 
		ThreadSleep(10);  
	}

	// do the inner side of the loop again right before exiting
	LogSystem::GetInstance()->FlushMessages(); 
}

void LogTaggedPrintv(char const * tag, char const * format, va_list args)
{
	log_t *log = new log_t(); 
	log->tag = tag; 
	log->text = Stringv( format, args );

	LogSystem::GetInstance()->m_logQueue.enqueue(log); 
}

void LogTaggedPrintf(char const * tag, char const * format, ...)
{
	va_list args;
	va_start( args, format );
	LogTaggedPrintv(tag, format, args);
	va_end( args );
}

void LogPrintf(char const * format, ...)
{
	va_list args;
	va_start( args, format );
	LogTaggedPrintv("Log", format, args);
	va_end( args );
}

void LogWarningf(char const * format, ...)
{
	va_list args;
	va_start( args, format );
	LogTaggedPrintv("Warning", format, args);
	va_end( args );
}

void LogErrorf(char const * format, ...)
{
	va_list args;
	va_start( args, format );
	LogTaggedPrintv("Error", format, args);
	va_end( args );
}

void LogThreadTest(const char* fileName)
{
	std::fstream file;
	file.open(fileName, std::fstream::in | std::fstream::app);
	std::string line;
	while ( std::getline(file,line )){
		LogTaggedPrintf("test", line.c_str());
	}
	ConsolePrintf("File saved to system log");
}

void LogTest(unsigned int thread_count, std::string fileName)
{
	// spin up thread_count threads.
	// each thread will open the src file (read-only mode so as not to have conflicts)
	// and read line by line, and log as follows
	// LogPrintf( "[%u:%u] %s", thread_no, line_no, line_text );
	for (unsigned int i = 0; i < thread_count; i++){
		ThreadCreateAndDetach((thread_cb) LogThreadTest, (void*) fileName.c_str());
	}
}

void LogShowTag(std::string tagName)
{
	LogSystem::GetInstance()->LogShowTag(tagName.c_str());
}

void LogHideTag(std::string tagName)
{
	LogSystem::GetInstance()->LogHideTag(tagName.c_str());
}

void LogShowAllTags()
{
	LogSystem::GetInstance()->LogShowAll();
}

void LogHideAllTags()
{
	LogSystem::GetInstance()->LogHideAll();
}

void LogToggleWhitelist()
{
	LogSystem::GetInstance()->ToggleWhitelist();
}

LogSystem::LogSystem()
{
	m_file = new std::fstream();
	m_timeStampFile = new std::fstream();
	char errorString[512];
	errno = 0;
	//_chmod("Logs/system_log.txt", _S_IREAD | _S_IWRITE);
	//fopen_s(&m_file, "Logs/system_log.txt", "w+");
	std::string dateTime = GetCurrentTimestampAsString();
	std::string dateLog = "Logs/system_log_" + dateTime + ".txt";
	m_timeStampFile->open(dateLog,  std::fstream::out | std::fstream::trunc );
	m_file->open("Logs/system_log.txt",  std::fstream::out | std::fstream::trunc);
	if (!m_file->is_open()){	 
		ConsolePrintf("Failed to open log system file. error %d\n", errno);
		perror(errorString);
		ConsolePrintf("%s\n", errorString);
	} 
	if (!m_timeStampFile->is_open()){	 
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
	m_timeStampFile->close();
}



void LogSystem::FlushMessages()
{
	log_t *data; 
	while (m_logQueue.dequeue(&data)) {
		if (CanWriteTag(data->tag)){
			// do something with the data
			//WriteLogToFile( data ); 
			m_hooks.lock();
			for (int i = 0; i < (int) m_hooks.size(); i++) {
				loghook_t* hook = m_hooks.at(i);
				hook->callback( *data, hook->user_argument ); 
			}
			m_hooks.leave();
		}
		delete data; 
	}
	m_isFlushing = false;
}

void LogSystem::Flush()
{
	m_isFlushing = true;
	//FlushMessages();
	while (m_isFlushing) {

	};
	m_file->flush();
	m_timeStampFile->flush();
}

void LogSystem::LogShowAll()
{
	m_filters.clear();
}

void LogSystem::LogHideAll()
{
	m_filtersAreWhitelist = true;
	m_filters.clear();
}

void LogSystem::LogShowTag(char const * tag)
{
	if (m_filtersAreWhitelist){
		m_filters.push_back(std::string(tag));
	} else {
		m_filters.remove(std::string(tag));
	}
}

void LogSystem::LogHideTag(char const * tag)
{
	if (!m_filtersAreWhitelist){
		m_filters.push_back(std::string(tag));
	} else {
		m_filters.remove(std::string(tag));
	}
}

void LogSystem::ToggleWhitelist()
{
	m_filtersAreWhitelist = !m_filtersAreWhitelist;
}

bool LogSystem::CanWriteTag(std::string tag)
{
	bool containsTag = m_filters.contains(tag);
	if (m_filtersAreWhitelist){
		return containsTag;
	} else {
		return !containsTag;
	}
	return false;
}

void LogSystem::LogHook(log_cb cb, void * user_arg)
{
	m_hooks.push_back( new loghook_t(cb, user_arg) ); 
}

LogSystem * LogSystem::GetInstance()
{
	if (s_instance == nullptr){
		s_instance = new LogSystem();
	}
	return s_instance;
}

loghook_t::loghook_t(log_cb cb, void * args)
{
	callback = cb;
	user_argument = args;
}


void WriteLogToFile(log_t * data,  void* user_args )
{
	UNUSED(user_args);
	TODO("what do with user args?");
	std::string logText = data->tag + ":: " + data->text + "\n";
	//fputs( logText.c_str(), m_file); 
	LogSystem::GetInstance()->m_file->write(logText.c_str(), logText.size());
	LogSystem::GetInstance()->m_timeStampFile->write(logText.c_str(), logText.size());
	
}

void WriteLogToConsole(log_t * data, void * user_args)
{
	UNUSED(user_args);
	std::string logText = data->tag + ":: " + data->text + "\n";
	ConsolePrintf(logText.c_str());
}

void WriteLogToOutput(log_t * data, void * user_args)
{
	UNUSED(user_args);
	std::string logText = data->tag + ":: " + data->text + "\n";
	OutputDebugStringA( logText.c_str() );
}
