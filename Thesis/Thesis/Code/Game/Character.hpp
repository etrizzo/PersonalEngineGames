#include "Game/GameCommon.hpp"

class Character{
public:
	Character();

	void InitFromXML(tinyxml2::XMLElement* characterDefinition);

	void AddExtroversion (float val);

protected:
	std::string m_name;
	// traits
	float m_social;			// Introversion --- Extroversion
	float m_information;	//		Sensing --- Intuition
	float m_decisions;		//		feeling --- thinking
	float m_structure;		//		judging --- perceiving

	Tags m_tags;
};