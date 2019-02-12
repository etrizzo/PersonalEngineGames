#include "Game/World.hpp"
#include "Game/Chunk.hpp"

World::World()
{
	m_chunkMaterial = Material::GetMaterial("block");
	ASSERT_OR_DIE(m_chunkMaterial != nullptr, "No block material loaded in material data");
}

void World::Update()
{
}

void World::Render()
{
	TODO("Establish best render order for chunks here");

	//set render state for all the chunks - we don't want to bind anything between them


	g_theRenderer->BindMaterial(m_chunkMaterial);
	g_theRenderer->BindModel(Matrix44::IDENTITY);
	g_theRenderer->BindRendererUniforms();		//binds the camera for this frame

	for (std::pair<IntVector2, Chunk*> chunkPair : World::s_chunks)
	{
		chunkPair.second->Render();
	}
}

void World::ActivateChunk(const IntVector2& chunkCoords)
{
	Chunk* newChunk = new Chunk(chunkCoords);			//create
	newChunk->GenerateBlocks();							//generate blocks
	newChunk->CreateMesh();								//create GPU mesh

	s_chunks.insert(std::pair<IntVector2, Chunk*>(chunkCoords, newChunk));		//add to the world's map of chonks
}
