#include "Tags.hpp"

Tags::Tags(const std::string & commaSeparatedTagNames)
{
	m_tags = Strings();
	SetOrRemoveTags(commaSeparatedTagNames);
}

Tags::~Tags()
{
	
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
		m_tags.push_back(tagName);
	}
}

void Tags::RemoveTag(const std::string & tagName)
{
	for( Strings::iterator tagIter = m_tags.begin(); tagIter != m_tags.end(); ++tagIter )
	{
		if( *tagIter == tagName )
		{
			m_tags.erase( tagIter );
			break;
		}
	}
}

bool Tags::HasTag(const std::string & tagName)
{
	for (std::string tag : m_tags){
		if (tag == tagName){
			return true;
		}
	}
	return false;
}
