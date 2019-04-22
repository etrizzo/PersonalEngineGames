#include "Act.hpp"

Act::Act(tinyxml2::XMLElement * actElement)
{
	m_name = ParseXmlAttribute(*actElement, "name", "NO_NAME");
	m_number = ParseXmlAttribute(*actElement, "number", m_number);
	m_minNodes = ParseXmlAttribute(*actElement, "minNodes", 1);
}
