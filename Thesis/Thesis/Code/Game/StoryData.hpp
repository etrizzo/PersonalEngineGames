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
	StoryData(tinyxml2::XMLElement* nodeElement, eNodeType type);
	StoryData(StoryData* clone);
	~StoryData();

	std::string GetName() const;
	std::string ToString() const;

	//character utilities
	bool AreAllCharactersSet() const;
	void ClearCharacters();
	void SetCharacter(int charSlot, Character* charToSet);
	unsigned int GetNumCharacters() const;
	bool DoesCharacterMeetSlotRequirementsAtEdge(Character* character, unsigned int charSlot, StoryEdge* atEdge);

	void SetPosition(Vector2 pos);
	Vector2 GetPosition() const;

	bool operator==( const StoryData& compare ) const;
	bool operator!=( const StoryData& compare ) const;

	//test bits
	std::string m_name;
	float m_value;

	//actual members
	std::string m_id;
	std::string m_action;
	std::vector<CharacterRequirementSet> m_characterReqs;
	StoryRequirements m_storyReqs;
	EffectSet m_effects;

	//generation info
	mutable std::string m_actionWithCharacters;		//mutable to update in GetString
	eNodeType m_type;
	unsigned int m_numCharacters;
	std::vector<Character*> m_characters;

	Vector2 m_graphPosition = Vector2::HALF;
};