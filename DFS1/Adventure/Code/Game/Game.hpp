#pragma once
#include "GameCommon.hpp"
#include "Game/GameState.hpp"
#include "Engine/DataTypes/DirectedGraph.hpp"
#include "Game/StoryGraph.hpp"


class Entity;
class Map;
class Actor;
class Party;
class Adventure;
class DebugRenderSystem;

class Game{

public:
	~Game();											// destructor: do nothing (for speed)
	Game();											// default constructor: do nothing (for speed)

	SpriteRenderPath*	m_renderPath	= nullptr;
	//RenderScene*		m_currentScene	= nullptr;

	bool m_isPaused;
	bool m_devMode;
	bool m_fullMapMode;
	bool m_isMapViewerMode = false;
	bool m_renderingEdges = true;
	Clock* m_gameClock;
	Camera* m_camera	= nullptr;		//might change 
	Camera* m_uiCamera	= nullptr;		//stays the same from encounter to encounter
	Party* m_party = nullptr;

	GameState* m_currentState = nullptr;
	GameState* m_transitionToState = nullptr;

	float m_timeEnteredState;
	bool m_isFinishedTransitioning;
	float m_transitionLength;

	unsigned int m_indexOfSelectedMenuItem = 0;
	SpriteSheet* m_miscSpriteSheet = nullptr;
	SpriteSheet* m_speechBubbleSpriteSheet = nullptr;

	SoundID m_attractMusicID;
	SoundPlaybackID m_attractPlayback;
	SoundID m_victoryMusicID;
	SoundPlaybackID m_victoryPlayback;
	AABB2 m_dialogueBox;

	Vector2 m_normalizedMousePos;

	DebugRenderSystem* m_debugRenderSystem;

	Vector2 GetPlayerPosition() const;

	void PostStartup();

	void Update	(float deltaSeconds);
	void HandleInput();

	void Render();
	void RenderSelectArrow(AABB2 boxToDrawIn);
	void RenderXboxStartButton(AABB2 boxToDrawIn);
	void RenderXboxBackButton(AABB2 boxToDrawIn);

	void Pause();
	void Unpause();
	void TogglePause();
	void ToggleDevMode();
	inline bool IsDevMode() const {return m_devMode;};

	void TransitionToState(GameState* newState);
	void TriggerTransition();
	AABB2 SetUICamera();
	AABB2 SetGameCamera();
	AABB2 GetUIBounds();
	AABB2 GetMainCameraBounds();

	float GetDeltaSeconds();

	void UpdateMenuSelection(int direction = 1);

	void DebugWinAdventure();
	void DebugCompleteQuest(int index = 0);
	void DebugSetDifficulty(int difficulty);
	void DebugSetPlayerSpeed(int newSpeed);
	void DebugSpawnActor(std::string actorName);
	void DebugSpawnItem(std::string itemName);
	void DebugRerollPlayerAppearance();

	void SetCurrentMap(Map* newMap);
	void GoToMap(std::string mapName);

	void ToggleState(bool& stateToToggle);
	void LookAtNextMap(int direction);

	void ShowActorStats();

	/*=========
	Thesis Stuff
	==========*/
	StoryGraph* m_graph;
	bool m_thesisMode;

	

	void InitGraphDefault();
	void InitGraphMurder();
	void InitGraphDialogue();

	void InitCharacterArray();

	void GenerateGraph();
	void ClearGraph();
	void GeneratePlotNodes(int numToGenerate = NUM_PLOT_NODES_TO_GENERATE);
	void GenerateDetailNodes(int numToGenerate = NUM_DETAIL_NODES_TO_GENERATE);

	void GenerateNodePairs(int numToGenerate = NUM_NODE_PAIRS_TO_GENERATE);
	void AddPlotAndOutcomeNodeInPair();


private:
	void LoadSounds();
	void LoadTileDefinitions();
	void LoadMapDefinitions();
	void LoadClothingDefinitions();
	void LoadEntityDefinitions();
	void LoadActorDefinitions();
	void LoadProjectileDefinitions();
	void LoadPortalDefinitions();
	void LoadItemDefinitions();
	void LoadDecorationDefinitions();
	void LoadAdventureDefinitions();
	void LoadQuestDefinitions();
	void LoadVillageDefinitions();

	void LoadGraphDatasets();

	int m_screenWidth = 10;


	RGBA m_currentFadeColor;
	RGBA m_startFadeColor;
};

RENDER_SLOT GetRenderSlotForEquipSlot(EQUIPMENT_SLOT slot);

bool WasBackJustPressed();		//'back' functionality - esc, b
bool WasStartJustPressed();		//BROAD 'start' functionality - p, enter, start, a
bool WasSelectJustPressed();		//enter or a ONLY
bool WasPauseJustPressed();			//p or start ONLY
bool WasExitJustPressed();			//ESC only
bool WasUpJustPressed();
bool WasDownJustPressed();
bool WasRightJustPressed();
bool WasLeftJustPressed();

bool IsUpKeyDown();
bool IsDownKeyDown();
bool IsRightKeyDown();
bool IsLeftKeyDown();
void CheckArrowKeys();

extern Game* g_theGame;