#include "Effect.hpp"
#include "Game/StoryState.hpp"
#include "Game/StoryDataDefinition.hpp"
#include "Game/StoryData.hpp"


Effect::Effect(tinyxml2::XMLElement * element, StoryDataDefinition* parentData)
{
	std::string type = ParseXmlAttribute(*element, "effectType", "character");
	if (type == "character"){
		m_type = EFFECT_TYPE_CHARACTER;
	} else if (type == "story"){
		m_type = EFFECT_TYPE_STORY;
	}
	m_characterIndexString = ParseXmlAttribute(*element, "character", "NO_CHARACTER");
	std::string stripped = m_characterIndexString;
	Strip(stripped, '*');
	m_characterID = atoi(stripped.c_str());
	//m_characterID = (unsigned int) ParseXmlAttribute(*element, "character", -1);
	m_parentData = parentData;
}

Effect_TagChange::Effect_TagChange(tinyxml2::XMLElement * element, StoryDataDefinition* parentData)
	:Effect(element, parentData)
{
	std::string tag = ParseXmlAttribute(*element, "tag", "NO_TAG");
	std::string value = ParseXmlAttribute(*element, "target", "true");
	std::string type = ParseXmlAttribute(*element, "type", "boolean");
	

	m_tag = TagPair(tag, value, type);
}

bool Effect_TagChange::ApplyToState(StoryState * state, StoryData* instancedData)
{
	if (m_type == EFFECT_TYPE_CHARACTER){
		if (instancedData == nullptr)
		{
			//we can't tell what will happen with a character without any instance data
			return false;
		}
		Character* character = instancedData->m_characters[m_characterID];
		CharacterState* characterState = state->GetCharacterStateForCharacter(character);
		characterState->m_tags.SetTagWithValue(m_tag.GetName(), m_tag.GetValue(), m_tag.GetType());
		if (m_tag.GetType() == "character"){
			//if you're setting a character name in the story state, read the actual name of that specific character on the node
			std::string charName = instancedData->ReadCharacterNameFromDataString(m_characterIndexString);
			characterState->m_tags.SetTagWithValue(m_tag.GetName(), charName, m_tag.GetType());
			return true;
		} else {
			characterState->m_tags.SetTagWithValue(m_tag.GetName(), m_tag.GetValue(), m_tag.GetType());
			return true;
		}
		return true;
	} else if (m_type == EFFECT_TYPE_STORY){
		if (m_tag.GetType() == "character"){
			if (instancedData == nullptr)
			{
				//we can't tell what will happen with a character without any instance data
				return false;
			}
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

