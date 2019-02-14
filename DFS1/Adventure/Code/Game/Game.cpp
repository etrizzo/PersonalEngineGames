#include "Game.hpp"
#include "Game/TileDefinition.hpp"
#include "Game/MapDefinition.hpp"
#include "Game/ActorDefinition.hpp"
#include "Game/ProjectileDefinition.hpp"
#include "Game/PortalDefinition.hpp"
#include "Game/AdventureDefinition.hpp"
#include "Game/Adventure.hpp"
#include "Game/ItemDefinition.hpp"
#include "Game/Item.hpp"
#include "Game/DecorationDefinition.hpp"
#include "Game/Decoration.hpp"
#include "Game/VictoryCondition.hpp"
#include "Engine/Renderer/SpriteAnimSetDef.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Game/DebugRenderSystem.hpp"
#include "Game/QuestDefinition.hpp"

#include "Game/Map.hpp"
#include "Game/Party.hpp"
#include <map>



Game::~Game()
{
	//delete m_currentAdventure;
	delete m_party;
	delete g_tileSpriteSheet;
	delete m_currentState;

	//delete all the definitions
}

Game::Game()
{

	m_renderPath = new SpriteRenderPath();
	m_renderPath->m_renderer = g_theRenderer;

	Texture* tileTexture = g_theRenderer->CreateOrGetTexture("Terrain_32x32.png", IMAGE_DIRECTORY, false);
	g_tileSpriteSheet = new SpriteSheet(*tileTexture, 32,32);
	Texture* miscTexture = g_theRenderer->CreateOrGetTexture("MiscItems_4x4.png");
	m_miscSpriteSheet = new SpriteSheet(*miscTexture, 4,4);
	Texture* bubbleTexture = g_theRenderer->CreateOrGetTexture("speechbubbles_5x6.png");
	m_speechBubbleSpriteSheet = new SpriteSheet(*bubbleTexture, 6,5);
	Texture* portraitTexture = g_theRenderer->CreateOrGetTexture("HUMANOIDS/PORTRAITS/portrait_all.png");
	g_portraitSpriteSheet = new SpriteSheet(*portraitTexture, 17,9);

	g_theRenderer->LoadSpritesFromSpriteAtlas(m_speechBubbleSpriteSheet,32);

	m_isPaused = false;
	m_devMode = false;
	m_fullMapMode = false;
	//m_gameTime = 0.f;
	m_gameClock = new Clock(GetMasterClock());
	
	m_thesisMode = g_gameConfigBlackboard.GetValue("ThesisOnlyMode", false);

	if (!m_thesisMode){
		LoadSounds();
		LoadTileDefinitions();
		LoadMapDefinitions();

		LoadClothingDefinitions();

		LoadEntityDefinitions();
		LoadQuestDefinitions();
		LoadAdventureDefinitions();
	}

	m_screenWidth = 10;
	m_camera = new Camera();
	m_camera->SetColorTarget( g_theRenderer->m_defaultColorTarget );
	m_camera->SetDepthStencilTarget( g_theRenderer->m_defaultDepthTarget );
	m_camera->SetProjectionOrtho(m_screenWidth, g_gameConfigBlackboard.GetValue("windowAspect", 1.f), 0.f,100.f);
	//Vector2 center = m_camera->GetBounds().GetCenter();
	//m_camera->LookAt(Vector3(center.x, center.y, -1.f), Vector3(center.x, center.y, .5f));


	// Setup ui camera - draws from (0,0) to (1,1) always
	m_uiCamera = new Camera();

	// Setup what it will draw to
	m_uiCamera->SetColorTarget( g_theRenderer->m_defaultColorTarget );
	m_uiCamera->SetDepthStencilTarget( g_theRenderer->m_defaultDepthTarget );

	m_uiCamera->SetProjectionOrtho(1.f, g_gameConfigBlackboard.GetValue("windowAspect", 1.f), 0.f,100.f);


	m_dialogueBox = m_uiCamera->GetBounds().GetPercentageBox(.18f,0.03f,.82f,.23f);

	g_theRenderer->SetCamera( m_camera ); 

	g_theRenderer->ClearDepth( 1.0f ); 
	g_theRenderer->EnableDepth( COMPARE_LESS, true );
	m_debugRenderSystem = new DebugRenderSystem();
	m_debugRenderSystem->Startup(m_camera);
	m_debugRenderSystem->ToggleInfo();
	m_debugRenderSystem->ToggleScreenGrid();

	m_party =  new Party();
}

