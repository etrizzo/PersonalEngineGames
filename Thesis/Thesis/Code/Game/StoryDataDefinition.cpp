#include "StoryDataDefinition.hpp"
#include "Game/Character.hpp"
#include "Game/CharacterState.hpp"
#include "Game/StoryState.hpp"
#include "Game/ActionDefinition.hpp"
#include "Game/StoryData.hpp"

//StoryDataDefinition::StoryDataDefinition(std::string name, float value)
//{
//	m_name = name;
//	m_value = value;
//	//m_action = name;
//}

StoryDataDefinition::StoryDataDefinition( eNodeType type)
{
	m_type = type;
}

//StoryDataDefinition::StoryDataDefinition(StoryDataDefinition * clone)
//{
//
//	//test bits
//	m_name	= clone->m_name;
//	m_value		= clone->m_value;
//
//	//actual members
//	m_id						= clone->m_id;
//	m_actions					= clone->m_actions;
//	//m_characterReqs	= clone->m_characterReqs;
//	//m_storyReqs						= clone->m_storyReqs;
//	//m_effectSet						= new EffectSet(clone->m_effectSet, this);
//
//	m_numCharacters			= clone->m_numCharacters;
//	m_characters			= std::vector<Character*>();
//	//m_characterReqs			= std::vector<CharacterRequirementSet*>();
//
//	m_type = clone->m_type;
//	for (unsigned int i = 0; i < m_numCharacters; i++){
//		m_characters.push_back(nullptr);
//		//m_characterReqs.push_back(clone->m_characterReqs[i]->Clone());
//		//m_characterReqs[i]->SetAllRequirementsStoryDataDefinition(this);
//	}
//	m_graphPosition =  Vector2(GetRandomFloatInRange(.4f, .6f), GetRandomFloatInRange(.4f, .6f));
//}

StoryDataDefinition::~StoryDataDefinition()
{
}

void StoryDataDefinition::InitFromXML(tinyxml2::XMLElement* nodeElement)
{
	if (m_type == DETAIL_NODE){
		InitAsDetailNode(nodeElement);
		//InitAsDetailNode(nodeElement);
	} else if (m_type == PLOT_NODE){
		InitAsPlotNode(nodeElement);
	} else {
		ERROR_AND_DIE("Node initialized without type.");
	}
}

void StoryDataDefinition::InitAsDetailNode(tinyxml2::XMLElement * nodeElement)
{
	m_characters = std::vector<Character*>();
	m_actions = std::vector<ActionDefinition*>();
	//m_characterReqs=std::vector<CharacterRequirementSet*>();
	m_numCharacters = 0;

	m_chanceToPlaceData = ParseXmlAttribute(*nodeElement, "chanceToPlace", 1.f);

	//parse characters
	tinyxml2::XMLElement* charactersElement = nodeElement->FirstChildElement("Characters");
	for (tinyxml2::XMLElement* charElement = charactersElement->FirstChildElement("Character"); charElement != nullptr; charElement = charElement->NextSiblingElement("Character")){
		m_numCharacters++;
	}

	//fill data with empty stuff
	for (int i= 0; i < m_numCharacters; i++){
		m_characters.push_back(nullptr);
		m_characterReqs.push_back(new CharacterRequirementSet());
	}

	//fill character requirements with actual data that's available
	tinyxml2::XMLElement* charReqElement = nodeElement->FirstChildElement("CharacterRequirements");
	for (tinyxml2::XMLElement* reqElement = charReqElement->FirstChildElement("Character"); reqElement != nullptr; reqElement = reqElement->NextSiblingElement("Character")){
		int charIndex = (unsigned int) ParseXmlAttribute(*reqElement, "index", (int) -1);
		if (charIndex >= 0 && charIndex < m_characterReqs.size()){
			m_characterReqs[charIndex]->InitFromXML(reqElement, this);
		}
	}

	//create guaranteed effect set
	tinyxml2::XMLElement* guaranteedElement = nodeElement->FirstChildElement("Guaranteed");
	if (guaranteedElement != nullptr){
		m_guaranteedEffects = new EffectSet(guaranteedElement->FirstChildElement("EffectSet"), this);
	} else {
		m_guaranteedEffects = new EffectSet((tinyxml2::XMLElement*) nullptr, this);
	}

	//create action definitions
	tinyxml2::XMLElement* actionsElement = nodeElement->FirstChildElement("Actions");
	for (tinyxml2::XMLElement* actionElement = actionsElement->FirstChildElement("Action"); actionElement != nullptr; actionElement = actionElement->NextSiblingElement("Action")){
		ActionDefinition* actionDef = new ActionDefinition(actionElement, this);
		m_actions.push_back(actionDef);
	}

	tinyxml2::XMLElement* storyEffects = nodeElement->FirstChildElement("StoryEffects");
	if (storyEffects != nullptr){
		m_storyEffects = new EffectSet(storyEffects->FirstChildElement("EffectSet"), this);
	} else {
		m_storyEffects = new EffectSet((tinyxml2::XMLElement*) nullptr, this);
	}
}

