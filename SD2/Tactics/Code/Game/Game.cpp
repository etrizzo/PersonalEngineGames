#include "Game.hpp"

#include "Game/BlockDefinition.hpp"
#include "Game/MapDefinition.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/OrbitCamera.hpp"
#include "Game/Map.hpp"
#include "Game/Actor.hpp"
#include "Game/Encounter.hpp"
#include "Game/EncounterState.hpp"
#include "Game/App.hpp"
#include "GameState.hpp"
#include "Game/FlyoutText.hpp"
#include "Engine/Core/XmlUtilities.hpp"
#include "Game/IsoSprite.hpp"
#include "Game/IsoAnimation.hpp"
#include "Game/AnimSet.hpp"
#include <map>


Game::~Game()
{

}

Game::Game()
{
	m_tileTexture = g_theRenderer->CreateOrGetTexture("Terrain_32x32.png");
	g_tileSpriteSheet = new SpriteSheet(*m_tileTexture, 32, 32);


	//LoadTileDefinitions();
	LoadMapDefinitions();
	LoadSprites();

	m_encounterState = new EncounterState();

	Clock* masterClock = GetMasterClock();

	m_gameClock = new Clock(masterClock);


}

void Game::InitCameras(Vector3 nearBottomLeft, Vector3 farTopRight)
{
	TODO("reorg camera initialization/encounter state initialization ")
	m_encounterState->Enter("heightMap.png");		//move this later but for cameras keep it here


	//make an orbit camera 
	m_camera = new OrbitCamera((float) m_encounterState->GetMapDimensions().x * 1.25f, -120.f, 45.f, m_encounterState->GetCursorPos());

	// Setup what it will draw to
	m_camera->SetColorTarget( g_theRenderer->m_defaultColorTarget );
	m_camera->SetDepthStencilTarget( g_theRenderer->m_defaultDepthTarget );

	m_camera->SetProjectionOrtho( nearBottomLeft, farTopRight );  


	// Setup ui camera - draws from (0,0) to (1,1) always
	m_uiCamera = new Camera();

	// Setup what it will draw to
	m_uiCamera->SetColorTarget( g_theRenderer->m_defaultColorTarget );
	m_uiCamera->SetDepthStencilTarget( g_theRenderer->m_defaultDepthTarget );

	m_uiCamera->SetProjectionOrtho(1.f, g_gameConfigBlackboard.GetValue("windowAspect", 1.f), 0.f,100.f);
	Vector2 center = m_uiCamera->GetBounds().GetCenter();
	m_uiCamera->LookAt( Vector3( center.x, center.y, -1.f ), Vector3(center.x, center.y, .5f)); 
}

void Game::Update(float deltaSeconds)
{
	m_deltaSeconds = deltaSeconds;		//remove once we have clock stuff
	if (!m_isPaused){
		m_gameTime+=deltaSeconds;
		m_currentState->Update(deltaSeconds);
	}
}

void Game::Render()
{
	SetMainCamera();
	m_currentState->RenderGame();
	SetUICamera();
	m_currentState->RenderUI();
	m_currentState->RenderTransition();
	SetMainCamera();
}

void Game::HandleInput()
{
	m_currentState->HandleInput();
}

void Game::RotateCamera(Vector2 thumbstickPos, float zoomIn, float zoomOut)
{
	float zoom = (-zoomIn + zoomOut) * 20.f * m_deltaSeconds;
	float degrees = 50.f * m_deltaSeconds; 
	float azimuth = 50.f * m_deltaSeconds;
	
	degrees *= -thumbstickPos.x;
	azimuth *= thumbstickPos.y;
	float newSize = m_camera->GetOrthographicSize() + zoom;
	newSize = ClampFloat(newSize, 5.f, 40.f);
	m_camera->SetSphericalCoordinate(m_camera->m_radius, m_camera->m_rotation + degrees, m_camera->m_azimuth + azimuth);
	//m_camera->SetProjectionOrtho(newSize, 1.f, 0.f, 100.f);
	m_camera->SetProjectionOrtho(newSize, g_gameConfigBlackboard.GetValue("windowAspect", 1.f), 0.f, 100.f);
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
	if ( !m_devMode){
		m_devMode = true;
	} else {
		m_devMode = false;
	}
}

