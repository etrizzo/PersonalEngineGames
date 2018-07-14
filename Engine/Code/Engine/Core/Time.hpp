//-----------------------------------------------------------------------------------------------
// Time.hpp
//
#pragma once
#include <stdint.h>
#include <string>


struct time_data_t 
{
	float game_delta_time; 
	float game_time; 
	float system_delta_time; 
	float system_time;
};



// Gets raw performance counter
uint64_t GetPerformanceCounter(); 

// converts a performance count the seconds it represents
double PerformanceCountToSeconds( uint64_t hpc ); 

uint64_t SecondsToPerformanceCount(double seconds);

std::string GetCurrentTimestampAsString();


class LocalTimeData 
{
public:
	LocalTimeData();

public:
	uint64_t m_hpc_per_second; 
	double m_seconds_per_hpc; 
}; 


//------------------------------------------------------------------------
// Declaring on the global scope - will 
// cause constructor to be called before 
// our entry function. 
static LocalTimeData gLocalTimeData;