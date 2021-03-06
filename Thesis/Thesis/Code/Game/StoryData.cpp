#include "StoryData.hpp"
#include "Game/Character.hpp"
#include "Game/CharacterState.hpp"
#include "Game/StoryState.hpp"
#include "Game/Action.hpp"
#include "Game/ActionModifier.hpp"


StoryData::StoryData(StoryDataDefinition * definition, int actionIndex)
{
	m_definition = definition;
	m_numCharacters = m_definition->m_numCharacters;
	for (unsigned int i = 0; i < m_definition->m_numCharacters; i++){
		m_characters.push_back(nullptr);
		//m_characterReqs.push_back(clone->m_characterReqs[i]->Clone());
		//m_characterReqs[i]->SetAllRequirementsStoryData(this);
	}
	if (actionIndex == -1){
		int actionsNum = m_definition->m_actions.size();
		m_action = new Action(m_definition->m_actions[GetRandomIntLessThan(actionsNum)]);
	} else {
		m_action = new Action(m_definition->m_actions[actionIndex]);
	}

	//TBD stuff
	m_name = m_definition->m_name;
	m_id = m_definition->m_id;
	m_type = m_definition->m_type;

	m_graphPosition =  Vector2(GetRandomFloatInRange(.4f, .7f), GetRandomFloatInRange(.4f, .7f));
}

StoryData::StoryData(std::string name)
{
	m_name = name;
	m_type = DEFAULT_NODE;
}

StoryData::StoryData( eNodeType type)
{
	m_type = type;
}

StoryData::StoryData(StoryData * clone)
{

	//test bits
	m_name	= clone->m_name;

	//actual members
	m_id						= clone->m_id;
	m_action					= clone->m_action;
	//m_characterReqs	= clone->m_characterReqs;
	//m_storyReqs						= clone->m_storyReqs;
	//m_effectSet						= new EffectSet(clone->m_effectSet, this);

	m_numCharacters			= clone->m_numCharacters;
	m_characters			= std::vector<Character*>();
	//m_characterReqs			= std::vector<CharacterRequirementSet*>();

	m_type = clone->m_type;
	for (unsigned int i = 0; i < m_numCharacters; i++){
		m_characters.push_back(nullptr);
		//m_characterReqs.push_back(clone->m_characterReqs[i]->Clone());
		//m_characterReqs[i]->SetAllRequirementsStoryData(this);
	}
	m_graphPosition =  Vector2(GetRandomFloatInRange(.4f, .6f), GetRandomFloatInRange(.4f, .6f));
}

StoryData::~StoryData()
{
}

std::string StoryData::GetName() const
{
	if (m_type == DEFAULT_NODE){
		return m_name;
	}
	if (AreAllCharactersSet()){
		if (m_actionWithCharacters == ""){
			//set the action string with the set characters
			Strings splitString;
			Split(m_action->m_definition->m_baseText, '*', splitString);
			if (splitString.size() == 1){
				return m_action->m_instancedText;
				//m_actionWithCharacters = m_action->m_instancedText;
				//return m_actionWithCharacters;
			}
			for (int i = 0; i < (int) splitString.size(); i++){
				if (i % 2 != 0){
					//if odd index, this is NOT inside *'s, so add it as usual to character string 
					m_actionWithCharacters +=  splitString[i];
				} else {
					//if even index, you're inside a ** pair - parse to index
					int charIndex = ParseStringInt(splitString[i]);
					m_actionWithCharacters += m_characters[charIndex]->GetName();
				}
			}
		}
		m_action->m_instancedText = m_actionWithCharacters;
		return m_actionWithCharacters;
	}
	return m_action->m_instancedText;
}

std::string StoryData::ToString() const
{
	if (m_type == DEFAULT_NODE){
		return m_name;
	}
	return m_action->m_instancedText;
}

float StoryData::UpdateAndGetChance(StoryState * incomingEdge)
{
	m_action->SetChanceFromEdge(this, incomingEdge);
	return m_action->m_instancedChance;
}

//void StoryData::UpdateState(EffectSet * effects)
//{
//	for (StoryEdge* edge 
//}

void StoryData::AddData(StoryData * data)
{
}

bool StoryData::AreAllCharactersSet() const
{
	for (Character* character : m_characters){
		if (character == nullptr){
			return nullptr;
		}
	}
	return true;
}

