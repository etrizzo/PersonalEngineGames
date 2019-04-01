#include "StoryState.hpp"
#include "Game/Game.hpp"
#include "Game/Action.hpp"
#include "Game/StoryDataDefinition.hpp"

StoryState::StoryState(float cost, int numCharacters, StoryGraph* graph)
{
	m_cost = cost;
	m_characterStates = std::vector<CharacterState*>();
	if (graph != nullptr){
		for (int i = 0; i < numCharacters; i++){
			m_characterStates.push_back(new CharacterState(graph->GetCharacter(i)));
		}
	}

	m_possibleActRange = IntRange(0, MAX_ACTS);
}

StoryState::StoryState(const StoryState & copy)
{
	m_cost = copy.m_cost;
	for(CharacterState* charState : copy.m_characterStates){
		CharacterState* newState = new CharacterState(charState);
		m_characterStates.push_back(newState);
	}
	m_possibleActRange = copy.m_possibleActRange;
	m_storyTags = copy.m_storyTags;
}

StoryState::~StoryState()
{
	for (CharacterState* state : m_characterStates){
		delete state;
	}
	m_characterStates.clear();
}

void StoryState::ClearExpiredState()
{
	for (CharacterState* state : m_characterStates)
	{
		state->m_tags.ClearExpiredTags();
	}

	m_storyTags.ClearExpiredTags();
}

void StoryState::UpdateFromNode(StoryData * data)
{
	TODO("update effect nodes");
	//if (data->AreAllCharactersSet()){
	// apply specific data effects
	for(Effect* effect :data->m_action->m_definition->m_effects->m_effects){
		effect->ApplyToState(this, data);
	}
	//apply the definitions default effects
	for (Effect* effect : data->m_definition->m_guaranteedEffects->m_effects){
		effect->ApplyToState(this, data);
	}

	//apply the story effects
	for (Effect* effect : data->m_definition->m_storyEffects->m_effects){
		effect->ApplyToState(this, data);
	}

	//this is an outgoing edge - clamp the act range to the node's mins.
	m_possibleActRange.min = Max(m_possibleActRange.min, data->m_definition->m_actRange.min);

	//}
	//if (m_cost > 0.f){
	//	m_cost = -1.f;
	//} else {
	//	m_cost--;
	//}
}

void StoryState::UpdateFromNodeDefinition(StoryDataDefinition * dataDef)
{
	for (Effect* effect : dataDef->m_storyEffects->m_effects)
	{
		effect->ApplyToState(this, nullptr);
	}
	m_possibleActRange.min = Max(m_possibleActRange.min, dataDef->m_actRange.min);

}

void StoryState::PredictUpdateOnCharacter(Character * character, unsigned int indexOnNode, StoryData * node)
{
	CharacterState* stateToUpdate = GetCharacterStateForCharacter(character);
	for(Effect* effect : node->m_action->m_definition->m_effects->m_effects){
		effect->ApplyToCharacterState(stateToUpdate);
	}
}

void StoryState::PredictUpdateOnCharacterFromDefinition(Character * character, unsigned int indexOnNode, StoryDataDefinition * definition)
{
	CharacterState* stateToUpdate = GetCharacterStateForCharacter(character);
	for(Effect* effect : definition->m_guaranteedEffects->m_effects){
		effect->ApplyToCharacterState(stateToUpdate);
	}
}

float StoryState::GetCost() const
{
	return m_cost;
}

std::string StoryState::ToString() const
{
	
	//if (g_theGame->IsDevMode()){
	//	return GetDevString();
	//} 
	std::string printStr = Stringf("%3.2f\n", GetCost());
	return printStr;
}

std::string StoryState::GetDevString() const
{
	std::string printStr;
	if (m_enddata != nullptr && m_enddata->m_type != DEFAULT_NODE){
		printStr = Stringf("Tags:\n%s\nCost: %3.2f\nChance: %3.2f\n==========================\n", m_storyTags.GetTagsAsString().c_str(), GetCost(), m_enddata->m_action->m_instancedChance);
	} else {
		printStr = Stringf("Tags:\n%sCost: %3.2f\n==========================\n", m_storyTags.GetTagsAsString().c_str(), GetCost());
	}
	for(CharacterState* state : m_characterStates){
		printStr+= (state->ToString() + "\n");
	}
	return printStr;
}

CharacterState* StoryState::GetCharacterStateForCharacter(Character * character)
{
	for (CharacterState* state : m_characterStates){
		if (state->m_character->GetName() == character->GetName()){
			return state;
		}
	}
	return nullptr;
}

CharacterState* StoryState::GetCharacterStateAtIndex(int index)
{
	return m_characterStates[index];
}

CharacterState * StoryState::GetCharacterStateForCharacterIndex(int charIndex)
{
	return nullptr;
}

void StoryState::SetStartAndEnd(StoryNode * start, StoryNode * end)
{
	m_startData = start->m_data;
	m_enddata = end->m_data;
}

float StoryState::UpdateAndGetChance()
{
	if (m_enddata->m_type == DEFAULT_NODE){
		return 1.f;
	}
	return m_enddata->UpdateAndGetChance(this);
}

void StoryState::SetAsVoid()
{
	m_isVoid = true;
}
