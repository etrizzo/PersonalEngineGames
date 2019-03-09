#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/ParticleEmitter.hpp"


class RenderScene;

class ParticleSystem{
public:
	ParticleSystem();

	void Update(float ds);
	ParticleEmitter* CreateEmitter(Vector3 position);
	ParticleEmitter* CreateEmitterUnderTransform(Transform* t);

	void RemoveEmitter(ParticleEmitter* emitter);

	void PreRenderForCamera(Camera* m_camera);




	std::vector<ParticleEmitter*> m_emitters;
	RenderScene* m_scene = nullptr;
};