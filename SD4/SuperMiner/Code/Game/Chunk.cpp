#include "Game/Chunk.hpp"
#include "Game/BlockDefinition.hpp"
#include "Game/BlockLocator.hpp"

Chunk::Chunk(IntVector2 chunkCoords)
{
	m_chunkCoords = chunkCoords;
	m_cpuMesh = MeshBuilder();
}

Chunk::~Chunk()
{
	//unlink neighbors
	if (m_eastNeighbor != nullptr){
		m_eastNeighbor->m_westNeighbor = nullptr;
	}
	if (m_westNeighbor != nullptr){
		m_westNeighbor->m_eastNeighbor = nullptr;
	}
	if (m_northNeighbor != nullptr){
		m_northNeighbor->m_southNeighbor = nullptr;
	}
	if (m_southNeighbor != nullptr){
		m_southNeighbor->m_northNeighbor = nullptr;
	}
	//delete mesh
	delete m_gpuMesh;
}

void Chunk::LoadOrGenerateBlocks()
{
	if (!LoadFromDisk())
	{
		GenerateBlocks();
	}
}

void Chunk::CreateMesh()
{
	m_cpuMesh.Clear();
	m_cpuMesh.Begin(PRIMITIVE_TRIANGLES, true);
	m_cpuMesh.ReserveVerts(CHUNK_LAYER_DIMS_XY * 4.f * 6.f);		//~ 2 faces for the top and bottom of a chunk 

	//loop through ur blocks
	for (int blockIndex = 0; blockIndex < BLOCKS_PER_CHUNK; blockIndex++)
	{
		AddVertsForBlockAtIndex(blockIndex);
	}
	m_cpuMesh.End();
	m_gpuMesh = m_cpuMesh.CreateMesh(VERTEX_TYPE_3DPCU);
	m_isGPUMeshDirty = false;
}

bool Chunk::DoesChunkHaveAllNeighbors() const
{
	return (m_eastNeighbor != nullptr) && (m_westNeighbor != nullptr) && (m_northNeighbor != nullptr) && (m_southNeighbor != nullptr);
}

bool Chunk::ShouldChunkRebuildMesh() const
{
	return DoesChunkHaveAllNeighbors() && (m_isGPUMeshDirty || !DoesChunkHaveMesh());
}

bool Chunk::DoesChunkHaveMesh() const
{
	return m_gpuMesh != nullptr;
}

void Chunk::SetBlockType(int blockIndex, eBlockType newType)
{
	m_blocks[blockIndex].SetType(newType);
	m_isGPUMeshDirty = true;
	//check if neighbors need to be dirtied
	if (IsBlockIndexOnEastEdge(blockIndex) && m_eastNeighbor != nullptr)
	{
		m_eastNeighbor->m_isGPUMeshDirty = true;
	}

	if (IsBlockIndexOnWestEdge(blockIndex) && m_westNeighbor != nullptr)
	{
		m_westNeighbor->m_isGPUMeshDirty = true;
	}

	if (IsBlockIndexOnNorthEdge(blockIndex) && m_northNeighbor != nullptr)
	{
		m_northNeighbor->m_isGPUMeshDirty = true;
	}
	
	if (IsBlockIndexOnSouthEdge(blockIndex) && m_southNeighbor != nullptr)
	{
		m_southNeighbor->m_isGPUMeshDirty = true;
	}
}

IntVector2 Chunk::GetChunkCoords()
{
	return m_chunkCoords;
}



void Chunk::Update()
{
}

void Chunk::Render() const
{
	if (m_gpuMesh != nullptr){
		g_theRenderer->DrawMesh(m_gpuMesh->m_subMeshes[0]);
	} else {
		AABB2 bounds = AABB2(GetBounds().mins.XY(), GetBounds().maxs.XY());
		g_theRenderer->DrawAABB2(bounds, RGBA::MAGENTA);
	}
}

int Chunk::GetBlockIndexForBlockCoordinates(const IntVector3 & blockCoords)
{
	//we can also do this with BITS! FASTER!!
	int index = blockCoords.x | (blockCoords.y << CHUNK_BITS_X) | (blockCoords.z << (CHUNK_BITS_X + CHUNK_BITS_Y));

	//it just so happens that all the bits don't overlap, so we can bit or instead of add! more free cycles.
	return index;
}

