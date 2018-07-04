#pragma once
#include "Engine/Core/Time.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include <vector>

#define PROFILE_LOG_SCOPE(tag) ProfileLogScoped __timer__ ##__LINE__ ## (tag)
#define PROFILE_LOG_SCOPE_FUNCTION()  ProfileLogScoped __timer__ ##__LINE__ ## (__FUNCTION__)

class ProfileLogScoped
{
public:
	ProfileLogScoped( const char* tag ); 
	~ProfileLogScoped(); 

public:
	uint64_t m_startHPC; 
	char const * m_tag;        
};



struct profileMeasurement_t
{
public:
	profileMeasurement_t(){};
	profileMeasurement_t(std::string id);
	~profileMeasurement_t();

	char m_id[64];
	uint64_t m_startHPC;
	uint64_t m_endHPC;

	profileMeasurement_t* m_parent = nullptr;
	std::vector<profileMeasurement_t*> m_children;		//it's a bad idea to have this be a vector - we'll discuss later

	void AddChild(profileMeasurement_t* child);
	void AddParent(profileMeasurement_t* parent);
	void RemoveChild(profileMeasurement_t* child);
	void Finish();
	float GetMilliseconds() const;
	
};


class Profiler
{
public:
	Profiler();
	~Profiler();
	//need last frames info
	//need to keep track of the top of a stack
	//std::stack<profileMeasurement_t*> m_stack;		//don't actually need the whole stack because profileMeasurement_t has children and parent
	profileMeasurement_t* m_stack = nullptr;			//just updating this and hooking in the parent/children. This is the current node
	profileMeasurement_t* m_prevStack = nullptr;		//this is if you only want 2 frames of history - we will have at least 128 so think on that one

	void MarkFrame();
	void Push(const char* name);
	void Pop();
	void DestroyMeasurementTreeRecursively(profileMeasurement_t* tree);
	profileMeasurement_t* ProfileGetPreviousFrame( unsigned int skip_count = 0 ); 

	profileMeasurement_t* CreateMeasurement(char const* id);

	static Profiler* GetInstance();


	static Profiler* s_profilerInstance;
};
