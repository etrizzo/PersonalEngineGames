#pragma once
#include "Game/GameCommon.hpp"
#include "Game/StoryRequirementSet.hpp"
#include "Game/CharacterRequirementSet.hpp"
#include "Game/EffectSet.hpp"

class Character;
class StoryData;
class StoryState;
class ActionDefinition;



class StoryDataDefinition{
public:
	StoryDataDefinition(){};
	StoryDataDefinition( eNodeType type);
	//StoryDataDefinition(StoryDataDefinition* clone);
	~StoryDataDefinition();

	void InitFromXML(tinyxml2::XMLElement* nodeElement);
	void InitAsOutcomeNode(tinyxml2::XMLElement* nodeElement);
	void InitAsEventNode(tinyxml2::XMLElement* nodeElement);

	std::string GetName() const;
	std::string ToString() const;

	//state utilities
	//updates node's story state w/ outbound edges with the effects set
	void AddData(StoryDataDefinition* data);

	//character utilities
	bool DoesCharacterMeetSlotRequirementsAtEdge(Character* character, unsigned int charSlot, StoryEdge* atEdge);
	unsigned int GetNumCharacters() const;
	//bool DoesCharacterMeetSlotRequirementsAtEdge(Character* character, unsigned int charSlot, StoryEdge* atEdge);
	CharacterRequirementSet* GetRequirementsForCharacter(Character* character);

	//story utilities
	float GetEdgeFitness(StoryState* edgeState);
	bool DoesEdgeMeetStoryRequirements(StoryState* edgeState);

	Character* GetCharacterFromDataString(std::string data);
	std::string ReadCharacterNameFromDataString(std::string data);

	void SetPosition(Vector2 pos);
	Vector2 GetPosition() const;

	bool operator==( const StoryDataDefinition& compare ) const;
	bool operator!=( const StoryDataDefinition& compare ) const;


	//actual members
	std::string m_id;
	//vector of character requirements - 1 set for each character on the node.
	std::vector<CharacterRequirementSet*> m_characterReqs;
	StoryRequirementSet* m_storyReqs;
	//StoryRequirements m_storyReqs;
	//EffectSet* m_effectSet;
	EffectSet* m_guaranteedEffects = nullptr;
	EffectSet* m_storyEffects	= nullptr;
	std::vector<ActionDefinition*> m_actions;

	float m_chanceToPlaceData = 1.f;
	bool m_shouldLockIncomingEdge = false;
	




	//test bits
	std::string m_name;
	float m_value;


	//generation info
	//mutable std::string m_actionWithCharacters;		//mutable to update in GetString
	eNodeType m_type;
	unsigned int m_numCharacters;
	std::vector<Character*> m_characters;

	Vector2 m_graphPosition = Vector2::HALF;
};