Vector2 Game::GetPlayerPosition() const
{
	if (m_party != nullptr){
		return m_party->GetPlayerCharacter()->GetPosition();
	}
	return Vector2::ZERO;
}

void Game::PostStartup()
{
	LoadVillagerNames();
	LoadVillageNames();
	m_graph = StoryGraph();
	

	InitGraphMurder();
	GenerateGraph();
}

void Game::LoadVillagerNames()
{
	FILE *fp = nullptr;
	fopen_s( &fp, "Data/Data/VillagerNames.txt", "r" );
	char lineCSTR [1000];
	std::string line;
	int MAX_LINE_LENGTH = 1000;

	ASSERT_OR_DIE(fp != nullptr, "NO VILLAGER NAME FILE FOUND");
	while (fgets( lineCSTR, MAX_LINE_LENGTH, fp ) != NULL)
	{
		line = "";
		line.append(lineCSTR);
		Strip(line, '\n');
		m_villagerNames.push_back(line);
	}
	int x = 0;
}

void Game::LoadVillageNames()
{
	FILE *fp = nullptr;
	fopen_s( &fp, "Data/Data/VillageNames.txt", "r" );
	char lineCSTR [1000];
	std::string line;
	int MAX_LINE_LENGTH = 1000;

	ASSERT_OR_DIE(fp != nullptr, "NO VILLAGE NAME FILE FOUND");
	while (fgets( lineCSTR, MAX_LINE_LENGTH, fp ) != NULL)
	{
		line = "";
		line.append(lineCSTR);
		Strip(line, '\n');
		m_villageNames.push_back(line);
	}
	int x = 0;
}

void Game::Update(float deltaSeconds)
{
	//m_gameTime+=deltaSeconds;
	if(!m_isPaused){
		m_currentState->Update(deltaSeconds);
	}
}

//void Game::UpdateAttract(float deltaSeconds)
//{
//	if (!m_isFinishedTransitioning && m_transitionToState != nullptr){
//		float timeInTranstion = m_gameTime - m_timeEnteredState;
//		float percThroughTransition = timeInTranstion / m_transitionLength;
//		g_theAudio->SetSoundPlaybackVolume(m_attractPlayback, 1.f - percThroughTransition);
//	}
//}


void Game::HandleInput()
{
	m_normalizedMousePos = g_theInput->GetMouseNormalizedScreenPosition(GetUIBounds());
	m_currentState->HandleInput();
}

void Game::Render()
{
	g_theRenderer->SetCullMode(CULLMODE_NONE);
	g_theRenderer->DisableDepth();
	g_theRenderer->ClearScreen(RGBA::BLACK);

	g_theGame->SetGameCamera();
	g_theRenderer->BindCamera(m_camera);
	m_currentState->Render();
	g_theGame->SetGameCamera();

	m_debugRenderSystem->UpdateAndRender();

}


void Game::RenderSelectArrow(AABB2 boxToDrawIn)
{
	g_theRenderer->DrawTextInBox2D(">", boxToDrawIn, Vector2(.5f,.5f), boxToDrawIn.GetHeight() * .8f);
}


void Game::RenderXboxStartButton(AABB2 boxToDrawIn)
{
	const Texture* buttonTexture = m_miscSpriteSheet->GetTexture();
	AABB2 texCoords = m_miscSpriteSheet->GetTexCoordsForSpriteCoords(IntVector2(3,0));
	g_theRenderer->DrawTexturedAABB2(boxToDrawIn, *buttonTexture, texCoords.mins, texCoords.maxs, RGBA::WHITE );
}

void Game::RenderXboxBackButton(AABB2 boxToDrawIn)
{

	const Texture* buttonTexture = m_miscSpriteSheet->GetTexture();
	AABB2 texCoords = m_miscSpriteSheet->GetTexCoordsForSpriteCoords(IntVector2(2,0));
	g_theRenderer->DrawTexturedAABB2(boxToDrawIn, *buttonTexture, texCoords.mins, texCoords.maxs, RGBA::WHITE );

}

void Game::Pause()
{
	m_isPaused = true;
}

void Game::Unpause()
{
	m_isPaused = false;
}

void Game::TogglePause()
{
	if ( !m_isPaused){
		m_isPaused = true;
	} else {
		m_isPaused = false;
	}
}

