#pragma once
#include "GameCommon.hpp"
#include "Game/GameState.hpp"

//enum GameState{
//	GAME_STATE_ATTRACT,
//	GAME_STATE_ENCOUNTER,
//	NUM_GAME_STATES
//};

class Camera;
class OrbitCamera;
class Encounter;
class EncounterState;
class Map;
class FlyoutText;
class IsoSprite;

class Game{

public:
	~Game();											// destructor: do nothing (for speed)
	Game();											// default constructor: do nothing (for speed)

	bool m_isPaused	 = false;
	bool m_devMode	 = false;
	float m_gameTime = 0.f;
	EncounterState* m_encounterState	= nullptr;
	//Encounter* m_currentEncounter	= nullptr;
	//Map* m_currentMap;
	OrbitCamera* m_camera	= nullptr;
	Camera* m_uiCamera		= nullptr;
	Texture* m_tileTexture	= nullptr;

	GameState* m_currentState	   = new GameState_Attract();
	GameState* m_transitionToState = nullptr;

	Clock* m_gameClock = nullptr;

	tinyxml2::XMLDocument m_doc;

	void InitCameras(Vector3 nearBottomLeft, Vector3 farTopRight);

	void Update(float deltaSeconds);
	void Render();
	void HandleInput();
	void RotateCamera(Vector2 thumbstickPos, float zoomIn, float zoomOut);

	void TogglePause();
	void ToggleDevMode();

	void SetCameraTarget(Vector3 newTarget);
	void SetDollyCamTarget(Vector3 newTarget);
	//void MoveCursor(const IntVector2 direction);
	void TransitionToState(GameState* newState);
	void TriggerTransition();

	AABB2 SetUICamera();
	AABB2 SetMainCamera();

	AABB2 GetUIBounds();
	bool IsDevMode() {return m_devMode;};

	FlyoutText* AddFlyoutText(const std::string& text, const Vector3& pos, float textSize = .5f, float durationSeconds= .5f, const RGBA& tint = RGBA::WHITE, float speed = 3.f);


	EncounterState* GetEncounterState() const { return m_encounterState; }
	void SetCursorPos(IntVector2 newPos);
	IntVector2 GetCursorPosXY();

	bool SpawnActor(IntVector2 position, std::string type, int factionNumber);

	enum EntityTypes{
		NUM_ENTITIES
	};

private:

	void LoadTileDefinitions();
	void LoadMapDefinitions();
	void LoadSprites();
	void LoadSpritesFromFile(std::string xmlFile);
	void LoadIsoSpritesFromFile(std::string xmlFile);
	void LoadIsoAnimsFromFile(std::string xmlFile);
	void LoadAnimSetsFromFile(std::string xmlFile);

	float m_startTransitionTime = 0.f;
	float m_timeEnteredState = 0.f;
	float m_transitionLength = .5f;
	float m_deltaSeconds = 0.f;


};

extern Game* g_theGame;