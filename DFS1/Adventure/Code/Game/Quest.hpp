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
	~Quest();

	bool UpdateAndCheckIfComplete();		//updates all victory conditions
	bool IsComplete() const { return m_isComplete; };
	void CompleteQuest();
	std::string GetText();
	void SpeakToGiver();
	bool IsMainQuest() const;
	DialogueSet* GetCurrentDialogueSet();

	int m_currentIndex = 0;

	std::vector<VictoryCondition*> m_conditions; 
	bool m_isComplete;

	Actor* m_questGiver;
	Adventure* m_adventure;
	QuestDefinition* m_definition;

	
};