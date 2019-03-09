#include "GlowParticles.hpp"
#include "Game/Chunk.hpp"


GlowEmitter::GlowEmitter(ParticleEmitter * emitter, int blockIndex)
{
	m_emitter = emitter;
	m_blockIndex = blockIndex;
}

GlowEmitter::~GlowEmitter()
{
	delete m_emitter;
}

GlowParticles::GlowParticles(Chunk * chunk)
{
	m_chunk = chunk;
	m_system = new ParticleSystem();
	
}

void GlowParticles::Update()
{
	m_system->Update(GetMasterClock()->GetDeltaSeconds());
}

void GlowParticles::AddEmitterAtBlockIndex(int blockIndex)
{
	Vector3 position = m_chunk->GetCenterPointForBlockInWorldCoordinates(blockIndex);
	ParticleEmitter* emitter = m_system->CreateEmitter(position);
	emitter->SetSpawnRate(4.f);
	emitter->SetSpawnPositionOffset(SphericalSpawnLocation);
	emitter->SetSpawnVelocity(GlowSpawnVelocity);
	emitter->SetSpawnColor(SpawnGlowColor);
	emitter->SetColorOverTime(FadeInAndOut);
	emitter->SetLifetime(GlowParticleLifetime);
	emitter->SetSpawnSize(GlowParticleSize);
	m_emitters.push_back(new GlowEmitter(emitter, blockIndex));
}

void GlowParticles::RemoveEmitterAtBlockIndex(int blockIndex)
{
	for (int i = m_emitters.size() - 1; i >= 0; i--)
	{
		if (m_emitters[i]->m_blockIndex == blockIndex)
		{
			GlowEmitter* emitter = m_emitters[i];
			RemoveAtFast(m_emitters, i);
			m_system->RemoveEmitter(m_emitters[i]->m_emitter);
			delete emitter;
		}
	}
}

void GlowParticles::RenderForCamera(Camera * camera)
{
	m_system->PreRenderForCamera(camera);
	
	//draw each emitter
	//g_theRenderer->BindMaterial(m_glowMaterial);
	for (GlowEmitter* emitter : m_emitters)
	{
		g_theRenderer->BindModel(emitter->m_emitter->m_renderable->m_transform.GetWorldMatrix());
		g_theRenderer->BindStateAndDrawMesh(emitter->m_emitter->m_renderable->m_mesh->m_subMeshes[0]);
	}
	
}

RGBA SpawnGlowColor()
{
	return RGBA::GetRandomMixedColor(RGBA(255,240,200,0), .9f);
}

float GlowParticleLifetime()
{
	return GetRandomFloatInRange(3.f, 5.f);
}

float GlowParticleSize()
{
	return GetRandomFloatInRange(.02f, .05f);
}

Vector3 GlowSpawnVelocity(Transform * t)
{
	UNUSED(t);
	float x = GetRandomFloatInRange(0.001f, .01f);
	float y = GetRandomFloatInRange(0.001f, .01f);
	float z = GetRandomFloatInRange(0.001f, .01f);
	return Vector3(x, y, z);
}
