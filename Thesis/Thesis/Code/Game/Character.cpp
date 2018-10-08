#include "Character.hpp"

Character::Character()
{
}

void Character::InitFromXML(tinyxml2::XMLElement * characterDefinition)
{
	m_name = ParseXmlAttribute(*characterDefinition, "name", "NO_NAME");
}

bool Character::HasTag(TagPair tag)
{
	return m_tags.HasTag(tag);
}

std::string Character::GetName() const
{
	return m_name;
}
