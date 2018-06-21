#pragma once
#include "Game/EntityDefinition.hpp"



class DecorationDefinition: public EntityDefinition{
public:
	DecorationDefinition(tinyxml2::XMLElement* itemElement);
	~DecorationDefinition();

	bool m_allowsWalking	= false;
	bool m_allowsSwimming	= false;
	bool m_allowsFlying		= false;

	static std::map< std::string, DecorationDefinition* >		s_definitions;
	static DecorationDefinition* GetDecorationDefinition(std::string definitionName);

};