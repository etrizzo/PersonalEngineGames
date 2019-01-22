#include "AnimSet.hpp"

std::map<std::string, AnimSet*> AnimSet::s_animSets;

AnimSet::AnimSet(tinyxml2::XMLElement *animSetElement)
{
	for (tinyxml2::XMLElement* setElement = animSetElement->FirstChildElement("set"); setElement != nullptr; setElement = setElement->NextSiblingElement("set")){
		std::string id = ParseXmlAttribute(*setElement, "id", "NO_ID");
		std::string animID = ParseXmlAttribute(*setElement->FirstChildElement("anim"), "id", "NO_ID");
		m_animsBySetID.insert(std::pair<std::string, std::string>(id, animID));
	}
	std::string setID = ParseXmlAttribute(*animSetElement, "id", "NO_ID");
	s_animSets.insert(std::pair<std::string, AnimSet*>(setID, this));
}

std::string AnimSet::GetAnimName(std::string name)
{
	auto containsAnim = m_animsBySetID.find((std::string)name); 
	std::string animID  = "";
	if (containsAnim != m_animsBySetID.end()){
		animID = containsAnim->second;
	}
	return animID;
}

AnimSet * AnimSet::GetAnimSet(std::string name)
{
	auto containsAnim = s_animSets.find((std::string)name); 
	AnimSet* anim = nullptr;
	if (containsAnim != s_animSets.end()){
		anim = containsAnim->second;
	}
	return anim;
}
