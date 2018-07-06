#pragma once
#include "Engine/Core/Profiler.hpp"
#include <map>

class Renderer;

//a line of the report
class ProfilerReportEntry
{
public:
	ProfilerReportEntry(std::string id, ProfilerReportEntry* parent = nullptr);
	void PopulateTree(profileMeasurement_t* node);
	void PopulateFlat(profileMeasurement_t* node);
	void AccumulateData(profileMeasurement_t* node);
	ProfilerReportEntry* CreateOrGetChild(std::string id);

	void SetPercentagesForTree(double rootTime);

	double GetTotalElapsedTime() const;
	double GetSelfElapsedTime() const;

	std::string GetTotalPercentTime() const;
	std::string GetSelfPercentTime() const;
	std::string GetTotalMillisecondsAsString() const;
	std::string GetSelfMillisecondsAsString() const;
	
	
public:
	std::string m_id;
	unsigned int m_callCount;
	double m_totalTime;		//inclusive time 	- total time that block took (including children)
	double m_selfTime;		//exclusive time 	- how much time the function took itself, and no other functions (EXCLUDING children)
							// get the total time for the node, then subtract all of the childrens' times
	double m_percentTime;
	double m_selfPercentTime;
	//keep more information if you want it. I.e., mean/median/etc

	//keeps track of the hierarchy
	ProfilerReportEntry* m_parent;
	std::vector<ProfilerReportEntry*> m_children;
	//std::map<std::string, ProfilerReportEntry*> m_children;
};


class ProfilerReport
{
public:
	ProfilerReportEntry * m_root;
	void GenerateReportTreeFromFrame(profileMeasurement_t* root);
	void GenerateReportFlatFromFrame(profileMeasurement_t* root);
	double GetTotalFrameTime();

	void SortBySelfTime();
	void SortByTotalTime();
};




bool CompareProfileEntrysSelfTime (ProfilerReportEntry* i,ProfilerReportEntry* j);
bool CompareProfileEntrysTotalTime (ProfilerReportEntry* i,ProfilerReportEntry* j);