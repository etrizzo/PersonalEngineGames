#pragma once
#include "Game/GameCommon.hpp"

class Character;

class TagPair{
public:
	TagPair() {};
	TagPair(std::string name, std::string value = "true");

	std::string GetName();
	std::string GetValue();

	bool HasName(std::string tagName);
	bool HasValue(std::string tagValue);


	float		ReadValueAsFloat(float failValue = -1.f);
	int			ReadValueAsInt	(int failValue = -1);
	bool		ReadValueAsBool	(bool failValue = false);
	Character*	ReadValueAsCharacter(Character* failValue = nullptr);
	
	Strings		ReadValueAsStrings();
	


private:
	std::string m_tag;
	std::string m_tagValue;
};