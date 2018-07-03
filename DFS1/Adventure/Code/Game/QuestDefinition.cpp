#include "QuestDefinition.hpp"

QuestDefinition::QuestDefinition(tinyxml2::XMLElement * questElement)
{
	m_name = ParseXmlAttribute(*questElement, "name", "NO_NAME");
	m_isMainQuest = ParseXmlAttribute(*questElement, "mainQuest", false);
}

QuestDefinition * QuestDefinition::GetQuestDefinition(std::string name)
{
	return nullptr;
}
