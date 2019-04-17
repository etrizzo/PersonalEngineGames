#include "Game/World.hpp"
#include "Game/Chunk.hpp"
#include "Game/Game.hpp"
#include "Game/Player.hpp"
#include "Game/RaycastResult.hpp"
#include "Game/DebugRenderSystem.hpp"
#include "Game/GlowParticles.hpp"
#include "Game/GameCamera.hpp"

World::World()
{
	m_chunkOpaqueMaterial = Material::GetMaterial("chunk_opaque");
	m_glowMaterial = Material::GetMaterial("glow_particle");
	ASSERT_OR_DIE(m_chunkOpaqueMaterial != nullptr, "No block material loaded in material data");
	float activationRadiusInBlocks = g_gameConfigBlackboard.GetValue("activationRadius", 100.f);
	float deactivationRadiusInBlocks = activationRadiusInBlocks + 25.f;
	m_chunkActivationRadiusChunkDistance = activationRadiusInBlocks / (float) CHUNK_SIZE_X;
	m_chunkDeactivationRadiusChunkDistance = deactivationRadiusInBlocks / (float) CHUNK_SIZE_X;
	SetActivationRadius(m_chunkActivationRadiusChunkDistance);

	float fogFarPlane = activationRadiusInBlocks - 38.f;
	float fogNearPlane = fogFarPlane * .5f;
	m_fogData;
	m_fogData.SetFogBuffer(RGBA::BEEFEE, fogNearPlane, fogFarPlane, 0.f, 1.f);

	m_worldClock = new Clock(GetMasterClock());
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

	delete m_worldClock;
}

void World::HandleInput()
{
	if (g_theInput->WasKeyJustPressed('T'))
	{
		m_worldClock->SetScale(50.f);
	}
	if (g_theInput->WasKeyJustReleased('T'))
	{
		m_worldClock->SetScale(1.f);
	}
}

void World::Update()
{
	PROFILE_PUSH_FUNCTION_SCOPE();
	UpdateTimeOfDay();
	//we want block placement/digging to happen before chunk management so that we can mark the changed chunk for re-building mesh THAT FRAME (bc it's usually the closest dirty chunk)
	UpdateBlockPlacementAndDigging();
	UpdateChunks();     //nothing for now
	UpdateDirtyLighting();
	ManageChunks();     //activate, deactivate, re-build meshes, etc.

	UpdateDebugStuff();

	
}

