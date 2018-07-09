#pragma once
#include "Game/GameCommon.hpp"
class VictoryCondition;
class QuestReward;
class ActorDefinition;

class QuestDefinition{
public:
	QuestDefinition(tinyxml2::XMLElement* questElement);

	std::string m_name;
	std::vector<VictoryCondition*> m_conditions; 
	std::string m_text;
	bool m_isMainQuest;
	bool m_isSequential;
	QuestReward* m_questReward;
	ActorDefinition* m_giverDefinition;

	static std::map<std::string, QuestDefinition*> s_definitions;
	static QuestDefinition* GetQuestDefinition(std::string name);

protected:
	void ParseVictoryConditions(tinyxml2::XMLElement* stepsElement);
	void ParseReward(tinyxml2::XMLElement* rewardElement);
};