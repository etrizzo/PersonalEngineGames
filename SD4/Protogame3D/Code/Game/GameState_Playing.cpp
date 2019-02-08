#include "Game/GameState_Playing.hpp"
#include "Game/Game.hpp"
#include "Game/Map.hpp"
#include "Game/Player.hpp"
#include "Game/DebugRenderSystem.hpp"
#include "Engine/Renderer/ForwardRenderPath.hpp"
#include "Engine/Renderer/PerspectiveCamera.hpp"


GameState_Playing::GameState_Playing()
{
	m_renderPath = new ForwardRenderPath(g_theRenderer);
	//m_renderPath->m_renderer = g_theRenderer;
	m_scene = new RenderScene(g_theRenderer);


	m_couchMaterial = Material::GetMaterial("couch");


	m_sun = m_scene->AddNewDirectionalLight(Vector3(-0, 20, -0.f), RGBA::WHITE.GetColorWithAlpha(180), Vector3(-35.f, -30.f, 0.f));	

	//m_sun->SetUsesShadows(true);
	//m_scene->SetShadowCameraTransform(m_sun->m_transform);

	m_scene->AddCamera(g_theGame->m_currentCamera);
	g_theGame->m_mainCamera->AddSkybox("galaxy2.png", RIGHT, UP, FORWARD);
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
	m_timeInState+=ds;
	float rotation = 45.f * ds;
	m_thaMiku->Rotate(Vector3(0.f, rotation,0.f));
	m_thaShip->Rotate(Vector3(0.f, rotation, 0.f));
	m_thaOrb->Rotate(Vector3(0.f, rotation, 0.f));
	m_player->Update();
	m_particleSystem->Update(ds);
	//Update spawners before the rest of entities bc it can add entities
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

	if (g_theGame->IsDevMode()){		//draw cube at origin
		g_theGame->m_debugRenderSystem->MakeDebugRenderBasis(0.f, m_scene->m_shadowCamera->m_transform.GetWorldPosition(), 5.f, m_scene->m_shadowCamera->m_transform.GetWorldMatrix());
	}

	m_particleSystem->m_emitters[0]->CameraPreRender(g_theGame->m_currentCamera);
	g_theRenderer->BindModel(Matrix44::IDENTITY);

	//set shadow camera's position for this frame
	m_scene->SetShadowCameraTransform(*m_player->m_shadowCameraTransform);
	
	m_renderPath->Render(m_scene);

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

	if (g_theInput->WasKeyJustPressed(VK_OEM_6)){
		UpdateShader(1);
	}
	if (g_theInput->WasKeyJustPressed(VK_OEM_4)){
		UpdateShader(-1);
	}


	if (g_theInput->WasKeyJustPressed('L')){
		AddNewPointLight( g_theGame->m_currentCamera->GetPosition() + g_theGame->m_currentCamera->GetForward(), RGBA::WHITE);
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
	m_thaShip = new Entity(Vector3(8.f, 5.f, 12.f), "scifi_fighter_mk6.obj", "normal");
	Entity* otherShip = new Entity(Vector3(8.f, 10.f, 12.f), "scifi_fighter_mk6.obj", "normal");
	//m_thaShip->SetDiffuseTexture("SciFi_Fighter-MK6-diffuse.png");

	m_thaMiku = new Entity(Vector3(-2.f, 3.f, 10.f), "miku.obj", "miku.mtl");
	//m_thaMiku->Rotate(Vector3(0.f,180.f,0.f));
	m_thaMiku->m_renderable->SetShader("lit_alpha", 0);
	m_thaMiku->m_renderable->SetShader("lit_alpha", 1);
	m_thaMiku->m_renderable->SetShader("world_normal", 2);

	Entity* otherMiku = new Entity(Vector3(-2.f, 8.f, 10.f), "miku.obj", "miku.mtl");
	otherMiku->m_renderable->SetShader("lit_alpha", 0);
	otherMiku->m_renderable->SetShader("lit_alpha", 1);
	otherMiku->m_renderable->SetShader("world_normal", 2);

	m_thaOrb = new Entity();
	m_thaOrb->m_renderable = new Renderable(RENDERABLE_CUBE, 3.f);
	m_thaOrb->SetMaterial(Material::GetMaterial("normal"));
	m_thaOrb->SetPosition(Vector3(-8.f, 4.f, 10.f));

	Entity* otherOrb = new Entity();
	otherOrb->m_renderable = new Renderable(RENDERABLE_CUBE, 3.f);
	otherOrb->SetMaterial(Material::GetMaterial("normal"));
	otherOrb->SetPosition(Vector3(-8.f, 9.f, 10.f));

	m_particleSystem = new ParticleSystem();
	m_particleSystem->CreateEmitter(Vector3(0.f, 10.f, 15.f));
	m_particleSystem->m_emitters[0]->SetSpawnRate(200.f);
	m_particleSystem->m_emitters[0]->SetSpawnColor(RGBA::GetRandomRainbowColor);

	m_scene->AddRenderable(m_particleSystem->m_emitters[0]->m_renderable);
	m_scene->AddRenderable(m_thaShip->m_renderable);
	m_scene->AddRenderable(m_thaMiku->m_renderable);
	m_scene->AddRenderable(m_thaOrb->m_renderable);

	m_scene->AddRenderable(otherShip->m_renderable);
	m_scene->AddRenderable(otherMiku->m_renderable);
	m_scene->AddRenderable(otherOrb->m_renderable);


	//m_scene->AddNewPointLight(Vector3::ZERO, RGBA::WHITE);
	//m_scene->AddNewSpotLight(Vector3(0.f, 4.f, -5.f), RGBA::WHITE, 20.f, 23.f);		//camera light
	//m_scene->AddNewSpotLight(Vector3(0.f, 5.f, 5.f), RGBA(255,255,128,255));			//orbiting light
	//m_scene->AddNewPointLight(Vector3(0.f, 5.f, 15.f), RGBA(255, 128, 70,255));		//reddish point light
	
	//m_cameraLight = m_scene->m_lights[0];
	//m_orbitLight = (SpotLight*) m_scene->m_lights[1];

	SpawnPlayer(Vector3::ZERO);
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
		if (m_player->m_aboutToBeDeleted){
			m_gameLost = true;
			m_player->RemoveRenderable();
			g_theGame->TransitionToState(new GameState_Defeat(this));
		}
	}
}

