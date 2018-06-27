#include "Game/GameCommon.hpp"
#include "Game/Enemy.hpp"

class Spawner :public Entity {
public:
	Spawner(Vector2 startPos, GameState_Playing* playState);
	~Spawner();

	void Update() override;
	void Damage();
	bool IsPointInside(const Vector3& point) const;

	Vector2 m_positionXZ;
	StopWatch m_spawnRate;
	int m_maxEnemies = 10;
	//reference to enemies this spawner has created
	// Note: spawner doesn't own entities once they're created, the playstate does
	std::vector<Enemy*> m_spawnedEnemies;	
	AABB3 m_boxCollider;
	float m_spawnRadius = 5.f;

private:
	//removes enemies about to be deleted from list references of your enemies
	void ClearDestroyedEnemies();
	bool CanSpawnEnemy();
	void SpawnNewEnemy();

};