void Game::SetCameraTarget(Vector3 newTarget)
{
	m_camera->SetTarget(newTarget);
}

void Game::SetDollyCamTarget(Vector3 newTarget)
{
	m_camera->SetTargetAndPosition(newTarget);
}


void Game::TransitionToState(GameState* newState)
{
	/*if (newState == GAME_STATE_ATTRACT){
		m_encounterState->Enter("heightMap.png");
	}*/
	m_transitionToState = newState;
	m_currentState->StartTransition();
	//m_startTransitionTime = m_gameTime;
}

void Game::TriggerTransition()
{
	m_currentState = m_transitionToState;
	m_transitionToState = nullptr;
	m_timeEnteredState = m_gameTime;
	//m_currentState->ResetTime();
}

AABB2 Game::SetUICamera()
{
	g_theRenderer->SetCamera( m_uiCamera ); 
	
	//g_theRenderer->ClearDepth( 1.0f ); 
	//g_theRenderer->EnableDepth( COMPARE_ALWAYS, false );
	g_theRenderer->DisableDepth();
	return m_uiCamera->GetBounds();
}

AABB2 Game::SetMainCamera()
{
	g_theRenderer->SetCamera( m_camera ); 
 
	g_theRenderer->ClearDepth( 1.0f ); 
	g_theRenderer->EnableDepth( COMPARE_LESS, true );
	return m_camera->GetBounds();		//not really gonna be necessary, probably
}

AABB2 Game::GetUIBounds()
{
	return m_uiCamera->GetBounds();
}

FlyoutText * Game::AddFlyoutText(const std::string & text, const Vector3 & pos, float textSize, float durationSeconds, const RGBA & tint, float speed)
{
	if (m_encounterState != nullptr){
		return m_encounterState->m_encounter->AddFlyoutText(text, pos, textSize, durationSeconds, tint, speed);
	}
	return nullptr;
}

void Game::SetCursorPos(IntVector2 newPos)
{
	m_encounterState->SetCursorPos(newPos);
}

IntVector2 Game::GetCursorPosXY()
{
	return m_encounterState->GetCursorPosXY();
}

bool Game::SpawnActor(IntVector2 position, std::string type, int factionNumber)
{
	//error check faction number
	if (factionNumber < 0 || factionNumber >= NUM_FACTIONS){
		return false;
	}
	Actor* a = m_encounterState->m_encounter->AddActor(position, (Faction) factionNumber, type);
	return (a != nullptr);
}