void Game::ToggleDevMode()
{
	m_devMode = !m_devMode;
}

void Game::TransitionToState(GameState* newState)
{
	m_transitionToState = newState;
	m_currentState->StartTransition();
}

void Game::TriggerTransition()
{
	
	
	//m_transitionToState->m_soundtrackPlayback = m_currentState->m_soundtrackPlayback;
	m_currentState = m_transitionToState;
	m_currentState->Transition();
	m_transitionToState = nullptr;
	m_timeEnteredState = m_gameClock->GetCurrentSeconds();
}

AABB2 Game::SetUICamera()
{
	g_theRenderer->SetCamera( m_uiCamera ); 

	g_theRenderer->DisableDepth();
	return m_uiCamera->GetBounds();
}

AABB2 Game::SetGameCamera()
{
	g_theRenderer->SetCamera( m_camera ); 

	g_theRenderer->ClearDepth( 1.0f ); 
	g_theRenderer->DisableDepth();
	return m_camera->GetBounds();		//not really gonna be necessary, probably
}

AABB2 Game::GetUIBounds()
{
	return m_uiCamera->GetBounds();
}

AABB2 Game::GetMainCameraBounds()
{
	return m_camera->GetBounds();
}

float Game::GetDeltaSeconds()
{
	return m_gameClock->GetDeltaSeconds();
}

//Map * Game::CreateMap(std::string mapName, std::string mapType)
//{
//	MapDefinition* newMapDef = MapDefinition::GetMapDefinition(mapType);
//	if (newMapDef == nullptr){
//		ERROR_AND_DIE("No such map");
//	}
//	Map* newMap = new Map(mapName, newMapDef);
//	m_mapsByName.insert(std::pair<std::string, Map*> (mapName, newMap));
//	return newMap;
//}

void Game::UpdateMenuSelection(int direction)
{
	m_indexOfSelectedMenuItem+=direction;
}


void Game::DebugWinAdventure()
{
	if (m_currentState->m_currentAdventure != nullptr){
		m_currentState->m_currentAdventure->DebugWinAdventure();
	} else {
		ConsolePrintf(RGBA::RED, "No current adventure - start the game");
	}
}

void Game::DebugCompleteQuest(int index)
{
	if (m_currentState->m_currentAdventure != nullptr){
		m_currentState->m_currentAdventure->DebugCompleteQuest(index);
	} else {
		ConsolePrintf(RGBA::RED, "No current adventure - start the game");
	}
}

void Game::DebugSetDifficulty(int difficulty)
{
	if (m_currentState->m_currentAdventure != nullptr){
		m_currentState->m_currentAdventure->SetDifficulty(difficulty);
	} else {
		ConsolePrintf(RGBA::RED, "No current adventure - start the game");
	}
}

void Game::DebugSetPlayerSpeed(int newSpeed)
{
	g_theGame->m_party->GetPlayerCharacter()->m_stats.SetStat(STAT_MOVEMENT, newSpeed);
}

void Game::DebugSpawnActor(std::string actorName)
{
	if (m_currentState->m_currentAdventure != nullptr){
		Vector2 pos = g_theGame->m_party->GetPlayerCharacter()->GetPosition();
		pos = pos + Vector2::ONE * GetRandomFloatInRange(-5.f, 5.f);
		m_currentState->m_currentAdventure->m_currentMap->SpawnNewActor(actorName, pos);
	} else {
		ConsolePrintf(RGBA::RED, "No current adventure - start the game");
	}
	
}

void Game::DebugSpawnItem(std::string itemName)
{
	if (m_currentState->m_currentAdventure != nullptr){
		Vector2 pos = g_theGame->m_party->GetPlayerCharacter()->GetPosition();
		pos = pos + Vector2::ONE * GetRandomFloatInRange(-5.f, 5.f);
		m_currentState->m_currentAdventure->m_currentMap->SpawnNewItem(itemName, pos);
	} else {
		ConsolePrintf(RGBA::RED, "No current adventure - start the game");
	}
}

void Game::DebugRerollPlayerAppearance()
{
	m_party->m_currentPlayer->RandomizeAppearance();
}

void Game::SetCurrentMap(Map * newMap)
{
	m_currentState->m_currentAdventure->SetCurrentMap(newMap);
}

