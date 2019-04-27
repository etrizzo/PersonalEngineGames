#include "Game/GameState_Playing.hpp"
#include "Game/Game.hpp"
#include "Game/Map.hpp"
#include "Game/Player.hpp"
#include "Game/DebugRenderSystem.hpp"
#include "Game/FlowerPot.hpp"
#include "Game/Asteroid.hpp"
#include "Game/Missile.hpp"
#include "Game/Enemy.hpp"
#include "Game/EnemySpawner.hpp"
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
	//g_theGame->m_mainCamera->AddSkybox("galaxy2.png", RIGHT, UP, FORWARD);
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
		//g_theGame->m_debugRenderSystem->DetachCamera();
	}
}

void GameState_Playing::Update(float ds)
{
	PROFILE_PUSH_FUNCTION_SCOPE();
	m_timeInState+=ds;
	m_player->Update();
	m_particleSystem->Update(ds);
	//Update spawners before the rest of entities bc it can add entities
	for (FlowerPot* pot : m_flowerPots)
	{
		pot->Update();
	}
	for (Asteroid* ast : m_asteroids)
	{
		ast->Update();
	}
	for (Missile* missile : m_missiles)
	{
		missile->Update();
	}
	for (Enemy* enemy : m_enemies)
	{
		enemy->Update();
	}
	for (EnemySpawner* spawner : m_spawners)
	{
		spawner->Update();
	}

	if (m_asteroidSpawnClock.CheckAndReset())
	{
		SpawnAsteroid();
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
	RenderEntities();

	g_theRenderer->ReleaseTexture(0);
	g_theRenderer->ReleaseTexture(1);
	g_theRenderer->ReleaseShader();

	g_theGame->m_debugRenderSystem->UpdateAndRender();
	g_theRenderer->EnableDepth( COMPARE_LESS, true ); 
	
}

void GameState_Playing::RenderEntities()
{
	for (Entity* entity : m_allEntities)
	{
		entity->Render();
	}

	m_player->Render();
}

void GameState_Playing::RenderUI()
{
	AABB2 bounds = g_theGame->SetUICamera();

	std::string debugText = Stringf("Player pos: (%3.2f, %3.2f)", g_theGame->GetPlayerPositionXY().x, g_theGame->GetPlayerPositionXY().y);
	g_theRenderer->DrawTextInBox2D(debugText, bounds, Vector2::TOP_LEFT_PADDED, .02f, TEXT_DRAW_SHRINK_TO_FIT);

	RenderPlayerHealthBar(bounds);
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

void GameState_Playing::SpawnMissile(Vector3 position, Asteroid * target)
{
	Missile* missile = new Missile(position, target);
	m_missiles.push_back(missile);
	m_allEntities.push_back((Entity*)missile);
}

void GameState_Playing::SpawnEnemy(const Vector3 & position)
{
	Entity* target = GetClosestAlliedEntity(position);
	Enemy* newEnemy = new Enemy(position, target);
	m_enemies.push_back(newEnemy);
	m_allEntities.push_back((Entity*) newEnemy);

}

Entity * GameState_Playing::GetClosestAlliedEntity(const Vector3 & position)
{
	Entity* target = nullptr;
	float shortestDistance = 100000.f;
	for (FlowerPot* pot : m_flowerPots)
	{
		float distance = GetDistanceSquared(position, pot->GetPosition());
		if (distance < shortestDistance)
		{
			shortestDistance = distance;
			target = (Entity*)pot;
		}
	}

	float playerDist = GetDistanceSquared(position, m_player->GetPosition());
	if (playerDist < shortestDistance)
	{
		shortestDistance = playerDist;
		target = (Entity*) m_player;
	}

	return target;
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
	m_particleSystem = new ParticleSystem();
	m_particleSystem->CreateEmitter(Vector3(0.f, 10.f, 15.f));
	m_particleSystem->m_emitters[0]->SetSpawnRate(200.f);
	m_particleSystem->m_emitters[0]->SetSpawnColor(RGBA::GetRandomRainbowColor);

	m_scene->AddRenderable(m_particleSystem->m_emitters[0]->m_renderable);


	//m_scene->AddNewPointLight(Vector3::ZERO, RGBA::WHITE);
	//m_scene->AddNewSpotLight(Vector3(0.f, 4.f, -5.f), RGBA::WHITE, 20.f, 23.f);		//camera light
	//m_scene->AddNewSpotLight(Vector3(0.f, 5.f, 5.f), RGBA(255,255,128,255));			//orbiting light
	//m_scene->AddNewPointLight(Vector3(0.f, 5.f, 15.f), RGBA(255, 128, 70,255));		//reddish point light
	
	//m_cameraLight = m_scene->m_lights[0];
	//m_orbitLight = (SpotLight*) m_scene->m_lights[1];

	m_asteroidSpawnClock = StopWatch();
	m_asteroidSpawnClock.SetTimer(1.f);

	SpawnPlayer(Vector3::ZERO);
	
	float walkingRadius = g_theGame->GetMap()->GetWalkableRadius();

	SpawnFlowerPot(RangeMapFloat(.25f, 0.f, 1.f, -walkingRadius, walkingRadius));
	SpawnFlowerPot(RangeMapFloat(.5f, 0.f, 1.f, -walkingRadius, walkingRadius));
	SpawnFlowerPot(RangeMapFloat(.75f, 0.f, 1.f, -walkingRadius, walkingRadius));

	float mapRadius = g_theGame->GetMap()->GetRadius();
	CreateSpawner(Vector3(-mapRadius, g_theGame->GetMap()->GetHeightFromXPosition(-mapRadius), g_theGame->GetMap()->GetCenter().z));
	CreateSpawner(Vector3(mapRadius, g_theGame->GetMap()->GetHeightFromXPosition(-mapRadius), g_theGame->GetMap()->GetCenter().z));
}

void GameState_Playing::RenderPlayerHealthBar(const AABB2 & uiBounds)
{
	float percentageOfHealth = m_player->GetPercentageOfHealth();

	AABB2 healthBox = uiBounds.GetPercentageBox(.1f, .05f, .9f, .1f);
	g_theRenderer->DrawAABB2(healthBox, RGBA::BLACK);
	float healthHeight = healthBox.GetHeight();

	healthBox.AddPaddingToSides(healthHeight * -.1f, healthHeight * -.1f);
	g_theRenderer->DrawAABB2(healthBox, RGBA::RED);

	AABB2 actualHealthArea = healthBox.GetPercentageBox(0.f, 0.f, percentageOfHealth, 1.f);
	g_theRenderer->DrawAABB2(actualHealthArea, RGBA::GREEN);

	//draw reload points
	float percentageOfHealthForReload = RELOAD_HEALTH_COST / PLAYER_MAX_HEALTH;
	float percentageToDrawAt = percentageOfHealthForReload;
	float barWidth = .001f;
	while (percentageToDrawAt < 1.f)
	{
		AABB2 barToDraw = healthBox.GetPercentageBox(percentageToDrawAt - barWidth, 0.0f, percentageToDrawAt + barWidth, 1.0f);
		g_theRenderer->DrawAABB2(barToDraw, RGBA::YELLOW);
		percentageToDrawAt += percentageOfHealthForReload;
	}
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

	
	g_theGame->m_mainCamera->m_transform.SetParent(m_player->m_cameraTarget);
	g_theGame->m_mainCamera->m_transform.SetLocalPosition(Vector3(0.f, 2.f, -12.f));
}

void GameState_Playing::SpawnFlowerPot(float xPosition)
{
	FlowerPot* flowerPot = new FlowerPot(xPosition);
	m_flowerPots.push_back(flowerPot);
	m_allEntities.push_back((Entity*)flowerPot);
}

void GameState_Playing::SpawnAsteroid()
{
	float randomStartX = GetRandomFloatInRange(-g_theGame->m_currentMap->GetRadius(), g_theGame->m_currentMap->GetRadius());
	float randomTargetX = GetRandomFloatInRange(-g_theGame->m_currentMap->GetRadius(), g_theGame->m_currentMap->GetRadius());

	float startY = g_theGame->GetMap()->GetRadius() * 2.f;
	float targetY = g_theGame->m_currentMap->GetHeightFromXPosition(randomTargetX);

	float z = g_theGame->m_currentMap->GetCenter().z;

	Asteroid* newAsteroid = new Asteroid(Vector3(randomStartX, startY, z), Vector3(randomTargetX, targetY, z));
	m_asteroids.push_back(newAsteroid);
	m_allEntities.push_back((Entity*)newAsteroid);

}

void GameState_Playing::CreateSpawner(Vector3 pos)
{
	EnemySpawner* spawner = new EnemySpawner(pos);
	m_spawners.push_back(spawner);
	m_allEntities.push_back((Entity*)spawner);
}

void GameState_Playing::DeleteEntities()
{
	//remove entities from special lists
	for (int i = (int)m_asteroids.size() - 1; i >= 0; i--)
	{
		Asteroid* asteroid = m_asteroids[i];
		if (asteroid->m_aboutToBeDeleted)
		{
			RemoveAtFast(m_asteroids, i);
		}
	}

	for (int i = (int)m_flowerPots.size() - 1; i >= 0; i--)
	{
		FlowerPot* flowerpot = m_flowerPots[i];
		if (flowerpot->m_aboutToBeDeleted)
		{
			RemoveAtFast(m_flowerPots, i);
		}
	}

	for (int i = (int)m_missiles.size() - 1; i >= 0; i--)
	{
		Missile* missile = m_missiles[i];
		if (missile->m_aboutToBeDeleted)
		{
			RemoveAtFast(m_missiles, i);
		}
	}

	for (int i = (int)m_enemies.size() - 1; i >= 0; i--)
	{
		Enemy* enemy = m_enemies[i];
		if (enemy->m_aboutToBeDeleted)
		{
			RemoveAtFast(m_enemies, i);
		}
	}


	//remove and delete from AllEntities
	for (int i = (int)m_allEntities.size() - 1; i >= 0; i--) {
		Entity* entity = m_allEntities[i];
		if (entity->m_aboutToBeDeleted) {
			RemoveAtFast(m_allEntities, i);
			delete entity;
		}
	}
}

