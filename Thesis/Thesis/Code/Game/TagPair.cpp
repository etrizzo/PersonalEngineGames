#include "TagPair.hpp"

TagPair::TagPair(std::string name, std::string value)
{
	m_tag = name;
	m_tagValue = value;
}

std::string TagPair::GetName()
{
	return m_tag;
}

std::string TagPair::GetValue()
{
	return m_tagValue;
}

bool TagPair::HasName(std::string tagName)
{
	return m_tag == tagName;
}

bool TagPair::HasValue(std::string tagValue)
{
	return m_tagValue == tagValue;
}
