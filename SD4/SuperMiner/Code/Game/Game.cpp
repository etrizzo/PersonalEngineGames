#include "Game.hpp"
#include "Game/TileDefinition.hpp"
#include "Engine/Renderer/PerspectiveCamera.hpp"
#include "Game/DebugRenderSystem.hpp"
#include "Game/Player.hpp"
#include "Game/BlockDefinition.hpp"
#include "Engine/Renderer/Light.hpp"
#include "Engine/Renderer/ParticleSystem.hpp"
#include "Engine/Networking/UDPTests.hpp"
#include "Engine/Networking/NetSession.hpp"
#include "Engine/Networking/NetConnection.hpp"



Game::~Game()
{
	if (m_debugRenderSystem->IsActive()){
		m_debugRenderSystem->Shutdown();
	}
}

Game::Game()
{
	
	m_tileTexture = g_theRenderer->CreateOrGetTexture("Terrain_32x32.png");
	g_blockSpriteSheet = new SpriteSheet(*m_tileTexture, 32, 32);
	m_isPaused = false;
	m_devMode = false;
	//m_gameTime = 0.f;



	
	m_mainCamera = new PerspectiveCamera();

	// Setup what it will draw to
	m_mainCamera->SetColorTarget( g_theRenderer->m_defaultColorTarget );
	m_mainCamera->SetDepthStencilTarget( g_theRenderer->m_defaultDepthTarget );

	//Matrix44 m_worldBasis = Matrix44(Vector3(0.f,-1.f, 0.f), 		//right
	//								Vector3(0.f, 0.f, 1.f), 		//up
	//								Vector3(1.f, 0.f, 0.f));		//forward

	

	m_mainCamera->SetPerspectiveOrtho(70.f, g_gameConfigBlackboard.GetValue("windowAspect", 1.f), 0.1f, 450.f);
	Matrix44 projection = m_mainCamera->m_projMatrix.GetTop();
	//Matrix44 newProj = Matrix44(g_worldToEngine);
	//newProj.Append(projection);
	//m_mainCamera->m_projMatrix.Load(newProj);
	m_mainCamera->m_projMatrix.m_top.Append(g_worldToEngine);
	//m_mainCamera->Translate(Vector3(-4.f,0.f, (float) CHUNK_SIZE_Z * .5f));

	m_uiCamera = new Camera();

	// Setup what it will draw to
	m_uiCamera->SetColorTarget( g_theRenderer->m_defaultColorTarget );
	m_uiCamera->SetDepthStencilTarget( g_theRenderer->m_defaultDepthTarget );


	m_uiCamera->SetProjectionOrtho(1.f, g_gameConfigBlackboard.GetValue("windowAspect", 1.f), 0.f, 100.f);
	Vector2 center = m_uiCamera->GetBounds().GetCenter(); 

	SetMainCamera();

	Clock* masterClock = GetMasterClock();

	m_gameClock = new Clock(masterClock);

	m_currentState = new GameState_Loading();
	//m_udp = new UDPTest();

	m_session = new NetSession();

}

