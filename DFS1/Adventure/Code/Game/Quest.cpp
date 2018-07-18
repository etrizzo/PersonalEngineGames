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
	m_conditions[0]->m_active = true;

	if (m_definition->m_giverDefinition != nullptr){
		Tile* actorPos = m_definition->GetSpawnTile(m_adventure->m_startingMap);
		actorPos->MarkAsSpawned();
		Vector2 pos = actorPos->GetApproximateCenter();
		m_questGiver = m_adventure->m_startingMap->SpawnNewActor(m_definition->m_giverDefinition, pos, 0.f, m_adventure->m_difficulty);
		m_questGiver->AssignAsQuestGiver(this);
	}
}

bool Quest::UpdateAndCheckIfComplete()
{
	if (!m_isComplete){
		bool allComplete = true;
		for (int i = 0; i < m_conditions.size(); i++){
			VictoryCondition* condition = m_conditions[i];
			if (!condition->m_complete){		//if the condition has not already been completed
				if (!condition->CheckIfComplete()){		//updates and checks for completion
					allComplete = false;
				} else {
 					//m_currentIndex++;
					if (m_questGiver != nullptr){
						m_questGiver->AdvanceQuest();
					}
					if (m_definition->m_isSequential){
						if (i + 1 < m_conditions.size()){
							condition->m_active = false;
							m_conditions[i + 1]->m_active = true;
						}
						//TODO("Unlock next step in quest if the quest is sequential")
					}
				}
			}
		}
		if (allComplete){
			CompleteQuest();
		}
		return allComplete;
	}
	return m_isComplete;
}

void Quest::CompleteQuest()
{
	if (!m_isComplete){
		m_isComplete = true;
		for (VictoryCondition* condition : m_conditions){
			condition->m_complete = true;
		}
		m_definition->m_questReward->GiveReward(this);
		if (m_questGiver != nullptr){
			m_questGiver->FinishQuest();
		}
	}
}

std::string Quest::GetText()
{
	return m_definition->m_text;
}

void Quest::SpeakToGiver()
{
	for (int i = 0; i < m_conditions.size(); i++){
		VictoryCondition* condition = m_conditions[i];
		if (!condition->m_complete){
			if (condition->SpeakToGiver()){
				//m_currentIndex++;
				if (m_definition->m_isSequential){
					if (i + 1 < m_conditions.size()){
						condition->m_active = false;
						m_conditions[i + 1]->m_active = true;
					}
					//TODO("Unlock next step in quest if the quest is sequential")
				}
				break;		//once you encounter speaktoactor condition, break out of the loop (only updates 1 condition)
			}
		}
	}
}

bool Quest::IsMainQuest() const
{
	return m_definition->m_isMainQuest;
}

DialogueSet * Quest::GetCurrentDialogueSet()
{
	return m_definition->GetDialogueSet(m_currentIndex);
}
