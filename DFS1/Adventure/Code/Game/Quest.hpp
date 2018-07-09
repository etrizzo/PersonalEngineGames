#pragma once
#include "Game/GameCommon.hpp"
#include "Game/VictoryCondition.hpp"
#include "Game/QuestDefinition.hpp"

class Actor;
class QuestReward;
class Adventure;

class Quest{
public:
	Quest(QuestDefinition* def, Adventure* currentAdventure);

	bool CheckIfComplete();		//updates all victory conditions
	void CompleteQuest();
	std::string GetText();
	void SpeakToGiver();

	std::vector<VictoryCondition*> m_conditions; 
	bool m_isComplete;

	Actor* m_questGiver;
	Adventure* m_adventure;
	QuestDefinition* m_definition;
	
};