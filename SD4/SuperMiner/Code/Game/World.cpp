#include "Game/World.hpp"
#include "Game/Chunk.hpp"
#include "Game/Game.hpp"
#include "Game/Player.hpp"
#include "Game/RaycastResult.hpp"
#include "Game/DebugRenderSystem.hpp"

World::World()
{
	m_chunkMaterial = Material::GetMaterial("block");
	ASSERT_OR_DIE(m_chunkMaterial != nullptr, "No block material loaded in material data");
	float activationRadiusInBlocks = g_gameConfigBlackboard.GetValue("activationRadius", 100.f);
	float deactivationRadiusInBlocks = activationRadiusInBlocks + 25.f;
	m_chunkActivationRadiusChunkDistance = activationRadiusInBlocks / (float) CHUNK_SIZE_X;
	m_chunkDeactivationRadiusChunkDistance = deactivationRadiusInBlocks / (float) CHUNK_SIZE_X;
	SetActivationRadius(m_chunkActivationRadiusChunkDistance);
}

World::~World()
{
	std::vector<Chunk*> chunksToDeactivate = std::vector<Chunk*>();
	for (std::pair<IntVector2, Chunk*> chunkPair : World::m_chunks)
	{
		chunksToDeactivate.push_back(chunkPair.second);
	}

	for (int i = 0; i < (int) chunksToDeactivate.size(); i++)
	{
		DeactivateChunk(chunksToDeactivate[i]);
	}
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
	newChunk->LoadOrGenerateBlocks();							//generate blocks

	//6. Add new chunk to the world's static map OF chunks
	m_chunks.insert(std::pair<IntVector2, Chunk*>(chunkCoords, newChunk));		//add to the world's map of chonks

	//7. link up with neighbors.
	SyncChunkWithNeighbors(newChunk);

	//3. Construct mesh
	//newChunk->CreateMesh();								//create GPU mesh

	//4. Create mesh

	//5. Update chunk state/status. Is chunk missing? Is the VBO built? Is the state activated?

	newChunk->InitializeLighting();
	
}

void World::DeactivateChunk(Chunk* chunkToDeactivate)
{
	// 1. Unlink yourself from your neighbors
			//happens in chunk deconstructor
	// 2. De-register yourself from the world
	m_chunks.erase(chunkToDeactivate->GetChunkCoords());
	// 3. Release/destroy VBO ID on the GPU 
		//(in deconstructor for now)
	// 4. Save chunk to disk
	chunkToDeactivate->SaveToDisk();
	//5. Delete chunk
	delete chunkToDeactivate;
}

bool World::IsChunkActive(const IntVector2 & chunkCoords)
{
	auto foundChunk = m_chunks.find(chunkCoords);
	return foundChunk != m_chunks.end();
}

Chunk * World::GetChunkAtCoordinates(const IntVector2 & chunkCoords) const
{
	auto foundChunk = m_chunks.find(chunkCoords);
	if (foundChunk == m_chunks.end()){
		return nullptr;
	}
	return foundChunk->second;
}

IntVector2 World::GetChunkCoordinatesFromWorldCoordinates(const Vector3 & worldPos) const
{
	float floatX = worldPos.x / (float) CHUNK_SIZE_X;
	float floatY = worldPos.y / (float) CHUNK_SIZE_Y;
	int x = (int) floorf(floatX);
	int y = (int) floorf(floatY);
	/*IntVector3 blockPositionWorld = IntVector3((int) floorf(worldPos.x), (int) floorf(worldPos.y), (int) floorf(worldPos.z));
	int x = blockPositionWorld.x / CHUNK_SIZE_X;
	int y = blockPositionWorld.y / CHUNK_SIZE_Y;*/
	return IntVector2(x,y);
}

BlockLocator World::GetBlockLocatorAtWorldPosition(const Vector3 & worldPos) const
{
	IntVector2 chunkCoords = GetChunkCoordinatesFromWorldCoordinates(worldPos);
	int index = 0;
	Chunk* chunk = GetChunkAtCoordinates(chunkCoords);
	if (chunk != nullptr){
		index = chunk->GetBlockIndexFromWorldPosition(worldPos);
	}
	return BlockLocator(index, chunk);
}

RaycastResult World::Raycast(const Vector3 & start, const Vector3 & forwardNormal, float maxDistance) const
{
	TODO("Update raycast to account for the \"kissing corners\" case by checking displacement from previous block");
	int numSteps = (int) (maxDistance  * (1.f / RAYCAST_STEP_SIZE));
	Vector3 step = (forwardNormal * RAYCAST_STEP_SIZE);
	BlockLocator prevBlock = GetBlockLocatorAtWorldPosition(start);
	BlockLocator nextBlock = prevBlock;

	if (!nextBlock.IsBlockAir())
	{
		//we started in a solid block
		RaycastResult result;
		result.m_ray = Ray3D(start, forwardNormal);
		result.m_maxDistance = maxDistance;
		result.m_impactPosition = start;
		result.m_endPosition = start + (forwardNormal * maxDistance);
		result.m_impactFraction = 0.f;
		result.m_impactDistance = 0.f;
		result.m_impactBlock = BlockLocator(nextBlock.m_blockIndex, nextBlock.m_chunk);
		result.m_impactNormal = -forwardNormal;
		return result;
	}
	Vector3 raycastPosition = start;
	//step down the 
	for (int i = 0; i < numSteps; i++)
	{
		nextBlock = GetBlockLocatorAtWorldPosition(raycastPosition);
		if (nextBlock != prevBlock)
		{
			if(!nextBlock.IsBlockAir())
			{
				//we have a hit, return a raycastresult
				RaycastResult result;
				result.m_ray = Ray3D(start, forwardNormal);
				result.m_maxDistance = maxDistance;
				result.m_impactPosition = raycastPosition;
				result.m_endPosition = start + (forwardNormal * maxDistance);
				result.m_impactFraction = (float) i / (float) numSteps;
				result.m_impactDistance = result.m_impactFraction * maxDistance;
				result.m_impactBlock = BlockLocator(nextBlock.m_blockIndex, nextBlock.m_chunk);
				result.m_impactNormal = (prevBlock.GetBlockCenterWorldPosition() - nextBlock.GetBlockCenterWorldPosition());		//note: need to do world position bc across chunk your normal will get fucked up
				return result;
			} else {
				//advance thy raycast
				prevBlock = nextBlock;
			}
		}
		raycastPosition += step;
	}
	//a miss
	RaycastResult result;
	result.m_ray = Ray3D(start, forwardNormal);
	result.m_maxDistance = maxDistance;
	result.m_endPosition = start + (forwardNormal * maxDistance);
	result.m_impactPosition = result.m_endPosition;
	result.m_impactFraction = 1.f;
	result.m_impactDistance = maxDistance;
	result.m_impactBlock = BlockLocator(nextBlock.m_blockIndex, nextBlock.m_chunk);
	result.m_impactNormal = Vector3::ZERO;
	return result;
}

