#include "Game/Enemy.hpp"
#include "Game/Game.hpp"
#include "Game/Map.hpp"
#include "Game/DebugRenderSystem.hpp"
#include "Engine/Renderer/PerspectiveCamera.hpp"
#include "Engine/Renderer/Sprite.hpp"
#include "Game/FlowerPot.hpp"

Enemy::Enemy( const Vector3& position, Entity* target)
{
	m_targetEntity = target;
	float size = .5f;
	m_collider = Sphere(Vector3::ZERO, size);


	m_sprite = new Sprite(g_theRenderer->CreateOrGetTexture("minion.png"), AABB2::ZERO_TO_ONE, Vector2(.5f, 0.f), Vector2::ONE * size);

	m_ageInSeconds = 0.f;
	m_aboutToBeDeleted = false;

	m_noClipMode = false;


	m_rateOfAttack = StopWatch();
	m_rateOfAttack.SetTimer(.3f);

	m_positionXY = position.XZ();

	m_health = ENEMY_MAX_HEALTH;


	SetPosition(position);
	SetScale(Vector3(size, size, size));
	SetWorldPosition();
}

Enemy::~Enemy()
{

	
}

void Enemy::Update()
{

	float ds = g_theGame->GetDeltaSeconds();
	m_ageInSeconds += ds;

	Vector3 displacement = m_targetEntity->GetPosition() - GetPosition();
	if (displacement.GetLengthSquared() > (.5f))
	{
		Vector3 direction = displacement.GetNormalized();
		direction.z = 0.f;

		Translate(direction * m_speed * GetMasterClock()->GetDeltaSeconds());
	}


	//float newX = m_positionXY.x + movement;
	//float radius = g_theGame->m_currentMap->m_collider.m_radius;
	//m_positionXY.x = ClampFloat(newX, -radius * WALKABLE_AREA_AS_PERCENTAGE_OF_RADIUS, radius * WALKABLE_AREA_AS_PERCENTAGE_OF_RADIUS);
	//m_positionXY.y = GetHeightAtCurrentPos();

	SetWorldPosition();

}

void Enemy::Damage()
{
	m_health -= 1.f;
	if (m_health <= 0.f) {
		m_aboutToBeDeleted = true;
	}
}


float Enemy::GetPercentageOfHealth() const
{
	float t = RangeMapFloat(m_health, 0.f, ENEMY_MAX_HEALTH, 0.f, 1.f);
	return t;
}



void Enemy::SetWorldPosition()
{

	Vector3 pos = Vector3(GetPosition().x, GetHeightAtCurrentPos(), GetPosition().z);
	m_collider.SetPosition(pos);
	m_transform.SetLocalPosition(pos);
	//SetPosition(pos);
}


float Enemy::GetHeightAtCurrentPos()
{
	return g_theGame->m_currentMap->GetHeightFromXPosition(GetPosition().x);
}

