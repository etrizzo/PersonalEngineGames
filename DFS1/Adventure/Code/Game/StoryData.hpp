#pragma once
#include "Game/GameCommon.hpp"
#include "Game/StoryRequirementSet.hpp"
#include "Game/CharacterRequirementSet.hpp"
#include "Game/EffectSet.hpp"
#include "Game/StoryDataDefinition.hpp"

class Character;
class StoryState;
class Action;

class StoryData{
public:
	StoryData(){};
	StoryData(StoryDataDefinition* definition, int actionIndex = -1);
	StoryData(std::string name, int defaultAct);
	StoryData( eNodeType type);
	StoryData(StoryData* clone);
	~StoryData();


	std::string GetName() const;
	std::string ToString() const;

	int GetAct() const;

	float UpdateAndGetChance(StoryState* incomingEdge);

	float GetModifierWeight(StoryState* incomingEdge);
	//state utilities
	//updates node's story state w/ outbound edges with the effects set
	void AddData(StoryData* data);

	//character utilities
	bool AreAllCharactersSet() const;
	void ClearCharacters();
	void SetCharacter(int charSlot, Character* charToSet);
	unsigned int GetNumCharacters() const;
	
	CharacterRequirementSet* GetRequirementsForCharacter(Character* character);

	void SetCharacters(std::vector<Character*> characters);
	//returns whether or not the state described is compatible with this nodes incoming requirements
	bool IsCompatibleWithIncomingEdge(StoryState* edgeState);

	Character* GetCharacterFromDataString(std::string data);
	std::string ReadCharacterNameFromDataString(std::string data);

	bool DoesNodeEndAct() const;
	eProgressionType GetProgressionType() const { return m_definition->m_progressionType; }

	void SetPosition(Vector2 pos);
	Vector2 GetPosition() const;

	std::string ParseCharacterNamesIntoString(std::string characterString) const;

	bool operator==( const StoryData& compare ) const;
	bool operator!=( const StoryData& compare ) const;

	//actual members
	std::string m_id;
	std::string m_name;
	Action* m_action;
	StoryDataDefinition* m_definition = nullptr;

	mutable std::string m_actionWithCharacters;		//mutable to update in GetString
	eNodeType m_type;
	unsigned int m_numCharacters;
	std::vector<Character*> m_characters;

	Vector2 m_graphPosition = Vector2::HALF;

	int m_defaultAct = 1;
};