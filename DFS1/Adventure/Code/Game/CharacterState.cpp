#include "CharacterState.hpp"

CharacterState::CharacterState(Character * parent)
{
	m_character = parent;

	m_tags = TagSet(parent->m_tags);
}

CharacterState::CharacterState(CharacterState * previousState)
{
	m_character = previousState->m_character;
	m_tags = TagSet(previousState->m_tags);
}

std::string CharacterState::ToString() const
{
	std::string toString = Stringf("Character: %s\n%s\n==========================\n", m_character->GetName().c_str(), m_tags.GetTagsAsString().c_str());

	return toString;
}

bool CharacterState::HasTag(TagPair tag)
{
	return m_tags.HasTag(tag);
}
