#include "AIBehavior.hpp"

AIBehavior::AIBehavior(const tinyxml2::XMLElement & behaviorElement)
{
	m_name = behaviorElement.Name();
	m_baseUtility = ParseXmlAttribute(behaviorElement, "baseUtility", m_baseUtility);
}

AIBehavior * AIBehavior::CreateAIBehavior(const tinyxml2::XMLElement & behaviorElement)
{
	UNUSED(behaviorElement);
	return nullptr;
}
