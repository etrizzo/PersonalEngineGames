//-----------------------------------------------------------------------------------------------
// Time.cpp
//	

//-----------------------------------------------------------------------------------------------
#include "Engine/Core/Time.hpp"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <time.h>
#include "StringUtils.hpp"






uint64_t GetPerformanceCounter()
{
	uint64_t hpc;
	::QueryPerformanceCounter( (LARGE_INTEGER*)&hpc ); 
	return hpc; 
}

double PerformanceCountToSeconds(uint64_t hpc)
{
	return (double)hpc * gLocalTimeData.m_seconds_per_hpc; 
}

uint64_t SecondsToPerformanceCount(double seconds)
{
	return (uint64_t)( seconds * gLocalTimeData.m_hpc_per_second); 
}

std::string GetCurrentTimestampAsString()
{
	time_t t = time(0);
	struct tm now;
	localtime_s(&now, &t);
	//std::string timeString = Stringf("%d_%d_%d.%d::%d::%d", now.tm_mon, now.tm_mday, now.tm_year, now.tm_hour, now.tm_min, now.tm_sec );
	std::string timeString = Stringf("%d_%d_%d_%d_%d_%d", now.tm_mon, now.tm_mday, now.tm_year + 1900, now.tm_hour, now.tm_min, now.tm_sec );

	return timeString;
    //std::cout << (now->tm_year + 1900) << '-' 
    //     << (now->tm_mon + 1) << '-'
    //     <<  now->tm_mday
    //     << "\n";
}

LocalTimeData::LocalTimeData()
{
	::QueryPerformanceFrequency( (LARGE_INTEGER*) &m_hpc_per_second ); 

	// do the divide now, to not pay the cost later
	m_seconds_per_hpc = 1.0 / (double)m_hpc_per_second; 
}
