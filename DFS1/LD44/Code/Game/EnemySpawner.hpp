#pragma once
#include "Game/GameCommon.hpp"
#include "Game/Entity.hpp"

class EnemySpawner : public Entity
{
public:
	EnemySpawner(Vector3 position);

	void Update() override;
	void Render() override;

	StopWatch m_spawnTimer;
};