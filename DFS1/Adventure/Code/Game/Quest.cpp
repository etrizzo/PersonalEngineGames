#include "Quest.hpp"
#include "Game/QuestReward.hpp"
#include "Game/Adventure.hpp"
#include "Game/Map.hpp"
#include "Game/Actor.hpp"

Quest::Quest(QuestDefinition * def, Adventure * currentAdventure)
{
	m_definition = def;
	m_adventure = currentAdventure;

	for (VictoryCondition* condition : m_definition->m_conditions){
		VictoryCondition* newCondition = condition->Clone(currentAdventure, this);
		m_conditions.push_back(newCondition);
	}

	m_questGiver = m_adventure->m_startingMap->GetActorOfType(m_definition->m_giverDefinition);
	if (m_questGiver != nullptr){
		m_questGiver->m_questGiven = this;
	}
}

bool Quest::CheckIfComplete()
{
	bool allComplete = true;
	for (VictoryCondition* condition : m_conditions){
		if (!condition->m_complete){		//if the condition has not already been completed
			if (!condition->CheckIfComplete()){		//updates and checks for completion
				allComplete = false;
			} else {
				if (m_definition->m_isSequential){
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

std::string Quest::GetText()
{
	return m_definition->m_text;
}

void Quest::SpeakToGiver()
{
	for (VictoryCondition* condition : m_conditions){
		if (!condition->m_complete){
			if (condition->SpeakToGiver()){
				break;		//once you encounter speaktoactor condition, break out of the loop (only updates 1 condition)
			}
		}
	}
}
