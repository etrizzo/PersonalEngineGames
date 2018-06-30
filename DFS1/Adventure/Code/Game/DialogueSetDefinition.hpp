#pragma once
#include "Game/GameCommon.hpp"

class Dialogue;

class DialogueSetDefinition{
public:
	DialogueSetDefinition(tinyxml2::XMLElement* dialoguesElement);

	std::string name;
	std::vector<Dialogue*> m_dialogues;
};