void GameState_Playing::SpawnPlayer(Vector3 pos)
{
	if (m_player != nullptr){
		delete m_player;
	}
	m_player = new Player(this, pos);

	m_scene->AddRenderable(m_player->m_renderable);
	m_scene->AddRenderable(m_player->m_turretRenderable);
	m_scene->AddRenderable(m_player->m_laserSightRenderable);

	g_theGame->m_mainCamera->m_transform.SetParent(m_player->m_cameraTarget);
}

void GameState_Playing::UpdateShader(int direction)
{
	//m_debugShader= eDebugShaders(ClampInt(m_debugShader + direction, 0, NUM_DEBUG_SHADERS - 1));
	int newShader = m_debugShader + direction + NUM_DEBUG_SHADERS;
	m_debugShader= eDebugShaders(newShader % NUM_DEBUG_SHADERS);

}

void GameState_Playing::SetShader()
{
	std::string shaderName = "invalid";
	switch (m_debugShader){
	case SHADER_LIT:
		shaderName = "default_lit";
		break;
	case SHADER_NORMAL		:
		shaderName = "vertex_normal";
		break;
	case SHADER_TANGENT		:
		shaderName = "tangent";
		break;
	case SHADER_BITANGENT	:
		shaderName = "bitangent";
		break;
	case SHADER_NORMALMAP	:
		shaderName = "normal_map";
		break;
	case SHADER_WORLDNORMAL	:
		shaderName = "world_normal";
		break;
	case SHADER_DIFFUSE		:
		shaderName = "diffuse";
		break;
	case SHADER_SPECULAR	:
		shaderName = "specular";
		break;
	}
	
	g_theGame->m_debugRenderSystem->MakeDebugRender2DText(shaderName.c_str());

	g_theRenderer->UseShader(shaderName);
	m_player->m_renderable->SetShader(shaderName);
	g_theGame->m_currentMap->m_renderable->SetShader(shaderName);
	m_couchMaterial->SetShader(shaderName);
}

std::string GameState_Playing::GetShaderName() const
{
	switch (m_debugShader){
	case SHADER_LIT:
		return "default_lit";
		break;
	case SHADER_NORMAL		:
		return "vertex_normal";
		break;
	case SHADER_TANGENT		:
		return "tangent";
		break;
	case SHADER_BITANGENT	:
		return "bitangent";
		break;
	case SHADER_NORMALMAP	:
		return "normal_map";
		break;
	case SHADER_WORLDNORMAL	:
		return "world_normal";
		break;
	case SHADER_DIFFUSE :
		return "diffuse";
		break;
	case SHADER_SPECULAR :
		return "specular";
		break;

	}
	return "NO_SHADER";
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

