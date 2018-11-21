#include "Effect.hpp"
#include "Game/StoryState.hpp"
#include "Game/StoryDataDefinition.hpp"
#include "Game/StoryData.hpp"


Effect::Effect(tinyxml2::XMLElement * element, StoryDataDefinition* parentData)
{
	m_type = (eEffectType) ParseXmlAttribute(*element, "type", (int) EFFECT_TYPE_CHARACTER);
	m_characterID = (unsigned int) ParseXmlAttribute(*element, "character", -1);
	m_parentData = parentData;
}

Effect_TagChange::Effect_TagChange(tinyxml2::XMLElement * element, StoryDataDefinition* parentData)
	:Effect(element, parentData)
{
	std::string tag = ParseXmlAttribute(*element, "hasTag", "NO_TAG");
	std::string value = ParseXmlAttribute(*element, "target", "true");
	std::string type = ParseXmlAttribute(*element, "type", "boolean");

	m_tag = TagPair(tag, value, type);
}

bool Effect_TagChange::ApplyToState(StoryState * state, StoryData* instancedData)
{
	if (m_type == EFFECT_TYPE_CHARACTER){
		Character* character = instancedData->m_characters[m_characterID];
		CharacterState* characterState = state->GetCharacterStateForCharacter(character);
		characterState->m_tags.SetTagWithValue(m_tag.GetName(), m_tag.GetValue(), m_tag.GetType());
		if (m_tag.GetType() == "character"){
			std::string charName = instancedData->ReadCharacterNameFromDataString(m_tag.GetValue());
			characterState->m_tags.SetTagWithValue(m_tag.GetName(), charName, m_tag.GetType());
			return true;
		} else {
			characterState->m_tags.SetTagWithValue(m_tag.GetName(), m_tag.GetValue(), m_tag.GetType());
			return true;
		}
		return true;
	} else if (m_type == EFFECT_TYPE_STORY){
		if (m_tag.GetType() == "character"){
			std::string charName = instancedData->ReadCharacterNameFromDataString(m_tag.GetValue());
			state->m_storyTags.SetTagWithValue(m_tag.GetName(), charName, m_tag.GetType());
			return true;
		} else {
			state->m_storyTags.SetTagWithValue(m_tag.GetName(), m_tag.GetValue(), m_tag.GetType());
			return true;
		}
	}
	return false;
}

bool Effect_TagChange::ApplyToCharacterState(CharacterState * state)
{
	//if the tag type must be read differently (i.e. for characters), do that now
	if (m_tag.GetType() == "character"){
		std::string charName = m_parentData->ReadCharacterNameFromDataString(m_tag.GetValue());
		state->m_tags.SetTagWithValue(m_tag.GetName(), charName, m_tag.GetType());
		return true;
	} else {
		state->m_tags.SetTagWithValue(m_tag.GetName(), m_tag.GetValue(), m_tag.GetType());
		return true;
	}
}

Effect * Effect_TagChange::Clone()
{
	Effect_TagChange* newTag = new Effect_TagChange();
	newTag->m_characterID = m_characterID;
	newTag->m_parentData = m_parentData;
	newTag->m_tag = TagPair(m_tag.GetName(), m_tag.GetValue(), m_tag.GetType());
	return (Effect*) newTag;
}

Effect_TraitChange::Effect_TraitChange(tinyxml2::XMLElement * element, StoryDataDefinition* parentData)
	:Effect(element, parentData)
{
	TODO("Implement trait change effect");
}

bool Effect_TraitChange::ApplyToState(StoryState * state, StoryData* instancedData)
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
