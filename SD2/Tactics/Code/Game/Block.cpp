#include "Block.hpp"
#include "Game/Game.hpp"
#include  "Game/Map.hpp"


Block::Block(int x, int y, int z, BlockDefinition* tileDef)
{
	m_coordinates = IntVector3(x,y, z);
	m_blockDef = tileDef;
	m_health = tileDef->m_maxHealth;
}

Block::Block(IntVector3 & coords, BlockDefinition* tileDef)
{
	m_coordinates = coords;
	m_blockDef = tileDef;
	if (tileDef != nullptr){
		m_health = tileDef->m_maxHealth;
	}
}



//not necessary right now
//void Block::GetNeighbors(Block(&neighbors)[8])
//{
//
//	neighbors[0] = *(g_theGame->m_currentMap->TileAt(m_coordinates + STEP_NORTHWEST));
//	neighbors[1] = *(g_theGame->m_currentMap->TileAt(m_coordinates + STEP_NORTH)	);
//	neighbors[2] = *(g_theGame->m_currentMap->TileAt(m_coordinates + STEP_NORTHEAST));
//	neighbors[3] = *(g_theGame->m_currentMap->TileAt(m_coordinates + STEP_WEST)		);
//	neighbors[4] = *(g_theGame->m_currentMap->TileAt(m_coordinates + STEP_EAST)		);
//	neighbors[5] = *(g_theGame->m_currentMap->TileAt(m_coordinates + STEP_SOUTHWEST));
//	neighbors[6] = *(g_theGame->m_currentMap->TileAt(m_coordinates + STEP_SOUTH)	);
//	neighbors[7] = *(g_theGame->m_currentMap->TileAt(m_coordinates + STEP_SOUTHEAST));
//}

AABB3 Block::GetBounds() const
{
	Vector3 mins = (m_coordinates.GetVector3() - (Vector3::ONE * .5f));
	Vector3 maxs = mins + (Vector3::ONE * (float) BLOCK_WIDTH);
	return AABB3(mins, maxs);
}

Vector3 Block::GetDimensions() const
{
	return Vector3::ONE * (float) BLOCK_WIDTH;
}

Vector3 Block::GetCenter() const
{
	//return (m_coordinates.GetVector3() + Vector3(BLOCK_WIDTH * .5f, BLOCK_WIDTH * .5f, BLOCK_WIDTH * .5f));
	return m_coordinates.GetVector3();
}

Vector3 Block::GetTopCenter() const
{
	return (m_coordinates.GetVector3() + Vector3(0.f, (float) BLOCK_WIDTH * .5f, 0.f));
}

Plane Block::GetTopPlane() const
{
	AABB3 bounds = GetBounds();
	float hoverAbove = 0.01f * BLOCK_WIDTH;
	Vector3 nl = Vector3 ( bounds.mins.x, bounds.maxs.y + hoverAbove, bounds.mins.z);
	Vector3 nr = Vector3 ( bounds.maxs.x, bounds.maxs.y + hoverAbove, bounds.mins.z);
	Vector3 fr  =Vector3 ( bounds.maxs.x, bounds.maxs.y + hoverAbove, bounds.maxs.z);
	Vector3 fl = Vector3 ( bounds.mins.x, bounds.maxs.y + hoverAbove, bounds.maxs.z);
	return Plane(nl, nr, fr, fl);
}


BlockDefinition* Block::GetTileDefinition()
{
	return m_blockDef;
}

IntVector2 Block::GetXYCoords() const
{
	return IntVector2(m_coordinates.x, m_coordinates.z);
}

void Block::SetType(BlockDefinition* newType)
{
	m_blockDef = newType;
	m_health = newType->m_maxHealth;
}
