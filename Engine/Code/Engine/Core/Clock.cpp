#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Time.hpp"

Clock* Clock::m_masterClock = nullptr;

Clock::Clock(Clock * parent)
{
	m_parent =  parent;
	m_time_scale = 1.f;
	m_paused = false;
	Reset();
	if (m_parent != nullptr){
		m_parent->AddChild(this);
	}
}

Clock::~Clock()
{
	for (Clock* child : m_children){
		delete(child);
	}
}

//------------------------------------------------------------------------
// resets the clock (called in constructor after
// rest of setup  happens)
void Clock::Reset() 
{
	m_start_hpc = GetPerformanceCounter();
	m_last_frame_hpc = m_start_hpc; 

	frame.Reset(); 
	total.Reset(); 

	// no need to reset children (why?)
}

//------------------------------------------------------------------------
// only gets called on clocks that have no parent (are root clocks)
void Clock::BeginFrame()
{
	if( m_parent == nullptr ){
		uint64_t hpc = GetPerformanceCounter(); 
		uint64_t delta  = hpc - m_last_frame_hpc; 
		Advance( delta ); 

	}
}


//------------------------------------------------------------------------
// advance the clock
void Clock::Advance( uint64_t delta ) 
{
	// Step 0: Update frame count
	// I personally increment frame_count whether it is paused or notas I 
	// use it mostly for reporting, but a good argument could also be made
	// for it respecting paused (for example: using it to make sure something 
	// only happens once a frame, and you treat being paused as staying on the 
	// same frame).  
	++m_frame_count; 

	// Step 1:  Scale/Modify the delta based on internal state
	// ... 
	delta=(uint64_t) (delta * m_time_scale);
	if (m_paused){
		delta = 0;
	}

	// Step 2: Update frame and total variables.  
	// ...
	frame.hpc+=delta;
	frame.hp_seconds = PerformanceCountToSeconds(delta);
	frame.seconds = (float) frame.hp_seconds;

	total.hpc+=delta;
	total.hp_seconds += PerformanceCountToSeconds(delta);
	total.seconds = (float) total.hp_seconds;

	m_last_frame_hpc = frame.hpc;

	// Step 3: Call advance on all children 
	// ...
	for (Clock* child : m_children){
		child->Advance(delta);
	}
}

void Clock::AddChild(Clock * child)
{
	m_children.push_back(child);
}

double Clock::GetCurrentTime() const
{
	return (double) (total.hpc - m_start_hpc);
}

void Clock::SetPaused(bool paused)
{
	m_paused = paused;
}

void Clock::SetScale(float scale)
{
	m_time_scale = scale;
}

void Clock::TogglePause()
{
	if (m_paused){
		SetPaused(false);
	} else {
		SetPaused(true);
	}
}

float Clock::GetCurrentSeconds() const
{
	return (float) PerformanceCountToSeconds(total.hpc - m_start_hpc);
}
float Clock::GetDeltaSeconds() const
{
	return frame.seconds;
}

uint64_t Clock::GetCurrentHPC() const
{
	return total.hpc - m_start_hpc;
}

void time_unit_t::Reset()
{
	hpc = GetPerformanceCounter(); 
	hp_seconds = 0.0;  // high precision seconds
	seconds = 0.f;      
}

//-----------------------------------------------------------------------------------------------
double InitializeTime( LARGE_INTEGER& out_initialTime )
{
	LARGE_INTEGER countsPerSecond;
	QueryPerformanceFrequency( &countsPerSecond );
	QueryPerformanceCounter( &out_initialTime );
	return( 1.0 / static_cast< double >( countsPerSecond.QuadPart ) );
}


Clock * GetMasterClock()
{
	if (Clock::m_masterClock == nullptr){
		Clock::m_masterClock = new Clock();
	}

	return Clock::m_masterClock;
}

void ClockSystemBeginFrame()
{
	GetMasterClock()->BeginFrame();
}

//-----------------------------------------------------------------------------------------------
double GetCurrentTimeSeconds()
{
	static LARGE_INTEGER initialTime;
	static double secondsPerCount = InitializeTime( initialTime );
	LARGE_INTEGER currentCount;
	QueryPerformanceCounter( &currentCount );
	LONGLONG elapsedCountsSinceInitialTime = currentCount.QuadPart - initialTime.QuadPart;

	double currentSeconds = static_cast< double >( elapsedCountsSinceInitialTime ) * secondsPerCount;
	return currentSeconds;
}