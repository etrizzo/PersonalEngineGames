#pragma once
#include "Game/GameCommon.hpp"

class Chunk;

class World
{
public:
	World();


	void Update();	//nothing right now probs
	void Render();

	void ActivateChunk(const IntVector2& chunkCoords);

	Material* m_chunkMaterial;		//for convenience

	std::map<IntVector2, Chunk*> s_chunks;
};