void World::Render()
{
	TODO("Establish best render order for chunks here");


	PROFILE_PUSH("RenderChunks");
	RenderChunks();
	PROFILE_POP();

	PROFILE_PUSH("RenderParticles");
	RenderGlowParticles();
	PROFILE_POP();

	if (g_theGame->IsDebugLighting())
	{
		RenderDebugLightingPoints();
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

void World::UpdateTimeOfDay()
{
	PROFILE_PUSH_FUNCTION_SCOPE();
	m_worldTime += m_worldClock->GetDeltaSeconds() * FRAME_TIME_SCALE;
	m_percThroughDay = m_worldTime - (floorf(m_worldTime));
	m_timeOfDay = InAndOutBurger(m_percThroughDay );		//flip the cosine and range map 0-1
	
	/*if (m_outdoorLightScalar > .5f)
	{
		m_outdoorLightScalar = 1.f - m_outdoorLightScalar;
	}*/

	m_lightningPerlinValue = Compute1dPerlinNoise(m_worldClock->GetCurrentSeconds() * .8f, 1.8f, 9, .5f, 2.f, true, 0);
	m_glowPerlinValue = Compute1dPerlinNoise(m_worldClock->GetCurrentSeconds(), 1.f, 1, .5f, 2.f, true, 1);


	//range map and clamp perlin vals
	m_lightningValue = ClampFloat(m_lightningPerlinValue, .6f, .9f);
	m_lightningValue = RangeMapFloat(m_lightningValue, .6f, .9f, 0.f, 1.f);

	m_glowPerlinValue = RangeMapFloat(m_glowPerlinValue, -1.f, 1.f, .8f, 1.f);

	//update the value we're passing to the shader
	m_outdoorLightScalar = max(m_lightningValue * .9f, m_timeOfDay);

	m_skyColor = Interpolate(RGBA::NICEBLACK, RGBA::BEEFEE, m_timeOfDay);
	m_skyColor = Interpolate(m_skyColor, RGBA::WHITE, m_lightningValue);
	m_fogData.SetFogColor(m_skyColor);


	
}

void World::UpdateDebugStuff()
{
	PROFILE_PUSH_FUNCTION_SCOPE();

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
	if (g_theGame->IsDevMode() || g_theGame->GetPlayer()->m_playState->m_gameCamera->GetMode() != CAMERA_MODE_FIRSTPERSON) {
		

		if (dig.DidImpact())
		{
			pointColor = RGBA::GREEN;
			raycastColor = RGBA::GREEN;
		}
		
		RGBA xrayColor = RGBA::GRAY;
		RGBA visibleColor = raycastColor;
		g_theGame->m_debugRenderSystem->MakeDebugRenderLineSegment(dig.m_ray.m_position, dig.m_endPosition, xrayColor, xrayColor, 0.f, xrayColor, xrayColor, DEBUG_RENDER_HIDDEN);
		g_theGame->m_debugRenderSystem->MakeDebugRenderLineSegment(dig.m_ray.m_position, dig.m_impactPosition, visibleColor, visibleColor, 0.f, visibleColor, visibleColor, DEBUG_RENDER_USE_DEPTH);

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
		//RGBA wireColor = RGBA::BLANCHEDALMOND.GetColorWithAlpha(100);
		RGBA quadColor = RGBA::BLANCHEDALMOND.GetColorWithAlpha(32);
		g_theGame->m_debugRenderSystem->MakeDebugRenderQuad(0.f, blockCenter + halfNormal, Vector2::HALF * .95f, quadRight, quadUp, quadColor, quadColor, DEBUG_RENDER_IGNORE_DEPTH);
		//g_theGame->m_debugRenderSystem->MakeDebugRenderWireAABB3(0.f, blockCenter, .505f, wireColor, wireColor, DEBUG_RENDER_IGNORE_DEPTH);
	}

	if (g_theGame->IsDebugLighting())
	{
		//if (g_theInput->WasKeyJustPressed('L') || m_debugLightingPointGPUMesh == nullptr)
		//{
			UpdateDebugLightingPoints();
		//}
	}
}

void World::UpdateDebugLightingPoints()
{
	if (!m_dirtyLightingBlocks.empty())
	{
		m_debugLightingPointCPUMesh.Clear();
		m_debugLightingPointCPUMesh.Begin(PRIMITIVE_TRIANGLES, true);
		for (int i = 0; i < (int) m_dirtyLightingBlocks.size(); i++)
		{
			Vector3 blockPoint = m_dirtyLightingBlocks[i].GetBlockCenterWorldPosition();
			m_debugLightingPointCPUMesh.AppendCube(blockPoint, Vector3::ONE * .05f, RGBA::YELLOW, RIGHT, UP, FORWARD);
		}
		m_debugLightingPointCPUMesh.End();

		delete m_debugLightingPointGPUMesh;
		m_debugLightingPointGPUMesh = m_debugLightingPointCPUMesh.CreateMesh();
	}
}

void World::UpdateBlockPlacementAndDigging()
{
}

void World::UpdateChunks()
{
	PROFILE_PUSH_FUNCTION_SCOPE();
	for (std::pair<IntVector2, Chunk*> chunkPair : World::m_chunks)
	{
		chunkPair.second->Update();
	}
}

void World::UpdateDirtyLighting()
{
	PROFILE_PUSH_FUNCTION_SCOPE();
	if (!g_theGame->IsDebugLighting())
	{
		//m_dirtyLihgtingBlocks is a queue of dirty blocks 
		//(or a "deque", which is kind of like a vector bc 
		//  you can push and pop at both ends really fast)
		while (!m_dirtyLightingBlocks.empty())
		{
			BlockLocator dirtyLightBLock = m_dirtyLightingBlocks.front();
			m_dirtyLightingBlocks.pop_front();

			if (dirtyLightBLock.IsValid())
			{
				//do the dirty lighting calculation on the block locator
				UpdateDirtyBlockLighting(dirtyLightBLock);
				dirtyLightBLock.m_chunk->SetMeshDirty();
			}
		}
	}
	else {
		if (g_theInput->WasKeyJustPressed('L'))
		{
			//do one step of lighting
			std::deque<BlockLocator> lightingToDoThisFrame;
			lightingToDoThisFrame.swap(m_dirtyLightingBlocks);
			while (!lightingToDoThisFrame.empty())
			{
				BlockLocator dirtyBlock = lightingToDoThisFrame.front();
				lightingToDoThisFrame.pop_front();
				UpdateDirtyBlockLighting(dirtyBlock);
				dirtyBlock.m_chunk->SetMeshDirty();
				//this still pushes to m_dirtyLightBlocks, so next time we press L we'll have new blocks to process.
			}
			
		}
	}
}

void World::ManageChunks()
{
	PROFILE_PUSH_FUNCTION_SCOPE();
	TryToActivateChunks();
	TryToBuildChunkMesh();
	TryToDeactivateChunks();
}

void World::RenderChunks()
{
	//set render state for all the chunks - we don't want to bind anything between them
	m_chunkOpaqueMaterial->SetProperty("TIME_OF_DAY", m_outdoorLightScalar);
	m_chunkOpaqueMaterial->SetProperty("indoorGlowValue", m_glowPerlinValue);
	g_theRenderer->BindMaterial(m_chunkOpaqueMaterial);
	g_theRenderer->BindModel(Matrix44::IDENTITY);
	g_theRenderer->BindRendererUniforms();		//binds the camera for this frame
	g_theRenderer->BindFog(m_fogData);

	for (std::pair<IntVector2, Chunk*> chunkPair : World::m_chunks)
	{
		chunkPair.second->Render();
	}
}

void World::RenderGlowParticles()
{
	TODO("Optimize binding render state for glowstone particles, probably");
	m_glowMaterial->SetProperty("TIME_OF_DAY", m_outdoorLightScalar);
	m_glowMaterial->SetProperty("indoorGlowValue", m_glowPerlinValue);
	g_theRenderer->BindMaterial(m_glowMaterial);
	for (std::pair<IntVector2, Chunk*> chunkPair : World::m_chunks)
	{
		if (chunkPair.second->m_isCloseToPlayer)
		{
			chunkPair.second->m_glowSystem->RenderForCamera((Camera*) g_theGame->m_mainCamera);
		}
	}
}

void World::RenderDebugLightingPoints()
{
	if (!m_dirtyLightingBlocks.empty()) {
		g_theRenderer->SetDepth(COMPARE_ALWAYS, false);
		g_theRenderer->ReleaseShader();
		g_theRenderer->ReleaseTexture();
		g_theRenderer->DrawMesh(m_debugLightingPointGPUMesh->m_subMeshes[0]);
	}
}

void World::UpdateDirtyBlockLighting(BlockLocator & block)
{
	// hey, u cute af ;)
	if (block.IsValid())
	{
		bool changedIndoor = UpdateBlockIndoorLighting(block);
		bool changedOutdoor = UpdateBlockOutdoorLighting(block);

		Block& actualBlock = block.GetBlock();

		if (changedIndoor || changedOutdoor)
		{
			//mark your non-opaque neighbors as dirty if anything changed
			BlockLocator east = block.GetEast();
			BlockLocator west = block.GetWest();
			BlockLocator south = block.GetSouth();
			BlockLocator north = block.GetNorth();
			BlockLocator up = block.GetUp();
			BlockLocator down = block.GetDown();

			if (!east.IsBlockFullyOpaque()) { SetBlockLightDirty(east); }
			if (!west.IsBlockFullyOpaque()) { SetBlockLightDirty(west); }
			if (!south.IsBlockFullyOpaque()) { SetBlockLightDirty(south); }
			if (!north.IsBlockFullyOpaque()) { SetBlockLightDirty(north); }
			if (!up.IsBlockFullyOpaque()) { SetBlockLightDirty(up); }
			if (!down.IsBlockFullyOpaque()) { SetBlockLightDirty(down); }
		}
		actualBlock.ClearLightDirty();
	}
}

bool World::UpdateBlockIndoorLighting(BlockLocator & block)
{
	//get the correct value, compare with old value.
	uchar oldVal = (uchar) block.GetBlock().GetIndoorLightLevel();
	uchar correctVal = GetCorrectIndoorLightLevel(block);

	Block& actualBlock = block.GetBlock();

	if (oldVal != correctVal)
	{
		actualBlock.SetIndoorLighting(correctVal);
		return true;
	}
	return false;
}

bool World::UpdateBlockOutdoorLighting(BlockLocator & block)
{
	//get the correct value, compare with old value.
	uchar oldVal = (uchar) block.GetBlock().GetOutdoorLightLevel();
	uchar correctVal = GetCorrectOutdoorLightLevel(block);

	Block& actualBlock = block.GetBlock();

	if (oldVal != correctVal)
	{
		actualBlock.SetOutdoorLighting(correctVal);
		return true;
	}
	return false;
}

uchar World::GetCorrectIndoorLightLevel(const BlockLocator & block)
{
	/*
	- My Light level is the MAXIMUM of:
		+ My own block definition's glow value (13 for glowstone, 0 or stone, etc.)
		+ IF and only if i am not fully opaque: my brightest neighbor's light value - 1
	*/
	Block& actualBlock = block.GetBlock();
	uchar definitionGlow = actualBlock.GetDefinitionInternalLightLevel();
	uchar neighborGlow = 0U;
	BlockLocator east = block.GetEast();
	BlockLocator west = block.GetWest();
	BlockLocator south = block.GetSouth();
	BlockLocator north = block.GetNorth();
	BlockLocator up = block.GetUp();
	BlockLocator down = block.GetDown();

	if (!actualBlock.IsFullyOpaque()) 
	{
		//set neighborGlow to be maxNeighbor - 1
		//find max neighbor
		uchar maxNeighborGlow = 0U;

		uchar eastLevel		= east.GetIndoorLightLevel();
		uchar westLevel		= west.GetIndoorLightLevel();
		uchar southLevel	= south.GetIndoorLightLevel();
		uchar northLevel	= north.GetIndoorLightLevel();
		uchar upLevel		= up.GetIndoorLightLevel();
		uchar downLevel		= down.GetIndoorLightLevel();

		//set maxNeihgborGlow to max
		if (eastLevel	> maxNeighborGlow) { maxNeighborGlow = eastLevel; }
		if (westLevel	> maxNeighborGlow) { maxNeighborGlow = westLevel; }
		if (southLevel	> maxNeighborGlow) { maxNeighborGlow = southLevel; }
		if (northLevel	> maxNeighborGlow) { maxNeighborGlow = northLevel; }
		if (upLevel		> maxNeighborGlow) { maxNeighborGlow = upLevel; }
		if (downLevel	> maxNeighborGlow) { maxNeighborGlow = downLevel; }

		if (maxNeighborGlow > 0) {
			neighborGlow = maxNeighborGlow - 1U;
		}
	}

	return (uchar) Max(neighborGlow, definitionGlow);
}

uchar World::GetCorrectOutdoorLightLevel(const BlockLocator & block)
{
	/*
	- My outdoor Light level is:
		+ 15 if i'm sky
		+ max of (0, maxNeighbor - 1)
	*/
	Block& actualBlock = block.GetBlock();
	if (actualBlock.IsSky())
	{
		return MAX_OUTDOOR_LIGHT;
	}
	uchar neighborGlow = 0U;
	BlockLocator east = block.GetEast();
	BlockLocator west = block.GetWest();
	BlockLocator south = block.GetSouth();
	BlockLocator north = block.GetNorth();
	BlockLocator up = block.GetUp();
	BlockLocator down = block.GetDown();

	if (!actualBlock.IsFullyOpaque())
	{
		//set neighborGlow to be maxNeighbor - 1
		//find max neighbor
		uchar maxNeighborGlow = 0U;

		uchar eastLevel = east.GetOutdoorLightLevel();
		uchar westLevel = west.GetOutdoorLightLevel();
		uchar southLevel = south.GetOutdoorLightLevel();
		uchar northLevel = north.GetOutdoorLightLevel();
		uchar upLevel = up.GetOutdoorLightLevel();
		uchar downLevel = down.GetOutdoorLightLevel();

		//set maxNeihgborGlow to max
		if (eastLevel > maxNeighborGlow) { maxNeighborGlow = eastLevel; }
		if (westLevel > maxNeighborGlow) { maxNeighborGlow = westLevel; }
		if (southLevel > maxNeighborGlow) { maxNeighborGlow = southLevel; }
		if (northLevel > maxNeighborGlow) { maxNeighborGlow = northLevel; }
		if (upLevel > maxNeighborGlow) { maxNeighborGlow = upLevel; }
		if (downLevel > maxNeighborGlow) { maxNeighborGlow = downLevel; }

		if (maxNeighborGlow > 0) {
			neighborGlow = maxNeighborGlow - 1U;
		}
	}

	return max(neighborGlow, 0);
}

void World::SetBlockLightDirty(BlockLocator & block)
{
	Block& actualBlock = block.GetBlock();
	//only want to check lighting on blocks that accept lighting (aren't opaque) and aren't already in the dirty list
	if ( !actualBlock.IsLightDirty())
	{
		actualBlock.SetLightDirty();
		m_dirtyLightingBlocks.push_back(block);
	}
}

void World::TryToActivateChunks()
{
	PROFILE_PUSH_FUNCTION_SCOPE();
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
	PROFILE_PUSH_FUNCTION_SCOPE();
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
	PROFILE_PUSH_FUNCTION_SCOPE();
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