void Game::PostStartup()
{
	PROFILE_LOG_SCOPE_FUNCTION();
	m_soundTrackID = g_theAudio->CreateOrGetSound("Data/Audio/OrbitalColossus.mp3");

	new BlockDefinition(BLOCK_AIR,			false,  AABB2::ZERO_TO_ONE, AABB2::ZERO_TO_ONE, AABB2::ZERO_TO_ONE);
	new BlockDefinition(BLOCK_GRASS,		true, g_blockSpriteSheet->GetTexCoordsForSpriteCoords(IntVector2(1,0)),  g_blockSpriteSheet->GetTexCoordsForSpriteCoords(IntVector2(3,3)),  g_blockSpriteSheet->GetTexCoordsForSpriteCoords(IntVector2(4,3)));
	new BlockDefinition(BLOCK_STONE,		true, g_blockSpriteSheet->GetTexCoordsForSpriteCoords(IntVector2(7,4)),  g_blockSpriteSheet->GetTexCoordsForSpriteCoords(IntVector2(7,4)),  g_blockSpriteSheet->GetTexCoordsForSpriteCoords(IntVector2(7,4)));
	new BlockDefinition(BLOCK_SNOW,			true, g_blockSpriteSheet->GetTexCoordsForSpriteCoords(IntVector2(1,3)),  g_blockSpriteSheet->GetTexCoordsForSpriteCoords(IntVector2(1,3)),  g_blockSpriteSheet->GetTexCoordsForSpriteCoords(IntVector2(1,3)));
	new BlockDefinition(BLOCK_SAND,			true, g_blockSpriteSheet->GetTexCoordsForSpriteCoords(IntVector2(6,1)),  g_blockSpriteSheet->GetTexCoordsForSpriteCoords(IntVector2(6,1)),  g_blockSpriteSheet->GetTexCoordsForSpriteCoords(IntVector2(6,1)));
	new BlockDefinition(BLOCK_COBBLESTONE,	true, g_blockSpriteSheet->GetTexCoordsForSpriteCoords(IntVector2(1,4)),  g_blockSpriteSheet->GetTexCoordsForSpriteCoords(IntVector2(1,4)),  g_blockSpriteSheet->GetTexCoordsForSpriteCoords(IntVector2(1,4)));
	new BlockDefinition(BLOCK_OAK_PLANKS,	true, g_blockSpriteSheet->GetTexCoordsForSpriteCoords(IntVector2(30,1)),  g_blockSpriteSheet->GetTexCoordsForSpriteCoords(IntVector2(30,1)),  g_blockSpriteSheet->GetTexCoordsForSpriteCoords(IntVector2(30,1)));
	new BlockDefinition(BLOCK_GLASS,		false, g_blockSpriteSheet->GetTexCoordsForSpriteCoords(IntVector2(30,3)),  g_blockSpriteSheet->GetTexCoordsForSpriteCoords(IntVector2(30,3)),  g_blockSpriteSheet->GetTexCoordsForSpriteCoords(IntVector2(30,3)));
	new BlockDefinition(BLOCK_GOLD,			true, g_blockSpriteSheet->GetTexCoordsForSpriteCoords(IntVector2(23,5)),  g_blockSpriteSheet->GetTexCoordsForSpriteCoords(IntVector2(23,5)),  g_blockSpriteSheet->GetTexCoordsForSpriteCoords(IntVector2(23,5)));


	m_currentMap = nullptr;
	m_playState = new GameState_Playing();

	m_debugRenderSystem = new DebugRenderSystem();
	DebugStartup();
	m_debugRenderSystem->ToggleInfo();
	m_debugRenderSystem->ToggleScreenGrid();
	

	m_currentState = new GameState_Attract();
	//m_currentMap = new Map("Heightmap_2.png", AABB2(-150.f, -150.f, 150.f, 150.f), -5.f, 5.f, IntVector2(10,10), 20.f);
	//m_currentMap = new Map("Heightmap.png", AABB2(-100.f, -100.f, 100.f, 100.f), -5.f, 2.f, IntVector2(20,20), 40.f);

	//m_soundtrackPlayback = g_theAudio->PlaySound(m_soundTrackID, true, .5f);

	//m_objectSyncSimTimer = StopWatch();
	//m_objectSyncSimTimer.SetTimer(NET_OBJECT_SYNC_SIM_TIME);

	

}

void Game::Update()
{
	PROFILE_PUSH_FUNCTION();
	//PROFILE_LOG_SCOPE_FUNCTION();
	float ds = GetDeltaSeconds();
	m_currentState->Update(ds);

	//m_udp->Update();
	m_session->Update();


	PROFILE_POP();
	
}

void Game::Render()
{
	PROFILE_PUSH_FUNCTION_SCOPE();
	m_currentState->Render();
	//RenderGame();
	RenderUI();
}