IntVector3 Chunk::GetBlockCoordinatesForBlockIndex(int blockIndex)
{
	//tier BITS 
	//if our dimensions are powers of 2, then we can use bitshifting
	//int z = blockIndex >> (CHUNK_BITS_X + CHUNK_BITS_Y);
	//int layerOffset = z * CHUNK_LAYER_DIMS_XY;
	//int y = ( blockIndex - (z * layerOffset) ) >> (CHUNK_BITS_X);
	//int x =   blockIndex - (layerOffset) - (y * CHUNK_SIZE_X) ;

	////tier BITS2: WE CAN GO FURTHER - MASKS!
	//  v zbitsv      v xbits
	//	111 1111 1111 1111
	//	         ^ybits

	
	int z = GetBlockZCoordinate(blockIndex);
	int y = GetBlockYCoordinate(blockIndex);
	int x = GetBlockXCoordinate(blockIndex);
	// this ends up being 0b00001111, always for pow of 2

	return IntVector3(x,y,z);
}

std::string Chunk::GetChunkFileFormatForChunkCoords(const IntVector2 & chunkCoords)
{
	return Stringf("Saves/Chunk_%i,%i.chunk", chunkCoords.x, chunkCoords.y);
}

int Chunk::GetBlockIndexFromWorldPosition(const Vector3 & worldPos) const
{
	Vector3 chunkWorldOffset = Vector3((float) m_chunkCoords.x * CHUNK_SIZE_X, (float) m_chunkCoords.y * CHUNK_SIZE_Y, 0.f);
	Vector3 chunkPos = worldPos - chunkWorldOffset;
	IntVector3 blockCoords = IntVector3((int) floorf(chunkPos.x), (int) floorf(chunkPos.y), (int) floorf(chunkPos.z));
	return GetBlockIndexForBlockCoordinates(blockCoords);
}

AABB3 Chunk::GetBounds() const
{
	Vector3 mins = Vector3((float) m_chunkCoords.x * CHUNK_SIZE_X, (float) m_chunkCoords.y * CHUNK_SIZE_Y, 0.f);
	Vector3 maxs = mins  + Vector3((float) CHUNK_SIZE_X,(float) CHUNK_SIZE_Y,(float) CHUNK_SIZE_Z);
	return AABB3(mins, maxs);
}

Block & Chunk::GetBlock(int blockIndex)
{
	return m_blocks[blockIndex];
}

void Chunk::GenerateBlocks()
{
	for (int z = 0; z < CHUNK_SIZE_Z; z++)
	{
		for (int y = 0; y < CHUNK_SIZE_Y; y++)
		{
			for (int x = 0; x < CHUNK_SIZE_X; x++)
			{
				//scoot over to the chunks position for the noise computation
				int worldX = x + (m_chunkCoords.x * CHUNK_SIZE_X);
				int worldY = y + (m_chunkCoords.y * CHUNK_SIZE_Y);
				float height = Compute2dPerlinNoise((float) worldX, (float) worldY,  300.f, 3);
				float heightMapped = RangeMapFloat(height, -1.f, 1.f, SEA_LEVEL, (float) CHUNK_SIZE_Z * .6f);		//range map perlin noise from -1 to 1 into [0,128]
				int blockIndex = Chunk::GetBlockIndexForBlockCoordinates(IntVector3(x,y,z));

				//later choose different things for different heights u kno
				if ((float) z < heightMapped )
				{
					if (z < heightMapped - 3.f){
						m_blocks[blockIndex].SetType(BLOCK_STONE);
					} else {
						m_blocks[blockIndex].SetType(BLOCK_GRASS);
					}
				} else {
					m_blocks[blockIndex].SetType(BLOCK_AIR);
				}
			}
		}
	}
}

bool Chunk::LoadFromDisk()
{
	//for now, try to load from disk (we'll be touching disk for every load, FOR NOW)
	std::string filePath = Chunk::GetChunkFileFormatForChunkCoords(m_chunkCoords);      //returns Chunk_x,y.chunk

	std::vector<unsigned char> chunkFileBuffer;
	bool wasLoaded = LoadBinaryFileIntoBuffer(filePath, chunkFileBuffer);
	if (!wasLoaded){
		return false;
	}

	//reads the header, go through the RLE data and read it
	m_isSavedOrUntouched = PopulateFromBuffer(chunkFileBuffer);
	return m_isSavedOrUntouched;
}

