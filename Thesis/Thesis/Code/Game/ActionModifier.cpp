#include "ActionModifier.hpp"
#include "Game/CharacterRequirementSet.hpp"
#include "Game/StoryData.hpp"
#include "Game/StoryState.hpp"

ActionModifier::ActionModifier(tinyxml2::XMLElement * actionElement, StoryDataDefinition* parent)
{
	m_characterID = ParseXmlAttribute(*actionElement, "character", -1);
	m_multiplier = ParseXmlAttribute(*actionElement, "multiplier", 1.f);

	m_parentData = parent;
	m_requirements = new CharacterRequirementSet();
	m_requirements->InitFromXML(actionElement->FirstChildElement("CharacterRequirements"), parent);

}

bool ActionModifier::PassesForEdge(StoryData * parentData, StoryState * edge)
{
	CharacterState* charToCheck = edge->GetCharacterStateForCharacter(parentData->m_characters[m_characterID]);
	bool meetsRequirements = m_requirements->DoesCharacterMeetAllRequirements(charToCheck);
	return meetsRequirements;
}
