#include "EnemySpawner.hpp"
#include "Game/Game.hpp"
#include "Game/DebugRenderSystem.hpp"

EnemySpawner::EnemySpawner(Vector3 position)
{
	SetPosition(position);
	m_spawnTimer = StopWatch();
	m_spawnTimer.SetTimer(GetRandomFloatInRange(1.7f, 2.4f));
}

void EnemySpawner::Update()
{
	Entity::Update();
	if (m_spawnTimer.CheckAndReset() && g_theGame->m_playState->m_enemies.size() < MAX_ENEMIES_SPAWNED)
	{
		g_theGame->m_playState->SpawnEnemy(m_transform.GetWorldPosition());
	}
}

void EnemySpawner::Render()
{
	g_theGame->m_debugRenderSystem->MakeDebugRenderWireAABB3(0.0f, m_transform.GetWorldPosition());
}
