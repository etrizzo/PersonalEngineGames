#pragma once
#include "ParticleSystem.hpp"
#include "Engine/Renderer/RenderScene.hpp"


ParticleSystem::ParticleSystem()
{
	m_emitters = std::vector<ParticleEmitter*>();
}

void ParticleSystem::Update(float ds)
{
	for (ParticleEmitter* emitter : m_emitters){
		emitter->Update(ds);
	}

	for (int i = (int) m_emitters.size() -1; i >=0; i--){
		
		if (m_emitters[i]->IsDead()){
			ParticleEmitter* e = m_emitters[i];
			RemoveAtFast(m_emitters, i);
			m_scene->RemoveRenderable(e->m_renderable);
			delete e;
		}
	}
}

ParticleEmitter*  ParticleSystem::CreateEmitter(Vector3 position)
{
	ParticleEmitter* e = new ParticleEmitter(position);
	m_emitters.push_back(e);
	if (m_scene != nullptr){
		m_scene->AddRenderable(e->m_renderable);
	}
	return e;
}

ParticleEmitter* ParticleSystem::CreateEmitterUnderTransform(Transform * t)
{
	ParticleEmitter* emitter = new ParticleEmitter(Vector3::ZERO);
	emitter->m_transform.SetParent(t);
	m_emitters.push_back(emitter);
	if (m_scene != nullptr){
		m_scene->AddRenderable(emitter->m_renderable);
	}
	return emitter;
}

void ParticleSystem::PreRenderForCamera(Camera *camera)
{
	for (ParticleEmitter* em : m_emitters){
		em->CameraPreRender(camera);
	}
}
