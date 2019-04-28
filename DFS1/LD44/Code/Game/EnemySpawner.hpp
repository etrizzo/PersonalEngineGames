#pragma once
#include "Game/GameCommon.hpp"
#include "Game/Entity.hpp"

class EnemySpawner : public Entity
{
	float m_minSpawnRate = MIN_ENEMY_SPAWN_TIMER;
	float m_maxSpawnRate = MAX_ENEMY_SPAWN_TIMER;
public:
	EnemySpawner(const Vector3& position);

	void Update() override;
	void Render() override;

	StopWatch m_spawnTimer;
};