#include "VictoryCondition.hpp"
#include "Game/Game.hpp"
#include "Game/Map.hpp"
#include "Game/Actor.hpp"
#include "Game/ActorDefinition.hpp"
#include "Game/Adventure.hpp"

VictoryCondition::VictoryCondition(const tinyxml2::XMLElement * conditionsElement)
{
	m_name = conditionsElement->Name();
	m_complete = false;
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
		newCondition  = (VictoryCondition*) new VictoryCondition_KillActor(conditionElement);
	}
	return newCondition;
}


VictoryCondition_KillActor::VictoryCondition_KillActor(const tinyxml2::XMLElement * conditionsElement)
	:VictoryCondition(conditionsElement)
{
	m_actorToKillName = ParseXmlAttribute(*conditionsElement, "actorToKill", "");
	m_numberToKill = ParseXmlAttribute(*conditionsElement, "numToKill", 1);
	m_numberKilled = 0;
}

VictoryCondition_KillActor * VictoryCondition_KillActor::Clone() const
{
	return new VictoryCondition_KillActor(*this);
}

bool VictoryCondition_KillActor::CheckIfComplete()
{
	if (!m_complete){
		for(Actor* actor: g_theGame->m_currentAdventure->m_currentMap->m_allActors){
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
