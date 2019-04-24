#include "Action.hpp"
#include "ActionModifier.hpp"

Action::Action(ActionDefinition * def)
{
	m_definition = def;
	m_instancedText = def->m_baseText;
	m_instancedChance = def->m_baseChance;
}

bool Action::DoesActionEndAct() const
{
	return m_definition->m_endsAct;
}

void Action::SetChanceFromEdge(StoryData * parentData, StoryState * edge)
{
	m_instancedChance = m_definition->m_baseChance;
	for (ActionModifier* modifier : m_definition->m_modifiers){
		if (modifier->PassesForEdge(parentData, edge)){
			m_instancedChance *= modifier->m_multiplier;
		}
	}
}

float Action::GetModifierWeightFromEdge(StoryData * parentData, StoryState * edge)
{
	float modifierWeight = 1.f;
	for (ActionModifier* modifier : m_definition->m_modifiers) {
		if (modifier->PassesForEdge(parentData, edge)) {
			modifierWeight *= modifier->m_multiplier;
		}
	}
	return modifierWeight;
}
