#include "Game/Chunk.hpp"
#include "Game/BlockDefinition.hpp"

Chunk::Chunk(IntVector2 chunkCoords)
{
	m_chunkCoords = chunkCoords;
	m_cpuMesh = MeshBuilder();
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
				float height = Compute2dPerlinNoise((float) worldX, (float) worldY, (float) CHUNK_LAYER_DIMS_XY );
				float heightMapped = RangeMapFloat(height, -1.f, 1.f, 0.f, (float) CHUNK_SIZE_Z);		//range map perlin noise from -1 to 1 into [0,128]
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

void Chunk::CreateMesh()
{
	m_cpuMesh.Clear();
	m_cpuMesh.Begin(PRIMITIVE_TRIANGLES, true);

	//loop through ur blocks
	for (int blockIndex = 0; blockIndex < BLOCKS_PER_CHUNK; blockIndex++)
	{
		AddVertsForBlockAtIndex(blockIndex);
	}
	m_cpuMesh.End();
	m_gpuMesh = m_cpuMesh.CreateMesh(VERTEX_TYPE_3DPCU);
}

void Chunk::Update()
{
}

void Chunk::Render() const
{
	if (m_gpuMesh != nullptr){
		g_theRenderer->DrawMesh(m_gpuMesh->m_subMeshes[0]);
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

	constexpr int X_MASK = CHUNK_SIZE_X - 1;
	constexpr int Y_MASK = CHUNK_SIZE_Y - 1;
	int z = blockIndex >>(CHUNK_BITS_X + CHUNK_BITS_Y);
	int y = (blockIndex >> (CHUNK_BITS_X)) & (Y_MASK);
	int x = blockIndex & (X_MASK);
	// this ends up being 0b00001111, always for pow of 2

	return IntVector3(x,y,z);
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
		
		//Add verts for top (abfe)					//up	//right
		m_cpuMesh.AppendPlane(center + topOffset, FORWARD, RIGHT, Vector2::HALF, s_blockTopBottomColor, blockDef->m_topUVs.mins, blockDef->m_topUVs.maxs);

		//add verts for right (dhfb)
		m_cpuMesh.AppendPlane(center + rightOffset, UP, FORWARD, Vector2::HALF, s_blockNorthSouthColor, blockDef->m_sideUVs.mins, blockDef->m_sideUVs.maxs);

		//add verts for left (gcae)
		m_cpuMesh.AppendPlane(center - rightOffset, UP, -FORWARD, Vector2::HALF, s_blockNorthSouthColor, blockDef->m_sideUVs.mins, blockDef->m_sideUVs.maxs);

		//add verts for front (hgef)
		m_cpuMesh.AppendPlane(center + forwardOffset, UP, -RIGHT, Vector2::HALF, s_blockEastWestColor, blockDef->m_sideUVs.mins, blockDef->m_sideUVs.maxs);

		//add verts for back (cdba)
		m_cpuMesh.AppendPlane(center - forwardOffset, UP, RIGHT, Vector2::HALF, s_blockEastWestColor, blockDef->m_sideUVs.mins, blockDef->m_sideUVs.maxs);

		//add verts for bottom (hgcd)
		m_cpuMesh.AppendPlane(center - topOffset, -FORWARD, -RIGHT, Vector2::HALF, s_blockTopBottomColor, blockDef->m_sideUVs.mins, blockDef->m_sideUVs.maxs);
	}
}

Vector3 Chunk::GetCenterPointForBlockInWorldCoordinates(int blockIndex)
{
	IntVector3 blockCoords = GetBlockCoordinatesForBlockIndex(blockIndex);
	Vector3 chunkOffsetFromOrigin = Vector3((float) m_chunkCoords.x * (float) CHUNK_SIZE_X, (float) m_chunkCoords.y * (float) CHUNK_SIZE_Y, 0.f );
	Vector3 center = Vector3(.5f,.5f,.5f) + blockCoords.GetVector3() + chunkOffsetFromOrigin;
	return center;
}
