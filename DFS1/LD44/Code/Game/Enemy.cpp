#include "Game/Enemy.hpp"
#include "Game/Game.hpp"
#include "Game/Map.hpp"
#include "Game/DebugRenderSystem.hpp"
#include "Engine/Renderer/PerspectiveCamera.hpp"
#include "Engine/Renderer/Sprite.hpp"
#include "Game/FlowerPot.hpp"

Enemy::Enemy( const Vector3& position, Entity* target)
{
	m_animSet = new SpriteAnimSet(g_theGame->m_playState->m_enemyAnimDefinition);
	m_targetEntity = target;
	float size = .5f;
	m_collider = Sphere(Vector3::ZERO, size);
	m_percThroughAnimationToExecuteAttack = .8f;

	//distance which the enemy will stop and start attacking
	m_stopDistanceSquared = GetRandomFloatInRange(.5f, 1.0f);


	m_sprite = new Sprite(m_animSet->GetCurrentTexture(), AABB2::ZERO_TO_ONE, Vector2(.5f, 0.f), Vector2::ONE * size);

	m_ageInSeconds = 0.f;
	m_aboutToBeDeleted = false;

	m_noClipMode = false;


	m_rateOfAttack = StopWatch();
	m_rateOfAttack.SetTimer(ENEMY_RATE_OF_ATTACK);

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

	UpdateBehavior();

	switch (m_currentBehavior) 
	{
	case BEHAVIOR_FOLLOW_TARGET:
		RunBehaviorFollow();
		break;
	case BEHAVIOR_ATTACK:
		RunBehaviorAttack();
		break;
	}

	UpdateAnimation();

	SetWorldPosition();

}

void Enemy::TakeDamage()
{
	m_health -= 1.f;
	if (m_health <= 0.f) {
		m_aboutToBeDeleted = true;
	}
}

void Enemy::ExecuteAttack()
{
	m_targetEntity->TakeDamage();
	g_theAudio->PlayOneOffSoundFromGroup("minion");
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

void Enemy::UpdateBehavior()
{
	Vector3 displacement = m_targetEntity->GetPosition() - GetPosition();
	float displacementLengthSquared = displacement.GetLengthSquared();
	if (m_currentBehavior != BEHAVIOR_FOLLOW_TARGET && (displacementLengthSquared > m_attackRange))
	{
		m_currentBehavior = BEHAVIOR_FOLLOW_TARGET;

	}
	else if (m_currentBehavior == BEHAVIOR_FOLLOW_TARGET && displacementLengthSquared <= m_stopDistanceSquared)
	{
		m_currentBehavior = BEHAVIOR_ATTACK;
	}
}

void Enemy::RunBehaviorFollow()
{
	Vector3 displacement = m_targetEntity->GetPosition() - GetPosition();
	Vector3 direction = displacement.GetNormalized();
	direction.z = 0.f;
	m_facing = direction;
	if (displacement.GetLengthSquared() > m_stopDistanceSquared)
	{
		m_animState = ANIM_STATE_WALK;
	}
	else
	{
		m_animState = ANIM_STATE_IDLE;
	}

	Translate(direction * m_speed * GetMasterClock()->GetDeltaSeconds());
}

void Enemy::RunBehaviorAttack()
{
	if (m_targetEntity != nullptr && m_rateOfAttack.CheckAndReset())
	{
		BeginAttack();
	}
}

