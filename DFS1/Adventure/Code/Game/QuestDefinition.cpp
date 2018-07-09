#include "QuestDefinition.hpp"
#include "Game/ActorDefinition.hpp"
#include "Game/VictoryCondition.hpp"
#include "Game/QuestReward.hpp"

std::map<std::string, QuestDefinition*> QuestDefinition::s_definitions;

QuestDefinition::QuestDefinition(tinyxml2::XMLElement * questElement)
{
	m_name = ParseXmlAttribute(*questElement, "name", "NO_NAME");
	m_isMainQuest = ParseXmlAttribute(*questElement, "mainQuest", false);
	std::string giverType = ParseXmlAttribute(*questElement, "giverType", "None");
	if (giverType == "None"){
		m_giverDefinition = nullptr;
	} else {
		m_giverDefinition = ActorDefinition::GetActorDefinition(giverType);
	}
	m_text = ParseXmlAttribute(*questElement, "text", m_name);

	tinyxml2::XMLElement* victoryStepsElement = questElement->FirstChildElement("Steps");
	ParseVictoryConditions(victoryStepsElement);

	tinyxml2::XMLElement* rewardElement = questElement->FirstChildElement("Reward");
	ParseReward(rewardElement);
}

QuestDefinition * QuestDefinition::GetQuestDefinition(std::string name)
{
	auto containsDef = s_definitions.find((std::string)name); 
	QuestDefinition* def = nullptr;
	if (containsDef != s_definitions.end()){
		def = containsDef->second;
	} else {
		ERROR_AND_DIE("No quest named: " + name);
	}
	return def;
}

void QuestDefinition::ParseVictoryConditions(tinyxml2::XMLElement * stepsElement)
{
	m_conditions = std::vector<VictoryCondition*>();
	for (tinyxml2::XMLElement* victoryConditionElement = stepsElement->FirstChildElement(); victoryConditionElement != NULL; victoryConditionElement = victoryConditionElement->NextSiblingElement()){
		VictoryCondition* newCondition = VictoryCondition::CreateVictoryCondition(victoryConditionElement);
		m_conditions.push_back(newCondition);
	}
}

void QuestDefinition::ParseReward(tinyxml2::XMLElement * rewardElement)
{
	m_questReward = QuestReward::CreateQuestReward(rewardElement);
}
