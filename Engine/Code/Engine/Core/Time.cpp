//-----------------------------------------------------------------------------------------------
// Time.cpp
//	

//-----------------------------------------------------------------------------------------------
#include "Engine/Core/Time.hpp"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>






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

LocalTimeData::LocalTimeData()
{
	::QueryPerformanceFrequency( (LARGE_INTEGER*) &m_hpc_per_second ); 

	// do the divide now, to not pay the cost later
	m_seconds_per_hpc = 1.0 / (double)m_hpc_per_second; 
}