void Game::HandleInput()
{
	PROFILE_PUSH_FUNCTION_SCOPE();
	//m_debugRenderSystem->HandleInput();
	m_currentState->HandleInput();
}



void Game::RenderLoadScreen()
{
	AABB2 uiBounds = SetUICamera();
	g_theRenderer->DrawAABB2(uiBounds, RGBA(100,100,100,255));
	g_theRenderer->DrawTextInBox2D("Loading...", uiBounds, Vector2::HALF, .2f, TEXT_DRAW_SHRINK_TO_FIT);
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
	m_timeEnteredState = m_gameClock->GetCurrentSeconds();
	m_currentState->EnterState();
	//m_currentState->ResetTime();
}

float Game::GetDeltaSeconds()
{
	float ds = m_gameClock->GetDeltaSeconds();

	if (g_theGame->m_isPaused){
		ds = 0.f;
	}
	return ds;
}

Vector2 Game::GetPlayerPositionXZ() const
{
	if (m_playState != nullptr){
		return m_playState->m_player->m_positionXZ;
	}
	return Vector2::ZERO;
}

Player * Game::GetPlayer() const
{
	if (m_playState != nullptr){
		return m_playState->m_player;
	}
	return nullptr;
}

World * Game::GetWorld() const
{
	if (m_playState == nullptr){
		return nullptr;
	}
	return m_playState->m_world;
}


AABB2 Game::SetUICamera()
{
	g_theRenderer->SetCamera( m_uiCamera ); 

	g_theRenderer->ClearDepth( 1.0f ); 
	//g_theRenderer->EnableDepth( COMPARE_ALWAYS, false );
	g_theRenderer->DisableDepth();
	return m_uiCamera->GetBounds();
}

AABB2 Game::SetMainCamera()
{
	m_currentCamera = m_mainCamera;
	SetGameCamera();
	return m_mainCamera->GetBounds();		//not really gonna be necessary, probably
}

AABB2 Game::GetUIBounds()
{
	return m_uiCamera->GetBounds();
}

Vector3 Game::GetCurrentCameraUp()
{
	return m_currentCamera->GetUp();
}

Vector3 Game::GetCurrentCameraRight()
{
	return m_currentCamera->GetRight();
}

