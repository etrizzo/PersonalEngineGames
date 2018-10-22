#pragma once
#include "Game/CharacterState.hpp"

class StoryData;

class StoryState{
public:
	StoryState(float cost, int numCharacters);
	StoryState(const StoryState& copy);
	StoryState(){};

	void UpdateFromNode(StoryData* data);
	void PredictUpdateOnCharacter(Character* character, unsigned int indexOnNode, StoryData* node);



	float GetCost() const;
	std::string ToString() const;
	CharacterState* GetCharacterStateForCharacter(Character* character);
	CharacterState* GetCharacterStateAtIndex(int index);
	CharacterState* GetCharacterStateForCharacterIndex(int charIndex);

	float m_cost;
	std::vector<CharacterState*> m_characterStates;
};