#include "DialogueSetDefinition.hpp"
#include "Game/Dialogue.hpp"

DialogueSetDefinition::DialogueSetDefinition(tinyxml2::XMLElement * dialoguesElement)
{
	for (tinyxml2::XMLElement* lineElement = dialoguesElement->FirstChildElement("Dialogue"); lineElement != nullptr ; lineElement = lineElement->NextSiblingElement("Dialogue")){
		std::string content = ParseXmlAttribute(*lineElement, "content", "NO_CONTENT");
		Dialogue* d = new Dialogue(content);
		m_dialogues.push_back(d);
	}
}
