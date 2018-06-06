#pragma once
#include "GameCommon.hpp"
#include "Game/GameState.hpp"


class Entity;
class Map;
class Player;
class Adventure;

class Game{

public:
	~Game();											// destructor: do nothing (for speed)
	Game();											// default constructor: do nothing (for speed)

	SpriteRenderPath*	m_renderPath	= nullptr;
	//RenderScene*		m_currentScene	= nullptr;

	bool m_isPaused;
	bool m_devMode;
	bool m_fullMapMode;
	float m_gameTime;
	//Map* m_currentMap;
	Adventure* m_currentAdventure;
	Camera* m_camera	= nullptr;		//might change 
	Camera* m_uiCamera	= nullptr;		//stays the same from encounter to encounter
	//std::map<std::string, Map*> m_mapsByName;
	Player* m_player;

	GameState* m_currentState = nullptr;
	GameState* m_transitionToState = nullptr;

	//GAME_STATE m_currentState = STATE_ATTRACT;
	//GAME_STATE m_transitionToState = NO_STATE;
	float m_timeEnteredState;
	bool m_isFinishedTransitioning;
	float m_transitionLength;

	unsigned int m_indexOfSelectedMenuItem = 0;
	SpriteSheet* m_miscSpriteSheet = nullptr;

	SoundID m_attractMusicID;
	SoundPlaybackID m_attractPlayback;
	SoundID m_victoryMusicID;
	SoundPlaybackID m_victoryPlayback;

	static SpriteAnimSetDef* s_humanoidAnimSetDef;

	Vector2 GetPlayerPosition() const;


	void Update			(float deltaSeconds);
	void UpdateAttract	(float deltaSeconds);
	void UpdatePlaying	(float deltaSeconds);
	void UpdatePaused	(float deltaSeconds);
	void UpdateInventory(float deltaSeconds);
	void UpdateVictory	(float deltaSeconds);
	void UpdateDefeat	(float deltaSeconds);
	void UpdateMapMode	(float deltaSeconds);

	void Render();
	void RenderAttract();
	void RenderPlaying();
	void RenderPaused();
	void RenderInventory();
	void RenderVictory();
	void RenderDefeat();
	void DrawCurrentInventory();
	void RenderSelectArrow(AABB2 boxToDrawIn);
	void RenderMapMode();
	void RenderVictoryConditionsInBox(AABB2 boxToDrawIn);
	void RenderXboxStartButton(AABB2 boxToDrawIn);
	void RenderXboxBackButton(AABB2 boxToDrawIn);

	void TogglePause();
	void ToggleDevMode();

	void TransitionToState(GameState* newState);
	void TriggerTransition();
	AABB2 SetUICamera();
	AABB2 SetMainCamera();
	AABB2 GetUIBounds();
	AABB2 GetMainCameraBounds();

	void UpdateMenuSelection(int direction = 1);

	//void StartStateTransition(GAME_STATE newState, float transitionTime = .5f, RGBA transitionColor = RGBA(0,0,0,255));
	//void Transition();
	//void FadeIn();
	void StartAdventure(std::string adventureDefName);

	void ToggleState(bool& stateToToggle);
	void LookAtNextMap(int direction);

private:
	void LoadSounds();
	void LoadTileDefinitions();
	void LoadMapDefinitions();
	void LoadEntityDefinitions();
	void LoadActorDefinitions();
	void LoadProjectileDefinitions();
	void LoadPortalDefinitions();
	void LoadItemDefinitions();
	void LoadAdventureDefinitions();

	int m_screenWidth = 10;

	void SpawnRandomTestGoblin();

	RGBA m_currentFadeColor;
	RGBA m_startFadeColor;
};


bool WasBackJustPressed();		//'back' functionality - esc, b
bool WasStartJustPressed();		//BROAD 'start' functionality - p, enter, start, a
bool WasSelectJustPressed();		//enter or a ONLY
bool WasPauseJustPressed();			//p or start ONLY
bool WasExitJustPressed();			//ESC only
bool WasUpJustPressed();
bool WasDownJustPressed();
bool WasRightJustPressed();
bool WasLeftJustPressed();
void CheckArrowKeys();

extern Game* g_theGame;