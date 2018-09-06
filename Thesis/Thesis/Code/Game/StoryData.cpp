#include "StoryData.hpp"

StoryData::StoryData(std::string name, float value)
{
	m_name = name;
	m_value = value;
}

StoryData::~StoryData()
{
}

std::string StoryData::GetName() const
{
	return m_name;
}

std::string StoryData::ToString() const
{
	return Stringf("%s  |  %f", m_name.c_str(), m_value);
}

bool StoryData::operator==(const StoryData & compare) const
{
	if (m_name == compare.m_name){
		if (m_value == compare.m_value){
			return true;
		}
	}
	return false;
}

bool StoryData::operator!=(const StoryData & compare) const
{
	if (m_name == compare.m_name){
		if (m_value == compare.m_value){
			return false;
		}
	}
	return true;
}
