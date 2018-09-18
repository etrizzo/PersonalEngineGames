#pragma once
#include "Game/Asteroid.hpp"
#include "Game/Bullet.hpp"
#include "Game/Ship.hpp"
#include "Engine/Audio/AudioSystem.hpp"



#define MAX_STARS 1500
#define MAX_ASTEROIDS 1000
#define MAX_BULLETS 1000
#define MAX_SHIPS 1

//min and max speed for asteroid velocity and rotation
#define MIN_SPEED 40.f
#define MAX_SPEED 100.f

#define MAX_RADIUS 80.f
#define BULLET_RADIUS 3.f
#define BULLET_SPEED 1000.f

class Star;
class MeshBuilder;

class Game{

public:
	~Game() {}											// destructor: do nothing (for speed)
	Game();											// default constructor: do nothing (for speed)


	//asteroids list
	Asteroid* m_asteroids[MAX_ASTEROIDS];
	int m_numAsteroidsAlive;
	int m_numAsteroidsInWave;
	////bullet list
	Bullet* m_bullets[MAX_BULLETS];
	int m_numBullets;
	////ship list (?)
	////Ship* ships[MAX_SHIPS];
	Ship* m_ship;
	float m_starburstCharge;

	Camera* m_camera;

	std::vector<Star*> m_stars;
	MeshBuilder* m_starBuilder;
	SubMesh* m_starMesh;


	bool m_isPaused;
	bool m_devMode;
	float m_gameTime;
	float m_lastShot;
	float m_lastSpawnedStar = 0.f;

	float m_asteroidRadii[3];

	SoundID m_laserSoundID;
	SoundID m_popSoundID;
	SoundID m_sountrackID;

	AABB2 screenBox = AABB2(SCREEN_MIN, SCREEN_MIN, SCREEN_MAX, SCREEN_MAX);


	void Update(float deltaSeconds);
	void UpdateStars(float deltaSeconds);
	void RenderBackground();
	void AddStars();
	void AddNewStar(const AABB2& starBox, const float& perlinVal);
	void Render();
	void CreateStarMesh();
	void RenderStars();
	void RenderStarNoise();
	RGBA GetPerlin2DColorAtPosition(const float& x, const float& y, int seed = 0);
	RGBA GetPerlin3DColorAtPosition(const float& x, const float& y, int seed = 0, float stride = 5.f);

	void TogglePause();
	void ToggleDevMode();

	void SpawnWave();

	void AddAsteroid(int size = -1, bool placeAtPoint = false, Vector2 & pos = Vector2());
	void BreakAsteroid(int i);
	void DeleteAsteroid(int i);
	void DeleteAsteroid();
	Asteroid* RandomAsteroid(int size);

	void FireBullet();
	void FireStarburst();
	void BeginStarburst();
	void DeleteBullet(int i);


	void KillShip();
	void SpawnShip();

	void TestAsteroid();


	



};