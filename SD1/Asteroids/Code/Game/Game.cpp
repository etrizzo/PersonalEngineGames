#include "Game.hpp"
#include "Star.hpp"
#include "Engine/Renderer/Camera.hpp"

#define GRID_SIZE 30

Game::Game()
{
	m_isPaused = false;
	m_devMode = false;

	m_numAsteroidsAlive = 0;
	m_numAsteroidsInWave = GetRandomIntInRange(4,6);
	m_gameTime = 0.f;
	m_starburstCharge = -1.f;

	m_asteroidRadii[0] = MAX_RADIUS/4.f;
	m_asteroidRadii[1] = MAX_RADIUS/2.f;
	m_asteroidRadii[2] = MAX_RADIUS;
	
	m_sountrackID = g_theAudio->CreateOrGetSound("Data/Audio/Pixel River.mp3");
	m_popSoundID = g_theAudio->CreateOrGetSound("Data/Audio/pop.ogg");
	m_laserSoundID = g_theAudio->CreateOrGetSound("Data/Audio/laser3.wav");

	g_theAudio->PlaySound(m_sountrackID, true);
	//make asteroids
	SpawnWave();

	//TestAsteroid();


	m_ship = new Ship();

	m_camera = new Camera();

	// Setup what it will draw to
	m_camera->SetColorTarget( g_theRenderer->m_defaultColorTarget );
	m_camera->SetDepthStencilTarget( g_theRenderer->m_defaultDepthTarget );


	// for -1 to 1
	//m_uiCamera->SetProjectionOrtho( Vector3(-1.f, -1.f, 0.f), Vector3(1.f, 1.f, 100.f) );  
	//m_uiCamera->LookAt( Vector3( 0.0f, 0.0f, -10.0f ), Vector3::ZERO ); 
	m_camera->SetProjectionOrtho(SCREEN_MAX, g_gameConfigBlackboard.GetValue("windowAspect", 1.f), 0.f,100.f);
	Vector2 center = m_camera->GetBounds().GetCenter();
	m_camera->LookAt( Vector3( center.x, center.y, -1.f ), Vector3(center.x, center.y, .5f)); 

	g_theRenderer->SetCamera( m_camera ); 

	g_theRenderer->ClearDepth( 1.0f ); 
	g_theRenderer->EnableDepth( COMPARE_LESS, true );
	m_starBuilder = new MeshBuilder();
	m_starMesh = new SubMesh(Vertex3D_PCU::LAYOUT);
	m_stars.reserve(MAX_STARS);
}

void Game::Update(float deltaSeconds)
{
	/*if (g_theInput->WasKeyJustPressed('P')){
		TogglePause();
	}*/
	

	if (!m_isPaused){
		//if all asteroids have been destroyed, spawn a new wave
		if (m_numAsteroidsAlive == 0){
			SpawnWave();
		}
		if (m_starburstCharge >=0.f){
			m_starburstCharge+=(deltaSeconds/3.f);
		}

		m_gameTime+=deltaSeconds;		//TODO: time;

		//update asteroids
		for (int i = 0; i < m_numAsteroidsAlive; i++){
			m_asteroids[i]->Update(deltaSeconds);

			//check for ship collision
			if (m_ship->m_alive){
				if (DoDiscsOverlap(m_asteroids[i]->m_physicsRadius, m_ship->m_physicsRadius)){
					BreakAsteroid(i);
					KillShip();
					break;
				}
			}

			//check for bullet collision
			for (int j = 0; j < m_numBullets; j++){
				if (DoDiscsOverlap(m_asteroids[i]->m_physicsRadius, m_bullets[j]->m_physicsRadius)){
					BreakAsteroid(i);
					//DeleteAsteroid(i);
					DeleteBullet(j);
					break;
				}

			}
		}

		//update ship
		m_ship->Update(deltaSeconds);
		if (m_ship->m_firingStarburst && (m_gameTime - m_lastShot > .00005f)){
			FireBullet();
		}

		//update bullets
		for (int i = 0; i < m_numBullets; i++){
			m_bullets[i]->Update(deltaSeconds);
			if (m_bullets[i]->m_ageInSeconds > 2.f){
				DeleteBullet(i);
			}
		}

		AddStars();
		UpdateStars(deltaSeconds);
	}
}

