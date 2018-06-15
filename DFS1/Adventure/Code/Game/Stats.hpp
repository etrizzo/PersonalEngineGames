#pragma once
#include "GameCommon.hpp"

class Stats{
public:
	Stats();
	Stats(Stats minStats, Stats maxStats);
	Stats(IntRange allStatRange);
	Stats(int val);
	~Stats(){};

	void SetStatsFromRanges(Stats minStats, Stats maxStats);
	int GetStat(STAT_ID id) const;
	int GetStat(std::string statName) const;
	
	void SetStat(std::string statName, int newValue);
	void SetStat(STAT_ID id, int newValue);
	void Add(const Stats statsToAdd);

protected:
	int m_statValues[NUM_STAT_IDS];


public:
	static STAT_ID GetStatIDForName(std::string statName);
	static std::string GetNameForStatID(STAT_ID id);
};