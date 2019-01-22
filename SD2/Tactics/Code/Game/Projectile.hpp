#pragma once
#include "Game/GameCommon.hpp"
#include "Game/Actor.hpp"

class Projectile : public Entity {
public:
	Projectile(Map* map, Vector3 startPosition, Vector3 target, int damage, float critChance, float blockChance, float maxRange, float gravity = 9.8f, float cosmeticOffset = .5f);

	virtual void Update(float deltaSeconds);
	virtual void Render();

	
	float m_gravity = 9.8f;
	float m_launchSpeed = 0.f;
	float m_launchAngle = 0.f;
	float m_distance = 0.f;
	Vector3 m_launchPosition = Vector3::ZERO;
	Vector3 m_direction = Vector3::ZERO;
	float m_cosmeticOffset = .5f;		//cosmetic offset

	Actor* m_target = nullptr;
	int m_damage = 0;
	float m_critChance = 0.f;
	float m_blockChance = 0.f;

};