void Game::UpdateStars(float deltaSeconds)
{
	PROFILE_PUSH_FUNCTION_SCOPE();
	for (unsigned int starIndex = 0; starIndex < m_stars.size(); starIndex++){
		Star* star = m_stars[starIndex];
		star->Update(deltaSeconds);
	}


	for (unsigned int starIndex = 0; starIndex < m_stars.size(); starIndex++){
		Star* star = m_stars[starIndex];
		if (star->m_aboutToBeDeleted){
			m_stars.erase(m_stars.begin() + starIndex);
			delete(star);
		}
	}
}

void Game::RenderBackground()
{
	PROFILE_PUSH_FUNCTION_SCOPE();
	float red = Compute2dPerlinNoise(m_gameTime, 0.f, 5.f, 2, .5f, 2.f, true, 0);
	float green = Compute2dPerlinNoise(m_gameTime, 0.f, 7.f, 1, .5f, 2.f, true, 1);
	float blue = Compute2dPerlinNoise(m_gameTime, 0.f, 4.f, 3, .5f, 2.f, true, 2);
	RGBA screenColor = RGBA();
	float maxRed = .08f;
	float maxGreen = .08f;
	float maxBlue = .2f;
	float minBlue = .03f;
	screenColor.SetAsFloats(RangeMapFloat(red, -1.f, 1.f, 0.f, maxRed),RangeMapFloat(green, -1.f, 1.f, 0.f, maxGreen),RangeMapFloat(blue, -1.f, 1.f, minBlue, maxBlue), 1.f);
	g_theRenderer->DrawAABB2(screenBox, screenColor);

	RenderStars();
	//Get colors for 2 blended boxes to make the bg change color softly
	RGBA color1 = GetPerlin3DColorAtPosition(screenBox.mins.x, screenBox.maxs.y, 3);
	RGBA color2 = GetPerlin3DColorAtPosition(screenBox.mins.x, screenBox.mins.y, 3);
	RGBA color3 = GetPerlin3DColorAtPosition(screenBox.maxs.x, screenBox.mins.y, 3);
	RGBA color4 = GetPerlin3DColorAtPosition(screenBox.maxs.x, screenBox.maxs.y, 3);

	unsigned char boxAlpha = 30;
	g_theRenderer->DrawBlendedAABB2(screenBox, color1.GetColorWithAlpha(boxAlpha), color2.GetColorWithAlpha(boxAlpha), color3.GetColorWithAlpha(boxAlpha), color4.GetColorWithAlpha(boxAlpha));
	g_theRenderer->DrawBlendedAABB2(screenBox, color1.GetColorWithAlpha(boxAlpha), color2.GetColorWithAlpha(boxAlpha), color3.GetColorWithAlpha(boxAlpha), color4.GetColorWithAlpha(boxAlpha));

	//RenderStars();

	//debug
	//RenderStarNoise();
	//g_theRenderer->DrawTextInBox2D(std::to_string(m_stars.size()), screenBox, Vector2(.5f,1.f), 20.f);
}

