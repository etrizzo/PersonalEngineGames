#include "Tile.hpp"
#include "Game/Game.hpp"
#include  "Game/Map.hpp"


Tile::Tile(int x, int y, TileDefinition* tileDef)
{
	m_coordinates = IntVector2(x,y);
	m_tileDef = tileDef;
	m_health = tileDef->m_maxHealth;
}

Tile::Tile(IntVector2 & coords, TileDefinition* tileDef)
{
	m_coordinates = coords;
	m_tileDef = tileDef;
	m_health = tileDef->m_maxHealth;
}


//	returns neighbors of the current tile x:
// 0 1 2
// 3 x 4
// 5 6 7

void Tile::DamageTile(TileDefinition* typeIfDestroyed, int damage)
{
	if (m_tileDef->m_isDestructible){
		AABB2 oldtexCoords = m_tileDef->GetTexCoordsAtHealth((int) m_health);
		m_health -=damage;
		AABB2 newtexCoords = m_tileDef->GetTexCoordsAtHealth((int) m_health);
		
		if (m_health <= 0){
			m_tileDef = typeIfDestroyed;
		}

	}
}

//should move to map? GetNeighbors(Tile) or GetNeighbors (Intvector2 coords)
void Tile::GetNeighbors(Tile(&neighbors)[8])
{

	neighbors[0] = *(g_theGame->m_currentMap->TileAt(m_coordinates + STEP_NORTHWEST));
	neighbors[1] = *(g_theGame->m_currentMap->TileAt(m_coordinates + STEP_NORTH)	);
	neighbors[2] = *(g_theGame->m_currentMap->TileAt(m_coordinates + STEP_NORTHEAST));
	neighbors[3] = *(g_theGame->m_currentMap->TileAt(m_coordinates + STEP_WEST)		);
	neighbors[4] = *(g_theGame->m_currentMap->TileAt(m_coordinates + STEP_EAST)		);
	neighbors[5] = *(g_theGame->m_currentMap->TileAt(m_coordinates + STEP_SOUTHWEST));
	neighbors[6] = *(g_theGame->m_currentMap->TileAt(m_coordinates + STEP_SOUTH)	);
	neighbors[7] = *(g_theGame->m_currentMap->TileAt(m_coordinates + STEP_SOUTHEAST));
}

AABB2 Tile::GetBounds() const
{
	IntVector2 maxs = m_coordinates + IntVector2(TILE_WIDTH, TILE_WIDTH);
	return AABB2(m_coordinates.GetVector2(), maxs.GetVector2());
}

Vector2 Tile::GetCenter() const
{
	return (m_coordinates.GetVector2() + Vector2(TILE_WIDTH * .5f, TILE_WIDTH * .5f));
}

Vector2 Tile::GetApproximateCenter() const
{
	return (GetCenter() + Vector2(GetRandomFloatInRange(TILE_WIDTH * -.001f, TILE_WIDTH *.001f), GetRandomFloatInRange(TILE_WIDTH *-.001f, TILE_WIDTH *.001f)));
}

TileDefinition* Tile::GetTileDefinition()
{
	return m_tileDef;
}

void Tile::SetType(TileDefinition* newType)
{
	m_tileDef = newType;
	m_health = newType->m_maxHealth;
}
