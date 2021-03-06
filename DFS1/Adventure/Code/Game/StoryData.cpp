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

StoryData::StoryData(std::string name, int defaultAct)
{
	m_name = name;
	m_type = DEFAULT_NODE;

	m_defaultAct = defaultAct;
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
			m_actionWithCharacters = ParseCharacterNamesIntoString(m_action->m_definition->m_baseText);
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

int StoryData::GetAct() const
{
	if (m_definition != nullptr)
	{	
		return m_definition->m_actRange.min;
	} else {
		return m_defaultAct;
	}
}

float StoryData::UpdateAndGetChance(StoryState * incomingEdge)
{
	m_action->SetChanceFromEdge(this, incomingEdge);
	return m_action->m_instancedChance;
}

float StoryData::GetModifierWeight(StoryState * incomingEdge)
{
	return m_action->GetModifierWeightFromEdge(this, incomingEdge);
}

//void StoryData::UpdateState(EffectSet * effects)
//{
//	for (StoryEdge* edge 
//}

void StoryData::AddData(StoryData * data)
{
	//this function exists to be compatible with directed graph template
	UNUSED(data);
}

bool StoryData::AreAllCharactersSet() const
{
	for (Character* character : m_characters){
		if (character == nullptr){
			return false;
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

CharacterRequirementSet* StoryData::GetRequirementsForCharacter(Character * character)
{
	if (m_definition != nullptr){
		for (int i = 0; i < (int) m_definition->m_characterReqs.size(); i++){
			if (m_characters[i] == character){
				return  m_definition->m_characterReqs[i];
			}
		}
	}
	return nullptr;
	
}

void StoryData::SetCharacters(std::vector<Character*> characters)
{
	for (int i = 0; i < (int) characters.size(); i++){
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
	 if (i >= 0 && i < (int) m_characters.size()){
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

bool StoryData::DoesNodeEndAct() const
{
	if (m_type == DEFAULT_NODE)
	{
		return false;
	}
	return m_action->DoesActionEndAct();
}

void StoryData::SetPosition(Vector2 pos)
{
	m_graphPosition = pos;
}

Vector2 StoryData::GetPosition() const
{
	return m_graphPosition;
}

std::string StoryData::ParseCharacterNamesIntoString(std::string characterString) const
{
	std::string retString = "";
	//set the action string with the set characters
	Strings splitString;
	Split(characterString, '*', splitString);
	if (splitString.size() == 1) {
		return characterString;
		//m_actionWithCharacters = m_action->m_instancedText;
		//return m_actionWithCharacters;
	}
	
	for (int i = 0; i < (int) splitString.size(); i++)
	{
		if (ContainsNumber(splitString[i]))
		{
			int charIndex = ParseStringInt(splitString[i]);
			retString += m_characters[charIndex]->GetName();
		} else
		{
			retString += splitString[i];
		}

	}

	/*
	//if you have an even number of splits, you're starting with a name
	if (splitString.size() % 2 == 0)
	{
		for (int i = 0; i < (int)splitString.size(); i++) {
			if (i % 2 != 0) {
				//if odd index, this is NOT inside *'s, so add it as usual to character string 
				retString += splitString[i];
			}
			else {
				//if even index, you're inside a ** pair - parse to index
				int charIndex = ParseStringInt(splitString[i]);
				retString += m_characters[charIndex]->GetName();
			}
		}
	} else //if you have an odd number of splits, you're starting with text
	{
		for (int i = 0; i < (int)splitString.size(); i++) {
			if (i % 2 == 0) {
				//if even index, this is NOT inside *'s, so add it as usual to character string 
				retString += splitString[i];
			}
			else {
				//if odd index, you're inside a ** pair - parse to index
				int charIndex = ParseStringInt(splitString[i]);
				retString += m_characters[charIndex]->GetName();
			}
		}

	}
	*/
	return retString;
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
