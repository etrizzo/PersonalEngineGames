#include "StoryState.hpp"
#include "Game/Game.hpp"

StoryState::StoryState(float cost, int numCharacters)
{
	m_cost = cost;
	m_characterStates = std::vector<CharacterState*>();
	
	for (int i = 0; i < numCharacters; i++){
		m_characterStates.push_back(new CharacterState(g_theGame->m_graph.GetCharacter(i)));
	}
}

float StoryState::GetCost() const
{
	return m_cost;
}

CharacterState* StoryState::GetCharacterStateForCharacter(Character * character)
{
	for (CharacterState* state : m_characterStates){
		if (state->m_character = character){
			return state;
		}
	}
}

CharacterState* StoryState::GetCharacterStateForCharacter(int index)
{
	return m_characterStates[index];
}
