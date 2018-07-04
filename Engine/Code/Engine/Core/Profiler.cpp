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
	float s = PerformanceCountToSeconds(diff);
	ConsolePrintf("%s took: %.4f ms", m_tag, s * 1000.f);
}



profileMeasurement_t::profileMeasurement_t(std::string id)
{
	for (int i = 0; i < id.size(); i++){
		if (i >= 64){
			break;
		}
		m_id[i] = id[i];
	}
	m_startHPC = GetPerformanceCounter();
}

profileMeasurement_t::~profileMeasurement_t()
{
	for (int i = m_children.size()-1; i >=0; i--){
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
	for (int i = m_children.size()-1; i >=0; i--){
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
	return PerformanceCountToSeconds(m_endHPC - m_startHPC) * 1000.f;
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
	if (m_stack != nullptr){
		// update previous info
		if (m_prevStack != nullptr){
			DestroyMeasurementTreeRecursively(m_prevStack);
		}

		m_prevStack = m_stack;
		Pop();
		ASSERT_OR_DIE(m_stack == nullptr, "MarkFrame() was used wrong! You weren't at the root of the measurement tree! SOMEONE FORGOT TO POP!!!");
	} 

	Push("frame");
	//profileMeasurement_t* measure = CreateMeasurement("frame");
	//m_stack = measure;
}

void Profiler::Push(const char * name)
{
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

void Profiler::Pop()
{
	ASSERT_OR_DIE(m_stack != nullptr, "TOO MANY POPS AND NOT ENOUGH PUSH!! YOU FUCKED UUUPPPPP!!!!!!");
	m_stack->Finish();
	m_stack = m_stack->m_parent;
}

void Profiler::DestroyMeasurementTreeRecursively(profileMeasurement_t* tree)
{
	delete tree;
}

profileMeasurement_t * Profiler::ProfileGetPreviousFrame(unsigned int skip_count)
{
	return m_prevStack;
}

profileMeasurement_t * Profiler::CreateMeasurement(char const * id)
{
	profileMeasurement_t *measure = new profileMeasurement_t(id);
	// fill id and start time
	// ...
	return measure; 
}
Profiler * Profiler::GetInstance()
{
	if (Profiler::s_profilerInstance == nullptr){
		s_profilerInstance = new Profiler();
	}
	return s_profilerInstance;
}
#else
// empty functions for profiling not defined
void Profiler::MarkFrame() {};
void Profiler::Push(const char * name) {};
void Profiler::Pop() {};
profileMeasurement_t * Profiler::CreateMeasurement(char const * id) { return nullptr; };
void Profiler::DestroyMeasurementTreeRecursively(profileMeasurement_t* tree) {};

#endif