#include "Game/GameCommon.hpp"
#include "Engine/Renderer/ParticleSystem.hpp"

class Chunk;


RGBA SpawnGlowColor();
float GlowParticleLifetime();
float GlowParticleSize();
Vector3 GlowSpawnVelocity(Transform* t);

struct GlowEmitter
{
public:
	GlowEmitter(ParticleEmitter* emitter, int blockIndex);
	~GlowEmitter();
	ParticleEmitter* m_emitter;
	int m_blockIndex;
};

class GlowParticles
{
public:
	GlowParticles(Chunk* chunk);

	void Update();

	void AddEmitterAtBlockIndex(int blockIndex);
	void RemoveEmitterAtBlockIndex(int blockIndex);

	void RenderForCamera(Camera* camera);

	ParticleSystem* m_system = nullptr;
	Chunk* m_chunk = nullptr;
	
	std::vector<GlowEmitter*> m_emitters = std::vector<GlowEmitter*>();

};
