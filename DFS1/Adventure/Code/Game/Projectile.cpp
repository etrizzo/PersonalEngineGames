#include "Projectile.hpp"
#include "ProjectileDefinition.hpp"
#include "Map.hpp"

Projectile::Projectile(ProjectileDefinition * definition, Map * entityMap, Vector2 initialPos, float initialRotation, std::string faction, int bonusStrength)
	:Entity((EntityDefinition*)definition, entityMap, initialPos, initialRotation)
{
	m_faction = faction;
	m_damage = definition->m_damageRange.GetRandomInRange() + bonusStrength;
	m_facing = Vector2::MakeDirectionAtDegrees(initialRotation);
	m_localDrawingBox = AABB2(-definition->m_drawingRadius, -definition->m_drawingRadius, definition->m_drawingRadius, definition->m_drawingRadius);
}

Projectile::~Projectile()
{

}

void Projectile::Update(float deltaSeconds)
{
	if (!m_aboutToBeDeleted){
		Entity::Update(deltaSeconds);
		
		Vector2 newPos = GetPosition() + (m_facing * m_speed * deltaSeconds );
		Tile* currentTile = m_map->TileAtFloat(newPos);
		if (currentTile != nullptr && Entity::CanEnterTile(*currentTile)){
			Entity::SetPosition(newPos);
		} else {
			m_aboutToBeDeleted = true;
		}
	}
	
}

void Projectile::Render()
{
	Entity::Render();
}

void Projectile::RunCorrectivePhysics()
{
}
