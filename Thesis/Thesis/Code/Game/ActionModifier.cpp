#include "ActionModifier.hpp"
#include "Game/CharacterRequirementSet.hpp"
#include "Game/StoryRequirementSet.hpp"
#include "Game/StoryData.hpp"
#include "Game/StoryState.hpp"

ActionModifier::ActionModifier(tinyxml2::XMLElement * actionElement, StoryDataDefinition* parent)
{
	m_characterID = ParseXmlAttribute(*actionElement, "character", -1);
	m_multiplier = ParseXmlAttribute(*actionElement, "multiplier", 1.f);

	m_parentData = parent;
	m_characterRequirements = new CharacterRequirementSet();
	m_storyRequirements = new StoryRequirementSet();
	m_characterRequirements->InitFromXML(actionElement->FirstChildElement("CharacterRequirements"), parent);
	m_storyRequirements->InitFromXML(actionElement->FirstChildElement("StoryRequirements"), parent);

}

bool ActionModifier::PassesForEdge(StoryData * parentData, StoryState * edge)
{
	//CharacterState* charToCheck = edge->GetCharacterStateForCharacter(parentData->m_characters[m_characterID]);
	bool meetsCharRequirements = m_characterRequirements->DoesCharacterMeetAllRequirements(parentData->m_characters[m_characterID], edge);
	bool meetsStoryRequirements = m_storyRequirements->DoesEdgeMeetAllRequirements(edge);
	return meetsCharRequirements && meetsStoryRequirements;
}
