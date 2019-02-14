#pragma once
#include "Game/CharacterState.hpp"

class StoryData;
class StoryDataDefinition;

class StoryState{
public:
	StoryState(float cost, int numCharacters);
	StoryState(const StoryState& copy);
	StoryState(){};
	~StoryState();

	void UpdateFromNode(StoryData* data);
	void UpdateFromNodeDefinition(StoryDataDefinition* dataDef);		//only updates story state with what is concrete
	void PredictUpdateOnCharacter(Character* character, unsigned int indexOnNode, StoryData* node);
	//updates for the entire set of actions
	void PredictUpdateOnCharacterFromDefinition(Character* character, unsigned int indexOnNode, StoryDataDefinition* definition);



	float GetCost() const;
	std::string ToString() const;
	std::string GetDevString() const;
	CharacterState* GetCharacterStateForCharacter(Character* character);
	CharacterState* GetCharacterStateAtIndex(int index);
	CharacterState* GetCharacterStateForCharacterIndex(int charIndex);

	void SetStartAndEnd(StoryNode* start, StoryNode* end);
	float UpdateAndGetChance();
	void SetAsVoid();

	TagSet m_storyTags = TagSet();

	StoryData* m_startData = nullptr;
	StoryData* m_enddata = nullptr;

	float m_cost;
	std::vector<CharacterState*> m_characterStates;

	bool m_isVoid = false;
	bool m_isLocked = false;
};