#pragma once
#include "Game/GameCommon.hpp"
#include "Game/RaycastResult.hpp"

class Chunk;

class World
{
public:
	World();
	~World();

	void HandleInput();
	void Update();	
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

	Clock* m_worldClock;

private:
	//initialization
	void SetChunkActivationCheatSheet();

	//update loop
	void UpdateTimeOfDay();
	void UpdateDebugStuff();
	void UpdateDebugLightingPoints();
	void UpdateBlockPlacementAndDigging();
	void UpdateChunks();
	void UpdateDirtyLighting();
	void ManageChunks();


	

	//render loop
	void RenderChunks();
	void RenderGlowParticles();
	//must be done after everything else
	void RenderDebugLightingPoints();


	//lighting
	void UpdateDirtyBlockLighting(BlockLocator& block);
	//returns true if lighting changed
	bool UpdateBlockIndoorLighting(BlockLocator& block);
	//returns true if lighting changed
	bool UpdateBlockOutdoorLighting(BlockLocator& block);
	uchar GetCorrectIndoorLightLevel(const BlockLocator& block);
	uchar GetCorrectOutdoorLightLevel(const BlockLocator& block);
	


	void TryToActivateChunks();
	void TryToDeactivateChunks();
	void TryToBuildChunkMesh();


	void SyncChunkWithNeighbors(Chunk* chunk);
	float GetChunkDistanceFromPlayerSquared(Chunk* chunk) const;
	//bool ShouldDeactivateChunk(Chunk* chunk) const;
	
	float m_chunkActivationRadiusChunkDistance = 50.f;		//int?
	float m_chunkDeactivationRadiusChunkDistance = 70.f;
public:
	FogData_t	m_fogData;
	std::vector<IntVector2> m_chunkActivationOffsetsSortedByDistance;

	std::deque<BlockLocator> m_dirtyLightingBlocks;

	Material* m_chunkOpaqueMaterial;		//for convenience
	Material* m_glowMaterial;
	
	MeshBuilder m_debugLightingPointCPUMesh;
	Mesh* m_debugLightingPointGPUMesh = nullptr;

	std::map<IntVector2, Chunk*> m_chunks;
	
	RGBA m_skyColor = RGBA::BEEFEE;
	float m_timeOfDay = 0.f;
	float m_outdoorLightScalar = 0.f;
	float m_worldTime = 0.f;
	float m_percThroughDay = 0.f;

	//lightning
	float m_lightningPerlinValue = 0.f;
	float m_lightningValue = 0.f;
	float m_glowPerlinValue = 0.f;
};


bool CompareIntVector2ByDistanceToOrigin(const IntVector2& a, const IntVector2& b);