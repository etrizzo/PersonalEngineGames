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
	m_renderPath = new ForwardRenderPath(g_theRenderer);
	//m_renderPath->m_renderer = g_theRenderer;
	m_scene = new RenderScene(g_theRenderer);


	m_scene->AddCamera(g_theGame->m_currentCamera);
	g_theGame->m_mainCamera->AddSkybox("skybox.png", RIGHT, UP, FORWARD);
	g_theGame->m_mainCamera->m_skybox->m_transform.RotateByEuler(0.f,0.f,90.f);
	m_renderPath->SetFogColor(RGBA(40, 10, 90));
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
		g_theGame->m_debugRenderSystem->DetachCamera();
	}
}

void GameState_Playing::Update(float ds)
{
	PROFILE_PUSH_FUNCTION_SCOPE();
	Vector3 basis = Vector3::ZERO;
	
	//draw the basis at the origin
	g_theGame->m_debugRenderSystem->MakeDebugRenderBasis(0.f, basis, 1.f, Matrix44::IDENTITY, DEBUG_RENDER_HIDDEN);
	g_theGame->m_debugRenderSystem->MakeDebugRenderBasis(0.f, basis, 1.f, Matrix44::IDENTITY, DEBUG_RENDER_USE_DEPTH);

	m_timeInState+=ds;

	for (Entity* entity : m_allEntities){
		entity->Update();
	}

	DeleteEntities();
	CheckForVictory();
	CheckForDefeat();

	
}

void GameState_Playing::RenderGame()
{
	PROFILE_PUSH_FUNCTION_SCOPE();
	g_theRenderer->ClearScreen( RGBA::BLACK ); 
	g_theRenderer->ClearDepth( 1.0f ); 
	g_theRenderer->EnableDepth( COMPARE_LESS, true ); 

	g_theRenderer->BindModel(Matrix44::IDENTITY);
	
	m_renderPath->Render(m_scene);		//this clears to skybox! Warning!

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



	if (g_theInput->WasKeyJustPressed('L')){
		AddNewPointLight( g_theGame->m_currentCamera->GetPosition() + g_theGame->m_currentCamera->GetForward(), RGBA::WHITE);
	}

	if (!g_theGame->m_debugRenderSystem->m_isDetached){
		if (!m_gameLost){
			//m_player->HandleInput();
		}
	}
}

void GameState_Playing::RespawnPlayer()
{
	m_player->Respawn();
	m_gameLost = false;
}




Light* GameState_Playing::AddNewLight(std::string type, RGBA color)
{
	Vector3 pos = g_theGame->m_currentCamera->GetPosition() + g_theGame->m_currentCamera->GetForward();
	return m_scene->AddNewLight(type, pos, color);
}

Light* GameState_Playing::AddNewLight(std::string type, Vector3 pos, RGBA color)
{
	return m_scene->AddNewLight(type, pos, color);
}

Light* GameState_Playing::AddNewPointLight(Vector3 pos, RGBA color)
{
	return m_scene->AddNewPointLight(pos, color);
}

Light* GameState_Playing::AddNewSpotLight(Vector3 pos, RGBA color, float innerAngle, float outerAngle)
{
	return m_scene->AddNewSpotLight(pos, color, innerAngle, outerAngle);
}

Light* GameState_Playing::AddNewDirectionalLight(Vector3 pos, RGBA color, Vector3 rotation)
{
	return m_scene->AddNewDirectionalLight(pos, color, rotation);

}

void GameState_Playing::RemoveLight(int idx)
{
	m_scene->RemoveLight(idx);
}

void GameState_Playing::RemoveLight(Light * light)
{
	m_scene->RemoveLight(light);
}

void GameState_Playing::SetLightPosition(Vector3 newPos, unsigned int idx)
{
	m_scene->SetLightPosition(newPos, idx);
}

void GameState_Playing::SetLightColor(RGBA newColor, unsigned int idx)
{
	m_scene->SetLightColor(newColor, idx);
}

void GameState_Playing::SetLightColor(Vector4 newColor, unsigned int idx)
{
	m_scene->SetLightColor(newColor, idx);
}

void GameState_Playing::SetLightAttenuation(int lightIndex, Vector3 att)
{
	m_scene->SetLightAttenuation(lightIndex, att);

}

unsigned int GameState_Playing::GetNumActiveLights() const
{
	return (unsigned int) m_scene->m_lights.size();
}



void GameState_Playing::Startup()
{
	new BlockDefinition(BLOCK_AIR, AABB2::ZERO_TO_ONE, AABB2::ZERO_TO_ONE, AABB2::ZERO_TO_ONE);
	new BlockDefinition(BLOCK_GRASS, g_blockSpriteSheet->GetTexCoordsForSpriteCoords(IntVector2(1,0)),  g_blockSpriteSheet->GetTexCoordsForSpriteCoords(IntVector2(3,3)),  g_blockSpriteSheet->GetTexCoordsForSpriteCoords(IntVector2(4,3)));
	new BlockDefinition(BLOCK_STONE, g_blockSpriteSheet->GetTexCoordsForSpriteCoords(IntVector2(7,4)),  g_blockSpriteSheet->GetTexCoordsForSpriteCoords(IntVector2(7,4)),  g_blockSpriteSheet->GetTexCoordsForSpriteCoords(IntVector2(7,4)));
	new BlockDefinition(BLOCK_SNOW, g_blockSpriteSheet->GetTexCoordsForSpriteCoords(IntVector2(1,3)),  g_blockSpriteSheet->GetTexCoordsForSpriteCoords(IntVector2(1,3)),  g_blockSpriteSheet->GetTexCoordsForSpriteCoords(IntVector2(1,3)));


	m_world = new World();

	m_world->ActivateChunk(IntVector2(0,3));
	m_world->ActivateChunk(IntVector2(0,2));
	m_world->ActivateChunk(IntVector2(1,2));
	m_world->ActivateChunk(IntVector2(0,0));
	m_world->ActivateChunk(IntVector2(-1,2));
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

	m_scene->AddRenderable(m_player->m_renderable);

	g_theGame->m_mainCamera->m_transform.SetParent(m_player->m_cameraTarget);
}


void GameState_Playing::DeleteEntities()
{
	for (int i = (int) m_allEntities.size() - 1; i >= 0; i--){
		Entity* entity = m_allEntities[i];
		if (entity->m_aboutToBeDeleted){
			m_scene->RemoveRenderable(m_allEntities[i]->m_renderable);
			RemoveAtFast(m_allEntities, i);
			//delete entity;
		}
	}

	
}

