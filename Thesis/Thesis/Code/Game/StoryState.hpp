#pragma once
#include "Game/CharacterState.hpp"

class StoryState{
public:
	StoryState(float cost, int numCharacters);
	StoryState(){};

	float m_cost;

	float GetCost() const;
	CharacterState* GetCharacterStateForCharacter(Character* character);
	CharacterState* GetCharacterStateForCharacter(int index);

	std::vector<CharacterState*> m_characterStates;
};