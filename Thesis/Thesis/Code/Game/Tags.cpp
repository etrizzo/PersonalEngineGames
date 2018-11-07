#include "Tags.hpp"

Tags::Tags(const std::string & commaSeparatedTagNames)
{
	m_tags = std::vector<TagPair>();
	SetOrRemoveTags(commaSeparatedTagNames);
}

Tags::Tags(const Tags & copy)
{
	for (int i = 0; i < copy.m_tags.size(); i++){
		TagPair tag = TagPair(copy.m_tags[i].GetName(), copy.m_tags[i].GetValue(), copy.m_tags[i].GetType());
		m_tags.push_back(tag);
	}
}

Tags::~Tags()
{
	
}

int Tags::GetNumTags() const
{
	return m_tags.size();
}

std::string Tags::GetTagsAsString() const
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

void Tags::SetOrRemoveTags(const std::string & commaSeparatedTagNames)
{
	Strings unparsedTags = Strings();
	unsigned int oldIndex = 0;
	unsigned int commaIndex = (unsigned int) commaSeparatedTagNames.find(',');
	while (commaIndex != std::string::npos){
		std::string indexString = commaSeparatedTagNames.substr(oldIndex,commaIndex);
		unparsedTags.push_back(indexString); 

		oldIndex = commaIndex + 1;
		commaIndex = (unsigned int) commaSeparatedTagNames.find(',', oldIndex);
	}
	std::string indexString = commaSeparatedTagNames.substr(oldIndex);
	unparsedTags.push_back(indexString);  

	for(std::string tag : unparsedTags){
		if (tag[0] == '!'){
			RemoveTag(tag.substr(1));
		} else {
			SetTag(tag);
		}
	}
}

bool Tags::HasTags(const std::string & commaSeparatedTagNames)
{
	Strings unparsedTags = Strings();
	unsigned int oldIndex = 0;
	unsigned int commaIndex = (unsigned int) commaSeparatedTagNames.find(',');
	while (commaIndex != std::string::npos){
		std::string indexString = commaSeparatedTagNames.substr(oldIndex,commaIndex);
		unparsedTags.push_back(indexString); 

		oldIndex = commaIndex + 1;
		commaIndex = (unsigned int) commaSeparatedTagNames.find(',', oldIndex);
	}
	std::string indexString = commaSeparatedTagNames.substr(oldIndex);
	unparsedTags.push_back(indexString);  


	//bool allTagsCorrect = true; 
	for (std::string tag : unparsedTags){
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

void Tags::SetTag(const std::string & tagName)
{
	if (!HasTag(tagName)){
		m_tags.push_back(TagPair(tagName, "true", "boolean"));
	}
}

void Tags::SetTagWithValue(const std::string & tagName, const std::string & value, std::string type)
{
	if (!HasTag(tagName)){
		TagPair tag = TagPair(tagName, value, type);
		m_tags.push_back(tag);
	} else {
		//set the existing value
		for(int i = 0; i < m_tags.size(); i++){
			if (m_tags[i].GetName() == tagName){
				m_tags[i].SetValue(value);
			}
		}
	}
}

void Tags::RemoveTag(const std::string & tagName)
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

bool Tags::HasTag(const std::string & tagName)
{
	for (TagPair tag : m_tags){
		if (tag.HasName(tagName)){
			return true;
		}
	}
	return false;
}

bool Tags::HasTag(TagPair checkTag)
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

bool Tags::HasTagWithValue(const std::string & tagName, const std::string & value)
{
	for (TagPair tag : m_tags){
		if (tag.HasName(tagName) && tag.HasValue(value)){
			return true;
		}
	}
}

bool Tags::ContainsTagWithAnyValue(const std::string & tagName, const std::string & type)
{
	for (TagPair tag : m_tags){
		if (tag.HasName(tagName) && (tag.GetType() == type) ){
			return true;
		}
	}
	return false;
}
