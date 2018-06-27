#include "Game/GameState_Playing.hpp"
#include "Game/Game.hpp"
#include "Game/Map.hpp"
#include "Game/Entity.hpp"
#include "Game/Player.hpp"
#include "game/Enemy.hpp"
#include "Game/DebugRenderSystem.hpp"
#include "Engine/Renderer/ForwardRenderPath.hpp"
#include "Engine/Renderer/PerspectiveCamera.hpp"


GameState_Playing::GameState_Playing()
{
	m_renderPath = new ForwardRenderPath();
	m_renderPath->m_renderer = g_theRenderer;
	m_scene = new RenderScene();


	m_couchMaterial = Material::GetMaterial("couch");



	//m_thaShip = new Entity(Vector3::ZERO, "scifi_fighter_mk6.obj");
	//m_thaShip->SetDiffuseTexture("SciFi_Fighter-MK6-diffuse.png");

	//m_thaMiku = new Entity(Vector3(0.f, 3.f, 10.f), "miku.obj", "miku.mtl");
	//m_thaMiku->Rotate(Vector3(0.f,180.f,0.f));
	//m_thaMiku->m_renderable->SetShader("lit_alpha", 0);
	//m_thaMiku->m_renderable->SetShader("lit_alpha", 1);
	//m_thaMiku->m_renderable->SetShader("default_lit", 2);

	m_particleSystem = new ParticleSystem();
	m_particleSystem->CreateEmitter(Vector3(0.f, 4.f, 0.f));
	m_particleSystem->m_emitters[0]->SetSpawnRate(200.f);

	//m_scene->AddRenderable(m_particleSystem->m_emitters[0]->m_renderable);



	//m_scene->AddNewPointLight(Vector3::ZERO, RGBA::WHITE);
	//m_scene->AddNewSpotLight(Vector3(0.f, 4.f, -5.f), RGBA::WHITE, 20.f, 23.f);		//camera light
	//m_scene->AddNewSpotLight(Vector3(0.f, 5.f, 5.f), RGBA(255,255,128,255));			//orbiting light
	//m_scene->AddNewPointLight(Vector3(0.f, 5.f, 15.f), RGBA(255, 128, 70,255));		//reddish point light
	m_scene->AddNewDirectionalLight(Vector3(-10.f, 0.f, -10.f), RGBA::WHITE, Vector3(0.f, -90.f, -10.f));		//bluish directional light

	m_scene->AddCamera(g_theGame->m_currentCamera);
	g_theGame->m_mainCamera->AddSkybox("skybox.png");
}

void GameState_Playing::Update(float ds)
{
	m_timeInState+=ds;

	float deltaSeconds = ds;
	if (!g_theGame->m_isPaused){
		//the game stuff happens here
		//g_theGame->m_gameTime+=deltaSeconds;


		float degrees = 15.f * g_theGame->m_gameClock->GetCurrentSeconds(); 
		//
		Vector2 xz_pos = PolarToCartesian( 8.f, degrees ); 
		Vector3 pos = Vector3( xz_pos.x, 5.f, xz_pos.y ); 
	}

	//m_couchMaterial->SetProperty("SPECULAR_AMOUNT", m_specAmount);
	//m_couchMaterial->SetProperty("SPECULAR_POWER", m_specFactor);
	//m_particleSystem->Update(deltaSeconds);
	m_player->Update();
	for (Entity* entity : m_allEntities){
		entity->Update();
	}

	DeleteEntities();
	//g_theGame->m_mainCamera->m_transform.SetLocalPosition(m_player->GetPosition() + Vector3(0.f, 3.f, -5.f));
	
}