void Game::SetGameCamera(PerspectiveCamera* newCam)
{
	if (newCam != nullptr){
		m_currentCamera = newCam;
	}
	g_theRenderer->SetCamera( m_currentCamera ); 

	g_theRenderer->ClearDepth( 1.0f ); 
	g_theRenderer->EnableDepth( COMPARE_LESS, true );
	
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

void Game::DebugStartup()
{
	m_debugRenderSystem->Startup(m_mainCamera);
}

void Game::DebugShutdown()
{
	m_debugRenderSystem->Shutdown();
	SetMainCamera();
}

void Game::DebugClear()
{
	m_debugRenderSystem->Clear();
}

void Game::DebugToggleRendering()
{
	m_debugRenderSystem->ToggleRendering();
}

RenderScene * Game::GetScene()
{
	return m_playState->GetScene();
}

void Game::AddNewLight(std::string type, RGBA color)
{
	if (m_playState != nullptr){
		m_playState->AddNewLight(type, color);
	}
}

void Game::AddNewLight(std::string type, Vector3 pos, RGBA color)
{
	if (m_playState != nullptr){
		m_playState->AddNewLight(type, pos, color);
	}
}

void Game::AddNewPointLight(Vector3 pos, RGBA color)
{
	if (m_playState != nullptr){
		m_playState->AddNewPointLight(pos, color);
	}
}

void Game::AddNewSpotLight(Vector3 pos, RGBA color, float innerAngle, float outerAngle)
{
	if (m_playState != nullptr){
		m_playState->AddNewSpotLight(pos, color, innerAngle, outerAngle);
	}
}

void Game::AddNewDirectionalLight(Vector3 pos, RGBA color, Vector3 rotation)
{
	if (m_playState != nullptr){
		m_playState->AddNewDirectionalLight(pos, color, rotation);
	}
}

void Game::RemoveLight(int idx)
{
	if (m_playState != nullptr){
		m_playState->RemoveLight(idx);
	}
}

void Game::SetLightPosition(Vector3 newPos, unsigned int idx)
{
	if (m_playState != nullptr){
		m_playState->SetLightPosition(newPos, idx);
	}
}

void Game::SetLightColor(RGBA newColor, unsigned int idx)
{
	if (m_playState != nullptr){
		m_playState->SetLightColor(newColor, idx);
	}
}

void Game::SetLightColor(Vector4 newColor, unsigned int idx)
{
	if (m_playState != nullptr){
		m_playState->SetLightColor(newColor, idx);
	}
}

void Game::SetLightAttenuation(int lightIndex, Vector3 att)
{
	if (m_playState != nullptr){
		m_playState->SetLightAttenuation(lightIndex, att);
	}
}

unsigned int Game::GetNumActiveLights() const
{
	if (m_playState != nullptr){
		return m_playState->GetNumActiveLights();
	} else {
		ConsolePrintf(RGBA::RED, "no play state!");
		return 0;
	}
}




void Game::LoadTileDefinitions()
{
	tinyxml2::XMLDocument tileDefDoc;
	tileDefDoc.LoadFile("Data/Data/Tiles.xml");


	tinyxml2::XMLElement* root = tileDefDoc.FirstChildElement("TileDefinitions");
	for (tinyxml2::XMLElement* tileDefElement = root->FirstChildElement("TileDefinition"); tileDefElement != NULL; tileDefElement = tileDefElement->NextSiblingElement("TileDefinition")){
		TileDefinition newDefinition = TileDefinition(tileDefElement);
		TileDefinition::s_definitions.insert(std::pair<std::string, TileDefinition>(newDefinition.m_name, newDefinition));
	}


}



void Game::RenderGame()
{
	m_currentState->Render();
	//m_debugRenderSystem->UpdateAndRender();
}

void Game::RenderUI()
{
	AABB2 screenBounds = SetUICamera();
	AABB2 netSessionBox  = screenBounds.GetPercentageBox(Vector2(.02f, .6f), Vector2(.98f, .9f));

	//g_theRenderer->DrawAABB2(netSessionBox, RGBA::BLACK.GetColorWithAlpha(100));
	//g_theRenderer->DrawAABB2Outline(netSessionBox, RGBA::GetRandomRainbowColor());
	//m_session->RenderInfo(netSessionBox, g_theRenderer);
}






//------------------------------------------------------------------------
bool OnAdd( NetMessage msg, net_sender_t const &from ) 
{
	float val0;
	float val1; 
	float sum;

	if (!msg.Read( &val0 ) || !msg.Read( &val1 )) {
		// this probaby isn't a real connection to send us a bad message
		return false; 
	}

	sum = val0 + val1; 
	std::string printStr = Stringf("Add Response: %f + %f = %f", val0, val1, sum );
	ConsolePrintf( printStr.c_str() ); 

	// would could send back a response here if we want;
	// ...

	// would could send back a response here if we want;
	// ...

	NetMessage* addResponse = new NetMessage( "add_response" ); 
	addResponse->WriteData( printStr ); 
	from.m_connection->Send( addResponse ); 

	return true; 
}

bool OnAddResponse(NetMessage msg, net_sender_t const & from)
{
	UNUSED(from);
	float val0;
	float val1; 
	float sum;

	if (!msg.Read( &val0 ) || !msg.Read( &val1 )) {
		// this probably isn't a real connection to send us a bad message
		return false; 
	}

	sum = val0 + val1; 
	ConsolePrintf( "Add: %f + %f = %f", val0, val1, sum ); 




	return true; 
}

bool OnNetSyncSim(NetMessage msg, net_sender_t const & from)
{
	UNUSED(msg);
	UNUSED(from);
	return true;
}






Game* g_theGame = nullptr;