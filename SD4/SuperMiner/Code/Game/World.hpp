#pragma once
#include "Game/GameCommon.hpp"

class Chunk;

class World
{
public:
	World();


	void Update();	//nothing right now probs
	void Render();

	void SetActivationRadius(float newRadius);
	inline float GetActivationRadius() const { return m_chunkActivationRadiusChunkDistance; }
	

	void ActivateChunk(const IntVector2& chunkCoords);
	void DeactivateChunk(Chunk* chunkToDeactivate);

	bool IsChunkActive(const IntVector2& chunkCoords);

	IntVector2 GetChunkCoordinatesFromWorldCoordinates(const Vector3& worldPos) const;

private:
	//initialization
	void SetChunkActivationCheatSheet();

	//update loop
	void UpdateDebugStuff();
	void UpdateBlockPlacementAndDigging();
	void UpdateChunks();
	void ManageChunks();

	void TryToActivateChunks();
	void TryToDeactivateChunks();

	float GetChunkDistanceFromPlayerSquared(Chunk* chunk) const;
	//bool ShouldDeactivateChunk(Chunk* chunk) const;
	
	float m_chunkActivationRadiusChunkDistance = 5.f;		//int?
	float m_chunkDeactivationRadiusChunkDistance = 7.f;
public:
	
	std::vector<IntVector2> m_chunkActivationOffsetsSortedByDistance;

	Material* m_chunkMaterial;		//for convenience

	std::map<IntVector2, Chunk*> m_chunks;
};


bool CompareIntVector2ByDistanceToOrigin(const IntVector2& a, const IntVector2& b);