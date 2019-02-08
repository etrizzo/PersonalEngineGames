#pragma once
#include "Game/GameCommon.hpp"
#include "Game/TagSet.hpp"



class Character{
public:
	Character();

	void InitFromXML(tinyxml2::XMLElement* characterDefinition);
	bool HasTag(TagPair tag);

	std::string GetName() const;


	TagSet m_tags;

protected:
	std::string m_name;
	// traits

	//Actor* m_pawn = nullptr;	


};