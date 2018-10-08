#include "CharacterState.hpp"

CharacterState::CharacterState(Character * parent)
{
	m_character = parent;
}

bool CharacterState::HasTag(TagPair tag)
{
	return m_tags.HasTag(tag);
}
