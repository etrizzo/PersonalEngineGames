#pragma once
#include "Game/GameCommon.hpp"
#include "Game/Block.hpp"
class Map;



class Chunk{
public:
	Chunk(IntVector2 chunkCoords);
	~Chunk();

	void Update();
	void Render() const;

	void GenerateBlocks();
	void CreateMesh();

	bool DoesChunkHaveAllNeighbors() const;
	bool ShouldChunkRebuildMesh() const;
	bool DoesChunkHaveMesh() const;

	IntVector2 GetChunkCoords();

	static int GetBlockIndexForBlockCoordinates(const IntVector3& blockCoords);
	static IntVector3 GetBlockCoordinatesForBlockIndex(int blockIndex);

	AABB3 GetBounds() const;


	Chunk* m_eastNeighbor  = nullptr;
	Chunk* m_westNeighbor  = nullptr;
	Chunk* m_southNeighbor = nullptr;
	Chunk* m_northNeighbor = nullptr;

private:
	Block m_blocks[BLOCKS_PER_CHUNK];
	IntVector2 m_chunkCoords = IntVector2::INVALID_INDEX;
	//helpful for culling/raycasting. Could also just do a getter that does the math every call - play with that tradeoff of cycles vs. bytes 
	//AABB3 m_worldBounds;    

	//each chunk will have 1 VBO (for now)
	MeshBuilder m_cpuMesh;      //by value or a pointer? //note, meshbuilder has vectors for the verts/indices - that's just size/capacity and a data pointer to what's actually in it. so MeshBuilder has a uniform size
	Mesh*       m_gpuMesh = nullptr;      //by value or a pointer?

	//we'll probably want to know when the mesh is out of date
	bool m_isGPUMeshDirty = true; //isGPUMeshDirty;

	//does all checks for HSR and air blocks and stuff and adds it to the cpuMesh
	void AddVertsForBlockAtIndex(int blockIndex);

	Vector3 GetCenterPointForBlockInWorldCoordinates(int blockIndex);

};