void Game::GoToMap(std::string mapName)
{
	if (m_currentState->m_currentAdventure != nullptr){
		m_currentState->m_currentAdventure->GoToMap(mapName);
	} else {
		ConsolePrintf(RGBA::RED, "No current adventure - start the game");
	}
}

void Game::ToggleState(bool & stateToToggle)
{
	stateToToggle = !stateToToggle;
}

void Game::LookAtNextMap(int direction)
{
	m_currentState->m_currentAdventure->MoveToMapIndex(direction);
}

void Game::ShowActorStats()
{
	if (m_currentState->m_currentAdventure != nullptr){
		for (Actor* actor : m_currentState->m_currentAdventure->m_currentMap->m_allActors){
			ConsolePrintf(RGBA::GREEN, actor->GetName().c_str());
			std::string healthString = "  HEALTH: " + std::to_string(actor->m_health);
			ConsolePrintf(RGBA::YELLOW, healthString.c_str());
			for (int i = STAT_STRENGTH; i < NUM_STAT_IDS; i++){
				std::string statString = "   " + actor->m_stats.GetNameForStatID((STAT_ID) i) + " : " + std::to_string(actor->m_stats.GetStat((STAT_ID) i));
				ConsolePrintf(RGBA::CYAN, statString.c_str());
			}
		}
	}
}

void Game::LoadSounds()
{
	std::string attractPath = g_gameConfigBlackboard.GetValue("AttractMusic", std::string());
	std::string victoryPath = g_gameConfigBlackboard.GetValue("VictoryMusic", std::string());

	m_attractMusicID = g_theAudio->CreateOrGetSound(attractPath);
	m_victoryMusicID = g_theAudio->CreateOrGetSound(victoryPath);

}

void Game::LoadTileDefinitions()
{
	//read edge definitions
	tinyxml2::XMLDocument tileEdgeDoc;
	tileEdgeDoc.LoadFile("Data/Data/TileEdges.xml");
	tinyxml2::XMLElement* edgeroot = tileEdgeDoc.FirstChildElement("EdgeDefinitions");
	for (tinyxml2::XMLElement* edgeDefElement = edgeroot->FirstChildElement("EdgeDefinition"); edgeDefElement != NULL; edgeDefElement = edgeDefElement->NextSiblingElement("EdgeDefinition")){
		TileEdgeDefinition* newDefinition = new TileEdgeDefinition(edgeDefElement);
		//TileDefinition::s_definitions.insert(std::pair<std::string, TileDefinition*>(newDefinition->m_name, newDefinition));
	}

	//read tile definitions
	tinyxml2::XMLDocument tileDefDoc;
	tileDefDoc.LoadFile("Data/Data/Tiles.xml");
	tinyxml2::XMLElement* root = tileDefDoc.FirstChildElement("TileDefinitions");
	for (tinyxml2::XMLElement* tileDefElement = root->FirstChildElement("TileDefinition"); tileDefElement != NULL; tileDefElement = tileDefElement->NextSiblingElement("TileDefinition")){
		TileDefinition* newDefinition = new TileDefinition(tileDefElement);
		TileDefinition::s_definitions.insert(std::pair<std::string, TileDefinition*>(newDefinition->m_name, newDefinition));
	}
}

void Game::LoadMapDefinitions()
{
	tinyxml2::XMLDocument mapDefDoc;
	mapDefDoc.LoadFile("Data/Data/Maps.xml");


	tinyxml2::XMLElement* root = mapDefDoc.FirstChildElement("MapDefinitions");
	for (tinyxml2::XMLElement* tileDefElement = root->FirstChildElement("MapDefinition"); tileDefElement != NULL; tileDefElement = tileDefElement->NextSiblingElement("MapDefinition")){
		//had to make make MapDefinition take in pointer because was getting inaccessible error when trying to derference
		MapDefinition* newDefinition = new MapDefinition(tileDefElement);

		MapDefinition::s_definitions.insert(std::pair<std::string, MapDefinition*>(newDefinition->m_name, newDefinition));
	}
}

void Game::LoadClothingDefinitions()
{
	tinyxml2::XMLDocument mapDefDoc;
	mapDefDoc.LoadFile("Data/Data/ClothingSets.xml");


	tinyxml2::XMLElement* root = mapDefDoc.FirstChildElement("ClothingSets");
	for (tinyxml2::XMLElement* setDefElement = root->FirstChildElement("ClothingSet"); setDefElement != NULL; setDefElement = setDefElement->NextSiblingElement("ClothingSet")){
		//had to make make MapDefinition take in pointer because was getting inaccessible error when trying to derference
		ClothingSetDefinition* newDefinition = new ClothingSetDefinition(setDefElement);

		ClothingSetDefinition::s_definitions.insert(std::pair<std::string, ClothingSetDefinition*>(newDefinition->m_name, newDefinition));
	}
}

