#pragma once
#include "Engine/Renderer/Particle.hpp"
#include "Engine/Renderer/Renderable.hpp"

class Mesh;


/*
How we might use this?
To make a particle emitter when a bullet hits an asteroid:

PraticleEmitter* pe = CreateEmitter();		///on game
pe->SetBurstAmount(60,100);		//a range of initial burst
pe->SetSpawnVelocity( []() { return RandomRange(1.f, 5.f) * RandomPointOnSphere(); } );		//This is a lambda? Basically passing the function by reference kind of???? black magic???
pe->SetColorOverTime( [](float nt ) { return Lerp(RGBA::WHITE, RGBA::RED, nt); } );
	//alternative would be a static function GetColorOverTime(float) and then doing :
	// pe->SetColorOverTime(GetColorOverTime);
pe->SetLifetime([]() { return RandomRange(1.f,2.f) * t; } );
pe->KillWhenDOne();		//automaticallly destroy this when all particles have left
pe->m_transform.SetWorldPosition(bulletPosition);

//bullet damage/destruction

*/




typedef Vector3 (*SpawnVelocityCB) (Transform* t);
typedef Vector3 (*SpawnPositionOffsetCB) ();
typedef Vector3 (*SpawnForceCB)	();
typedef RGBA (*ColorOverTimeCB) (float t, const RGBA& currentColor);
typedef RGBA (*SpawnColorCB) ();
typedef float (*SpawnSizeCB) ();
typedef float (*LifetimeCB) ();


Vector3 SphericalSpawnLocation();
inline Vector3 DefaultSpawnLocation() { return Vector3::ZERO; };
Vector3 DefaultSpawnVelocity(Transform* t);
inline Vector3 NoSpawnVelocity(Transform* t) { UNUSED(t); return Vector3::ZERO; };
RGBA DefaultSpawnColor();

//color over time functions
RGBA DefaultColorOverTime(float t, const RGBA& current);
RGBA FadeOut(float t, const RGBA& current);
RGBA FadeInAndOut(float t, const RGBA& current);

float DefaultLifetime();
float DefaultSpawnSize();

inline Vector3 NoSpawnForce() {return Vector3::ZERO; };
inline Vector3 GravitySpawnForceZUp() { return Vector3::Z_AXIS * 9.8f; };


//for now, we might want a vector of emitters in Game that get updated there and passed to the renderpath
class ParticleEmitter{
public:
	ParticleEmitter(Vector3 position);
	~ParticleEmitter();

	void Update(float ds);
	void CameraPreRender(Camera* cam);		// want to pre-render PER CAMERA, i.e. for split-screen.
	//but, then we need a list of pre-render callbacks in our render scene that get registered with the scene
	void SpawnParticle();
	void SpawnParticles(unsigned int count);
	void SpawnBurst( int numToSpawn );

	void SetSpawnRate( float particlesPerSecond );

	bool IsDead() const;		//or "is safe to destroy"

	inline void SetSpawnVelocity(SpawnVelocityCB cb)				{ m_spawnVelocityCB = cb; } ;
	inline void SetSpawnColor(SpawnColorCB cb)						{ m_spawnColorCB = cb; };
	inline void SetColorOverTime(ColorOverTimeCB cb)				{ m_colorCB = cb; } ;
	inline void SetLifetime(LifetimeCB cb)							{ m_lifetimeCB = cb; } ;
	inline void SetSpawnSize(SpawnSizeCB cb)						{ m_spawnSizeCB = cb; } ;
	inline void SetSpawnPositionOffset(SpawnPositionOffsetCB cb)	{ m_spawnPositionCB = cb; };
	inline void SetSpawnForce(SpawnForceCB cb)						{ m_spawnForceCB = cb; };
	void KillWhenDone(bool val = true)			{ m_killWhenDone = val; } ;

public:
	Transform m_transform;
	Renderable* m_renderable;		//will update from camera every frame

	SubMesh* m_mesh;
	MeshBuilder* m_builder;

	std::vector<particle_t*> m_particles;

	float m_spawnRate;
	StopWatch m_spawnInterval;
	bool m_spawnOverTime;


	//want to make default functions for these in case they aren't specified
	//e.x., by default particle emitters will spawn particles with lifetime of 1
	SpawnVelocityCB			m_spawnVelocityCB;
	SpawnColorCB			m_spawnColorCB;
	ColorOverTimeCB			m_colorCB;
	SpawnSizeCB				m_spawnSizeCB;
	LifetimeCB				m_lifetimeCB;
	SpawnPositionOffsetCB	m_spawnPositionCB;
	SpawnForceCB			m_spawnForceCB;
	bool					m_killWhenDone;

};