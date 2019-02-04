#include "Action.hpp"
#include "ActionModifier.hpp"

Action::Action(ActionDefinition * def)
{
	m_definition = def;
	m_instancedText = def->m_baseText;
	m_instancedChance = def->m_baseChance;
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
