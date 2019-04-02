#include "TagSet.hpp"

TagSet::TagSet(const std::string & commaSeparatedTagNames)
{
	m_tags = std::vector<TagPair>();
	SetOrRemoveTags(commaSeparatedTagNames);
}

TagSet::TagSet(const TagSet & copy)
{
	for (int i = 0; i < (int) copy.m_tags.size(); i++){
		TagPair tag = TagPair(copy.m_tags[i].GetName(), copy.m_tags[i].GetValue(), copy.m_tags[i].GetType());
		tag.m_expires = copy.m_tags[i].m_expires;
		m_tags.push_back(tag);
	}
}


int TagSet::GetNumTagSet() const
{
	return m_tags.size();
}

std::string TagSet::GetTagsAsString() const
{
	std::string tags = Stringf("%-12s | %s\n", "Tag", "Value");
	tags += "-------------|-----------\n";
	for (TagPair tag : m_tags){
		std::string tagStr = Stringf("%-12s | %s\n", tag.GetName().c_str(), tag.GetValue().c_str());
		//tags+= tag.GetName() + ":" + tag.GetValue() + ", ";
		tags += tagStr;
	}
	return tags;
}

void TagSet::SetOrRemoveTags(const std::string & commaSeparatedTagNames)
{
	Strings unparsedTagSet = Strings();
	unsigned int oldIndex = 0;
	unsigned int commaIndex = (unsigned int) commaSeparatedTagNames.find(',');
	while (commaIndex != std::string::npos){
		std::string indexString = commaSeparatedTagNames.substr(oldIndex,commaIndex);
		unparsedTagSet.push_back(indexString); 

		oldIndex = commaIndex + 1;
		commaIndex = (unsigned int) commaSeparatedTagNames.find(',', oldIndex);
	}
	std::string indexString = commaSeparatedTagNames.substr(oldIndex);
	unparsedTagSet.push_back(indexString);  

	for(std::string tag : unparsedTagSet){
		if (tag[0] == '!'){
			RemoveTag(tag.substr(1));
		} else {
			SetTag(tag);
		}
	}
}

bool TagSet::HasTags(const std::string & commaSeparatedTagNames)
{
	Strings unparsedTagSet = Strings();
	unsigned int oldIndex = 0;
	unsigned int commaIndex = (unsigned int) commaSeparatedTagNames.find(',');
	while (commaIndex != std::string::npos){
		std::string indexString = commaSeparatedTagNames.substr(oldIndex,commaIndex);
		unparsedTagSet.push_back(indexString); 

		oldIndex = commaIndex + 1;
		commaIndex = (unsigned int) commaSeparatedTagNames.find(',', oldIndex);
	}
	std::string indexString = commaSeparatedTagNames.substr(oldIndex);
	unparsedTagSet.push_back(indexString);  


	//bool allTagSetCorrect = true; 
	for (std::string tag : unparsedTagSet){
		if (tag[0] == '!'){
			if (HasTag(tag.substr(1))){
				return false;
			}
		} else {
			if (!HasTag(tag)){
				return false;
			}
		}
	}
	return true;
}

void TagSet::SetTag(const std::string & tagName)
{
	if (!HasTag(tagName)){
		m_tags.push_back(TagPair(tagName, "true", "boolean"));
	}
}

void TagSet::SetTagWithValue(const std::string & tagName, const std::string & value, std::string type, bool expires)
{
	if (!HasTag(tagName)){
		TagPair tag = TagPair(tagName, value, type);
		tag.m_expires = expires;
		m_tags.push_back(tag);
	} else {
		//set the existing value
		for(int i = 0; i < (int) m_tags.size(); i++){
			if (m_tags[i].GetName() == tagName){
				m_tags[i].SetValue(value);
				m_tags[i].m_expires = expires;
			}
		}
	}
}

void TagSet::RemoveTag(const std::string & tagName)
{
	for( std::vector<TagPair>::iterator tagIter = m_tags.begin(); tagIter != m_tags.end(); ++tagIter )
	{
		if( tagIter->GetName() == tagName )
		{
			m_tags.erase( tagIter );
			break;
		}
	}
}

bool TagSet::HasTag(const std::string & tagName)
{
	for (TagPair tag : m_tags){
		if (tag.HasName(tagName)){
			return true;
		}
	}
	return false;
}

bool TagSet::HasTag(TagPair checkTag)
{
	for (TagPair tag : m_tags){
		if (tag.HasName(checkTag.GetName())){
			if ( tag.HasValue(checkTag.GetValue())){
				return true;
			} else {
				return false;
			}
		}
	}
	//if you don't have the tag, but the target is false, return true
	if (checkTag.GetValue() == "false"){
		return true;
	}
	return false;
}

bool TagSet::HasTagWithValue(const std::string & tagName, const std::string & value)
{
	for (TagPair tag : m_tags){
		if (tag.HasName(tagName) && tag.HasValue(value)){
			return true;
		}
	}
	return false;
}

bool TagSet::ContainsTagWithAnyValue(const std::string & tagName, const std::string & type)
{
	for (TagPair tag : m_tags){
		if (tag.HasName(tagName) && (tag.GetType() == type) ){
			return true;
		}
	}
	return false;
}

std::string TagSet::GetTagValue(std::string tagName, std::string defaultValue)
{
	for (TagPair tag : m_tags) {
		if (tag.HasName(tagName)) {
			return tag.GetValue();
		}
	}
	//default case - does not have tag
	return defaultValue;
}

void TagSet::ClearExpiredTags()
{
	for (int i = m_tags.size() - 1; i >= 0; i--)
	{
		if (m_tags[i].m_expires) {
			RemoveTag(m_tags[i].GetName());
		}
	}
}