void Game::LoadEntityDefinitions()
{
	//tinyxml2::XMLDocument humanoidDefDoc;
	//humanoidDefDoc.LoadFile("Data/Data/Humanoid.xml");
	SpriteAnimSetDef::LoadSetsFromFile("Humanoid.xml");
	//tinyxml2::XMLElement* spriteAnimDefElement = humanoidDefDoc.FirstChildElement("SpriteAnimSet");
	//s_humanoidAnimSetDef = new SpriteAnimSetDef(spriteAnimDefElement, *g_theRenderer);

	LoadItemDefinitions();
	LoadActorDefinitions();
	LoadProjectileDefinitions();
	LoadPortalDefinitions();
	LoadDecorationDefinitions();
}

void Game::LoadActorDefinitions()
{
	tinyxml2::XMLDocument actorDefDoc;
	actorDefDoc.LoadFile("Data/Data/Actors.xml");

	tinyxml2::XMLElement* root = actorDefDoc.FirstChildElement("ActorDefinitions");
	for (tinyxml2::XMLElement* tileDefElement = root->FirstChildElement("ActorDefinition"); tileDefElement != NULL; tileDefElement = tileDefElement->NextSiblingElement("ActorDefinition")){
		//had to make make MapDefinition take in pointer because was getting inaccessible error when trying to dereference
		ActorDefinition* newDefinition = new ActorDefinition(tileDefElement);
		ActorDefinition::s_definitions.insert(std::pair<std::string, ActorDefinition*>(newDefinition->m_name, newDefinition));
	}
}

void Game::LoadProjectileDefinitions()
{
	tinyxml2::XMLDocument projectileDefDoc;
	projectileDefDoc.LoadFile("Data/Data/Projectiles.xml");

	tinyxml2::XMLElement* root = projectileDefDoc.FirstChildElement("ProjectileDefinitions");
	for (tinyxml2::XMLElement* projectileDefElement = root->FirstChildElement("ProjectileDefinition"); projectileDefElement != NULL; projectileDefElement = projectileDefElement->NextSiblingElement("ProjectileDefinition")){
		ProjectileDefinition* newDefinition = new ProjectileDefinition(projectileDefElement);
		ProjectileDefinition::s_definitions.insert(std::pair<std::string, ProjectileDefinition*>(newDefinition->m_name, newDefinition));
	}
}

void Game::LoadPortalDefinitions()
{
	tinyxml2::XMLDocument projectileDefDoc;
	projectileDefDoc.LoadFile("Data/Data/Portals.xml");

	tinyxml2::XMLElement* root = projectileDefDoc.FirstChildElement("PortalDefinitions");
	for (tinyxml2::XMLElement* projectileDefElement = root->FirstChildElement("PortalDefinition"); projectileDefElement != NULL; projectileDefElement = projectileDefElement->NextSiblingElement("PortalDefinition")){
		PortalDefinition* newDefinition = new PortalDefinition(projectileDefElement);
		PortalDefinition::s_definitions.insert(std::pair<std::string, PortalDefinition*>(newDefinition->m_name, newDefinition));
	}
}

void Game::LoadItemDefinitions()
{
	tinyxml2::XMLDocument itemDefDoc;
	itemDefDoc.LoadFile("Data/Data/Items.xml");

	tinyxml2::XMLElement* root = itemDefDoc.FirstChildElement("ItemDefinitions");
	for (tinyxml2::XMLElement* itemDefElement = root->FirstChildElement("ItemDefinition"); itemDefElement != NULL; itemDefElement = itemDefElement->NextSiblingElement("ItemDefinition")){
		ItemDefinition* newDefinition = new ItemDefinition(itemDefElement);
		ItemDefinition::s_definitions.insert(std::pair<std::string, ItemDefinition*>(newDefinition->m_name, newDefinition));
	}
}

