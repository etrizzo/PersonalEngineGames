#include "Game/World.hpp"
#include "Game/Chunk.hpp"
#include "Game/Game.hpp"
#include "Game/Player.hpp"

World::World()
{
	m_chunkMaterial = Material::GetMaterial("block");
	ASSERT_OR_DIE(m_chunkMaterial != nullptr, "No block material loaded in material data");
	float activationRadiusInBlocks = g_gameConfigBlackboard.GetValue("activationRadius", 100.f);
	float deactivationRadiusInBlocks = g_gameConfigBlackboard.GetValue("deactivationRadius", 120.f);
	m_chunkActivationRadiusChunkDistance = activationRadiusInBlocks / (float) CHUNK_SIZE_X;
	m_chunkDeactivationRadiusChunkDistance = deactivationRadiusInBlocks / (float) CHUNK_SIZE_X;
	SetActivationRadius(m_chunkActivationRadiusChunkDistance);
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
	m_chunkActivationRadiusChunkDistance = newRadius;
	SetChunkActivationCheatSheet();
}

void World::ActivateChunk(const IntVector2& chunkCoords)
{
	//1. New Chunk
	Chunk* newChunk = new Chunk(chunkCoords);			//create
	//2. Generate or load
	TODO("Load chunks from disk");
	newChunk->GenerateBlocks();							//generate blocks

	//6. Add new chunk to the world's static map OF chunks
	m_chunks.insert(std::pair<IntVector2, Chunk*>(chunkCoords, newChunk));		//add to the world's map of chonks

	//7. link up with neighbors.
	SyncChunkWithNeighbors(newChunk);

	//3. Construct mesh
	//newChunk->CreateMesh();								//create GPU mesh

	//4. Create mesh

	//5. Update chunk state/status. Is chunk missing? Is the VBO built? Is the state activated?

	
	
}

void World::DeactivateChunk(Chunk* chunkToDeactivate)
{
	// 1. Unlink yourself from your neighbors
	TODO("Unlink from your neighbors");
	// 2. De-register yourself from the world
	m_chunks.erase(chunkToDeactivate->GetChunkCoords());
	// 3. Release/destroy VBO ID on the GPU (in deconstructor for now)
	// 4. Save chunk to disk
	TODO("Save chunks to disk if changed");
	//5. Delete chunk
	delete chunkToDeactivate;
}

bool World::IsChunkActive(const IntVector2 & chunkCoords)
{
	auto foundChunk = m_chunks.find(chunkCoords);
	return foundChunk != m_chunks.end();
}

Chunk * World::GetChunkAtCoordinates(const IntVector2 & chunkCoords)
{
	auto foundChunk = m_chunks.find(chunkCoords);
	if (foundChunk == m_chunks.end()){
		return nullptr;
	}
	return foundChunk->second;
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
	TryToActivateChunks();
	TryToBuildChunkMesh();
	TryToDeactivateChunks();
}

void World::TryToActivateChunks()
{
	IntVector2 playerChunk = g_theGame->GetPlayer()->GetCurrentChunkCoordinates();
	for (int i = 0; i < (int) m_chunkActivationOffsetsSortedByDistance.size(); i++)
	{
		IntVector2 nextOffset = m_chunkActivationOffsetsSortedByDistance[i];
		if (!IsChunkActive(playerChunk + nextOffset)){
			ActivateChunk(playerChunk + nextOffset);
			return;
		}
	}
}

void World::TryToDeactivateChunks()
{
	float furthestDistance = -1.f;
	Chunk* chunkToDeactivate = nullptr;
	float deactivateRadiusSquared = m_chunkDeactivationRadiusChunkDistance * m_chunkDeactivationRadiusChunkDistance;

	for (std::pair<IntVector2, Chunk*> chunkPair : m_chunks){
		float distSquaredFromPlayer = GetChunkDistanceFromPlayerSquared(chunkPair.second);
		if (distSquaredFromPlayer > deactivateRadiusSquared){
			if (furthestDistance < 0.f){
				//this is the first one out of the radius we've found
				furthestDistance = distSquaredFromPlayer;
				chunkToDeactivate = chunkPair.second;
			} else {
				if (furthestDistance < distSquaredFromPlayer){
					//we've found a further one
					furthestDistance = distSquaredFromPlayer;
					chunkToDeactivate = chunkPair.second;
				}
			}
		}
	}

	if (chunkToDeactivate != nullptr){
		DeactivateChunk(chunkToDeactivate);
	}
}

void World::TryToBuildChunkMesh()
{
	IntVector2 playerChunk = g_theGame->GetPlayer()->GetCurrentChunkCoordinates();
	for (int i = 0; i < (int) m_chunkActivationOffsetsSortedByDistance.size(); i++)
	{
		IntVector2 nextOffset = m_chunkActivationOffsetsSortedByDistance[i];
		Chunk* chunk = GetChunkAtCoordinates(playerChunk + nextOffset);
		if (chunk != nullptr && chunk->ShouldChunkRebuildMesh()){
			//the chunk is active but has no mesh
			chunk->CreateMesh();
			return;
		}
	}
}

void World::SyncChunkWithNeighbors(Chunk * chunk)
{
	IntVector2 chunkCoords = chunk->GetChunkCoords();
	Chunk* east = GetChunkAtCoordinates(chunkCoords + EAST);
	chunk->m_eastNeighbor = east;
	if (east != nullptr){
		east->m_westNeighbor = chunk;
	}

	Chunk* west = GetChunkAtCoordinates(chunkCoords - EAST);
	chunk->m_westNeighbor = west;
	if (west != nullptr){
		west->m_eastNeighbor = chunk;
	}

	Chunk* north = GetChunkAtCoordinates(chunkCoords + NORTH);
	chunk->m_northNeighbor = north;
	if (north != nullptr){
		north->m_southNeighbor = chunk;
	}

	Chunk* south = GetChunkAtCoordinates(chunkCoords - NORTH);
	chunk->m_southNeighbor = south;
	if (south != nullptr)
	{
		south->m_northNeighbor = chunk;
	}

}

float World::GetChunkDistanceFromPlayerSquared(Chunk * chunk) const
{
	IntVector2 chunkCoords = chunk->GetChunkCoords();
	IntVector2 playerCoords = g_theGame->GetPlayer()->GetCurrentChunkCoordinates();
	return (chunkCoords - playerCoords).GetLengthSquared();
}

void World::SetChunkActivationCheatSheet()
{
	m_chunkActivationOffsetsSortedByDistance = std::vector<IntVector2>();
	int radius = (int) m_chunkActivationRadiusChunkDistance;
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
