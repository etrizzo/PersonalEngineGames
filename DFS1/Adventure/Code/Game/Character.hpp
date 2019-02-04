#pragma once
#include "Game/GameCommon.hpp"
#include "Game/TagSet.hpp"



class Character{
public:
	Character();

	void InitFromXML(tinyxml2::XMLElement* characterDefinition);
	bool HasTag(TagPair tag);

	std::string GetName() const;

	void AddExtroversion (float val);

	TagSet m_tags;

protected:
	std::string m_name;
	// traits
	float m_social;			// Introversion --- Extroversion
	float m_information;	//		Sensing --- Intuition
	float m_decisions;		//		feeling --- thinking
	float m_structure;		//		judging --- perceiving


};