void Game::LoadDecorationDefinitions()
{
	tinyxml2::XMLDocument decoDefDoc;
	decoDefDoc.LoadFile("Data/Data/Decorations.xml");

	tinyxml2::XMLElement* root = decoDefDoc.FirstChildElement("DecorationDefinitions");
	for (tinyxml2::XMLElement* decoDefElement = root->FirstChildElement("DecorationDefinition"); decoDefElement != NULL; decoDefElement = decoDefElement->NextSiblingElement("DecorationDefinition")){
		DecorationDefinition* newDefinition = new DecorationDefinition(decoDefElement);
		DecorationDefinition::s_definitions.insert(std::pair<std::string, DecorationDefinition*>(newDefinition->m_name, newDefinition));
	}
}

void Game::LoadAdventureDefinitions()
{
	tinyxml2::XMLDocument projectileDefDoc;
	projectileDefDoc.LoadFile("Data/Data/Adventures.xml");

	tinyxml2::XMLElement* root = projectileDefDoc.FirstChildElement("AdventureDefinitions");
	for (tinyxml2::XMLElement* projectileDefElement = root->FirstChildElement("AdventureDefinition"); projectileDefElement != NULL; projectileDefElement = projectileDefElement->NextSiblingElement("AdventureDefinition")){
		AdventureDefinition* newDefinition = new AdventureDefinition(projectileDefElement);
		AdventureDefinition::s_definitions.insert(std::pair<std::string, AdventureDefinition*>(newDefinition->m_name, newDefinition));
	}
}

void Game::LoadQuestDefinitions()
{
	tinyxml2::XMLDocument questDefDoc;
	questDefDoc.LoadFile("Data/Data/Quests.xml");

	tinyxml2::XMLElement* root = questDefDoc.FirstChildElement("Quests");
	for (tinyxml2::XMLElement* questDefElement = root->FirstChildElement("Quest"); questDefElement != NULL; questDefElement = questDefElement->NextSiblingElement("Quest")){
		QuestDefinition* newDefinition = new QuestDefinition(questDefElement);
		QuestDefinition::s_definitions.insert(std::pair<std::string, QuestDefinition*>(newDefinition->m_name, newDefinition));
	}
}


void Game::InitGraphDefault()
{

	ClearGraph();

	ResetGraphData();
	ReadPlotNodes("Data/Data/GraphData/PlotGrammars.xml");
	ReadOutcomeNodes("Data/Data/GraphData/DetailGrammars.xml");
	ReadCharacters("Data/Data/GraphData/Characters.xml");
	InitCharacterArray();

	m_graph.GenerateStartAndEnd();
	ConsolePrintf("Default data loaded.");
}

void Game::InitGraphMurder()
{
	ClearGraph();

	ResetGraphData();
	ReadPlotNodes("Data/Data/GraphData/MurderMystery_EventNodes.xml");
	ReadOutcomeNodes("Data/Data/GraphData/MurderMystery_OutcomeNodes.xml");
	ReadCharacters("Data/Data/GraphData/MurderMystery_Characters.xml");
	InitCharacterArray();

	m_graph.GenerateStartAndEnd();
	ConsolePrintf("Murder mystery data loaded.");
}

void Game::InitGraphDialogue()
{
	ClearGraph();

	ResetGraphData();
	ReadPlotNodes("Data/Data/GraphData/Dialogue_EventNodes.xml");
	ReadOutcomeNodes("Data/Data/GraphData/Dialogue_OutcomeNodes.xml");
	ReadCharacters("Data/Data/GraphData/Dialogue_Characters.xml");
	InitCharacterArray();

	m_graph.GenerateStartAndEnd();
	ConsolePrintf("Dialogue data loaded.");
}

void Game::ReadPlotNodes(std::string filePath)
{
	m_graph.ReadEventNodesFromXML(filePath);
}

void Game::ReadOutcomeNodes(std::string filePath)
{
	m_graph.ReadDetailNodesFromXML(filePath);
}

void Game::ReadCharacters(std::string filePath)
{
	m_graph.ReadCharactersFromXML(filePath);
}

void Game::InitCharacterArray()
{
	m_graph.SelectCharactersForGraph();
}

void Game::ResetGraphData()
{
	m_graph.ClearGraphData();
}

void Game::GenerateGraph()
{
	//srand(1);
	bool generated = false;
	while (!generated){
		ClearGraph();
		m_graph.RunGenerationPairs(NUM_NODE_PAIRS_TO_GENERATE);
		generated = m_graph.AddEndingsToEachBranch();
	}
}

