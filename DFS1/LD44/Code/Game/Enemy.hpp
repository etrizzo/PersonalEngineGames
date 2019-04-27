#pragma once
#include "Game/Entity.hpp"

constexpr int ENEMY_MAX_HEALTH = 1;

class Enemy :public Entity {
public:
	Enemy(const Vector3& position, Entity* target);
	~Enemy();

	void Update() override;

	void Damage();

	float GetPercentageOfHealth() const;


	int m_health = ENEMY_MAX_HEALTH;
	float m_speed = 5.f;
	StopWatch m_rateOfAttack;
	
	

protected:
	Vector2 m_positionXY;
	Entity* m_targetEntity = nullptr;
	void SetWorldPosition();
	float GetHeightAtCurrentPos();

};