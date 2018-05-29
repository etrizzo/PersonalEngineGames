#pragma once
#include "Engine/Core/Clock.hpp"

//------------------------------------------------------------------------
// StopWatch - meant for game systems
// as it will use frame-start time for calculating
//------------------------------------------------------------------------
class StopWatch
{
public:
	// should default to Master if ref_clock is set to nullptr
	StopWatch( Clock *ref_clock = nullptr ); 

	// change our reference clock. 
	// should maintain elapsed time 
	void SetClock( Clock *ref_clock ); 

	// sets the "interval time" for
	// this stopwtach.
	// takes seconds to be easy to use
	// but under the hood it will use hpc
	bool SetTimer( float seconds ); 

	// if we really just want to use it as a timer
	// this returns how long since the last reset
	float GetElapsedTime();

	// elapsed time divided by the interval
	float GetNormalizedElapsedTime(); 

	// returns true if the interval has elapsed;
	bool HasElapsed(); 

	// resets the interval
	// (get_elapsed_time() should return 0.0f);
	void Reset(); 

	// checks the interval, if it has elapsed, will
	// return true and reset.  If not, will just return false (no reset)
	bool CheckAndReset(); 

	// returns has_elapsed(), and if so
	// will subtract one interval from 
	// the elapsed time (but maintaining 
	// any extra time we have accrued)
	bool Decrement(); 

	// like decrement, but returns how many 
	// times we have elapsed our timer
	// so say our timer was 0.5 seconds, 
	// and get_elapsed_time() was 2.2s.  
	// decrement_all() would return 4, and 
	// update itself so that get_elapsed_time()
	// would return 0.2s;
	unsigned int DecrementAll(); 

	// pause and resume [extra/optional]
	// void pause(); 
	// void resume()


public:
	// clock to use as reference
	Clock *m_reference; 

	// reference times; 
	// notice using these member
	// variables, our stop-watch doesn't need
	// an "update", like it would if we
	// were accumulating deltas
	uint64_t m_startHPC; 
	uint64_t m_intervalHPC; 
};

//------------------------------------------------------------------------
// RealStopWatch
// optional class that uses current time instead of beginning of frame time.
// More suitable to engine systems that run outside the game loop.
// (is not based off a clock, uses GetPerformanceCount directly)
//------------------------------------------------------------------------