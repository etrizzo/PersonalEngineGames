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

#include "Game/Map.hpp"
#include "Game/Player.hpp"
#include <map>

//SpriteAnimSetDef* Game::s_humanoidAnimSetDef;

Game::~Game()
{
	//delete m_currentAdventure;
	delete m_player;
	delete g_tileSpriteSheet;
	delete m_currentState;

	//delete all the definitions
}

Game::Game()
{

	m_renderPath = new SpriteRenderPath();
	m_renderPath->m_renderer = g_theRenderer;

	Texture* tileTexture = g_theRenderer->CreateOrGetTexture("Terrain_32x32.png");
	g_tileSpriteSheet = new SpriteSheet(*tileTexture, 32,32);
	Texture* miscTexture = g_theRenderer->CreateOrGetTexture("MiscItems_4x4.png");
	m_miscSpriteSheet = new SpriteSheet(*miscTexture, 4,4);

	m_isPaused = false;
	m_devMode = false;
	m_fullMapMode = false;
	//m_gameTime = 0.f;
	m_gameClock = new Clock(GetMasterClock());
	

	LoadSounds();
	LoadTileDefinitions();
	LoadMapDefinitions();
	LoadClothingDefinitions();
	LoadEntityDefinitions();
	LoadAdventureDefinitions();

	m_screenWidth = 10;
	m_camera = new Camera();
	m_camera->SetColorTarget( g_theRenderer->m_defaultColorTarget );
	m_camera->SetDepthStencilTarget( g_theRenderer->m_defaultDepthTarget );
	//m_camera->SetProjectionOrtho(Vector2(0.f,0.f), Vector2((float) m_screenWidth, (float) m_screenWidth));
	m_camera->SetProjectionOrtho(m_screenWidth, g_gameConfigBlackboard.GetValue("windowAspect", 1.f), 0.f,100.f);
	Vector2 center = m_camera->GetBounds().GetCenter();
	m_camera->LookAt(Vector3(center.x, center.y, -1.f), Vector3(center.x, center.y, .5f));


	// Setup ui camera - draws from (0,0) to (1,1) always
	m_uiCamera = new Camera();

	// Setup what it will draw to
	m_uiCamera->SetColorTarget( g_theRenderer->m_defaultColorTarget );
	m_uiCamera->SetDepthStencilTarget( g_theRenderer->m_defaultDepthTarget );

	m_uiCamera->SetProjectionOrtho(1.f, g_gameConfigBlackboard.GetValue("windowAspect", 1.f), 0.f,100.f);
	Vector2 uicenter = m_uiCamera->GetBounds().GetCenter();
	m_uiCamera->LookAt( Vector3( uicenter.x, uicenter.y, -1.f ), Vector3(uicenter.x, uicenter.y, .5f)); 




	g_theRenderer->SetCamera( m_camera ); 

	g_theRenderer->ClearDepth( 1.0f ); 
	g_theRenderer->EnableDepth( COMPARE_LESS, true );
	m_debugRenderSystem = new DebugRenderSystem();
	m_debugRenderSystem->Startup(m_camera);
	m_debugRenderSystem->ToggleInfo();
	m_debugRenderSystem->ToggleScreenGrid();
}

Vector2 Game::GetPlayerPosition() const
{
	if (m_player != nullptr){
		return m_player->GetPosition();
	}
	return Vector2::ZERO;
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
	
	m_currentState->HandleInput();
}

void Game::Render()
{
	g_theRenderer->SetCullMode(CULLMODE_NONE);
	g_theRenderer->DisableDepth();
	g_theRenderer->ClearScreen(RGBA::BLACK);

	m_currentState->Render();
	g_theGame->SetGameCamera();
	m_debugRenderSystem->UpdateAndRender();

}


//void Game::RenderDefeat()
//{
//	m_currentAdventure->Render();
//	g_theRenderer->DrawAABB2( m_camera->GetBounds(), m_startFadeColor);
//
//	RGBA textColor = RGBA::WHITE;
//	if (!m_isFinishedTransitioning){
//		float percThroughTransition = (m_gameTime - m_timeEnteredState)/m_transitionLength;
//		if (m_transitionToState == NO_STATE){
//			textColor = Interpolate( textColor.GetColorWithAlpha(0), textColor, percThroughTransition);
//		} else {
//			textColor = Interpolate( textColor, textColor.GetColorWithAlpha(0), percThroughTransition);
//		}
//	}
//
//	AABB2 camBounds = m_currentAdventure->m_currentMap->GetCameraBounds();
//	g_theRenderer->DrawTextInBox2D("Defeat :(", camBounds, Vector2(.5f,.5f), 1.f);
//	g_theRenderer->DrawTextInBox2D("Press 'start' or 'n' to respawn", camBounds, Vector2(.5f,.3f), .5f, TEXT_DRAW_SHRINK_TO_FIT);
//	g_theRenderer->DrawAABB2Outline(camBounds, RGBA::WHITE);
//}


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
	g_theAudio->StopSound(m_currentState->m_soundtrackPlayback);
	
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
	g_theGame->m_player->m_stats.SetStat(STAT_MOVEMENT, newSpeed);
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
	return g_theInput->WasKeyJustPressed(VK_UP) || g_primaryController->WasButtonJustPressed(XBOX_D_UP);
}

bool WasDownJustPressed()
{
	return g_theInput->WasKeyJustPressed(VK_DOWN) || g_primaryController->WasButtonJustPressed(XBOX_D_DOWN);
}

bool WasRightJustPressed()
{
	return g_theInput->WasKeyJustPressed(VK_RIGHT) || g_primaryController->WasButtonJustPressed(XBOX_D_RIGHT);
}

bool WasLeftJustPressed()
{
	return g_theInput->WasKeyJustPressed(VK_LEFT) || g_primaryController->WasButtonJustPressed(XBOX_D_LEFT);
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
