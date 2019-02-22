#pragma once
#include "GameCommon.hpp"
#include "Game/GameState_Playing.hpp"
#include "Engine/Renderer/ForwardRenderPath.hpp"
#include "Engine/Networking/NetMessage.hpp"

class Camera;
class PerspectiveCamera;
class DebugRenderSystem;
class Entity;
class Light;
class SpotLight;
class ParticleSystem;
class UDPTest;
class NetSession;

class Map;

#define NET_OBJECT_SYNC_SIM_TIME (1.f / 20.f)			//5 Hz sync rate (for accurate RTT


enum EntityTypes{
	NUM_ENTITIES
};

class Game{

public:
	~Game();											// destructor: do nothing (for speed)
	Game();											// default constructor: do nothing (for speed)

	NetSession* m_session = nullptr;
	//UDPTest* m_udp = nullptr;

	StopWatch m_objectSyncSimTimer;

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
	World* GetWorld() const;

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

	bool m_godMode;

private:
	float m_fps = 60.f;
	void LoadTileDefinitions();

	void RenderGame();
	void RenderUI();

	
};

extern Game* g_theGame;



bool OnAdd( NetMessage msg, net_sender_t const & from) ;
bool OnAddResponse( NetMessage msg, net_sender_t const & from) ;

//simulates the round trip time we would have with netobjects bc we're sending more often than heartbeat
bool OnNetSyncSim( NetMessage msg, net_sender_t const& from);

