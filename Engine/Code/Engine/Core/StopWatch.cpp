#pragma once
#include "Engine/Core/StopWatch.hpp"
#include "Engine/Core/Time.hpp"

StopWatch::StopWatch(Clock * ref_clock)
{
	SetClock(ref_clock);
}

void StopWatch::SetClock(Clock * ref_clock)
{
	if (ref_clock == nullptr){
		m_reference = GetMasterClock();
	} else {
		m_reference = ref_clock;
	}
	m_startHPC = m_reference->GetFrameHPC();
}

bool StopWatch::SetTimer(float seconds)
{
	if (seconds == 0.f){
		return false;
	}
	m_intervalHPC = SecondsToPerformanceCount((double) seconds);
	return true;
}

float StopWatch::GetElapsedTime()
{
	uint64_t elapsed = m_reference->GetFrameHPC() - m_startHPC;
	return (float) PerformanceCountToSeconds(elapsed);
}

float StopWatch::GetNormalizedElapsedTime()
{
	float elapsed = GetElapsedTime();
	return (elapsed / (float) (PerformanceCountToSeconds(m_intervalHPC)));
}

bool StopWatch::HasElapsed()
{
	uint64_t elapsed = m_reference->GetFrameHPC() - m_startHPC;
	return (elapsed > m_intervalHPC);
}

void StopWatch::Reset()
{
	m_startHPC = m_reference->GetFrameHPC();
}

bool StopWatch::CheckAndReset()
{
	bool elapsed = HasElapsed();
	if (elapsed){
		Reset();
	}
	return elapsed;
}

bool StopWatch::Decrement()
{
	bool elapsed = HasElapsed();
	if (elapsed){
		m_startHPC+= m_intervalHPC;
	}
	return elapsed;
}

unsigned int StopWatch::DecrementAll()
{
	unsigned int numElapsed = 0;
	while (Decrement()){
		numElapsed++;
	}
	return numElapsed;
}
