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
	bool m_isMainQuest;
	bool m_isSequential;
	QuestReward* m_questReward;
	ActorDefinition* m_giverDefinition;

	static std::map<std::string, QuestDefinition*> m_definitions;
	static QuestDefinition* GetQuestDefinition(std::string name);
};