void Game::LoadTileDefinitions()
{
	tinyxml2::XMLDocument tileDefDoc;
	tileDefDoc.LoadFile("Data/Data/Tiles.xml");


	tinyxml2::XMLElement* root = tileDefDoc.FirstChildElement("TileDefinitions");
	for (tinyxml2::XMLElement* tileDefElement = root->FirstChildElement("TileDefinition"); tileDefElement != NULL; tileDefElement = tileDefElement->NextSiblingElement("TileDefinition")){
		BlockDefinition newDefinition = BlockDefinition(tileDefElement);
		BlockDefinition::s_definitions.insert(std::pair<std::string, BlockDefinition>(newDefinition.m_name, newDefinition));
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

void Game::LoadSprites()
{
	LoadSpritesFromFile		("to_archer.sprite"); 
	LoadIsoSpritesFromFile	("archer_f.isosprite");
	LoadIsoAnimsFromFile	("archer_f.anim");
	LoadAnimSetsFromFile	("archer_f.animset");

	LoadSpritesFromFile		("to_knight.sprite"); 
	LoadIsoSpritesFromFile	("knight_f.isosprite");
	LoadIsoAnimsFromFile	("knight_f.anim");
	LoadAnimSetsFromFile	("knight_f.animset");

	LoadSpritesFromFile		("to_mage.sprite"); 
	LoadIsoSpritesFromFile	("mage_f.isosprite");
	LoadIsoAnimsFromFile	("mage_f.anim");
	LoadAnimSetsFromFile	("mage_f.animset");
}

void Game::LoadSpritesFromFile(std::string xmlFile)
{
	tinyxml2::XMLDocument spriteDefDoc;
	std::string filePath = "Data/Data/SpriteDefs/" + xmlFile;
	spriteDefDoc.LoadFile(filePath.c_str());

	for (tinyxml2::XMLElement* spriteDefElement = spriteDefDoc.FirstChildElement("sprite"); spriteDefElement != NULL; spriteDefElement = spriteDefElement->NextSiblingElement("sprite")){
		tinyxml2::XMLElement* uvElement		= spriteDefElement->FirstChildElement("uv");
		tinyxml2::XMLElement* pivotElement	= spriteDefElement->FirstChildElement("pivot");
		tinyxml2::XMLElement* ppuElement	= spriteDefElement->FirstChildElement("ppu");
		tinyxml2::XMLElement* texElement	= spriteDefElement->FirstChildElement("diffuse");

		std::string texFile = ParseXmlAttribute(*texElement, "src", "NO_FILE_FOUND");
		std::string uvLayout = ParseXmlAttribute(*uvElement, "layout", "pixel");

		Texture* tex			= g_theRenderer->CreateOrGetTexture(texFile);
		AABB2 uvs = AABB2::ZERO_TO_ONE;
		if (uvLayout == "pixel"){
			AABB2 pixelUvs = ParseXmlAttribute( *uvElement, "bounds", uvs);
			float texWidth =  (float) tex->GetWidth();
			float texHeight = (float) tex->GetHeight();
			uvs = AABB2(pixelUvs.mins.x / texWidth, (texHeight - pixelUvs.maxs.y - 1) / texHeight, pixelUvs.maxs.x / texWidth, (texHeight - pixelUvs.mins.y - 1) / texHeight);
		
			float ppu				= ParseXmlAttribute(*ppuElement, "pixels", 1.f);
			Vector2 pivot			= ParseXmlAttribute( *pivotElement, "position", Vector2::HALF);
			Vector2 dimensions		= Vector2(pixelUvs.GetWidth() / ppu, pixelUvs.GetHeight() / ppu);

			std::string spriteID = ParseXmlAttribute(*spriteDefElement, "id", "NO_ID");
			g_theRenderer->CreateOrGetSprite(spriteID, tex, uvs, pivot, dimensions);
		}

	}
}

void Game::LoadIsoSpritesFromFile(std::string xmlFile)
{
	tinyxml2::XMLDocument spriteDefDoc;
	std::string filePath = "Data/Data/SpriteDefs/" + xmlFile;
	spriteDefDoc.LoadFile(filePath.c_str());

	for (tinyxml2::XMLElement* spriteDefElement = spriteDefDoc.FirstChildElement("isosprite"); spriteDefElement != NULL; spriteDefElement = spriteDefElement->NextSiblingElement("isosprite")){
		IsoSprite* isosprite = new IsoSprite(spriteDefElement);
		IsoSprite::s_isoSprites.insert(std::pair<std::string, IsoSprite*>(isosprite->m_id, isosprite));
	}
}

void Game::LoadIsoAnimsFromFile(std::string xmlFile)
{
	tinyxml2::XMLDocument animDefDoc;
	std::string filePath = "Data/Data/SpriteDefs/" + xmlFile;
	animDefDoc.LoadFile(filePath.c_str());

	for (tinyxml2::XMLElement* spriteDefElement = animDefDoc.FirstChildElement("isoanim"); spriteDefElement != NULL; spriteDefElement = spriteDefElement->NextSiblingElement("isoanim")){
		new IsoAnimation(spriteDefElement);
		//IsoSprite::s_isoSprites.insert(std::pair<std::string, IsoSprite*>(isosprite->m_id, isosprite));
	}
}

void Game::LoadAnimSetsFromFile(std::string xmlFile)
{
	tinyxml2::XMLDocument animDefDoc;
	std::string filePath = "Data/Data/SpriteDefs/" + xmlFile;
	animDefDoc.LoadFile(filePath.c_str());

	for (tinyxml2::XMLElement* spriteDefElement = animDefDoc.FirstChildElement("animset"); spriteDefElement != NULL; spriteDefElement = spriteDefElement->NextSiblingElement("animset")){
		new AnimSet(spriteDefElement);
		//IsoSprite::s_isoSprites.insert(std::pair<std::string, IsoSprite*>(isosprite->m_id, isosprite));
	}
}



Game* g_theGame = nullptr;