void StoryDataDefinition::InitAsPlotNode(tinyxml2::XMLElement * nodeElement)
{
	tinyxml2::XMLElement* actionElement = nodeElement->FirstChildElement("Action");
	std::string actionText = ParseXmlAttribute(*actionElement, "text", "NO_ACTION");

	m_chanceToPlaceData = ParseXmlAttribute(*nodeElement, "chanceToPlace", 1.f);

	TODO("Pull num characters from character table in data")
	m_characters = std::vector<Character*>();
	m_characterReqs = std::vector<CharacterRequirementSet*>();
	m_numCharacters = 0;
	//find num characters
	Strings splitString;
	Split(actionText, '*', splitString);
	if (splitString.size() > 1){
		for (int i = 0; i < (int) splitString.size(); i++){
			if (i % 2 == 0){
				//if even index, you're inside a ** pair - parse to index
				m_numCharacters++;
			}
		}
	}
	//fill data with empty stuff
	for (int i= 0; i < m_numCharacters; i++){
		m_characters.push_back(nullptr);
		m_characterReqs.push_back(new CharacterRequirementSet());
	}

	//fill character requirements with actual data that's available
	tinyxml2::XMLElement* charReqElement = nodeElement->FirstChildElement("CharacterRequirements");
	for (tinyxml2::XMLElement* reqElement = charReqElement->FirstChildElement("Character"); reqElement != nullptr; reqElement = reqElement->NextSiblingElement("Character")){
		int charIndex = (unsigned int) ParseXmlAttribute(*reqElement, "index", (int) -1);
		if (charIndex >= 0 && charIndex < m_characterReqs.size()){
			m_characterReqs[charIndex]->InitFromXML(reqElement, this);
		}
	}



	//parse effects
	tinyxml2::XMLElement* allCharacterEffects = nodeElement->FirstChildElement("CharacterEffects");
	ActionDefinition* actionDef = new ActionDefinition(actionText, allCharacterEffects, this);
	m_actions.push_back(actionDef);
	//m_effectSet = new EffectSet(allEffects->FirstChildElement("EffectSet"), this);
	
	m_guaranteedEffects = new EffectSet(allCharacterEffects, this);

	tinyxml2::XMLElement* storyEffects = nodeElement->FirstChildElement("StoryEffects");
	if (storyEffects != nullptr){
		m_storyEffects = new EffectSet(storyEffects->FirstChildElement("EffectSet"), this);
	} else {
		m_storyEffects = new EffectSet((tinyxml2::XMLElement*) nullptr, this);
	}
	
}

std::string StoryDataDefinition::GetName() const
{
	//if (AreAllCharactersSet()){
	//	if (m_actionWithCharacters == ""){
	//		//set the action string with the set characters
	//		Strings splitString;
	//		Split(m_action, '*', splitString);
	//		if (splitString.size() == 1){
	//			m_actionWithCharacters = m_action;
	//			return m_actionWithCharacters;
	//		}
	//		for (int i = 0; i < (int) splitString.size(); i++){
	//			if (i % 2 != 0){
	//				//if odd index, this is NOT inside *'s, so add it as usual to character string 
	//				m_actionWithCharacters +=  splitString[i];
	//			} else {
	//				//if even index, you're inside a ** pair - parse to index
	//				int charIndex = ParseStringInt(splitString[i]);
	//				m_actionWithCharacters += m_characters[charIndex]->GetName();
	//			}
	//		}
	//	}
	//	return m_actionWithCharacters;
	//}
	std::string text = "";
	for (ActionDefinition* actionDef : m_actions){
		text+=actionDef->m_baseText;
		text+="\n";
	}
	return text;
}

std::string StoryDataDefinition::ToString() const
{
	std::string text = "";
	for (ActionDefinition* actionDef : m_actions){
		text+=actionDef->m_baseText;
		text+="\n";
	}
	return text;
}

//void StoryDataDefinition::UpdateState(EffectSet * effects)
//{
//	for (StoryEdge* edge 
//}

void StoryDataDefinition::AddData(StoryDataDefinition * data)
{
}



