#pragma once
#include "GameCommon.hpp"
#include "Game/GameState_Playing.hpp"
#include "Engine/Renderer/ForwardRenderPath.hpp"
#include "Engine/DataTypes/DirectedGraph.hpp"
#include "Game/StoryGraph.hpp"

class Camera;
class PerspectiveCamera;
class DebugRenderSystem;
class Entity;
class Light;
class SpotLight;
class ParticleSystem;

class Map;

class Game{

public:
	~Game();										
	Game();											

	//DirectedGraph<StoryData> m_graph; 
	StoryGraph m_graph;


	/*
	TO BE DELETED
	*/

	bool m_isPaused;
	bool m_devMode;
	//float m_gameTime;
	Map* m_currentMap;
	Camera* m_mainCamera;
	Camera* m_uiCamera;
	Camera* m_currentCamera;
	Texture* m_tileTexture;


	GameState* m_currentState;
	GameState* m_transitionToState = nullptr;
	GameState_Playing* m_playState;
	float m_timeEnteredState;

	Clock* m_gameClock = nullptr;

	float m_enemySeparateRadius = 1.5f;
	float m_enemyAlignmentRadius = 5.f;
	float m_enemyCohesionRadius = 5.f;
	float m_enemySeekWeight = 1.5f;		//these are in game to make them easy to change with console commands
	float m_enemySeparateWeight = 1.f;
	float m_enemyAlignmentWeight = .8f;
	float m_enemyCohesionWeight = .8f;
	


	DebugRenderSystem* m_debugRenderSystem;
	void PostStartup();

	void GenerateGraph();

	void Update();
	void Render();
	void HandleInput();

	void RenderLoadScreen();

	void TransitionToState(GameState* newState);
	void TriggerTransition();

	float GetDeltaSeconds();
	Vector2 GetPlayerPositionXZ() const;
	Player* GetPlayer() const;

	AABB2 SetMainCamera();
	AABB2 SetUICamera();
	AABB2 GetUIBounds();
	Vector3 GetCurrentCameraUp();
	Vector3 GetCurrentCameraRight();
	void SetGameCamera(PerspectiveCamera* newCam = nullptr);


	void TogglePause();
	void ToggleDevMode();

	void DebugStartup();
	void DebugShutdown();
	void DebugClear();
	void DebugToggleRendering();

	bool IsDevMode() const { return m_devMode; }

	RenderScene* GetScene();




	SoundID m_soundTrackID;
	SoundPlaybackID m_soundtrackPlayback;

	bool m_godMode;

private:

	void RenderGame();
	void RenderUI();

	void GraphTests();
	void StoryGraphTests();
};

extern Game* g_theGame;