#include "Projectile.hpp"
#include "Engine/Math/Trajectory.hpp"
#include "Game/Map.hpp"



Projectile::Projectile(Map * map, Vector3 startPosition, Vector3 target, int damage, float critChance, float blockChance, float maxRange, float gravity, float cosmeticOffset)
{
	m_map = map;
	m_gravity = gravity;
	m_launchPosition = startPosition;
	m_cosmeticOffset = cosmeticOffset;	

	Vector2 thetas = Vector2::ZERO;
	m_direction =  target - startPosition;
	m_direction.y = 0.f;
	m_distance = m_direction.NormalizeAndGetLength();
	m_launchSpeed= Trajectory::GetMinimumLaunchSpeed(9.8f, maxRange + .5f);
	float height = target.y - startPosition.y;
	Trajectory::GetLaunchAngles(&thetas, gravity, m_launchSpeed, m_distance, height);
	m_launchAngle = max(thetas.x, thetas.y);

	m_damage = damage;
	m_target = (Actor*) m_map->GetEntityInColumn(IntVector2(RoundToNearestInt(target.x), RoundToNearestInt(target.z)));
	m_critChance = critChance;
	m_blockChance = blockChance;
	
}

void Projectile::Update(float deltaSeconds)
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
		if (m_target != nullptr){
			m_target->TakeDamage(m_damage, m_critChance, m_blockChance);
		}
		m_aboutToBeDeleted = true;
	}

}

void Projectile::Render()
{
	if (!m_aboutToBeDeleted){
		g_theRenderer->DrawCube(m_worldPosition, Vector3::ONE * .2f, RGBA::WHITE);
	}
}
