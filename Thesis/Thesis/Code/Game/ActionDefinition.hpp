#include "Game/GameCommon.hpp"

class EffectSet;
class ActionModifier;
class StoryDataDefinition;

class ActionDefinition{
public:
	ActionDefinition(std::string text, tinyxml2::XMLElement* effectsElement, StoryDataDefinition* parent);		//for single action bullshit
	ActionDefinition(tinyxml2::XMLElement* actionElement, StoryDataDefinition* parent);

	std::string m_baseText;
	float m_baseChance;
	float m_chanceToPlaceAction = 1.f;
	std::vector<ActionModifier*> m_modifiers = std::vector<ActionModifier*>();
	EffectSet* m_effects = nullptr;
	StoryDataDefinition* m_parentData = nullptr;
};