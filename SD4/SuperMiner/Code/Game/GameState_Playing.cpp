#include "Game/GameState_Playing.hpp"
#include "Game/Game.hpp"
#include "Game/World.hpp"
#include "Game/BlockDefinition.hpp"
#include "Game/Player.hpp"
#include "Game/DebugRenderSystem.hpp"
#include "Engine/Renderer/ForwardRenderPath.hpp"
#include "Engine/Renderer/PerspectiveCamera.hpp"


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

	g_theGame->m_debugRenderSystem->UpdateAndRender();
	g_theRenderer->EnableDepth( COMPARE_LESS, true ); 
	
}

void GameState_Playing::RenderUI()
{
	AABB2 bounds = g_theGame->SetUICamera();

	//draw time of day
	std::string timeOfDayStr = Stringf("Time of Day: %3.2f", m_world->m_timeOfDay);
	g_theRenderer->DrawTextInBox2D(timeOfDayStr, bounds, Vector2(0.01f, .99f), .01f);


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
	SpawnPlayer(Vector3(-4.f,0.f, (float) CHUNK_SIZE_Z * .5f));

	//m_world->ActivateChunk(IntVector2(0,3));
	//m_world->ActivateChunk(IntVector2(0,2));
	//m_world->ActivateChunk(IntVector2(1,2));
	//m_world->ActivateChunk(IntVector2(0,0));
	//m_world->ActivateChunk(IntVector2(-1,2));
	//m_world->ActivateChunk(IntVector2(1,1));
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
	g_theGame->m_mainCamera->m_transform.SetParent(m_player->m_cameraTarget);
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

