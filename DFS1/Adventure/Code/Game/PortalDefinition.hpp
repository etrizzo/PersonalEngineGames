#pragma once
#include "Game/EntityDefinition.hpp"

class PortalDefinition: public EntityDefinition{
public:
	PortalDefinition(tinyxml2::XMLElement* portalElement);
	~PortalDefinition();


	static std::map< std::string, PortalDefinition* >		s_definitions;
	static PortalDefinition* GetPortalDefinition(std::string definitionName);

	bool m_requiresKey = false;
	bool m_hasReciprocalPortal = false;
	std::string m_reciprocalName;
};