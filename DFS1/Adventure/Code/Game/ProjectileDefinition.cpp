#include "ProjectileDefinition.hpp"

//	<ProjectileDefinitions>
//		<ProjectileDefinition name="Arrow">
//			<Movement>
//				<Fly speed="5.0"/>
//			</Movement>
//			<Damage amount="3~10"/>
//			<SpriteAnimSet spriteSheet="Misc.png" spriteLayout="16,16">
//				<SpriteAnim name="Idle" spriteIndexes="15" autoOrient="true"/>
//			</SpriteAnimSet>
//		</ProjectileDefinition>
//	</ProjectileDefinitions>


std::map<std::string, ProjectileDefinition*>	ProjectileDefinition::s_definitions;

ProjectileDefinition::ProjectileDefinition(tinyxml2::XMLElement * projElement)
	:EntityDefinition(projElement)
{
	tinyxml2::XMLElement* damageElement = projElement->FirstChildElement("Damage");
	m_damageRange = ParseXmlAttribute(*damageElement, "amount", m_damageRange); 
}

ProjectileDefinition::~ProjectileDefinition()
{
}

ProjectileDefinition * ProjectileDefinition::GetProjectileDefinition(std::string definitionName)
{
	auto containsDef = s_definitions.find((std::string)definitionName); 
	ProjectileDefinition* projDef = nullptr;
	if (containsDef != s_definitions.end()){
		projDef = containsDef->second;
	} else {
		ERROR_AND_DIE("No actor named: " + definitionName);
	}
	return projDef;
}
