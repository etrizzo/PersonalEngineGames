#pragma once
#include "GameCommon.hpp"
#include "Game/GameState_Playing.hpp"
#include "Engine/Renderer/ForwardRenderPath.hpp"


class Camera;
class PerspectiveCamera;
class DebugRenderSystem;
class Entity;
class Light;
class SpotLight;
class ParticleSystem;

class Map;



enum EntityTypes{
	NUM_ENTITIES
};

class Game{

public:
	~Game();											// destructor: do nothing (for speed)
	Game();											// default constructor: do nothing (for speed)

	bool m_isPaused;
	bool m_devMode;
	//float m_gameTime;
	Map* m_currentMap;
	PerspectiveCamera* m_mainCamera;
	Camera* m_uiCamera;
	PerspectiveCamera* m_currentCamera;
	Texture* m_tileTexture;


	GameState* m_currentState;
	GameState* m_transitionToState = nullptr;
	GameState_Playing* m_playState;
	float m_timeEnteredState;

	Clock* m_gameClock = nullptr;

	float m_enemySeparateRadius = 2.f;
	float m_enemySeekWeight = 1.5f;		//these are in game to make them easy to change with console commands
	float m_enemySeparateWeight = 1.f;
	


	DebugRenderSystem* m_debugRenderSystem;
	void PostStartup();

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


	void AddNewLight(std::string type, RGBA color = RGBA::WHITE);		//adds in front of camera
	void AddNewLight(std::string type, Vector3 pos, RGBA color = RGBA::WHITE);
	void AddNewPointLight(Vector3 pos, RGBA color);
	void AddNewSpotLight(Vector3 pos, RGBA color, float innerAngle = 20.f, float outerAngle = 25.f);
	void AddNewDirectionalLight(Vector3 pos, RGBA color, Vector3 rotation = Vector3::ZERO);
	void RemoveLight(int idx = 0);
	void SetLightPosition(Vector3 newPos, unsigned int idx = 0);
	void SetLightColor(RGBA newColor, unsigned int idx = 0);
	void SetLightColor(Vector4 newColor, unsigned int idx = 0);

	void SetLightAttenuation(int lightIndex, Vector3 att);

	unsigned int GetNumActiveLights() const;

	SoundID m_soundTrackID;
	SoundPlaybackID m_soundtrackPlayback;

private:
	void LoadTileDefinitions();

	void RenderGame();
	void RenderUI();

	//float m_specAmount = .5f;
	//float m_specFactor = 3.f;
	//int m_numActiveLights = 0;

	//eDebugShaders m_debugShader = SHADER_LIT;

	//void UpdateShader(int direction);
	//void SetShader();		//sets which shader to draw scene with
	//std::string GetShaderName() const;

	////for objects drawn using drawmeshimmediate
	//Material* m_couchMaterial;

	//ParticleSystem* m_particleSystem;
};

extern Game* g_theGame;