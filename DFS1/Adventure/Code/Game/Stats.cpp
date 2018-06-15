#include "Stats.hpp"

Stats::Stats()
{
	for (int i = 0; i < NUM_STAT_IDS; i++){
		m_statValues[i] = 0;
	}
}

Stats::Stats(Stats minStats, Stats maxStats)
{
	SetStatsFromRanges(minStats, maxStats);
}

Stats::Stats(IntRange allStatRange)
{
	for (int statNum = 0; statNum < NUM_STAT_IDS; statNum++){
		m_statValues[statNum] = allStatRange.GetRandomInRange();
	}
}

Stats::Stats(int val)
{
	for (int i = 0; i < NUM_STAT_IDS; i++){
		m_statValues[i] = val;
	}
}

void Stats::SetStatsFromRanges(Stats minStats, Stats maxStats)
{
	for (int statNum = 0; statNum < NUM_STAT_IDS; statNum++){
		IntRange statRange = IntRange(minStats.m_statValues[statNum], maxStats.m_statValues[statNum]);
		m_statValues[statNum] = statRange.GetRandomInRange();
	}
}

STAT_ID Stats::GetStatIDForName(std::string statName)
{
	if (statName == "Strength"){
		return STAT_STRENGTH;
	}
	if (statName == "Movement"){
		return STAT_MOVEMENT;
	}
	if (statName == "Defense"){
		return STAT_DEFENSE;
	}
	ERROR_AND_DIE("No case for stat: " + statName);
	return NUM_STAT_IDS;
}

std::string Stats::GetNameForStatID(STAT_ID id)
{
	if (id == STAT_STRENGTH){
		return "Strength";
	}
	if (id == STAT_MOVEMENT){
		return "Movement";
	}
	if (id == STAT_DEFENSE){
		return "Defense ";
	}
	ERROR_AND_DIE("No string name for stat id");
	
}

int Stats::GetStat(STAT_ID id) const
{
	return m_statValues[id];
}

int Stats::GetStat(std::string statName) const
{
	return GetStat(Stats::GetStatIDForName(statName));
}

void Stats::SetStat(std::string statName, int newValue)
{
	STAT_ID id = Stats::GetStatIDForName(statName);
	SetStat(id, newValue);
}

void Stats::SetStat(STAT_ID id, int newValue)
{
	m_statValues[id] = newValue;
}

void Stats::Add(const Stats statsToAdd)
{
	for (int statNum = 0; statNum < NUM_STAT_IDS; statNum++){
		STAT_ID statID = (STAT_ID) statNum;
		m_statValues[statID] += statsToAdd.GetStat(statID);
	}
}