void Game::AddStars()
{
	PROFILE_PUSH_FUNCTION_SCOPE();
	//if long enough since last star spawn & not at max stars
	if ((m_stars.size() <= MAX_STARS) && (m_gameTime > m_lastSpawnedStar + 0.3f)){
		//check every "tile" on the screen for perlin noise value. This determines chance for star spawn, and star size/opacity
		AABB2 tileArea;
		float step = SCREEN_MAX * (1.f / GRID_SIZE);
		for (int x = 0; x < GRID_SIZE; x++){
			for (int y = 0; y < GRID_SIZE; y++){
				float xPos = x * step;
				float yPos = y * step;
				tileArea = AABB2(xPos, yPos, xPos + step, yPos + step);
				float zOnTime =  m_gameTime* SCREEN_MAX * .1f;
				float perlinVal = Compute3dPerlinNoise(xPos, yPos, zOnTime, SCREEN_MAX * .5f);		//perlin noise waves cale should be about half the screen size
				if (perlinVal > -0.5f){
					float chanceForStars = (perlinVal + 1 ) * .6666f;	//fast range map for known range
					
					//check square of range-mapped perlin val to get much more spawns in high value areas, but some spawn in most areas
					if (CheckRandomChance(chanceForStars * chanceForStars)){
						AddNewStar(tileArea, chanceForStars * chanceForStars * chanceForStars);
						m_lastSpawnedStar = m_gameTime;
					}
				}
			}
		}
	}
	
}

void Game::AddNewStar(const AABB2& starBox, const float& perlinVal)
{
	Vector2 position = starBox.GetRandomPointInBox();
	//get star color based on position - lighten slightly
	RGBA color = GetPerlin3DColorAtPosition(position.x, position.y, 0, SCREEN_MAX);
	RGBA rainbow = RGBA::GetRandomRainbowColor();
	color = Interpolate(color, rainbow, .1f);
	color = Interpolate(color, RGBA::WHITE, .1f);
	color = color.GetColorWithAlpha(100);
	float size = GetRandomFloatInRange(.2f * perlinVal, 8.f * perlinVal);
	//float size = 20.f;
	float duration = GetRandomFloatInRange(2.f,3.f);
	float rotation = GetRandomFloatInRange(0.f,360.f);
	float rotateSpeed = GetRandomFloatInRange(-180.f, 180.f);

	Star* newStar = new Star(position, color, size, duration, 0.f, rotateSpeed);
	m_stars.push_back(newStar);
}

void Game::Render()
{
	CreateStarMesh();
	g_theRenderer->SetCullMode(CULLMODE_NONE);
	g_theRenderer->DisableDepth();
	RenderBackground();
	

	//render asteroids
	for (int i = 0; i < m_numAsteroidsAlive; i++){
		m_asteroids[i]->Render(m_devMode);
	}

	//render ship
	
	m_ship->Render(m_devMode);
	
	

	//render bullets
	for (int i = 0; i < m_numBullets; i++){
		m_bullets[i]->Render(m_devMode);
	}

}

void Game::CreateStarMesh()
{
	PROFILE_PUSH_FUNCTION_SCOPE();
	//old rendering :)
	//for (unsigned int starIndex = 0; starIndex < m_stars.size(); starIndex++){
	//	Star* star = m_stars[starIndex];
	//	star->Render();
	//}


	unsigned int starCount = m_stars.size();

	m_starBuilder->Clear();
	m_starBuilder->Begin(PRIMITIVE_LINES, false);
	for (unsigned int i = 0; i < starCount; i++){
		Star* s = m_stars[i];
		m_starBuilder->AppendVertices(s->m_vertices, s->m_transform);
	}
	m_starBuilder->End();
	//remakes the mesh every frame. eventually, we might keep this on the GPU
	m_starMesh->FromBuilder(m_starBuilder, VERTEX_TYPE_3DPCU);
}

void Game::RenderStars()
{
	PROFILE_PUSH_FUNCTION_SCOPE();
	//for (unsigned int starIndex = 0; starIndex < m_stars.size(); starIndex++){
	//	Star* star = m_stars[starIndex];
	//	star->Render();
	//}
	g_theRenderer->DrawMesh(m_starMesh);
}

void Game::RenderStarNoise()
{
	AABB2 tileArea;
	float step = SCREEN_MAX * .033f;
	
	for (int x = 0; x < 30; x++){
		for (int y = 0; y < 30; y++){
			RGBA color = RGBA::RED;
			float xPos = x * step;
			float yPos = y * step;
			tileArea = AABB2(xPos, yPos, xPos + step, yPos + step);
			float perlinVal = Compute3dPerlinNoise(xPos, yPos, m_gameTime* SCREEN_MAX * .1f, SCREEN_MAX * .5f);
			//if (perlinVal > -.5f){
				float chanceForStars = RangeMapFloat(perlinVal, -1.f, 1.f, 0.f, 1.f);
				color.ScaleAlpha(chanceForStars);
				g_theRenderer->DrawAABB2(tileArea, color );
			//}
		}
	}

}

