#include "Game/GameCommon.hpp"

class Character{
	Character();

	void InitFromXML(tinyxml2::XMLElement* characterDefinition);
};