bool Chunk::PopulateFromBuffer(const std::vector<unsigned char>& buffer)
{
	if (ValidateBufferFormat(buffer))
	{
		ReadBufferAsRLE(buffer);
		return true;
	} else {
		ConsolePrintf("Unable to populate chunk %i,%i from buffer.", m_chunkCoords.x, m_chunkCoords.y);
		return false;
	}
	
}

bool Chunk::ValidateBufferFormat(const std::vector<unsigned char>& buffer)
{
	ChunkFileHeader* header = (ChunkFileHeader*) buffer.data();
	return header->IsValid();
}

void Chunk::ReadBufferAsRLE(const std::vector<unsigned char>& buffer)
{
	int headerSize = sizeof(ChunkFileHeader);
	int blocksAdded = 0;
	for (int bytepair = headerSize; bytepair < buffer.size(); bytepair+=2)
	{
		if (blocksAdded >= BLOCKS_PER_CHUNK){
			ConsolePrintf(RGBA::RED, "Too many blocks saved in RLE file (Chunk %i,%i)", m_chunkCoords.x, m_chunkCoords.y);
			return;
		}
		//read type
		unsigned char type = buffer[bytepair];
		unsigned char runLength = buffer[bytepair + 1];

		//add blocks to the chunk according to type and run length
		for (int i = 0; i < runLength; i++ )
		{
			m_blocks[blocksAdded].SetType((eBlockType) type);
			blocksAdded++;
		}
	}
	if (blocksAdded != BLOCKS_PER_CHUNK){
		ConsolePrintf(RGBA::RED, "Not enough blocks in RLE file - interesting.... (Chunk %i,%i)", m_chunkCoords.x, m_chunkCoords.y);
	}
}

void Chunk::AddVertsForBlockAtIndex(int blockIndex)
{
	unsigned char blockType = m_blocks[blockIndex].m_blockID;
	if (blockType == 0U)
	{
		return;
	} else {
		BlockDefinition* blockDef = BlockDefinition::GetBlockDefinitionFromID(blockType);

		Vector3 center = GetCenterPointForBlockInWorldCoordinates(blockIndex);
		Vector3 topOffset = UP * .5f;
		Vector3 rightOffset = RIGHT * .5f;
		Vector3 forwardOffset = FORWARD * .5f;

		/* 
		   e-------f
		  /|      /|
		 / |     / |
		a--|----b  |
		|  g----|--h
		| /     | /
		c-------d

		*/

		//m_cpuMesh.AppendCube(center, Vector3::ONE, RGBA::WHITE, RIGHT, UP, FORWARD, blockDef->m_topUVs, blockDef->m_sideUVs, blockDef->m_bottomUVs);
		
		BlockLocator me(blockIndex, this);

		if (!me.GetUp().IsBlockFullyOpaque()){
			//Add verts for top (abfe)					//up	//right
			m_cpuMesh.AppendPlane(center + topOffset, FORWARD, RIGHT, Vector2::HALF, s_blockTopBottomColor, blockDef->m_topUVs.mins, blockDef->m_topUVs.maxs);
		}

		if (!me.GetSouth().IsBlockFullyOpaque()){
			//add verts for right (dhfb)
			m_cpuMesh.AppendPlane(center + rightOffset, UP, FORWARD, Vector2::HALF, s_blockNorthSouthColor, blockDef->m_sideUVs.mins, blockDef->m_sideUVs.maxs);
		}

		if (!me.GetNorth().IsBlockFullyOpaque()){
			//add verts for left (gcae)
			m_cpuMesh.AppendPlane(center - rightOffset, UP, -FORWARD, Vector2::HALF, s_blockNorthSouthColor, blockDef->m_sideUVs.mins, blockDef->m_sideUVs.maxs);
		}

		if (!me.GetEast().IsBlockFullyOpaque()){
			//add verts for front (hgef)
			m_cpuMesh.AppendPlane(center + forwardOffset, UP, -RIGHT, Vector2::HALF, s_blockEastWestColor, blockDef->m_sideUVs.mins, blockDef->m_sideUVs.maxs);
		}

		if (!me.GetWest().IsBlockFullyOpaque()){
			//add verts for back (cdba)
			m_cpuMesh.AppendPlane(center - forwardOffset, UP, RIGHT, Vector2::HALF, s_blockEastWestColor, blockDef->m_sideUVs.mins, blockDef->m_sideUVs.maxs);
		}

		if (!me.GetDown().IsBlockFullyOpaque()){
			//add verts for bottom (hgcd)
			m_cpuMesh.AppendPlane(center - topOffset, -FORWARD, -RIGHT, Vector2::HALF, s_blockTopBottomColor, blockDef->m_sideUVs.mins, blockDef->m_sideUVs.maxs);
		}
	}
}

