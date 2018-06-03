#include "PortalDefinition.hpp"


//	<PortalDefinitions>
//
//		<PortalDefinition name="TeleportVortex" reciprocal="TeleportVortex" requiresKey="false">
//		<Movement>
//			<Fly speed="0.0"/>
//		</Movement>
//		<SpriteAnimSet spriteSheet="Misc.png" spriteLayout="16,16">
//			<SpriteAnim name="Idle" spriteIndexes="25"/>
//		</SpriteAnimSet>
//		</PortalDefinition>
//
//	</PortalDefinitions>


std::map<std::string, PortalDefinition*>	PortalDefinition::s_definitions;

PortalDefinition::PortalDefinition(tinyxml2::XMLElement * portalElement)
	:EntityDefinition(portalElement)
{
	m_requiresKey = ParseXmlAttribute(*portalElement, "requiresKey", m_requiresKey);
	m_reciprocalName = ParseXmlAttribute(*portalElement, "reciprocal", (std::string) " " );
	if (!m_reciprocalName.compare(" ")){
		m_hasReciprocalPortal = false;
	} else {
		m_hasReciprocalPortal = true;
	}
}

PortalDefinition::~PortalDefinition()
{
}

PortalDefinition * PortalDefinition::GetPortalDefinition(std::string definitionName)
{
	auto containsDef = s_definitions.find((std::string)definitionName); 
	PortalDefinition* portalDef = nullptr;
	if (containsDef != s_definitions.end()){
		portalDef = containsDef->second;
	} else {
		ERROR_AND_DIE("No portal named: " + definitionName);
	}
	return portalDef;
}
