#pragma once
#include "Game/GameCommon.hpp"
#include "Game/RaycastResult.hpp"

class Chunk;

class World
{
public:
	World();
	~World();


	void Update();	//nothing right now probs
	void Render();

	void SetActivationRadius(float newRadius);
	inline float GetActivationRadius() const { return m_chunkActivationRadiusChunkDistance; }
	

	void ActivateChunk(const IntVector2& chunkCoords);
	void DeactivateChunk(Chunk* chunkToDeactivate);

	bool IsChunkActive(const IntVector2& chunkCoords);

	Chunk* GetChunkAtCoordinates(const IntVector2& chunkCoords) const;

	IntVector2 GetChunkCoordinatesFromWorldCoordinates(const Vector3& worldPos) const;

	BlockLocator GetBlockLocatorAtWorldPosition(const Vector3& worldPos) const;

	void SetBlockLightDirty(BlockLocator& block);

	//physics
	RaycastResult Raycast(const Vector3& start, const Vector3& forwardNormal, float maxDistance) const;


	void DebugDeactivateAllChunks();

private:
	//initialization
	void SetChunkActivationCheatSheet();

	//update loop
	void UpdateDebugStuff();
	void UpdateDebugLightingPoints();
	void UpdateBlockPlacementAndDigging();
	void UpdateChunks();
	void UpdateDirtyLighting();
	void ManageChunks();

	//must be done after everything else
	void RenderDebugLightingPoints();


	//lighting
	void UpdateDirtyBlockLighting(BlockLocator& block);
	uchar GetCorrectLightLevel(const BlockLocator& block);
	


	void TryToActivateChunks();
	void TryToDeactivateChunks();
	void TryToBuildChunkMesh();


	void SyncChunkWithNeighbors(Chunk* chunk);
	float GetChunkDistanceFromPlayerSquared(Chunk* chunk) const;
	//bool ShouldDeactivateChunk(Chunk* chunk) const;
	
	float m_chunkActivationRadiusChunkDistance = 50.f;		//int?
	float m_chunkDeactivationRadiusChunkDistance = 70.f;
public:
	std::vector<IntVector2> m_chunkActivationOffsetsSortedByDistance;

	std::deque<BlockLocator> m_dirtyLightingBlocks;

	Material* m_chunkMaterial;		//for convenience
	
	MeshBuilder m_debugLightingPointCPUMesh;
	Mesh* m_debugLightingPointGPUMesh;

	std::map<IntVector2, Chunk*> m_chunks;
};


bool CompareIntVector2ByDistanceToOrigin(const IntVector2& a, const IntVector2& b);