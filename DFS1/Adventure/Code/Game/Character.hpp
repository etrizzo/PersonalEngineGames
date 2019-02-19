#pragma once
#include "Game/GameCommon.hpp"
#include "Game/TagSet.hpp"


class Actor;

class Character{
public:
	Character();

	void InitFromXML(tinyxml2::XMLElement* characterDefinition);
	bool HasTag(TagPair tag);

	std::string GetName() const;
	void SetActor(Actor* actor);

	TagSet m_tags;
	

protected:
	Actor* m_actor = nullptr;
	std::string m_name;
	// traits

	//Actor* m_pawn = nullptr;	


};