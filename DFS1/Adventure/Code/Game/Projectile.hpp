#pragma once
#include "Game/Entity.hpp"
class ProjectileDefinition;

class Projectile: public Entity{
public:
	Projectile(){};
	Projectile(ProjectileDefinition* definition, Map* entityMap, Vector2 initialPos, float initialRotation, std::string faction, int bonusStrength = 0);
	~Projectile();

	void Update(float deltaSeconds);
	void Render();

	void RunCorrectivePhysics();

	ProjectileDefinition* m_definition;
	int m_damage;
};