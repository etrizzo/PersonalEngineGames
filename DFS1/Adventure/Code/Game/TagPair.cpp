#include "TagPair.hpp"
#include "Game/Game.hpp"

TagPair::TagPair(std::string name, std::string value,  std::string type)
{
	m_tag = name;
	m_tagValue = value;
	m_type = type;
}

std::string TagPair::GetName() const
{
	return m_tag;
}

std::string TagPair::GetValue() const
{
	return m_tagValue;
}

std::string TagPair::GetType() const
{
	return m_type;
}

void TagPair::SetValue(const std::string & value)
{
	m_tagValue = value;
}

bool TagPair::HasName(std::string tagName) const
{
	return m_tag == tagName;
}

bool TagPair::HasValue(std::string tagValue) const
{
	return m_tagValue == tagValue;
}

bool TagPair::ReadValueAsBool(bool failValue)
{
	if (m_tagValue == "true"){
		return true;
	}
	if (m_tagValue == "false"){
		return false;
	}
	return failValue;
}

Character * TagPair::ReadValueAsCharacter(Character * failValue)
{
	return g_theGame->m_graph->GetCharacterByName(m_tagValue);
}