RGBA Game::GetPerlin2DColorAtPosition(const float & x, const float & y, int seed)
{
	float red =		Compute2dPerlinNoise(x, y, SCREEN_MAX, 1, .5f, 2.f, true, seed);
	float green =	Compute2dPerlinNoise(x, y, SCREEN_MAX, 1, .5f, 2.f, true, seed + 1);
	float blue =	Compute2dPerlinNoise(x, y, SCREEN_MAX, 1, .5f, 2.f, true, seed + 2);
	RGBA retColor = RGBA();
	retColor.SetAsFloats(RangeMapFloat(red, -1.f, 1.f, 0.f, 1.f),RangeMapFloat(green, -1.f, 1.f, 0.f, 1.f),RangeMapFloat(blue, -1.f, 1.f, 0.f, 1.f), 1.f);
	return retColor;
}

RGBA Game::GetPerlin3DColorAtPosition(const float & x, const float & y, int seed, float stride)
{
	float red =		Compute3dPerlinNoise(x, y, m_gameTime, stride, 1, .5f, 2.f, true, seed);
	float green =	Compute3dPerlinNoise(x, y, m_gameTime, stride, 1, .5f, 2.f, true, seed + 1);
	float blue =	Compute3dPerlinNoise(x, y, m_gameTime, stride, 1, .5f, 2.f, true, seed + 2);
	RGBA retColor = RGBA();
	retColor.SetAsFloats(RangeMapFloat(red, -1.f, 1.f, 0.f, 1.f),RangeMapFloat(green, -1.f, 1.f, 0.f, 1.f),RangeMapFloat(blue, -1.f, 1.f, 0.f, 1.f), 1.f);
	return retColor;						   
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

void Game::SpawnWave()
{
	for (int i = 0; i < m_numAsteroidsInWave; i++){
		AddAsteroid();
	}
	m_numAsteroidsInWave+=2;
}


void Game::AddAsteroid(int size, bool placeAtPoint, Vector2& point){
	Asteroid* a = RandomAsteroid(size);
	if (placeAtPoint){
		a->m_position = point;
	}
	if (m_numAsteroidsAlive < MAX_ASTEROIDS){
		m_asteroids[m_numAsteroidsAlive++] = a;
	} else {
		//error sound or something
	}
}

void Game::BreakAsteroid(int i)
{
	int brokenAsteroidSize = m_asteroids[i]->m_size;
	Vector2 position = Vector2(m_asteroids[i]->m_position);
	DeleteAsteroid(i);
	g_theAudio->PlaySound(m_popSoundID);
	if (brokenAsteroidSize > 0){
		AddAsteroid(brokenAsteroidSize-1, true, position);
		AddAsteroid(brokenAsteroidSize-1, true, position);
	}
}

void Game::DeleteBullet(int i){
	delete(m_bullets[i]);
	m_bullets[i] = nullptr;
	m_bullets[i] = m_bullets[m_numBullets-1];
	m_bullets[m_numBullets-1] = nullptr;
	m_numBullets--;
}

void Game::KillShip()
{
	m_ship->m_alive = false;
}

void Game::SpawnShip()
{
	if (!m_ship->m_alive){
		delete(m_ship);
		m_ship = nullptr;
		m_ship = new Ship();
	}
}

void Game::TestAsteroid()
{
	Vector2 pos = Vector2(SCREEN_SIZE/4.f, SCREEN_SIZE/4.f);
	Vector2 vel = Vector2(0.f,0.f);
	Disc2 outer = Disc2(pos,MAX_RADIUS);
	Disc2 inner = Disc2(pos, MAX_RADIUS * .9f);
	float rotation = 0.f;
	float rotateSpeed = MIN_SPEED * .7f;
	int sides = 20;
	int size = 2;

	Asteroid* a =  new Asteroid(pos, vel, outer,inner, rotation, rotateSpeed, sides, size);
	if (m_numAsteroidsAlive < MAX_ASTEROIDS){
		m_asteroids[m_numAsteroidsAlive++] = a;
	} else {
		//error sound or something
	}
}

void Game::DeleteAsteroid(){
	if (m_numAsteroidsAlive >0){
		delete(m_asteroids[m_numAsteroidsAlive-1]);
		m_asteroids[m_numAsteroidsAlive-1] = nullptr;
		m_numAsteroidsAlive--;
	}
}

void Game::DeleteAsteroid(int i){
	if (m_numAsteroidsAlive >0){
		delete(m_asteroids[i]);
		m_asteroids[i] = m_asteroids[m_numAsteroidsAlive-1];
		m_asteroids[m_numAsteroidsAlive-1] = nullptr;
		m_numAsteroidsAlive--;
	}
}

Asteroid* Game::RandomAsteroid(int size){
	float scrSize = static_cast<float>(SCREEN_SIZE);

	Vector2 pos = Vector2(GetRandomFloatInRange(0.f,scrSize), GetRandomFloatInRange(0.f, scrSize));
	float moveSpeed = GetRandomFloatInRange(MIN_SPEED,MAX_SPEED);
	Vector2 vel = Vector2(GetRandomFloatInRange(-1.f,1.f),GetRandomFloatInRange(-1.f,1.f));
	vel*=moveSpeed;
	

	if (size == -1){
		 size = GetRandomIntInRange(0,2);
	}
	float maxSize = m_asteroidRadii[size];
	Disc2 outer = Disc2(pos, GetRandomFloatInRange(maxSize*.75f, maxSize));
	Disc2 inner = Disc2(pos, outer.radius *.85f);
	float rotation = GetRandomFloatInRange(0.f, 360.f);
	float rotateSpeed = GetRandomFloatInRange(MIN_SPEED* .4f,MAX_SPEED * .4f);
	if (CheckRandomChance(.5f)){
		rotateSpeed *=-1.f;
	}


	int sides = GetRandomIntInRange(15,25);

	//move it off the screen
	if (vel.x > vel.y){
		//moving in x direction faster than y direction
		if (vel.x > 0){	//moving right
			pos.x = -outer.radius;
		} else {
			pos.x = scrSize + outer.radius;
		}
	} else{
		//moving in y direction faster than x direction
		if (vel.y > 0){
			//moving up
			pos.y = -outer.radius;
		} else {
			pos.y = scrSize + outer.radius;
		}
	}
	
	return new Asteroid(pos, vel, outer,inner, rotation, rotateSpeed, sides, size);
}

void Game::FireBullet()
{
	if (m_ship->m_alive){
		if (m_numBullets < MAX_BULLETS){
			m_lastShot = m_gameTime;
			Vector2 nose = m_ship->m_position + (m_ship->m_facing * m_ship->m_drawingRadius.radius);	//get the location of the tip of the ship
			Disc2 radius = Disc2(nose, BULLET_RADIUS);
			RGBA c = RGBA();
			if (m_ship->m_firingStarburst){
				c = RGBA(255,180,0);
			} 
			
			Bullet* b = new Bullet(nose, m_ship->m_facing * BULLET_SPEED,radius,radius, 0.f, 0.f,20,c);		//new bullet
			m_bullets[m_numBullets++] = b;
			g_theAudio->PlaySound(m_laserSoundID);
		} else {
			//error sound or something
		}

		
	}
}

void Game::FireStarburst()
{
	m_ship->FireStarburst();
	m_starburstCharge = -1.f;

}

void Game::BeginStarburst()
{
	if (m_ship->CanFireStarburst()){
		m_ship->ChargeStarburst();
	}
}

