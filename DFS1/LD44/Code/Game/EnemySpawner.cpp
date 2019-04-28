#include "EnemySpawner.hpp"
#include "Game/Game.hpp"
#include "Game/DebugRenderSystem.hpp"

EnemySpawner::EnemySpawner(const Vector3& position)
{
	SetPosition(position);
	m_spawnTimer = StopWatch();
	m_spawnTimer.SetTimer(GetRandomFloatInRange(MIN_ENEMY_SPAWN_TIMER, MAX_ENEMY_SPAWN_TIMER));
}

void EnemySpawner::Update()
{
	Entity::Update();
	if (m_spawnTimer.CheckAndReset() && g_theGame->m_playState->m_enemies.size() < MAX_ENEMIES_SPAWNED)
	{
		g_theGame->m_playState->SpawnEnemy(m_transform.GetWorldPosition());
		m_spawnTimer.SetTimer(GetRandomFloatInRange(MIN_ENEMY_SPAWN_TIMER, MAX_ENEMY_SPAWN_TIMER));
	}
}

void EnemySpawner::Render()
{
	g_theGame->m_debugRenderSystem->MakeDebugRenderWireAABB3(0.0f, m_transform.GetWorldPosition());
}
