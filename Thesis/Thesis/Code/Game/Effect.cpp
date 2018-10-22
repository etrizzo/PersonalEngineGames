#include "Effect.hpp"
#include "Game/StoryState.hpp"
#include "Game/StoryData.hpp"


Effect::Effect(tinyxml2::XMLElement * element, StoryData* parentData)
{
	m_characterID = (unsigned int) ParseXmlAttribute(*element, "character", -1);
	m_parentData = parentData;
}

Effect_TagChange::Effect_TagChange(tinyxml2::XMLElement * element, StoryData* parentData)
	:Effect(element, parentData)
{
	std::string tag = ParseXmlAttribute(*element, "hasTag", "NO_TAG");
	std::string value = ParseXmlAttribute(*element, "target", "true");

	m_tag = TagPair(tag, value);
}

bool Effect_TagChange::ApplyToState(StoryState * state)
{
	Character* character = m_parentData->m_characters[m_characterID];
	CharacterState* characterState = state->GetCharacterStateForCharacter(character);
	characterState->m_tags.SetTagWithValue(m_tag.GetName(), m_tag.GetValue());
	return true;
}

bool Effect_TagChange::ApplyToCharacterState(CharacterState * state)
{
	state->m_tags.SetTagWithValue(m_tag.GetName(), m_tag.GetValue());
	return true;
}

Effect * Effect_TagChange::Clone()
{
	Effect_TagChange* newTag = new Effect_TagChange();
	newTag->m_characterID = m_characterID;
	newTag->m_parentData = m_parentData;
	newTag->m_tag = TagPair(m_tag.GetName(), m_tag.GetValue());
	return (Effect*) newTag;
}

Effect_TraitChange::Effect_TraitChange(tinyxml2::XMLElement * element, StoryData* parentData)
	:Effect(element, parentData)
{
	TODO("Implement trait change effect");
}

bool Effect_TraitChange::ApplyToState(StoryState * state)
{
	TODO("Implement trait change effect");
	return true;
}

bool Effect_TraitChange::ApplyToCharacterState(CharacterState * state)
{
	return false;
}

Effect * Effect_TraitChange::Clone()
{
	return nullptr;
}
