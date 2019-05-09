#pragma once
#include "Game/GameState.hpp"
#include "Game/GameCommon.hpp"

class Camera;
class PerspectiveCamera;
class DebugRenderSystem;
class Entity;
class Player;
class ParticleSystem;
class ForwardRenderPath;
//class RenderScene;
class World;
class GameCamera;

class GameState_Playing: public GameState{
public:
	GameState_Playing();
	~GameState_Playing();

	World* m_world;
	Player* m_player;
	//Light* m_cameraLight;
	//std::vector<Light*> m_lights = std::vector<Light*>();

	/*ForwardRenderPath* m_renderPath;
	RenderScene* m_scene;*/

	std::vector<Entity*> m_allEntities;

	void EnterState() override;
	void Update(float ds);
	void RenderGame();
	void RenderUI();
	void HandleInput();
	void RespawnPlayer();



	//RenderScene* GetScene() { return m_scene; }

	bool m_gameWon = false;
	bool m_gameLost = false;
	bool m_playing = false;
	Light* m_sun;

	GameCamera* m_gameCamera = nullptr;

protected:

	void Startup();

	float m_specAmount = .5f;
	float m_specFactor = 3.f;
	int m_numActiveLights = 0;

	void CheckForVictory();
	void CheckForDefeat();
	void SpawnPlayer(Vector3 pos);

	void DeleteEntities();


};



inline bool MyTestEventFunction(NamedProperties* args)
{
	ConsolePrintf("MyTestEventFunction executed: args.Event = %s, args.Health=%3.2f", args->Get("Event", "NO EVENT").c_str(), args->Get("Health", 0.f));
	return false;
}