void World::DebugDeactivateAllChunks()
{
	std::vector<Chunk*> chunksToDeactivate = std::vector<Chunk*>();
	for (std::pair<IntVector2, Chunk*> chunkPair : World::m_chunks)
	{
		chunksToDeactivate.push_back(chunkPair.second);
	}

	for (int i = 0; i < (int) chunksToDeactivate.size(); i++)
	{
		DeactivateChunk(chunksToDeactivate[i]);
	}

	m_chunks.clear();
}

void World::UpdateDebugStuff()
{
	

	RGBA raycastColor = RGBA::RED;
	RGBA pointColor = RGBA::WHITE;

	//draw the basis at the origin
	g_theGame->m_debugRenderSystem->MakeDebugRenderBasis(0.f, Vector3::ZERO, 1.f, Matrix44::IDENTITY, DEBUG_RENDER_HIDDEN);
	g_theGame->m_debugRenderSystem->MakeDebugRenderBasis(0.f, Vector3::ZERO, 1.f, Matrix44::IDENTITY, DEBUG_RENDER_USE_DEPTH);
	
	//draw the basis in front of the player
	Vector3 playerPos = g_theGame->GetPlayer()->GetPosition();
	Vector3 playerForward = g_theGame->GetPlayer()->GetForward();
	Vector3 basis = playerPos + (playerForward * 3.f);
	g_theGame->m_debugRenderSystem->MakeDebugRenderBasis(0.f, basis, .15f, Matrix44::IDENTITY, DEBUG_RENDER_HIDDEN);
	g_theGame->m_debugRenderSystem->MakeDebugRenderBasis(0.f, basis, .15f, Matrix44::IDENTITY, DEBUG_RENDER_USE_DEPTH);



	//draw the debug raycast
	RaycastResult dig = g_theGame->GetPlayer()->m_digRaycast;
	if (g_theGame->IsDevMode()) {
		

		if (dig.DidImpact())
		{
			pointColor = RGBA::GREEN;
			raycastColor = RGBA::GREEN;
		}
		if (g_theGame->IsDevMode()) {
			RGBA xrayColor = RGBA::GRAY;
			RGBA visibleColor = raycastColor;
			g_theGame->m_debugRenderSystem->MakeDebugRenderLineSegment(dig.m_ray.m_position, dig.m_endPosition, xrayColor, xrayColor, 0.f, xrayColor, xrayColor, DEBUG_RENDER_HIDDEN);
			g_theGame->m_debugRenderSystem->MakeDebugRenderLineSegment(dig.m_ray.m_position, dig.m_impactPosition, visibleColor, visibleColor, 0.f, visibleColor, visibleColor, DEBUG_RENDER_USE_DEPTH);
		}

		g_theGame->m_debugRenderSystem->MakeDebugRenderPoint(0.f, dig.m_impactPosition, RGBA::BLACK, RGBA::BLACK, DEBUG_RENDER_HIDDEN);
		g_theGame->m_debugRenderSystem->MakeDebugRenderPoint(0.f, dig.m_impactPosition, pointColor, pointColor, DEBUG_RENDER_USE_DEPTH);
	}

	//draw the face you hit
	if (dig.DidImpact() && dig.m_impactBlock.m_chunk != nullptr && dig.m_impactFraction > 0.f)
	{
		Vector3 blockCenter = dig.m_impactBlock.GetBlockCenterWorldPosition();
		Vector3 halfNormal = dig.m_impactNormal * .5f;
		Vector3 quadUp;
		Vector3 quadRight;
		if (dig.m_impactNormal == UP || dig.m_impactNormal == -UP)
		{
			quadUp = FORWARD;
			quadRight = Cross(FORWARD, dig.m_impactNormal);
		}
		else {
			quadUp = UP;
			quadRight = Cross(UP, dig.m_impactNormal);
		}
		RGBA wireColor = RGBA::BLANCHEDALMOND.GetColorWithAlpha(100);
		RGBA quadColor = RGBA::GREEN.GetColorWithAlpha(100);
		g_theGame->m_debugRenderSystem->MakeDebugRenderQuad(0.f, blockCenter + halfNormal, Vector2::HALF * .95f, quadRight, quadUp, quadColor, quadColor, DEBUG_RENDER_IGNORE_DEPTH);
		g_theGame->m_debugRenderSystem->MakeDebugRenderWireAABB3(0.f, blockCenter, .505f, wireColor, wireColor, DEBUG_RENDER_IGNORE_DEPTH);
	}
	
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
