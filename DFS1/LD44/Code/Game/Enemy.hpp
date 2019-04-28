#pragma once
#include "Game/Entity.hpp"

constexpr int ENEMY_MAX_HEALTH = 1;

class Enemy :public Entity {
public:
	Enemy(const Vector3& position, Entity* target);
	~Enemy();

	void Update() override;

	void TakeDamage() override;
	void ExecuteAttack() override;

	float GetPercentageOfHealth() const;

	eEnemyBehavior m_currentBehavior = BEHAVIOR_FOLLOW_TARGET;

	int m_health = ENEMY_MAX_HEALTH;
	float m_speed = 2.f;
	StopWatch m_rateOfAttack;

	float m_attackRange = 2.f;
	float m_stopDistanceSquared;
	
	

protected:
	Vector2 m_positionXY;
	Entity* m_targetEntity = nullptr;
	void SetWorldPosition();
	float GetHeightAtCurrentPos();

	void UpdateBehavior();
	void RunBehaviorFollow();
	void RunBehaviorAttack();
};