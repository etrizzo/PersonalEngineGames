#include "Profiler.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/Clock.hpp"

#include "Game/EngineBuildPreferences.hpp"

// global
Profiler* Profiler::s_profilerInstance;


ProfileLogScoped::ProfileLogScoped(const char * tag)
{
	m_startHPC = GetPerformanceCounter();
	m_tag = tag;
}

ProfileLogScoped::~ProfileLogScoped()
{
	uint64_t diff = GetPerformanceCounter() - m_startHPC;
	float s = (float) PerformanceCountToSeconds(diff);
	ConsolePrintf("%s took: %.4f ms", m_tag, s * 1000.f);
}



profileMeasurement_t::profileMeasurement_t(std::string id)
{
	//for (int i = 0; i < id.size(); i++){
	//	if (i >= 64){
	//		break;
	//	}
	//	m_id[i] = id[i];
	//}
	m_id = id;
	m_startHPC = GetPerformanceCounter();
}

profileMeasurement_t::~profileMeasurement_t()
{
	for (int i = (int) m_children.size()-1; i >=0; i--){
		delete m_children[i];
	}

	if (m_parent != nullptr){
		m_parent->RemoveChild(this);
	}
}

void profileMeasurement_t::AddChild(profileMeasurement_t * child)
{
	m_children.push_back(child);
	child->m_parent = this;
}

void profileMeasurement_t::AddParent(profileMeasurement_t * parent)
{
	if (parent != nullptr){
		m_parent = parent;
		m_parent->AddChild(this);
	} else {
		//remove child from current parent, if it exists
		if (m_parent != nullptr){
			m_parent->RemoveChild(this);
		}
		m_parent = parent;
	}
}

void profileMeasurement_t::RemoveChild(profileMeasurement_t * child)
{
	for (int i = (int) m_children.size()-1; i >=0; i--){
		if (m_children[i] == child){
			if (m_children.size() > 1){
				m_children[i] = m_children[m_children.size() - 1];
			}
			m_children.pop_back();
		}
	}
}

void profileMeasurement_t::Finish()
{
	m_endHPC = GetPerformanceCounter();
}

float profileMeasurement_t::GetMilliseconds() const
{
	return (float) PerformanceCountToSeconds(m_endHPC - m_startHPC) * 1000.f;
}

double profileMeasurement_t::GetSecondsAsDouble() const
{
	return PerformanceCountToSeconds(m_endHPC - m_startHPC);
}

double profileMeasurement_t::GetChildrenTime() const
{
	double childTime = 0.f;
	for (profileMeasurement_t* child : m_children){
		childTime += child->GetSecondsAsDouble();
	}
	return childTime;
}


//==========================================
// PROFILER
//==========================================

#if defined(PROFILING_ENABLED)

Profiler::Profiler()
{
}

Profiler::~Profiler()
{
	delete m_stack;
}


void Profiler::MarkFrame()
{

	if (m_isResuming){
		m_paused = false;
		m_isResuming = false;
		return;
	}
	if (!m_paused){
		
		if (m_stack != nullptr){
			// update previous info
				if (m_frames[m_currentFrameIndex] != nullptr){
					//if there's something in your current index, delete it to make space
					profileMeasurement_t* tree = m_frames[m_currentFrameIndex];
					DestroyMeasurementTreeRecursively(tree);
				}
				m_frames[m_currentFrameIndex] = m_stack;
			
			//m_prevStack = m_stack;
			Pop();
			ASSERT_OR_DIE(m_stack == nullptr, "MarkFrame() was used wrong! You weren't at the root of the measurement tree! SOMEONE FORGOT TO POP!!!");
		}

		if (m_isPausing){
			//if you paused during the last frame, ACTUALLY pause now (after you've cleaned up the last frame)
			m_paused = true;
			m_isPausing = false;
			return;
		} else {
			Push("frame");
			m_currentFrameIndex = (m_currentFrameIndex + 1) % PROFILER_MAX_FRAME_COUNT;
		}
	}
	//profileMeasurement_t* measure = CreateMeasurement("frame");
	//m_stack = measure;
}

void Profiler::Push(const char * name)
{
	if (!m_paused){
		profileMeasurement_t* measure = CreateMeasurement(name);
		if (m_stack == nullptr){
			m_stack = measure;		//lazy instantiation
		} else {
			//stack exists, so update children/parent
			//(roll this into measure->AddParent(m_stack))
			m_stack->AddChild(measure);
	
			m_stack = measure;
		}
	}
}

void Profiler::Pop()
{
	if (!m_paused){
		ASSERT_OR_DIE(m_stack != nullptr, "TOO MANY POPS AND NOT ENOUGH PUSH!! YOU FUCKED UUUPPPPP!!!!!!");
		m_stack->Finish();
		m_stack = m_stack->m_parent;
	}
}

void Profiler::DestroyMeasurementTreeRecursively(profileMeasurement_t* tree)
{
	if (!m_paused){
		delete tree;
	}
}

void Profiler::Pause()
{
	//set is pausing to trigger pause in markframe
	if (!m_paused){
		m_isPausing = true;
	}
}

void Profiler::Resume()
{
	//m_paused = false;
	if (m_paused){
		m_isResuming = true;
	}
}

void Profiler::TogglePause()
{
	if(!m_paused){
		Pause();
	} else {
		Resume();
	}
}

profileMeasurement_t * Profiler::ProfileGetPreviousFrame(unsigned int skip_count)
{
	//unsigned int skipInRange = skip_count % PROFILER_MAX_FRAME_COUNT;
	unsigned int frameIndex = (m_currentFrameIndex + PROFILER_MAX_FRAME_COUNT) - skip_count;		//add max frame count to wrapping around @ 0
	unsigned int actualIndex = frameIndex % PROFILER_MAX_FRAME_COUNT;
	return m_frames[actualIndex];
}

profileMeasurement_t * Profiler::CreateMeasurement(char const * id)
{
	profileMeasurement_t *measure = new profileMeasurement_t(id);
	return measure; 
}

#else
// empty functions for profiling not defined
Profiler::Profiler(){};
Profiler::~Profiler(){};
void Profiler::MarkFrame() {};
void Profiler::Push(const char * name) {};
void Profiler::Pop() {};
void Profiler::Pause(){};
void Profiler::Resume(){};
void Profiler::TogglePause(){};
profileMeasurement_t * Profiler::CreateMeasurement(char const * id) { return nullptr; };
void Profiler::DestroyMeasurementTreeRecursively(profileMeasurement_t* tree) {};
profileMeasurement_t * Profiler::ProfileGetPreviousFrame(unsigned int skip_count) { return nullptr; };
#endif

Profiler * Profiler::GetInstance()
{
	if (Profiler::s_profilerInstance == nullptr){
		s_profilerInstance = new Profiler();
	}
	return s_profilerInstance;
}

ProfilerScoped::ProfilerScoped(const char * tag)
{
	PROFILE_PUSH(tag);
}

ProfilerScoped::~ProfilerScoped()
{
	PROFILE_POP();
}
