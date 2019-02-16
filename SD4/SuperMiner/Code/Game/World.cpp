#include "Game/World.hpp"
#include "Game/Chunk.hpp"
#include "Game/Game.hpp"
#include "Game/Player.hpp"

World::World()
{
	m_chunkMaterial = Material::GetMaterial("block");
	ASSERT_OR_DIE(m_chunkMaterial != nullptr, "No block material loaded in material data");
	SetActivationRadius(g_gameConfigBlackboard.GetValue("activationRadius", m_chunkActivationRadius));
}

void World::Update()
{
	UpdateDebugStuff();
	//we want block placement/digging to happen before chunk management so that we can mark the changed chunk for re-building mesh THAT FRAME (bc it's usually the closest dirty chunk)
	UpdateBlockPlacementAndDigging();
	UpdateChunks();     //nothing for now
	ManageChunks();     //activate, deactivate, re-build meshes, etc.

	for (std::pair<IntVector2, Chunk*> chunkPair : World::m_chunks)
	{
		chunkPair.second->Update();
	}
}

void World::Render()
{
	TODO("Establish best render order for chunks here");

	//set render state for all the chunks - we don't want to bind anything between them
	g_theRenderer->BindMaterial(m_chunkMaterial);
	g_theRenderer->BindModel(Matrix44::IDENTITY);
	g_theRenderer->BindRendererUniforms();		//binds the camera for this frame

	for (std::pair<IntVector2, Chunk*> chunkPair : World::m_chunks)
	{
		chunkPair.second->Render();
	}
}

void World::SetActivationRadius(float newRadius)
{
	m_chunkActivationRadius = newRadius;
	SetChunkActivationCheatSheet();
}

void World::ActivateChunk(const IntVector2& chunkCoords)
{
	Chunk* newChunk = new Chunk(chunkCoords);			//create
	newChunk->GenerateBlocks();							//generate blocks
	newChunk->CreateMesh();								//create GPU mesh

	m_chunks.insert(std::pair<IntVector2, Chunk*>(chunkCoords, newChunk));		//add to the world's map of chonks
}

void World::DeactivateChunk(const IntVector2 & chunkCoords)
{
}

bool World::IsChunkActive(const IntVector2 & chunkCoords)
{
	auto foundChunk = m_chunks.find(chunkCoords);
	return foundChunk != m_chunks.end();
}

IntVector2 World::GetChunkCoordinatesFromWorldCoordinates(const Vector3 & worldPos) const
{
	IntVector3 blockPositionWorld = IntVector3((int) floorf(worldPos.x), (int) floorf(worldPos.y), (int) floorf(worldPos.z));
	int x = blockPositionWorld.x / CHUNK_SIZE_X;
	int y = blockPositionWorld.y / CHUNK_SIZE_Y;
	return IntVector2(x,y);
}

void World::UpdateDebugStuff()
{
}

void World::UpdateBlockPlacementAndDigging()
{
}

void World::UpdateChunks()
{
	for (std::pair<IntVector2, Chunk*> chunkPair : World::m_chunks)
	{
		chunkPair.second->Update();
	}
}

void World::ManageChunks()
{
	IntVector2 playerChunk = g_theGame->GetPlayer()->GetCurrentChunkCoordinates();
	for (int i = 0; i < m_chunkActivationOffsetsSortedByDistance.size(); i++)
	{
		IntVector2 nextOffset = m_chunkActivationOffsetsSortedByDistance[i];
		if (!IsChunkActive(playerChunk + nextOffset)){
			ActivateChunk(playerChunk + nextOffset);
			return;
		}
	}
}

void World::TryToActivateChunks()
{
}

void World::TryToDeactivateChunks()
{
}

void World::SetChunkActivationCheatSheet()
{
	m_chunkActivationOffsetsSortedByDistance = std::vector<IntVector2>();
	int radius = (int) m_chunkActivationRadius;
	int radiusSquared = radius * radius;
	IntVector2 origin = IntVector2(0,0);
	for (int x = -radius; x < radius; x++)
	{
		for (int y = -radius; y < radius; y++)
		{
			IntVector2 chunkCoords = IntVector2(x,y);
			if(chunkCoords.GetLengthSquared() < radiusSquared)
			{
				//it is in the activation circle
				m_chunkActivationOffsetsSortedByDistance.push_back(chunkCoords);
			}
		}
	}

	//sort by offset from origin
	std::sort(m_chunkActivationOffsetsSortedByDistance.begin(), m_chunkActivationOffsetsSortedByDistance.end(), CompareIntVector2ByDistanceToOrigin);
}

bool CompareIntVector2ByDistanceToOrigin(const IntVector2 & a, const IntVector2 & b)
{
	float aDistance = a.GetLengthSquared();
	float bDistance = b.GetLengthSquared();
	return aDistance < bDistance;
}
