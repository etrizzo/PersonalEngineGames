#pragma once
#include "Game/GameCommon.hpp"
#include "Game/StoryRequirements.hpp"
#include "Game/CharacterRequirementSet.hpp"
#include "Game/EffectSet.hpp"

class Character;
class StoryData;
class StoryState;

typedef Node<StoryData*, StoryState*> StoryNode ;
typedef DirectedEdge<StoryData*, StoryState*> StoryEdge;

enum eNodeType{
	PLOT_NODE,
	DETAIL_NODE,
	NUM_NODE_TYPES
};

class StoryData{
public:
	StoryData(){};
	StoryData(std::string name, float value);
	StoryData( eNodeType type);
	StoryData(StoryData* clone);
	~StoryData();

	void InitFromXML(tinyxml2::XMLElement* nodeElement);
	void InitAsDetailNode(tinyxml2::XMLElement* nodeElement);
	void InitAsPlotNode(tinyxml2::XMLElement* nodeElement);

	std::string GetName() const;
	std::string ToString() const;

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

	Character* GetCharacterFromDataString(std::string data);
	std::string ReadCharacterNameFromDataString(std::string data);

	void SetPosition(Vector2 pos);
	Vector2 GetPosition() const;

	bool operator==( const StoryData& compare ) const;
	bool operator!=( const StoryData& compare ) const;


	//actual members
	std::string m_id;
	std::string m_action;
	Strings m_actions;		//detail nodes 4 now
	Strings m_actionsWithCharacters;	//detail nodes 4 now
	std::vector<CharacterRequirementSet*> m_characterReqs;
	StoryRequirements m_storyReqs;
	EffectSet* m_effectSet;

	//test bits
	std::string m_name;
	float m_value;

	//generation info
	mutable std::string m_actionWithCharacters;		//mutable to update in GetString
	eNodeType m_type;
	unsigned int m_numCharacters;
	std::vector<Character*> m_characters;

	Vector2 m_graphPosition = Vector2::HALF;
};