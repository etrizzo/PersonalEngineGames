#pragma once
#include "Game/GameCommon.hpp"
#include "Game/VictoryCondition.hpp"
#include "Game/QuestDefinition.hpp"

class Actor;
class QuestReward;
class Adventure;

class Quest{
	Quest(QuestDefinition* def, Adventure* currentAdventure);
	Quest(tinyxml2::XMLElement* questElement);

	bool CheckIfComplete();		//updates all victory conditions
	void CompleteQuest();

	std::vector<VictoryCondition*> m_conditions; 
	bool m_isComplete;

	Actor* m_questGiver;
	Adventure* m_adventure;
	QuestDefinition* m_definition;
	
};