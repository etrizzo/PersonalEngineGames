#include "CharacterRequirement.hpp"
#include "Game/Character.hpp"
#include "Game/CharacterState.hpp"
#include "Game/StoryDataDefinition.hpp"

CharacterRequirement::CharacterRequirement(unsigned int charID, StoryDataDefinition* parentData, tinyxml2::XMLElement * element)
{
	m_characterID = charID;
	m_parentData = parentData;
	m_fitnessWeight = ParseXmlAttribute(*element, "weight", m_fitnessWeight);
}

CharacterRequirement_Tag::CharacterRequirement_Tag(unsigned int charID, StoryDataDefinition* parentData, tinyxml2::XMLElement * element)
	:CharacterRequirement(charID, parentData, element)
{
	std::string tagName = ParseXmlAttribute(*element, "tag", "NO_TAG");
	std::string value = ParseXmlAttribute(*element, "target", "true");
	std::string type = ParseXmlAttribute(*element, "type", "boolean");
	m_tag = TagPair(tagName, value, type);
}

CharacterRequirement * CharacterRequirement_Tag::Clone() const
{
	CharacterRequirement_Tag* newReq = new CharacterRequirement_Tag();
	newReq->m_characterID = m_characterID;
	newReq->m_tag = TagPair(m_tag.GetName(), m_tag.GetValue(), m_tag.GetType());
	return newReq;
}

bool CharacterRequirement_Tag::PassesRequirement(CharacterState * character)
{
	if (m_tag.GetType() == "character"){
		std::string characterName = m_parentData->ReadCharacterNameFromDataString(m_tag.GetValue());
		if (characterName == "NO_CHARACTER_FOUND" ){
			return character->m_tags.ContainsTagWithAnyValue(m_tag.GetName(), m_tag.GetType());
		} else {
			return character->HasTag(TagPair(m_tag.GetName(), characterName, m_tag.GetType()));
		}
	} else {
		return character->HasTag(m_tag);
	}
}

CharacterRequirement_Trait::CharacterRequirement_Trait(unsigned int charID, StoryDataDefinition* parentData, tinyxml2::XMLElement * element)
	:CharacterRequirement(charID, parentData, element)
{
	std::string traitString = ParseXmlAttribute(*element, "trait", "NO_TRAIT");
	m_trait = GetTraitFromString(traitString);
}

CharacterRequirement * CharacterRequirement_Trait::Clone() const
{
	CharacterRequirement_Trait* newReq = new CharacterRequirement_Trait();
	newReq->m_characterID = m_characterID;
	newReq->m_traitRange = m_traitRange;
	newReq->m_trait = m_trait;
	return newReq;
}

bool CharacterRequirement_Trait::PassesRequirement(CharacterState * character)
{
	return character->IsTraitInRange(m_trait, m_traitRange, m_wiggleroom);
}
