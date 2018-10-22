#include "TagPair.hpp"

TagPair::TagPair(std::string name, std::string value)
{
	m_tag = name;
	m_tagValue = value;
}

std::string TagPair::GetName() const
{
	return m_tag;
}

std::string TagPair::GetValue() const
{
	return m_tagValue;
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
