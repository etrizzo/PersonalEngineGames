#include "StoryData.hpp"
#include "Game/Character.hpp"
#include "Game/CharacterState.hpp"
#include "Game/StoryState.hpp"

StoryData::StoryData(std::string name, float value)
{
	m_name = name;
	m_value = value;
	m_action = name;
}

StoryData::StoryData(tinyxml2::XMLElement * nodeElement, eNodeType type)
{
	tinyxml2::XMLElement* actionElement = nodeElement->FirstChildElement("Action");
	m_action = ParseXmlAttribute(*actionElement, "text", "NO_ACTION");
	TODO("Change structure of action to incorporate characters");

	m_characters = std::vector<Character*>();
	m_characterReqs = std::vector<CharacterRequirementSet>();
	m_numCharacters = 0;
	//find num characters
	Strings splitString;
	Split(m_action, '*', splitString);
	if (splitString.size() > 1){
		for (int i = 0; i < (int) splitString.size(); i++){
			if (i % 2 == 0){
				//if even index, you're inside a ** pair - parse to index
				m_numCharacters++;
			}
		}
	}
	for (int i= 0; i < m_numCharacters; i++){
		m_characters.push_back(nullptr);
		m_characterReqs.push_back(CharacterRequirementSet());
	}


	m_type = type;
}

StoryData::StoryData(StoryData * clone)
{

	//test bits
	m_name	= clone->m_name;
	m_value		= clone->m_value;

	//actual members
	m_id						= clone->m_id;
	m_action					= clone->m_action;
	m_characterReqs	= clone->m_characterReqs;
	m_storyReqs						= clone->m_storyReqs;
	m_effects						= clone->m_effects;

	m_numCharacters			= clone->m_numCharacters;
	m_characters			= std::vector<Character*>();
	m_characterReqs			= std::vector<CharacterRequirementSet>();
	for (unsigned int i = 0; i < m_numCharacters; i++){
		m_characters.push_back(nullptr);
		m_characterReqs.push_back(clone->m_characterReqs[i]);
	}

	m_graphPosition =  Vector2(GetRandomFloatInRange(.4f, .6f), GetRandomFloatInRange(.4f, .6f));
}

StoryData::~StoryData()
{
}

std::string StoryData::GetName() const
{
	if (AreAllCharactersSet()){
		if (m_actionWithCharacters == ""){
			//set the action string with the set characters
			Strings splitString;
			Split(m_action, '*', splitString);
			if (splitString.size() == 1){
				m_actionWithCharacters = m_action;
				return m_actionWithCharacters;
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
		return m_actionWithCharacters;
	}
	return m_action;
}

std::string StoryData::ToString() const
{
	return Stringf("%s  |  %f", m_name.c_str(), m_value);
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
	CharacterState* charState = atEdge->GetCost()->GetCharacterStateForCharacter(character);
	return m_characterReqs[charSlot].DoesCharacterMeetRequirements(charState);
	//return true;
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
		if (m_value == compare.m_value){
			return true;
		}
	}
	return false;
}

bool StoryData::operator!=(const StoryData & compare) const
{
	if (m_name == compare.m_name){
		if (m_value == compare.m_value){
			return false;
		}
	}
	return true;
}
