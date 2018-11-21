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
	StoryData(std::string name);
	StoryData( eNodeType type);
	StoryData(StoryData* clone);
	~StoryData();


	std::string GetName() const;
	std::string ToString() const;

	float UpdateAndGetChance(StoryState* incomingEdge);

	//state utilities
	//updates node's story state w/ outbound edges with the effects set
	void AddData(StoryData* data);

	//character utilities
	bool AreAllCharactersSet() const;
	void ClearCharacters();
	void SetCharacter(int charSlot, Character* charToSet);
	unsigned int GetNumCharacters() const;
	bool DoesCharacterMeetSlotRequirementsAtEdge(Character* character, unsigned int charSlot, StoryEdge* atEdge);
	CharacterRequirementSet* GetRequirementsForCharacter(Character* character);

	void SetCharacters(std::vector<Character*> characters);
	//returns whether or not the state described is compatible with this nodes incoming requirements
	bool IsCompatibleWithIncomingEdge(StoryState* edgeState);

	Character* GetCharacterFromDataString(std::string data);
	std::string ReadCharacterNameFromDataString(std::string data);

	void SetPosition(Vector2 pos);
	Vector2 GetPosition() const;

	bool operator==( const StoryData& compare ) const;
	bool operator!=( const StoryData& compare ) const;

	//actual members
	std::string m_id;
	std::string m_name;
	Action* m_action;
	StoryDataDefinition* m_definition = nullptr;





	//Strings m_actions;		//detail nodes 4 now
	//Strings m_actionsWithCharacters;	//detail nodes 4 now
	//std::vector<CharacterRequirementSet*> m_characterReqs;
	//StoryRequirements m_storyReqs;
	//EffectSet* m_effectSet;

	//test bits

	//float m_value;

	//generation info

	mutable std::string m_actionWithCharacters;		//mutable to update in GetString
	eNodeType m_type;
	unsigned int m_numCharacters;
	std::vector<Character*> m_characters;

	Vector2 m_graphPosition = Vector2::HALF;
};