void StoryData::ClearCharacters()
{
	for (int i = 0; i < (int) m_characters.size(); i++){
		m_characters[i] = nullptr;
	}
}

void StoryData::SetCharacter(int charSlot, Character * charToSet)
{
	ASSERT_OR_DIE(charSlot < (int) m_characters.size(), "Attempted to set character out of index");
	m_characters[charSlot] = charToSet;
}

unsigned int StoryData::GetNumCharacters() const
{
	return m_numCharacters;
}

bool StoryData::DoesCharacterMeetSlotRequirementsAtEdge(Character * character, unsigned int charSlot, StoryEdge * atEdge)
{
	TODO("Implement checks on character requirements at edge.");
	//maybe edge needs character states?
	StoryState* edgeState = atEdge->GetCost();
	StoryState* resultState = new StoryState(*edgeState);
	resultState->PredictUpdateOnCharacter(character, charSlot, this);
	//resultState->UpdateFromNode(this);		//apply the potential new node's effects to see if the result still fits the graph

	CharacterState* charState = edgeState->GetCharacterStateForCharacter(character);
	CharacterState* resultingState = resultState->GetCharacterStateForCharacter(character);
	
	//check if the change would fit on this edge
	bool meetsExistingConditions = m_definition->m_characterReqs[charSlot]->DoesCharacterMeetAllRequirements(character, edgeState);
	if (meetsExistingConditions){
		//check if the change would fuck up future nodes
		StoryData* endData = atEdge->GetEnd()->m_data;
		//if this character has future requirements, have to meet them
		//NOTE: this should maybe be a recursive call to DoesCharacterMeetSlotRequirements?
		CharacterRequirementSet* charReqs = endData->GetRequirementsForCharacter(character);
		if (charReqs != nullptr){
			bool meetsFutureConditions = charReqs->DoesCharacterMeetAllRequirements(character, resultState);
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

CharacterRequirementSet* StoryData::GetRequirementsForCharacter(Character * character)
{
	if (m_definition != nullptr){
		for (int i = 0; i < m_definition->m_characterReqs.size(); i++){
			if (m_characters[i] == character){
				return  m_definition->m_characterReqs[i];
			}
		}
	}
	return nullptr;
	
}

void StoryData::SetCharacters(std::vector<Character*> characters)
{
	for (int i = 0; i < characters.size(); i++){
		SetCharacter(i, characters[i]);
	}
}

bool StoryData::IsCompatibleWithIncomingEdge(StoryState * edgeState)
{
	//check the character requirements
	for (CharacterState* charState : edgeState->m_characterStates){
		Character* character = charState->m_character;
		CharacterRequirementSet* reqs = GetRequirementsForCharacter(character);
		if (reqs != nullptr){
			//if the edges' character has requirements already established in this node, check them.
			if (!reqs->DoesCharacterMeetAllRequirements(character, edgeState)){
				//if any character doesn't meet the requirements, return false.
				return false;
			}
		}
	}

	if (m_definition != nullptr){
		//check the story requirements
		StoryRequirementSet* storyReqs = m_definition->m_storyReqs;
		if (!storyReqs->DoesEdgeMeetAllRequirements(edgeState)){
			return false;
		}
	}
	//if no characters violated requirements, return true.
	return true;
}

Character* StoryData::GetCharacterFromDataString(std::string data)
{
	 Strip(data, '*');
	 int i = atoi(data.c_str());
	 if (i == 0 && (data != "0" && data != "0*")){
		 return nullptr;
	 }
	 if (i >= 0 && i < m_characters.size()){
		 return m_characters[i];
	 } else {
		 return nullptr;
	 }
}

std::string StoryData::ReadCharacterNameFromDataString(std::string data)
{
	if (data == "none"){
		return data;
	}

	Character* character = GetCharacterFromDataString(data);
	if (character != nullptr){
		return character->GetName();
	} else {
		return "NO_CHARACTER_FOUND";
	}
}

void StoryData::SetPosition(Vector2 pos)
{
	m_graphPosition = pos;
}

Vector2 StoryData::GetPosition() const
{
	return m_graphPosition;
}



bool StoryData::operator==(const StoryData & compare) const
{
	if (m_name == compare.m_name){
		return true;
	}
	return false;
}

bool StoryData::operator!=(const StoryData & compare) const
{
	if (m_name == compare.m_name){
		return false;
		
	}
	return true;
}
