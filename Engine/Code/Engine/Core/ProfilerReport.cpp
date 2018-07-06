#include "Engine/Core/ProfilerReport.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/Renderer.hpp"

ProfilerReportEntry::ProfilerReportEntry(std::string id, ProfilerReportEntry * parent)
{
	m_id = id;
	m_parent = parent;
	m_children = std::vector<ProfilerReportEntry*>();

	m_totalTime = 0.0;
	m_selfTime = 0.0;
	m_percentTime = 0.0;
	m_callCount = 0;
}

void ProfilerReportEntry::PopulateTree(profileMeasurement_t * node)
{
	AccumulateData(node);

	for(profileMeasurement_t* child : node->m_children){
		ProfilerReportEntry* entry = CreateOrGetChild(child->m_id);		//if you're looking at a raycast node and you've already raycasted elsewhere, 	
																		// wanna add to that instead of making a new one
		entry->PopulateTree(child);		//update the child with the info from the node
	}
}

void ProfilerReportEntry::PopulateFlat(profileMeasurement_t * node)
{
	for(profileMeasurement_t* child : node->m_children){
		ProfilerReportEntry* entry = CreateOrGetChild(child->m_id);		//if you're looking at a raycast node and you've already raycasted elsewhere, 	
																		// wanna add to that instead of making a new one
		entry->AccumulateData(child);
		PopulateFlat(child);		//update the ROOT with the info from the CHILD node
	}
}

void ProfilerReportEntry::AccumulateData(profileMeasurement_t * node)
{
	m_callCount++;
	m_totalTime += node->GetSecondsAsDouble();		//accumulation of nodes with that id
	m_selfTime += (node->GetSecondsAsDouble() - node->GetChildrenTime());
	//m_percentTime = ?; //figure it out in post-process of tree
}

ProfilerReportEntry * ProfilerReportEntry::CreateOrGetChild(std::string id)
{
	for (ProfilerReportEntry* existingChild : m_children){
		if (existingChild->m_id == id){
			return existingChild;
		}
	}

	//if we reach this point, no child with that id, so make a new one
	ProfilerReportEntry* child = new ProfilerReportEntry(id, this);
	m_children.push_back(child);
	return child;
}

void ProfilerReportEntry::SetPercentagesForTree(double rootTime)
{
	m_percentTime = m_totalTime / rootTime;
	m_selfPercentTime = m_selfTime / rootTime;
	for( ProfilerReportEntry* child : m_children){
		child->SetPercentagesForTree(rootTime);
	}
}

double ProfilerReportEntry::GetTotalElapsedTime() const
{
	return m_totalTime;
}

double ProfilerReportEntry::GetSelfElapsedTime() const
{
	return m_selfTime;
}

std::string ProfilerReportEntry::GetTotalPercentTime() const
{
	std::string perc = Stringf("%6.2f", (float) m_percentTime * 100.f);
	perc += "%";
	return perc;
}

std::string ProfilerReportEntry::GetSelfPercentTime() const
{
	std::string perc = Stringf("%6.2f", (float) m_selfPercentTime * 100.f);
	perc += "%";
	return perc;
}

std::string ProfilerReportEntry::GetTotalMillisecondsAsString() const
{
	std::string s = Stringf("%10.2fms", GetTotalElapsedTime() * 1000.0);
	return s;
}

std::string ProfilerReportEntry::GetSelfMillisecondsAsString() const
{
	std::string s = Stringf("%10.2fms", GetSelfElapsedTime() * 1000.0);
	return s;
}

void ProfilerReport::GenerateReportTreeFromFrame(profileMeasurement_t * root)
{
		//probs wanna do a depth-first search because that's the order we want to display the nodes
	m_root  = new ProfilerReportEntry(root->m_id);
	m_root->PopulateTree( root );
	m_root->SetPercentagesForTree(root->GetSecondsAsDouble());
}
void ProfilerReport::GenerateReportFlatFromFrame(profileMeasurement_t * root)
{
	m_root  = new ProfilerReportEntry(root->m_id);
	m_root->PopulateFlat( root );
	m_root->SetPercentagesForTree(root->GetSecondsAsDouble());

}
double ProfilerReport::GetTotalFrameTime()
{
	return m_root->GetTotalElapsedTime();		//end - start lol
}
void ProfilerReport::SortBySelfTime()
{
	std::sort(m_root->m_children.begin(), m_root->m_children.end(), CompareProfileEntrysSelfTime);
}
void ProfilerReport::SortByTotalTime()
{
	std::sort(m_root->m_children.begin(), m_root->m_children.end(), CompareProfileEntrysTotalTime);
}
;


bool CompareProfileEntrysSelfTime(ProfilerReportEntry* i, ProfilerReportEntry* j)
{
	if (i->GetSelfElapsedTime() <= j->GetSelfElapsedTime()){
		return false;
	} else {
		return true;
	}
}

bool CompareProfileEntrysTotalTime(ProfilerReportEntry* i, ProfilerReportEntry* j)
{
	if (i->GetTotalElapsedTime() <= j->GetTotalElapsedTime()){
		return false;
	} else {
		return true;
	}
}