Vector3 Chunk::GetCenterPointForBlockInWorldCoordinates(int blockIndex)
{
	IntVector3 blockCoords = GetBlockCoordinatesForBlockIndex(blockIndex);
	Vector3 chunkOffsetFromOrigin = Vector3((float) m_chunkCoords.x * (float) CHUNK_SIZE_X, (float) m_chunkCoords.y * (float) CHUNK_SIZE_Y, 0.f );
	Vector3 center = Vector3(.5f,.5f,.5f) + blockCoords.GetVector3() + chunkOffsetFromOrigin;
	return center;
}

bool IsBlockIndexOnEastEdge(int blockIndex)
{
	return ((blockIndex & CHUNK_MASK_X) == CHUNK_MASK_X);
}

bool IsBlockIndexOnWestEdge(int blockIndex)
{
	return ((blockIndex & CHUNK_MASK_X) == 0);
}

bool IsBlockIndexOnSouthEdge(int blockIndex)
{
	return ((blockIndex & CHUNK_MASK_Y) == 0);
}

bool IsBlockIndexOnNorthEdge(int blockIndex)
{
	return ((blockIndex & CHUNK_MASK_Y) == CHUNK_MASK_Y);
}

bool IsBlockIndexOnBottomEdge(int blockIndex)
{
	return ((blockIndex & CHUNK_MASK_Z) == 0);
}

bool IsBlockIndexOnTopEdge(int blockIndex)
{
	return ((blockIndex & CHUNK_MASK_Z) == CHUNK_MASK_Z);
}

bool ChunkFileHeader::IsValid() const
{
	//check SMCD
	if (m_4cc[0] != 'S')
	{
		ConsolePrintf(RGBA::RED, "Invalid chunk header: %s", m_4cc);
		return false;
	}
	if (m_4cc[1] != 'M')
	{
		ConsolePrintf(RGBA::RED, "Invalid chunk header: %s", m_4cc);
		return false;
	}
	if (m_4cc[2] != 'C')
	{
		ConsolePrintf(RGBA::RED, "Invalid chunk header: %s", m_4cc);
		return false;
	}
	if (m_4cc[3] != 'D')
	{
		ConsolePrintf(RGBA::RED, "Invalid chunk header: %s", m_4cc);
		return false;
	}

	//check dimensions
	if (m_chunkBitsX != CHUNK_BITS_X)
	{
		ConsolePrintf(RGBA::RED, "Invalid chunk dimensions on x: %i (should be %i)", m_chunkBitsX, CHUNK_BITS_X);
		return false;
	}
	if (m_chunkBitsY != CHUNK_BITS_Y)
	{
		ConsolePrintf(RGBA::RED, "Invalid chunk dimensions on yx: %i (should be %i)", m_chunkBitsY, CHUNK_BITS_Y);
		return false;
	}

	if (m_chunkBitsZ != CHUNK_BITS_Z)
	{
		ConsolePrintf(RGBA::RED, "Invalid chunk dimensions on z: %i (should be %i)", m_chunkBitsZ, CHUNK_BITS_Z);
		return false;
	}

	//check reserved
	if (m_reserved1 != 0)
	{
		ConsolePrintf(RGBA::RED, "Reserved byte 1 is wrong: %i (should be %i)", m_reserved1, 0);
		return false;
	}
	if (m_reserved2 != 0)
	{
		ConsolePrintf(RGBA::RED, "Reserved byte 2 is wrong: %i (should be %i)", m_reserved2, 0);
		return false;
	}
	if (m_reserved3 != 0)
	{
		ConsolePrintf(RGBA::RED, "Reserved byte 3 is wrong: %i (should be %i)", m_reserved3, 0);
		return false;
	}

	//check format
	if (m_blockDataFormat != 'R')		//or other types later
	{
		ConsolePrintf(RGBA::RED, "Invalid chunk format");
		return false;
	}

	return true;
}
