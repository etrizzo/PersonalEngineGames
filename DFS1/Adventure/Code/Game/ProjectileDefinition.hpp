#pragma once
#include "Game/EntityDefinition.hpp"

class ProjectileDefinition: public EntityDefinition{
public:
	ProjectileDefinition(tinyxml2::XMLElement* projElement);
	~ProjectileDefinition();

	IntRange m_damageRange = IntRange(0);

	static std::map< std::string, ProjectileDefinition* >		s_definitions;
	static ProjectileDefinition* GetProjectileDefinition(std::string definitionName);
};