#include "StoryRequirement.hpp"
#include "Game/Character.hpp"
#include "Game/StoryState.hpp"
#include "Game/StoryDataDefinition.hpp"

StoryRequirement::StoryRequirement(unsigned int charID, StoryDataDefinition* parentData, tinyxml2::XMLElement * element)
{
	m_characterID = charID;
	m_parentData = parentData;
	m_fitnessWeight = ParseXmlAttribute(*element, "weight", m_fitnessWeight);
}

StoryRequirement_Tag::StoryRequirement_Tag(unsigned int charID, StoryDataDefinition* parentData, tinyxml2::XMLElement * element)
	:StoryRequirement(charID, parentData, element)
{
	std::string tagName = ParseXmlAttribute(*element, "tag", "NO_TAG");
	std::string value = ParseXmlAttribute(*element, "target", "true");
	std::string type = ParseXmlAttribute(*element, "type", "boolean");
	m_tag = TagPair(tagName, value, type);
}

StoryRequirement * StoryRequirement_Tag::Clone() const
{
	StoryRequirement_Tag* newReq = new StoryRequirement_Tag();
	newReq->m_characterID = m_characterID;
	newReq->m_tag = TagPair(m_tag.GetName(), m_tag.GetValue(), m_tag.GetType());
	return newReq;
}

bool StoryRequirement_Tag::PassesRequirement(StoryState * edge)
{
	if (m_tag.GetType() == "character"){
		std::string characterName = m_parentData->ReadCharacterNameFromDataString(m_tag.GetValue());
		if (characterName == "NO_CHARACTER_FOUND" ){
			return edge->m_storyTags.ContainsTagWithAnyValue(m_tag.GetName(), m_tag.GetType());
		} else {
			return edge->m_storyTags.HasTag(TagPair(m_tag.GetName(), characterName, m_tag.GetType()));
		}
	} else {
		return edge->m_storyTags.HasTag(m_tag);
	}
}