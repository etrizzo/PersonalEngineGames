#include "Character.hpp"
#include "Game/Actor.hpp"

Character::Character()
{
}

void Character::InitFromXML(tinyxml2::XMLElement * characterDefinition)
{
	m_name = ParseXmlAttribute(*characterDefinition, "name", "NO_NAME");

	tinyxml2::XMLElement* tagsElement = characterDefinition->FirstChildElement("Tags");
	for (tinyxml2::XMLElement* tagElem = tagsElement->FirstChildElement("Tag"); tagElem != nullptr; tagElem = tagElem->NextSiblingElement("Tag")){
		std::string tagName = ParseXmlAttribute(*tagElem, "tag", "NO_TAG");
		std::string value = ParseXmlAttribute(*tagElem, "value", "true");
		std::string type = ParseXmlAttribute(*tagElem, "type", "boolean");
		m_tags.SetTagWithValue(tagName, value, type);
	}
}

bool Character::HasTag(TagPair tag)
{
	return m_tags.HasTag(tag);
}

std::string Character::GetName() const
{
	if (m_actor != nullptr)
	{
		return m_actor->m_name;
	}
	return m_name;
}

void Character::SetActor(Actor * actor)
{
	m_actor = actor;
}
