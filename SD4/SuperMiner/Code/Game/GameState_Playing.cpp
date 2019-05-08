#include "Game/GameState_Playing.hpp"
#include "Game/Game.hpp"
#include "Game/World.hpp"
#include "Game/BlockDefinition.hpp"
#include "Game/Player.hpp"
#include "Game/DebugRenderSystem.hpp"
#include "Engine/Renderer/ForwardRenderPath.hpp"
#include "Engine/Renderer/PerspectiveCamera.hpp"
#include "Game/GameCamera.hpp"


GameState_Playing::GameState_Playing()
{
	g_theGame->m_mainCamera->AddSkybox("skybox.png", RIGHT, UP, FORWARD);
	g_theGame->m_mainCamera->m_skybox->m_transform.RotateByEuler(0.f,0.f,90.f);
}

GameState_Playing::~GameState_Playing()
{
	delete m_world;
	delete m_player;
}

void GameState_Playing::EnterState()
{
	m_gameWon = false;
	if (!m_playing){
		//initialize map
		m_playing = true;
		m_gameLost = false;

		//start up playing state
		Startup();
		//g_theGame->m_debugRenderSystem->DetachCamera();
	}
}

void GameState_Playing::Update(float ds)
{
	PROFILE_PUSH_FUNCTION_SCOPE();
	
	
	m_timeInState+=ds;

	for (Entity* entity : m_allEntities){
		entity->Update();
	}
	m_player->Update();
	m_world->Update();
	

	DeleteEntities();
	CheckForVictory();
	CheckForDefeat();

	
}

void GameState_Playing::RenderGame()
{
	PROFILE_PUSH_FUNCTION_SCOPE();
	g_theRenderer->ClearScreen( m_world->m_skyColor ); 
	g_theRenderer->ClearDepth( 1.0f ); 
	g_theRenderer->EnableDepth( COMPARE_LESS, true ); 

	g_theRenderer->BindModel(Matrix44::IDENTITY);
	
	m_world->Render();

	g_theRenderer->ReleaseTexture(0);
	g_theRenderer->ReleaseTexture(1);
	g_theRenderer->ReleaseShader();

	m_player->Render();

	PROFILE_PUSH("DebugRenderSystem");
	g_theGame->m_debugRenderSystem->UpdateAndRender();
	PROFILE_POP();
	g_theRenderer->EnableDepth( COMPARE_LESS, true ); 
	
}

void GameState_Playing::RenderUI()
{
	PROFILE_PUSH_FUNCTION_SCOPE();
	AABB2 bounds = g_theGame->SetUICamera();

	//draw time of day
	std::string timeOfDayStr = Stringf("Day: %3.2f   Percentage through day: %3.2f   Time of Day: %3.2f\n Lighting: %3.2f   Glow: %3.2f", m_world->m_worldTime, m_world->m_percThroughDay, m_world->m_timeOfDay, m_world->m_lightningPerlinValue, m_world->m_glowPerlinValue);
	g_theRenderer->DrawTextInBox2D(timeOfDayStr, bounds, Vector2(0.01f, 0.01f), .015f);


	//draw player debugt text
	IntVector2 chunkCoords = m_world->GetChunkCoordinatesFromWorldCoordinates(m_player->GetPosition());
	std::string playerDebug = Stringf("Physics mode: %s(F4)   Camera mode: %s(F5)   \nVelocity: (%3.2f, %3.2f, %3.2f) (Magnitude: %3.2f)   OnGround?: %s   ChunkCoords: (%i, %i)  ",
		m_player->GetPhysicsModeString().c_str(),
		m_gameCamera->GetModeString().c_str(),
		m_player->m_velocity.x, 
		m_player->m_velocity.y, 
		m_player->m_velocity.z,
		m_player->m_velocity.GetLength(),
		ToString(m_player->m_isOnGround).c_str(),
		chunkCoords.x, 
		chunkCoords.y);
	g_theRenderer->DrawTextInBox2D(playerDebug, bounds, Vector2(0.01f, .99f), .015f);

	//draw block UI
	BlockDefinition* playerPlaceBlock = BlockDefinition::GetBlockDefinitionFromID(m_player->m_currentPlaceBlockType);
	AABB2 blockUI = bounds.GetPercentageBox(.95f, .01f, .99f, .05f);
	blockUI.TrimToSquare();

	float padding = bounds.GetHeight() * .007f;
	blockUI.AddPaddingToSides(padding, padding);
	g_theRenderer->DrawAABB2(blockUI, RGBA::GRAY.GetColorWithAlpha(200));
	g_theRenderer->DrawAABB2Outline(blockUI, RGBA::WHITE);
	blockUI.AddPaddingToSides(-padding, -padding);

	
	g_theRenderer->DrawTexturedAABB2(blockUI, *g_blockSpriteSheet->GetTexture(), playerPlaceBlock->m_sideUVs.mins, playerPlaceBlock->m_sideUVs.maxs, RGBA::WHITE);
	g_theRenderer->DrawAABB2Outline(blockUI, RGBA::BLACK);
}

