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
	inline float GetActivationRadius() const { return m_chunkActivationRadius; }
	

	void ActivateChunk(const IntVector2& chunkCoords);
	void DeactivateChunk(const IntVector2& chunkCoords);

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
	
	float m_chunkActivationRadius = 5.f;		//int?
public:
	
	std::vector<IntVector2> m_chunkActivationOffsetsSortedByDistance;

	Material* m_chunkMaterial;		//for convenience

	std::map<IntVector2, Chunk*> m_chunks;
};


bool CompareIntVector2ByDistanceToOrigin(const IntVector2& a, const IntVector2& b);