bool StoryDataDefinition::DoesCharacterMeetSlotRequirementsAtEdge(Character * character, unsigned int charSlot, StoryEdge * atEdge)
{
	TODO("Implement checks on character requirements at edge.");
	//maybe edge needs character states?
	StoryState* edgeState = atEdge->GetCost();
	StoryState* resultState = new StoryState(*edgeState);
	resultState->PredictUpdateOnCharacterFromDefinition(character, charSlot, this);
	//resultState->UpdateFromNode(this);		//apply the potential new node's effects to see if the result still fits the graph

	CharacterState* charState = edgeState->GetCharacterStateForCharacter(character);
	CharacterState* resultingState = resultState->GetCharacterStateForCharacter(character);

	//check if the change would fit on this edge
	bool meetsExistingConditions = m_characterReqs[charSlot]->DoesCharacterMeetAllRequirements(charState);
	if (meetsExistingConditions){
		//check if the change would fuck up future nodes
		StoryData* endData = atEdge->GetEnd()->m_data;
		//if this character has future requirements, have to meet them
		//NOTE: this should maybe be a recursive call to DoesCharacterMeetSlotRequirements?
		CharacterRequirementSet* charReqs = endData->GetRequirementsForCharacter(character);
		if (charReqs != nullptr){
			bool meetsFutureConditions = charReqs->DoesCharacterMeetAllRequirements(resultingState);
			if (meetsFutureConditions){
				delete resultingState;
				return true;
			}
		} else {
			//if there are no requirements for this character on the end node, it's chill
			delete resultingState;
			return true;
		}
	}

	return false;
	//return true;
}

unsigned int StoryDataDefinition::GetNumCharacters() const
{
	return m_numCharacters;
}

//bool StoryDataDefinition::DoesCharacterMeetSlotRequirementsAtEdge(Character * character, unsigned int charSlot, StoryEdge * atEdge)
//{
//	TODO("Implement checks on character requirements at edge.");
//	//maybe edge needs character states?
//	StoryState* edgeState = atEdge->GetCost();
//	StoryState* resultState = new StoryState(*edgeState);
//	resultState->PredictUpdateOnCharacter(character, charSlot, this);
//	//resultState->UpdateFromNode(this);		//apply the potential new node's effects to see if the result still fits the graph
//
//	CharacterState* charState = edgeState->GetCharacterStateForCharacter(character);
//	CharacterState* resultingState = resultState->GetCharacterStateForCharacter(character);
//
//	//check if the change would fit on this edge
//	bool meetsExistingConditions = m_characterReqs[charSlot]->DoesCharacterMeetRequirements(charState);
//	if (meetsExistingConditions){
//		//check if the change would fuck up future nodes
//		StoryDataDefinition* endData = atEdge->GetEnd()->m_data;
//		//if this character has future requirements, have to meet them
//		//NOTE: this should maybe be a recursive call to DoesCharacterMeetSlotRequirements?
//		CharacterRequirementSet* charReqs = endData->GetRequirementsForCharacter(character);
//		if (charReqs != nullptr){
//			bool meetsFutureConditions = charReqs->DoesCharacterMeetRequirements(resultingState);
//			if (meetsFutureConditions){
//				delete resultingState;
//				return true;
//			}
//		} else {
//			//if there are no requirements for this character on the end node, it's chill
//			delete resultingState;
//			return true;
//		}
//	}
//
//	return false;
//	//return true;
//}

CharacterRequirementSet* StoryDataDefinition::GetRequirementsForCharacter(Character * character)
{
	for (int i = 0; i < m_characterReqs.size(); i++){
		if (m_characters[i] == character){
			return m_characterReqs[i];
		}
	}
	return nullptr;

}

float StoryDataDefinition::GetEdgeFitness(StoryState * edgeState)
{
	return m_storyReqs->GetEdgeFitness(edgeState);
}

bool StoryDataDefinition::DoesEdgeMeetStoryRequirements(StoryState * edgeState)
{
	return m_storyReqs->DoesEdgeMeetAllRequirements(edgeState);
}

Character* StoryDataDefinition::GetCharacterFromDataString(std::string data)
{
	Strip(data, '*');
	int i = atoi(data.c_str());
	if (i >= 0 && i < m_characters.size()){
		return m_characters[i];
	} else {
		return nullptr;
	}
}

std::string StoryDataDefinition::ReadCharacterNameFromDataString(std::string data)
{
	Character* character = GetCharacterFromDataString(data);
	if (character != nullptr){
		return character->GetName();
	} else {
		return "NO_CHARACTER_FOUND";
	}
}

void StoryDataDefinition::SetPosition(Vector2 pos)
{
	m_graphPosition = pos;
}

Vector2 StoryDataDefinition::GetPosition() const
{
	return m_graphPosition;
}



bool StoryDataDefinition::operator==(const StoryDataDefinition & compare) const
{
	if (m_name == compare.m_name){
		if (m_value == compare.m_value){
			return true;
		}
	}
	return false;
}

bool StoryDataDefinition::operator!=(const StoryDataDefinition & compare) const
{
	if (m_name == compare.m_name){
		if (m_value == compare.m_value){
			return false;
		}
	}
	return true;
}
