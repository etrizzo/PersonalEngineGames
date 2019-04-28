#pragma once
#include "Game/GameState.hpp"
#include "Game/GameCommon.hpp"

class Camera;
class PerspectiveCamera;
class DebugRenderSystem;
class Entity;
class Player;
class Enemy;
class Spawner;
class Light;
class SpotLight;
class ParticleSystem;
class ForwardRenderPath;
class RenderScene;
class FlowerPot;
class Asteroid;
class Missile;
class Enemy;
class EnemySpawner;
class SpriteAnimSetDef;

class GameState_Playing: public GameState{
public:
	GameState_Playing();

	Player* m_player;
	//Light* m_cameraLight;
	//std::vector<Light*> m_lights = std::vector<Light*>();

	ForwardRenderPath* m_renderPath;
	RenderScene* m_scene;

	std::vector<Entity*> m_allEntities;
	std::vector<FlowerPot*> m_flowerPots;
	std::vector<Asteroid*> m_asteroids;
	std::vector<Missile*> m_missiles;
	std::vector < Enemy*> m_enemies;
	std::vector<EnemySpawner*> m_spawners;

	void EnterState() override;
	void Update(float ds);
	void RenderGame();
	void RenderEntities();
	void RenderUI();
	void HandleInput();
	void RespawnPlayer();

	void SpawnMissile(Vector3 position, Asteroid* target);
	void SpawnEnemy(const Vector3& position);

	Entity* GetClosestAlliedEntity(const Vector3& position);

	Light* AddNewLight(std::string type, RGBA color = RGBA::WHITE);		//adds in front of camera
	Light* AddNewLight(std::string type, Vector3 pos, RGBA color = RGBA::WHITE);
	Light* AddNewPointLight(Vector3 pos, RGBA color);
	Light* AddNewSpotLight(Vector3 pos, RGBA color, float innerAngle = 20.f, float outerAngle = 25.f);
	Light* AddNewDirectionalLight(Vector3 pos, RGBA color, Vector3 rotation = Vector3::ZERO);
	void RemoveLight(int idx = 0);
	void RemoveLight(Light* light);
	void SetLightPosition(Vector3 newPos, unsigned int idx = 0);
	void SetLightColor(RGBA newColor, unsigned int idx = 0);
	void SetLightColor(Vector4 newColor, unsigned int idx = 0);

	void SetLightAttenuation(int lightIndex, Vector3 att);


	unsigned int GetNumActiveLights() const;

	RenderScene* GetScene() { return m_scene; }

	bool m_gameWon = false;
	bool m_gameLost = false;
	bool m_playing = false;
	Light* m_sun;


	SpriteAnimSetDef* m_playerAnimDefinition	= nullptr;
	SpriteAnimSetDef* m_enemyAnimDefinition		= nullptr;
	SpriteAnimSetDef* m_flowerAnimDefinition	= nullptr;
	SpriteAnimSetDef* m_resupplyAnimDefinition	= nullptr;

protected:

	void Startup();
	
	void RenderPlayerHealthBar(const AABB2& uiBounds);

	int m_numActiveLights = 0;

	eDebugShaders m_debugShader = SHADER_LIT;

	void CheckForVictory();
	void CheckForDefeat();
	void SpawnPlayer(Vector3 pos);
	void SpawnFlowerPot(float xPosition);
	void SpawnAsteroid();

	void CreateSpawner(Vector3 pos);

	void DeleteEntities();

	StopWatch m_asteroidSpawnClock;

	//for objects drawn using drawmeshimmediate
	Material* m_couchMaterial;

	ParticleSystem* m_particleSystem;

};