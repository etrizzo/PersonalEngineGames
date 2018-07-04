#pragma once
#include "..\Renderer\external\gl\glcorearb.h"
#include <vector>


//------------------------------------------------------------------------
//------------------------------------------------------------------------
struct time_unit_t 
{
	uint64_t hpc; 
	double hp_seconds;  // high precision seconds

	float seconds;      // convenience float seconds

	void Reset();
};

//------------------------------------------------------------------------
//------------------------------------------------------------------------
class Clock 
{
public:
	Clock( Clock *parent = nullptr );
	~Clock(); // make sure to cleanup the hierarchy

			  // resets the clock - everything should be zeros out
			  // and the reference hpc variables set 
			  // to the current hpc.
	void Reset(); 

	// used on a root clock
	// calculates elapsed time since last call
	// and calls advance.
	void BeginFrame(); 

	// advance the clock by the given hpc
	void Advance( uint64_t const hpc ); 

	// adds a clock as a child to this clock
	void AddChild( Clock *child ); 

	// returns time since this clock 
	// using the current hpc, and our start hpc
	// to get the most up-to-date time.
	double GetCurrentTime() const; 

	// manipulation
	void SetPaused( bool paused ); 
	void SetScale( float scale ); 

	void TogglePause();

	// add whatever other convenience methods you may want; 
	// ...

	float GetCurrentSeconds() const;
	float GetDeltaSeconds() const;
	uint64_t GetCurrentHPC() const;

	uint64_t GetFrameHPC() const { return m_last_frame_hpc; };

public:
	// I don't use "m_" here as 
	// the intended usage is to be able to go 
	// clock->total.seconds -or- clock->frame.seconds; 
	// these are only updated at begin_frame.
	time_unit_t frame; 
	time_unit_t total; 

	static Clock* m_masterClock;

private:
	// local data we need to track for the clock to work; 
	uint64_t m_start_hpc;      // hpc when the clock was last reset (made)
	uint64_t m_last_frame_hpc; // hpc during last begin_frame call

	double m_time_scale; 
	unsigned int m_frame_count;        // if you're interested in number of frames your game has processes
	bool m_paused;          

	// For the hierarchy 
	Clock *m_parent;
	std::vector<Clock*> m_children; 

	
}; 


//------------------------------------------------------------------------
// C Functions
//------------------------------------------------------------------------
// could calso be static Clock* Clock::GetMaster(); 
Clock* GetMasterClock(); 

// convenience - calls begin frame on the master clock
void ClockSystemBeginFrame();

// I now move this here - as this now refers to the master clock
// who is keeping track of the starting reference point. 
double GetCurrentTimeSeconds(); 

// I usually also add convenience functions so I'm not constantly fetching the master clock, like
// float GetDeltaTime()
// float GetFrameStartTime();
