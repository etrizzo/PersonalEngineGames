#include "ActionDefinition.hpp"
#include "Game/EffectSet.hpp"
#include "Game/ActionModifier.hpp"

ActionDefinition::ActionDefinition(std::string text, tinyxml2::XMLElement* effectsElement, StoryDataDefinition* parent)
{
	m_baseText = text;
	m_baseChance = 1.f;
	m_chanceToPlaceAction = 1.f;
	m_parentData = parent;
	m_modifiers = std::vector<ActionModifier*>();
	if (effectsElement != nullptr){
		m_effects = new EffectSet(effectsElement->FirstChildElement("EffectSet"), parent);
	} else {
		m_effects = new EffectSet((tinyxml2::XMLElement*) nullptr, parent);
	}
}

ActionDefinition::ActionDefinition(tinyxml2::XMLElement * actionElement, StoryDataDefinition* parent)
{
	m_parentData = parent;
	m_baseText = ParseXmlAttribute(*actionElement, "text", "NO_TEXT");
	m_baseChance = ParseXmlAttribute(*actionElement, "chance", 1.f);
	m_endsAct = ParseXmlAttribute(*actionElement, "endsAct", false);
	m_chanceToPlaceAction = ParseXmlAttribute(*actionElement, "chanceToPlace", 1.f);

	for (tinyxml2::XMLElement* modifierElement = actionElement->FirstChildElement("Modifier"); modifierElement != nullptr; modifierElement = modifierElement->NextSiblingElement("Modifier")){
		ActionModifier* newMod = new ActionModifier(modifierElement, m_parentData);
		m_modifiers.push_back(newMod);
	}
	
	tinyxml2::XMLElement* effectsElement = actionElement->FirstChildElement("EffectSet");
	if (effectsElement != nullptr){
		m_effects = new EffectSet(effectsElement, parent);
	} else {
		m_effects = new EffectSet();
	}
}
