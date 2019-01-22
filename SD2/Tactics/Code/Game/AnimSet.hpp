#pragma once
#include "Game/GameCommon.hpp"

class AnimSet{
public:
	AnimSet(tinyxml2::XMLElement *animSetElement);
	std::string GetAnimName( std::string name );

	std::map<std::string, std::string> m_animsBySetID;

	static std::map<std::string, AnimSet*> s_animSets;
	static AnimSet* GetAnimSet(std::string name);
};