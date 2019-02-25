#include "QuestDefinition.hpp"
#include "Game/ActorDefinition.hpp"
#include "Game/VictoryCondition.hpp"
#include "Game/QuestReward.hpp"
#include "Game/DialogueSet.hpp"
#include "Game/TileDefinition.hpp"
#include "Game/Map.hpp"

std::map<std::string, QuestDefinition*> QuestDefinition::s_definitions;

QuestDefinition::QuestDefinition(tinyxml2::XMLElement * questElement)
{
	m_name = ParseXmlAttribute(*questElement, "name", "NO_NAME");
	m_isMainQuest = ParseXmlAttribute(*questElement, "mainQuest", false);
	m_isSequential = ParseXmlAttribute(*questElement, "sequential", false);
	std::string giverType = ParseXmlAttribute(*questElement, "giverType", "None");
	if (giverType == "None"){
		m_giverDefinition = nullptr;
	} else {
		m_giverDefinition = ActorDefinition::GetActorDefinition(giverType);
		m_giverSpawnTileTag = ParseXmlAttribute(*questElement, "giverSpawnTag", "None");
		m_giverSpawnTileDefinition = ParseXmlAttribute(*questElement, "giverSpawnTile", (TileDefinition*) nullptr);
	}
	m_text = ParseXmlAttribute(*questElement, "text", m_name);

	tinyxml2::XMLElement* victoryStepsElement = questElement->FirstChildElement("Steps");
	ParseVictoryConditions(victoryStepsElement);

	tinyxml2::XMLElement* rewardElement = questElement->FirstChildElement("Reward");
	ParseReward(rewardElement);

	tinyxml2::XMLElement* dialoguesElement = questElement->FirstChildElement("DialogueSets");
	ParseDialogueSets(dialoguesElement);
}

DialogueSet * QuestDefinition::GetDialogueSet(int index)
{
	if ((int) m_dialogues.size() > index){
		return new DialogueSet(m_dialogues[index]);
	}
	return nullptr;
}

Tile * QuestDefinition::GetSpawnTile(Map * map) const
{
	if (m_giverSpawnTileDefinition != nullptr){
		if (m_giverSpawnTileTag == "None"){
			return &(map->GetSpawnTileOfType(m_giverSpawnTileDefinition));
		} else {
			return map->GetTaggedTileOfType(m_giverSpawnTileDefinition, m_giverSpawnTileTag);
		}
	} else {
		if (m_giverSpawnTileTag != "None"){
			return map->GetRandomTileWithTag(m_giverSpawnTileTag);
		} else {
			return &(map->GetRandomBaseTile());
		}
	}
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

void QuestDefinition::ParseDialogueSets(tinyxml2::XMLElement * dialogueElement)
{
	m_dialogues = std::vector<DialogueSetDefinition*>();
	if (dialogueElement != nullptr){
		for (tinyxml2::XMLElement* setElement = dialogueElement->FirstChildElement(); setElement != NULL; setElement = setElement->NextSiblingElement()){
			DialogueSetDefinition* newSet =  new DialogueSetDefinition(setElement);
			m_dialogues.push_back(newSet);
		}
	}
}