void Game::ClearGraph()
{
	m_graph.Clear();
}

void Game::GeneratePlotNodes(int numToGenerate)
{
	m_graph.GenerateSkeleton(numToGenerate);
}

void Game::GenerateDetailNodes(int numToGenerate)
{
	m_graph.AddDetailNodesToDesiredSize(numToGenerate + m_graph.GetNumNodes());
}

void Game::GenerateNodePairs(int numToGenerate)
{
	for (int i = 0; i < numToGenerate; i++){
		AddPlotAndOutcomeNodeInPair();
	}
}

void Game::AddPlotAndOutcomeNodeInPair()
{
	StoryNode* newNode = m_graph.AddSingleEventNode();
	m_graph.AddOutcomeNodesToEventNode(newNode);
}






Game* g_theGame = nullptr;

bool WasStartJustPressed()
{
	return g_theInput->WasKeyJustPressed(VK_RETURN) || g_theInput->WasKeyJustPressed('P') || g_primaryController->WasButtonJustPressed(XBOX_START) || g_primaryController->WasButtonJustPressed(XBOX_A);
}

bool WasPauseJustPressed()
{
	return g_theInput->WasKeyJustPressed('P') || g_primaryController->WasButtonJustPressed(XBOX_START);
}

RENDER_SLOT GetRenderSlotForEquipSlot(EQUIPMENT_SLOT slot)
{
	switch(slot){
	case EQUIP_SLOT_CHEST:
		return CHEST_SLOT;
		break;
	case EQUIP_SLOT_HEAD:
		return HAT_SLOT;
		break;
	case EQUIP_SLOT_LEGS:
		return LEGS_SLOT;
		break;
	case EQUIP_SLOT_WEAPON:
		return WEAPON_SLOT;
	}
	return NUM_RENDER_SLOTS;
}

bool WasBackJustPressed()
{
	return g_theInput->WasKeyJustPressed(VK_ESCAPE) || g_primaryController->WasButtonJustPressed(XBOX_B);

}

bool WasSelectJustPressed()
{
	return g_theInput->WasKeyJustPressed(VK_RETURN) || g_primaryController->WasButtonJustPressed(XBOX_A);

}
bool WasExitJustPressed()
{
	return g_theInput->WasKeyJustPressed(VK_ESCAPE);
}

bool WasUpJustPressed()
{
	return g_theInput->WasKeyJustPressed(VK_UP) || g_theInput->WasKeyJustPressed('W') || g_primaryController->WasButtonJustPressed(XBOX_D_UP);
}

bool WasDownJustPressed()
{
	return g_theInput->WasKeyJustPressed(VK_DOWN) || g_theInput->WasKeyJustPressed('S') || g_primaryController->WasButtonJustPressed(XBOX_D_DOWN);
}

bool WasRightJustPressed()
{
	return g_theInput->WasKeyJustPressed(VK_RIGHT) || g_theInput->WasKeyJustPressed('D') || g_primaryController->WasButtonJustPressed(XBOX_D_RIGHT);
}

bool WasLeftJustPressed()
{
	return g_theInput->WasKeyJustPressed(VK_LEFT) || g_theInput->WasKeyJustPressed('A') ||g_primaryController->WasButtonJustPressed(XBOX_D_LEFT);
}

bool IsUpKeyDown()
{
	return g_theInput->IsKeyDown(VK_LEFT) || g_theInput->IsKeyDown('W') || g_primaryController->IsButtonDown(XBOX_D_LEFT);
}

bool IsDownKeyDown()
{
	return g_theInput->IsKeyDown(VK_LEFT) || g_theInput->IsKeyDown('S') || g_primaryController->IsButtonDown(XBOX_D_LEFT);
}

bool IsRightKeyDown()
{
	return g_theInput->IsKeyDown(VK_LEFT) || g_theInput->IsKeyDown('D') || g_primaryController->IsButtonDown(XBOX_D_LEFT);
}

bool IsLeftKeyDown()
{
	return g_theInput->IsKeyDown(VK_LEFT) || g_theInput->IsKeyDown('A') || g_primaryController->IsButtonDown(XBOX_D_LEFT);
}

void CheckArrowKeys()
{
	if (WasUpJustPressed()){
		g_theGame->UpdateMenuSelection(-1);
	}
	if (WasDownJustPressed()){
		g_theGame->UpdateMenuSelection(1);
	}
}
