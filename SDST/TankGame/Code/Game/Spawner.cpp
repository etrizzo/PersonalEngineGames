#include "Spawner.hpp"
#include "Game/Game.hpp"
#include "Game/Map.hpp"
#include "Game/DebugRenderSystem.hpp"

Spawner::Spawner(Vector2 startPos, GameState_Playing * playState)
{
	float height = 5.f;
	float thiccness = 1.f;
	m_renderable = new Renderable();

	MeshBuilder mb = MeshBuilder();
	mb.Begin(PRIMITIVE_TRIANGLES, true);
	mb.AppendCube(Vector3::ZERO, Vector3(thiccness * 2.f, height * 2.f, thiccness * 2.f), RGBA::YELLOW);
	mb.End();
	m_renderable->SetMesh(mb.CreateMesh(VERTEX_TYPE_LIT));
	SetMaterial(Material::GetMaterial("cel_shaded_lit"), 0);
	

	m_positionXZ = startPos;
	m_playState = playState;
	Vector3 pos = g_theGame->m_currentMap->GetPositionAtCoord(startPos);
	SetPosition(pos);
	m_boxCollider = AABB3(pos, thiccness, height, thiccness);

	m_spawnRate = StopWatch(GetMasterClock());
	m_spawnRate.SetTimer(1.f);

	m_spawnedEnemies = std::vector<Enemy*>();
	m_playState = playState;
	m_health = 5;
}

Spawner::~Spawner()
{
	m_playState->m_scene->RemoveRenderable(m_renderable);
}

void Spawner::Update()
{
	PROFILE_PUSH_FUNCTION();
	if (CanSpawnEnemy()){
		SpawnNewEnemy();
	}
	ClearDestroyedEnemies();
	PROFILE_POP();
}

void Spawner::Damage()
{
 	m_health--;
	if (m_health <= 0){
		g_theAudio->PlayOneOffSoundFromGroup("explosion");
		m_aboutToBeDeleted = true;
	}
}

bool Spawner::IsPointInside(const Vector3 & point) const
{
	return m_boxCollider.IsPointInside(point);
}

void Spawner::ClearDestroyedEnemies()
{
	for(int i = (int) m_spawnedEnemies.size()-1; i >= 0; i--){
		if (m_spawnedEnemies[i]->m_aboutToBeDeleted){
			RemoveAtFast(m_spawnedEnemies, i);
		}
	}
}

bool Spawner::CanSpawnEnemy()
{
	if ((int) m_spawnedEnemies.size() >= m_maxEnemies){
		return false;
	} else {
		return m_spawnRate.CheckAndReset();
	}
}

void Spawner::SpawnNewEnemy()
{
	//get a random point in the spawn radius
	float spawnX = GetRandomFloatInRange(-m_spawnRadius, m_spawnRadius);
	float spawnY = GetRandomFloatInRange(-m_spawnRadius, m_spawnRadius);
	Vector2 spawnPos = m_positionXZ + Vector2(spawnX, spawnY);
	Enemy* newEnemy = m_playState->AddNewEnemy(spawnPos);
	m_spawnedEnemies.push_back(newEnemy);
}
