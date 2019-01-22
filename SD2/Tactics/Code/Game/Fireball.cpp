#include "Fireball.hpp"
#include "Engine/Math/Trajectory.hpp"
#include "Game/Map.hpp"

Fireball::Fireball(Map * map, Vector3 startPosition, Vector3 target, int damage, float maxRange, int areaRange, int verticalRange, float gravity, float cosmeticOffset) :
	Projectile(map, startPosition, target, damage, 0.f, 0.f, maxRange, gravity, cosmeticOffset)
{
	m_areaRange = areaRange;
	m_verticalRange = verticalRange;
	m_targetTile = IntVector2(RoundToNearestInt(target.x), RoundToNearestInt(target.z));
}

void Fireball::Update(float deltaSeconds)
{
	m_ageInSeconds+=deltaSeconds;
	Vector2 traj = Trajectory::Evaluate(m_gravity, m_launchSpeed, m_launchAngle, m_ageInSeconds);
	Vector3 pos3D = m_launchPosition + (m_direction * traj.x); 
	pos3D.y = m_launchPosition.y + traj.y + m_cosmeticOffset;		//add height offset
	m_worldPosition = pos3D;

	IntVector2 coords = IntVector2(RoundToNearestInt(m_worldPosition.x), RoundToNearestInt(m_worldPosition.z));
	if (m_map->GetHeightAtXY(coords) > m_worldPosition.y){
		m_aboutToBeDeleted = true;		//hit a wall
	} else if (traj.x > m_distance){
		//has reached the target
		m_map->ApplyDamageAOE(m_targetTile, m_areaRange, m_verticalRange, m_damage);
		m_aboutToBeDeleted = true;
	}
}

void Fireball::Render()
{
	if (!m_aboutToBeDeleted){
		g_theRenderer->DrawCube(m_worldPosition, Vector3::ONE * .2f, RGBA(255,128,0,255));
	}
}