void GameState_Playing::HandleInput()
{
	
	g_theGame->m_debugRenderSystem->HandleInput();
	m_world->HandleInput();

	if (g_theInput->WasKeyJustPressed(VK_F1)){
		g_theGame->ToggleDevMode();
	}
	if (g_theInput->WasKeyJustPressed('P') || g_theInput->GetController(0)->WasButtonJustPressed(XBOX_START) ){
		g_theGame->TransitionToState(new GameState_Paused(this));
	}

	if (g_theInput->WasKeyJustPressed('T')){
		g_theGame->m_gameClock->SetScale(.1f);
	}
	if (g_theInput->WasKeyJustReleased('T')){
		g_theGame->m_gameClock->SetScale(1.f);
	}


	if (g_theInput->WasKeyJustPressed(VK_F4))
	{
		m_player->m_physicsMode = eEntityPhysicsMode(((int)m_player->m_physicsMode + 1) % (int)NUM_PHYSICS_MODES);
	}

	if (g_theInput->WasKeyJustPressed(VK_F5)){
		m_gameCamera->SetMode(eCameraMode(((int)m_gameCamera->GetMode() + 1) % (int)NUM_CAMERA_MODES));
	}


	if (!g_theGame->m_debugRenderSystem->m_isDetached){
		if (!m_gameLost){
			m_player->HandleInput();
		}
	}
}

void GameState_Playing::RespawnPlayer()
{
	m_player->Respawn();
	m_gameLost = false;
}



void GameState_Playing::Startup()
{
	m_world = new World();
	SpawnPlayer(Vector3(-4.f,0.f, 120.0f));

	NamedProperties p;
	p.Set("Event", "Sunrise");
	////playground:
	FireEvent("Sunrise", p);		//no one's listening, does nothing
	SubscribeEventCallbackFunction("Sunrise", MyTestEventFunction);
	FireEvent("Sunrise", p);		//now MyTestEventFunction fires
	UnsubscribeEventCallbackFunction("Sunrise", MyTestEventFunction);
	FireEvent("Sunrise", p);		//back to no friends, does nothing

	p.Set("Event", "Test");
	SubscribeEventCallbackFunction("Test", MyTestEventFunction);
	FireEvent("Test", p);
	p.Set("Health", 20.f);
	FireEvent("Test", p);


	//std::string lastName("Eiserloh");
	//NamedProperties employmentInfoProperties;

	//NamedProperties p;
	//p.Set("FirstName", "Squirrel"); 	// Setting as c-string (const char*) data...
	//p.Set("LastName", lastName);	// Setting as std::string data...
	//p.Set("Height", 1.93f);
	//p.Set("Age", 45);
	//p.Set("IsMarried", true);
	//p.Set("Position", Vector2(3.5f, 6.2f));
	//p.Set("EyeColor", RGBA(77, 38, 23));
	//p.Set("EmploymentInfo", employmentInfoProperties); // NamedProperties inside NamedProperties

	//float health = p.Get("Height", 1.75f);

	//// Note the subtleties in the Set, Get, and return types for each of the following examples:


	//std::string first_name = p.Get("FirstName", "UNKNOWN");
	//std::string last_name = p.Get("LastName", "UNKNOWN");

	//first_name = p.Get("FirstName", first_name);
	//last_name = p.Get("LastName", last_name);

	//std::string color = p.Get("EyeColor", "green");
	//p.Set("EyeColor", "Purple");
	//color = p.Get("EyeColor", "green");
	
}

void GameState_Playing::CheckForVictory()
{
	if (!m_gameWon){
		//check victory conditions
	}
}

void GameState_Playing::CheckForDefeat()
{
	if (!m_gameLost){
		/*if (m_player->m_aboutToBeDeleted){
			m_gameLost = true;
			m_player->RemoveRenderable();
			g_theGame->TransitionToState(new GameState_Defeat(this));
		}*/
	}
}

void GameState_Playing::SpawnPlayer(Vector3 pos)
{
	if (m_player != nullptr){
		delete m_player;
	}
	m_player = new Player(this, pos);

	//g_theGame->m_mainCamera->Translate(pos);
	//g_theGame->m_mainCamera->m_transform.SetParent(&(m_player->m_transform));

	m_gameCamera = new GameCamera(g_theGame->m_mainCamera, m_player);
}


void GameState_Playing::DeleteEntities()
{
	for (int i = (int) m_allEntities.size() - 1; i >= 0; i--){
		Entity* entity = m_allEntities[i];
		if (entity->m_aboutToBeDeleted){
			RemoveAtFast(m_allEntities, i);
			//delete entity;
		}
	}

	
}

