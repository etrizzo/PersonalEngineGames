#include "VictoryCondition.hpp"
#include "Game/Game.hpp"
#include "Game/Map.hpp"
#include "Game/Actor.hpp"
#include "Game/ActorDefinition.hpp"
#include "Game/Item.hpp"
#include "Game/ItemDefinition.hpp"
#include "Game/Adventure.hpp"
#include "Game/QuestDefinition.hpp"
#include "Game/Quest.hpp"

VictoryCondition::VictoryCondition(const tinyxml2::XMLElement * conditionsElement, Quest* quest)
{
	m_name = conditionsElement->Name();
	m_complete = false;
	m_quest = quest;
}

//bool VictoryCondition::CheckIfComplete()
//{
//	return m_complete;
//}

VictoryCondition * VictoryCondition::CreateVictoryCondition(const tinyxml2::XMLElement * conditionElement)
{
	std::string conditionName = conditionElement->Name();
	VictoryCondition* newCondition = nullptr;
	if (conditionName == "KillActor"){
		newCondition  = (VictoryCondition*) new VictoryCondition_KillActor(conditionElement, nullptr);
	}
	if (conditionName == "SpeakToActor"){
		newCondition  = (VictoryCondition*) new VictoryCondition_SpeakToActor(conditionElement, nullptr);
	}
	if (conditionName == "CollectItem"){
		newCondition  = (VictoryCondition*) new VictoryCondition_CollectItem(conditionElement, nullptr);
	}
	return newCondition;
}


VictoryCondition_KillActor::VictoryCondition_KillActor(const tinyxml2::XMLElement * conditionsElement, Adventure* adv, Quest* quest)
	:VictoryCondition(conditionsElement, quest)
{
	m_currentAdventure = adv;
	m_actorToKillName = ParseXmlAttribute(*conditionsElement, "actorToKill", "");
	m_numberToKill = ParseXmlAttribute(*conditionsElement, "numToKill", 1);
	m_numberKilled = 0;
}

VictoryCondition_KillActor * VictoryCondition_KillActor::Clone(Adventure* adventure, Quest* quest) const
{
	VictoryCondition_KillActor* vc = new VictoryCondition_KillActor(*this);
	vc->m_currentAdventure = adventure;
	vc->m_quest = quest;
	return vc;
}

bool VictoryCondition_KillActor::CheckIfComplete()
{
	if (!m_complete){
		for(Actor* actor: m_currentAdventure->m_currentMap->m_allActors){
			if (actor->m_aboutToBeDeleted){
				if (actor->m_definition->m_name == m_actorToKillName){
					m_numberKilled++;
				}
			}
		}
		if (m_numberKilled >= m_numberToKill){
			m_complete = true;
		}
	}
	return m_complete;
}

std::string VictoryCondition_KillActor::GetText()
{
	std::string victoryText = "Kill ";
	if (m_numberToKill == 1){
		victoryText += "a " + m_actorToKillName + ". ";
	} else {
		victoryText += std::to_string(m_numberToKill) + " " + m_actorToKillName + "s. ";
	}
	victoryText+= "(" + std::to_string(m_numberKilled) + "/" + std::to_string(m_numberToKill) + ")";
	return victoryText;
}

VictoryCondition_SpeakToActor::VictoryCondition_SpeakToActor(const tinyxml2::XMLElement * conditionsElement, Adventure * adv, Quest* quest)
	:VictoryCondition(conditionsElement, quest)
{
	m_currentAdventure = adv;
	m_actorTypeName = ParseXmlAttribute(*conditionsElement, "actorToSpeakTo", "");
}

VictoryCondition_SpeakToActor * VictoryCondition_SpeakToActor::Clone(Adventure * adventure, Quest* quest) const
{
	VictoryCondition_SpeakToActor* vc = new VictoryCondition_SpeakToActor(*this);
	vc->m_currentAdventure = adventure;
	vc->m_quest = quest;
	vc->m_actorTypeName = quest->m_definition->m_giverDefinition->m_name;
	return vc;
}

bool VictoryCondition_SpeakToActor::CheckIfComplete()
{
	return m_complete;
}

std::string VictoryCondition_SpeakToActor::GetText()
{
	return "Speak to " + m_actorTypeName;
}

bool VictoryCondition_SpeakToActor::SpeakToGiver()
{
	m_complete = true;
	return true;
}

VictoryCondition_CollectItem::VictoryCondition_CollectItem(const tinyxml2::XMLElement * conditionsElement, Adventure * adv, Quest* quest)
	: VictoryCondition(conditionsElement, quest)
{
	m_itemName = ParseXmlAttribute(*conditionsElement, "itemToCollect", "NO_ITEM");
	m_numberToCollect = ParseXmlAttribute(*conditionsElement, "numToCollect", 1);
	m_keepItemAfterFinish = ParseXmlAttribute(*conditionsElement, "keepItem", true);
}

VictoryCondition_CollectItem * VictoryCondition_CollectItem::Clone(Adventure * adventure, Quest* quest) const
{
	VictoryCondition_CollectItem* vc = new VictoryCondition_CollectItem(*this);
	vc->m_currentAdventure = adventure;
	vc->m_quest = quest;
	return vc;
}

bool VictoryCondition_CollectItem::CheckIfComplete()
{
	if (!m_complete){
		for(Item* item: m_currentAdventure->m_currentMap->m_allItems){
			if (item->m_aboutToBeDeleted){
				if (item->m_definition->m_name == m_itemName){
					m_numberCollected++;
				}
			}
		}
		if (m_numberCollected >= m_numberToCollect){
			m_complete = true;
		}
	}
	return m_complete;
}

std::string VictoryCondition_CollectItem::GetText()
{
	std::string victoryText = "Collect ";
	if (m_numberToCollect == 1){
		victoryText += "a " + m_itemName + ". ";
	} else {
		victoryText += std::to_string(m_numberCollected) + " " + m_itemName + "s. ";
	}
	victoryText+= "(" + std::to_string(m_numberCollected) + "/" + std::to_string(m_numberToCollect) + ")";
	return victoryText;
}