void GameState_Playing::RenderGame()
{
	g_theRenderer->ClearScreen( RGBA::BLACK ); 
	g_theRenderer->ClearDepth( 1.0f ); 
	g_theRenderer->EnableDepth( COMPARE_LESS, true ); 

	if (g_theGame->IsDevMode()){		//draw cube at origin
		g_theRenderer->DrawCube(Vector3::ZERO,Vector3::ONE, RGBA::RED);
	}

	m_particleSystem->m_emitters[0]->CameraPreRender(g_theGame->m_currentCamera);
	//SetShader();

	//m_thaShip->m_renderable->GetEditableMaterial()->SetProperty("TINT", RGBA::RED);

	g_theRenderer->BindModel(Matrix44::IDENTITY);




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
	std::string specFactor = std::to_string(m_specFactor);
	specFactor.resize(5);
	//specFactor = "Spec Factor (J/K): " + specFactor;

	std::string specAmount = std::to_string(m_specAmount);
	specAmount.resize(5);
	//g_theGame->m_debugRenderSystem->MakeDebugRender2DText("Spec Amount(N/M): %s\nSpec Power(J/K): %s", specAmount.c_str(), specFactor.c_str());
	//g_theGame->m_debugRenderSystem->MakeDebugRender2DText(0.f, .01f, Vector2(1.f, .95f), "Shader ([/]): %s", GetShaderName().c_str());
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
	float ds = g_theGame->GetDeltaSeconds();
	float factorScale = 10.f;
	if (g_theInput->IsKeyDown('K')){
		m_specFactor+=(ds * factorScale);
		m_specFactor = ClampFloat(m_specFactor, 1.f, 99.999f);
	}
	if (g_theInput->IsKeyDown('J')){
		m_specFactor-=(ds * factorScale);
		m_specFactor = ClampFloat(m_specFactor, 1.f, 99.999f);
	}

	if (g_theInput->IsKeyDown('M')){
		m_specAmount+=ds;
		m_specAmount = ClampFloat(m_specAmount, 0.f, 1.f);
	}
	if (g_theInput->IsKeyDown('N')){
		m_specAmount-=ds;
		m_specAmount = ClampFloat(m_specAmount, 0.f, 1.f);
	}

	if (!g_theGame->m_debugRenderSystem->m_isDetached){
		m_player->HandleInput();
	}
}

void GameState_Playing::EnterState()
{
	m_player = new Player(this, Vector3::ZERO);

	AddNewEnemy(Vector2(5.f, 5.f));
	AddNewEnemy(Vector2(10.f, 8.f));
	AddNewEnemy(Vector2(30.f, -35.f));
	AddNewEnemy(Vector2(GetRandomFloatInRange(-100.f, 100.f), GetRandomFloatInRange(-100.f, 100.f)));
	AddNewEnemy(Vector2(GetRandomFloatInRange(-100.f, 100.f), GetRandomFloatInRange(-100.f, 100.f)));
	AddNewEnemy(Vector2(GetRandomFloatInRange(-100.f, 100.f), GetRandomFloatInRange(-100.f, 100.f)));
	AddNewEnemy(Vector2(GetRandomFloatInRange(-100.f, 100.f), GetRandomFloatInRange(-100.f, 100.f)));

	m_scene->AddRenderable(m_player->m_renderable);
	m_scene->AddRenderable(m_player->m_turretRenderable);
	m_scene->AddRenderable(m_player->m_laserSightRenderable);

	g_theGame->m_mainCamera->m_transform.SetParent(m_player->m_cameraTarget);
}



Enemy * GameState_Playing::AddNewEnemy(const Vector2 & pos)
{
	Enemy* enemy = new Enemy(pos, this);
	m_scene->AddRenderable(enemy->m_renderable);
	m_enemies.push_back(enemy);
	m_allEntities.push_back((Entity*) enemy);
	return enemy;
}

Bullet * GameState_Playing::AddNewBullet(const Transform& t)
{
	Bullet* b = new Bullet(t, this);
	m_scene->AddRenderable(b->m_renderable);
	m_bullets.push_back(b);
	m_allEntities.push_back((Entity*) b);
	return b;
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
	return m_scene->m_lights.size();
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

	for (int i = (int) m_enemies.size() -1; i >= 0; i--){
		Enemy* enemy = m_enemies[i];
		if (enemy->m_aboutToBeDeleted){
			RemoveAtFast(m_enemies,i);
			delete enemy;
		}
	}

	for (int i = (int) m_bullets.size() -1; i >= 0; i--){
		Bullet* bullet = m_bullets[i];
		if (bullet->m_aboutToBeDeleted){
			RemoveAtFast(m_bullets,i);
			delete bullet;
		}
	}
}
