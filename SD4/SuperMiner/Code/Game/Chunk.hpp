#pragma once
#include "Game/GameCommon.hpp"
#include "Game/Block.hpp"
#include "Game/BlockLocator.hpp"
class Map;
class GlowParticles;

constexpr int CHUNK_PARTICLES_VISIBLE_DISTANCE = 3;

class Chunk{
public:
	Chunk(IntVector2 chunkCoords);
	~Chunk();

	void Update();
	void Render() const;

	void LoadOrGenerateBlocks();
	void CreateMesh();
	void InitializeLighting();

	void SaveToDisk() const;

	bool DoesChunkHaveAllNeighbors() const;
	bool ShouldChunkRebuildMesh() const;
	bool DoesChunkHaveMesh() const;

	//wrapper functions for setblocktype that also does the lighting stuff we need.
	void DigBlock(int blockIndex, uchar newType = BLOCK_AIR);
	void PlaceBlock(int blockIndex, uchar newType);
	

	void SetMeshDirty() { m_isGPUMeshDirty = true; };

	IntVector2 GetChunkCoords();

	inline static int GetBlockXCoordinate(int blockIndex);
	inline static int GetBlockYCoordinate(int blockIndex);
	inline static int GetBlockZCoordinate(int blockIndex);
	inline static int GetBlockIndexForBlockCoordinates(const IntVector3& blockCoords);
	static IntVector3 GetBlockCoordinatesForBlockIndex(int blockIndex);
	static std::string GetChunkFileFormatForChunkCoords(const IntVector2& chunkCoords);
	
	int GetBlockIndexFromWorldPosition(const Vector3& worldPos) const;
	Vector3 GetCenterPointForBlockInWorldCoordinates(int blockIndex);

	AABB3 GetBounds() const;
	Block& GetBlock(int blockIndex);


	Chunk* m_eastNeighbor  = nullptr;
	Chunk* m_westNeighbor  = nullptr;
	Chunk* m_southNeighbor = nullptr;
	Chunk* m_northNeighbor = nullptr;

	GlowParticles* m_glowSystem;
	bool m_isCloseToPlayer = false;


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
	bool m_isSavedOrUntouched = true;	//starts true because purely generated chunks with no modifications don't need to be saved

	//generates the chunk brand new
	void GenerateBlocks();

	//Checks if the chunk is on disk, and loads it if it is. returns false if the chunk is not on disk.
	bool LoadFromDisk();
	//validates and reads according to the right file format (right now, just RLE
	bool PopulateFromBuffer(const std::vector<unsigned char>& buffer);

	bool ValidateBufferFormat(const std::vector<unsigned char>& buffer);
	void ReadBufferAsRLE(const std::vector<unsigned char>& buffer);

	void AppendChunkHeaderToBuffer(std::vector<unsigned char>& buffer, unsigned char fileType = 'R') const;
	void AppendBlocksToBufferRLE(std::vector<unsigned char>& buffer) const;

	//does all checks for HSR and air blocks and stuff and adds it to the cpuMesh
	void AddVertsForBlockAtIndex(int blockIndex);

	RGBA GetLightingTintForBlock(const BlockLocator& block);


	//sets a block type and dirties the mesh
	void SetBlockType(int blockIndex, uchar newType);


	bool ShouldSetBlockDirtyAtActivation(int blockIndex);
	void InitializeSkyBlocks();

};


bool IsBlockIndexOnEastEdge(int blockIndex);
bool IsBlockIndexOnWestEdge(int blockIndex);
bool IsBlockIndexOnSouthEdge(int blockIndex);
bool IsBlockIndexOnNorthEdge(int blockIndex);
bool IsBlockIndexOnBottomEdge(int blockIndex);
bool IsBlockIndexOnTopEdge(int blockIndex);



int Chunk::GetBlockXCoordinate(int blockIndex)
{
	return blockIndex & (CHUNK_MASK_X);
}

int Chunk::GetBlockYCoordinate(int blockIndex)
{
	return (blockIndex & CHUNK_MASK_Y) >> CHUNK_BITS_X;
}

int Chunk::GetBlockZCoordinate(int blockIndex)
{
	return  blockIndex >> (CHUNK_BITS_X + CHUNK_BITS_Y);
}




struct ChunkFileHeader
{
	//the 4 character code at the start of the file to say it's a chunk file
	unsigned char m_4cc[4] = {'S', 'M', 'C', 'D'}  ;     
	unsigned char m_version = 1;    //file format version, future proofing

	//verify when we're reading that these are the right dimensions - otherwise we can't read it
	unsigned char m_chunkBitsX = CHUNK_BITS_X;      
	unsigned char m_chunkBitsY = CHUNK_BITS_Y;
	unsigned char m_chunkBitsZ = CHUNK_BITS_Z;

	//Just reserving these bytes in case we want them later
	//still check that they're zeros, but they don't mean anything.
	unsigned char m_reserved1 = 0;
	unsigned char m_reserved2 = 0;
	unsigned char m_reserved3 = 0;

	//later, we might check what format is the smallest for chunks
	//could even do RLE along different directions (like z first instead of x first)
	unsigned char m_blockDataFormat = 'R';      //'R' = RLE Compressed

	bool IsValid() const;
};