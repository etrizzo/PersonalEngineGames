#pragma once
#include "Game/GameCommon.hpp"

class Character;

class TagPair{
public:
	TagPair() {};
	TagPair(std::string name, std::string value , std::string type);

	std::string GetName() const;
	std::string GetValue() const;
	std::string GetType() const;

	void SetValue(const std::string& value);

	bool HasName(std::string tagName) const;
	bool HasValue(std::string tagValue) const;


	float		ReadValueAsFloat(float failValue = -1.f);
	int			ReadValueAsInt	(int failValue = -1);
	bool		ReadValueAsBool	(bool failValue = false);
	Character*	ReadValueAsCharacter(Character* failValue = nullptr);
	
	Strings		ReadValueAsStrings();
	bool m_expires = false;


private:
	std::string m_tag;
	std::string m_tagValue;
	std::string m_type = "boolean";
	
};