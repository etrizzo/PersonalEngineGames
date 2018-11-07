#include "CharacterState.hpp"

CharacterState::CharacterState(Character * parent)
{
	m_character = parent;

	m_tags = Tags(parent->m_tags);
}

CharacterState::CharacterState(CharacterState * previousState)
{
	m_character = previousState->m_character;
	m_tags = Tags(previousState->m_tags);
	for (int i = 0; i < NUM_CHARACTER_TRAITS; i++){
		m_traits[i] = previousState->m_traits[i];
	}
}

std::string CharacterState::ToString() const
{
	std::string toString = Stringf("Character: %s\n%s\n==========================\n", m_character->GetName().c_str(), m_tags.GetTagsAsString().c_str());

	return toString;
}

bool CharacterState::IsTraitInRange(eCharacterTrait trait, const FloatRange & desiredRange, float wiggleroom)
{
	FloatRange existing = m_traits[trait];
	if (desiredRange.IsValueInRangeInclusive(existing.min) && desiredRange.IsValueInRangeInclusive(existing.max)){
		return true;
	}
	float distanceFromMins = desiredRange.GetDistanceFromEdge(existing.min);
	float distanceFromMaxs = desiredRange.GetDistanceFromEdge(existing.max);
	if (distanceFromMins <= wiggleroom && distanceFromMaxs <= wiggleroom){
		return true;
	}
	return false;
}

bool CharacterState::IsTraitInRange(eCharacterTrait trait, float min, float max, float wiggleroom)
{
	return IsTraitInRange(trait, FloatRange(min, max), wiggleroom);
}

bool CharacterState::HasTag(TagPair tag)
{
	return m_tags.HasTag(tag);
}
