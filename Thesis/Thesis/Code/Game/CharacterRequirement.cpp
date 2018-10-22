#include "CharacterRequirement.hpp"
#include "Game/Character.hpp"
#include "Game/CharacterState.hpp"

CharacterRequirement::CharacterRequirement(unsigned int charID, tinyxml2::XMLElement * element)
{
	m_characterID = charID;
}

CharacterRequirement_Tag::CharacterRequirement_Tag(unsigned int charID, tinyxml2::XMLElement * element)
	:CharacterRequirement(charID, element)
{
	std::string tagName = ParseXmlAttribute(*element, "tag", "NO_TAG");
	std::string value = ParseXmlAttribute(*element, "target", "true");
	m_tag = TagPair(tagName, value);
}

bool CharacterRequirement_Tag::PassesRequirement(CharacterState * character)
{
	return character->HasTag(m_tag);
}

CharacterRequirement_Trait::CharacterRequirement_Trait(unsigned int charID, tinyxml2::XMLElement * element)
	:CharacterRequirement(charID, element)
{
	std::string traitString = ParseXmlAttribute(*element, "trait", "NO_TRAIT");
	m_trait = GetTraitFromString(traitString);
}

bool CharacterRequirement_Trait::PassesRequirement(CharacterState * character)
{
	return character->IsTraitInRange(m_trait, m_traitRange, m_wiggleroom);
}
