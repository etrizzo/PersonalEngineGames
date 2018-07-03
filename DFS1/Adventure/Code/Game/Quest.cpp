#include "Quest.hpp"
#include "Game/QuestReward.hpp"

bool Quest::CheckIfComplete()
{
	bool allComplete = true;
	for (VictoryCondition* condition : m_conditions){
		if (!condition->m_complete){		//if the condition has not already been completed
			if (!condition->CheckIfComplete()){		//updates and checks for completion
				allComplete = false;
			} else {
				if (m_isSequential){
					TODO("Unlock next step in quest if the quest is sequential")
				}
			}
		}
	}
	if (allComplete){
		CompleteQuest();
	}
	return allComplete;
}

void Quest::CompleteQuest()
{
	m_isComplete = true;
	m_definition->m_questReward->GiveReward();
}
