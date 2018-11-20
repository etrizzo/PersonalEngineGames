#include "Action.hpp"

Action::Action(ActionDefinition * def)
{
	m_definition = def;
	m_instancedText = def->m_baseText;
	m_instancedChance = def->m_baseChance;
}
