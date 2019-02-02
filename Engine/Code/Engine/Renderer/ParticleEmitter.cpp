#pragma once

#include "ParticleEmitter.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Renderer/Camera.hpp"

ParticleEmitter::ParticleEmitter(Vector3 position)
{
	m_transform		= Transform();
	m_transform.SetLocalPosition(position);

	m_renderable	= new Renderable();		//will update from camera every frame
	m_renderable->SetMaterial(Material::GetMaterial("particle"));

	m_mesh = new SubMesh();
	m_builder =  new MeshBuilder();

	m_particles = std::vector<particle_t*> ();
	m_renderable->m_mesh->SetSubMesh(m_mesh, 0);
	

	m_spawnRate = .1f;
	m_spawnInterval = StopWatch();
	m_spawnOverTime = true;


	//want to make default functions for these in case they aren't specified
	//e.x., by default particle emitters will spawn particles with lifetime of 1
	m_spawnVelocityCB = DefaultSpawnVelocity;
	m_spawnColorCB	  = DefaultSpawnColor;
	m_colorCB		  = DefaultColor;
	m_lifetimeCB	  = DefaultLifetime;
	m_spawnSizeCB	  = DefaultSpawnSize;
	m_killWhenDone	  = false;
}

ParticleEmitter::~ParticleEmitter()
{
	delete m_builder;
}

void ParticleEmitter::Update(float ds)
{
	unsigned int particleCount = (unsigned int)m_particles.size();
	float t = GetMasterClock()->GetCurrentSeconds();
	if (m_spawnOverTime && m_spawnInterval.Decrement()){
		SpawnParticle();
	}
	for (int i = particleCount-1; i >= 0; i--){
		particle_t* p = m_particles[i];
		p->m_force = Vector3::ZERO;
		p->Update(ds);
		//p->SetTint(m_colorCB(p->GetNormalizedAge(t)));

		if (p->IsDead(t)){
			RemoveAtFast(m_particles, i);
			delete p;
		}
	}

	
}

void ParticleEmitter::CameraPreRender(Camera * cam)
{
	unsigned int particleCount = (unsigned int) m_particles.size();

	//m_builder->SetUpForVertex<Vertex3D_PCU>();
	Vector3 right = cam->GetRight();
	Vector3 up = cam->GetUp();
	m_builder->Clear();
	m_builder->Begin(PRIMITIVE_TRIANGLES, true);
	//Debug:
	//m_builder->AppendCube(cam->GetPosition(), Vector3::ONE, RGBA::CYAN);
	//m_builder->AppendCube(m_transform.GetWorldPosition(), Vector3::ONE, RGBA::MAGENTA);
	float t = GetMasterClock()->GetCurrentSeconds();
	for (unsigned int i = 0; i < particleCount; i++){
		particle_t* p = m_particles[i];
		RGBA colorOverTime = m_colorCB(p->GetNormalizedAge(t));
		RGBA color = Interpolate(p->m_tint, colorOverTime, .5f);
		//color = p->m_tint;
		m_builder->AppendPlane(p->m_position, up, right, Vector2::ONE * p->m_size, color, Vector2::ZERO, Vector2::ONE);
		//m_builder->AppendCube(p->m_position, Vector3::ONE * .1f, p->m_tint);
	}
	m_builder->End();
	//remakes the mesh every frame. eventually, we might keep this on the GPU
	m_mesh->FromBuilder(m_builder, VERTEX_TYPE_3DPCU);
	//m_mesh = m_builder->CreateMesh(VERTEX_TYPE_3DPCU);
	
}

void ParticleEmitter::SpawnParticle()
{
	// a lot of modules will control these default values.
	// this is how the particle will spawn
	// We can determine these through callbacks!!
	particle_t* p = new particle_t();
	p->m_position = m_transform.GetWorldPosition();
	p->m_velocity = m_spawnVelocityCB(&m_transform);
	p->m_tint = m_spawnColorCB();
	p->m_force = Vector3::ZERO;
	p->m_mass = 1.f;
	p->m_timeBorn = GetMasterClock()->GetCurrentSeconds();
	p->m_timeDead = p->m_timeBorn + m_lifetimeCB();
	p->m_size = m_spawnSizeCB();
	

	m_particles.push_back(p);
}

void ParticleEmitter::SpawnParticles(unsigned int count)
{
	for (unsigned int i = 0; i < count; i++){
		SpawnParticle();
	}
}

void ParticleEmitter::SpawnBurst(int numToSpawn)
{
	for (int i = 0; i < numToSpawn; i++){
		SpawnParticle();
	}
}

void ParticleEmitter::SetSpawnRate(float particlesPerSecond)
{
	if (particlesPerSecond == 0.f){
		m_spawnOverTime = false;
	} else {
		m_spawnOverTime = true;
		m_spawnInterval.SetTimer(1.f / particlesPerSecond);
	}
}

bool ParticleEmitter::IsDead () const
{
	return( m_killWhenDone && !m_spawnOverTime && m_particles.size() == 0);
}

Vector3 DefaultSpawnVelocity(Transform* t)
{
	UNUSED(t);
	float theta = GetRandomFloatInRange(0.f, 360.f);
	float azimuth = GetRandomFloatInRange(0.f, 90.f);
	Vector3 vel = SphericalToCartesian(1.f, theta, azimuth);
	return (vel + Vector3::Y_AXIS) * 2.f; 
}

RGBA DefaultSpawnColor()
{
	return RGBA::WHITE;
}

RGBA DefaultColor(float t)
{
	return RGBA::WHITE.GetColorWithAlpha((unsigned char) (255.f * (1.f - t)));
}

float DefaultLifetime()
{
	return GetRandomFloatInRange(.7f,.8f);
}

float DefaultSpawnSize()
{
	return .05f;
}
