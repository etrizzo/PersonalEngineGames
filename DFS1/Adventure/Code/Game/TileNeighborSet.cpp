#include "TileNeighborSet.hpp"
#include "Game/Map.hpp"

TileNeighborSet::TileNeighborSet(Tile * center, Map * map)
{
	m_center = center;
	IntVector2 coords = center->m_coordinates;
	Tile* checkTile;
	checkTile = map->TileAt(coords + STEP_NORTHWEST);
	m_neighbors[0] = checkTile;

	checkTile = map->TileAt(coords + STEP_NORTH);
	m_neighbors[1] = checkTile;


	checkTile = map->TileAt(coords + STEP_NORTHEAST);
	m_neighbors[2] = checkTile;

	checkTile = map->TileAt(coords + STEP_WEST);
	m_neighbors[3] = checkTile;

	checkTile = map->TileAt(coords + STEP_EAST);
	m_neighbors[4] = checkTile;

	checkTile = map->TileAt(coords + STEP_SOUTHWEST);
	m_neighbors[5] = checkTile;

	checkTile = map->TileAt(coords + STEP_SOUTH);
	m_neighbors[6] = checkTile;

	checkTile = map->TileAt(coords + STEP_SOUTHEAST);
	m_neighbors[7] = checkTile;
}

//find the most common tile type in your neighbors with a higher level than you (edges flow downward)
TileDefinition * TileNeighborSet::FindEdgeTileDefinition() const
{
	std::vector<TileDefinition*> defsHigherLevelThanMe;
	std::vector<int> defCount;
	for (Tile* t : m_neighbors){
		if (t != nullptr){
			if (t->GetTerrainLevel() > m_center->GetTerrainLevel()){
				TileDefinition* terrainDef = t->m_extraInfo->m_terrainDef;
				//Add the terrain def to the list, or increment existing value
				bool containsDef = false;
				for (int i = 0; i < defsHigherLevelThanMe.size(); i++){
					if (defsHigherLevelThanMe[i] == terrainDef){
						//one more tile of that type :)
						defCount[i] = defCount[i] + 1;
						containsDef = true;
					}
				}

				if (!containsDef){
					//if the definition hadn't been found already, add it w/ count 1
					defsHigherLevelThanMe.push_back(terrainDef);
					defCount.push_back(1);
				}
			}
		}
	}

	TileDefinition* highestCountDef = nullptr;
	int highestCount = 0;
	for (int i = 0; i < defsHigherLevelThanMe.size(); i++){
		if (defCount[i] > highestCount){
			highestCountDef = defsHigherLevelThanMe[i];
			highestCount = defCount[i];
		}
	}
	return highestCountDef;
}

Tile * TileNeighborSet::GetLeft() const
{
	return m_neighbors[3];
}

Tile * TileNeighborSet::GetRight() const
{
	return m_neighbors[4];
}

Tile * TileNeighborSet::GetTop() const
{
	return m_neighbors[1];
}

Tile * TileNeighborSet::GetBottom() const
{
	return m_neighbors[6];
}

Tile * TileNeighborSet::GetTopLeft() const
{
	return m_neighbors[0];
}

Tile * TileNeighborSet::GetTopRight() const
{
	return m_neighbors[2];
}

Tile * TileNeighborSet::GetBottomLeft() const
{
	return m_neighbors[5];
}

Tile * TileNeighborSet::GetBottomRight() const
{
	return m_neighbors[7];
}

AABB2 TileNeighborSet::GetTileEdge(TileDefinition * edgeTileDef)
{
	m_edgeToLookFor = edgeTileDef;
	TileEdgeDefinition* def = edgeTileDef->m_edgeDefinition;
	//check every configuration :))))))
	if (CheckConcaveTopLeft()){
		return def->GetTexCoordsForEdge(EDGE_CONCAVE_TOPLEFT);
	}
	if (CheckConcaveTopRight()){
		return def->GetTexCoordsForEdge(EDGE_CONCAVE_TOPRIGHT);
	}
	if (CheckConcaveBottomLeft()){
		return def->GetTexCoordsForEdge(EDGE_CONCAVE_BOTTOMLEFT);
	}
	if (CheckConcaveBottomRight()){
		return def->GetTexCoordsForEdge(EDGE_CONCAVE_BOTTOMRIGHT);
	}

	if (CheckConvexTopLeft()){
		return def->GetTexCoordsForEdge(EDGE_CONVEX_TOPLEFT);
	}
	if (CheckConvexTopRight()){
		return def->GetTexCoordsForEdge(EDGE_CONVEX_TOPRIGHT);
	}
	if (CheckConvexBottomLeft()){
		return def->GetTexCoordsForEdge(EDGE_CONVEX_BOTTOMLEFT);
	}
	if (CheckConvexBottomRight()){
		return def->GetTexCoordsForEdge(EDGE_CONVEX_BOTTOMRIGHT);
	}

	if (CheckPureLeft()){
		return def->GetTexCoordsForEdge(EDGE_PURE_LEFT);
	}
	if (CheckPureRight()){
		return def->GetTexCoordsForEdge(EDGE_PURE_RIGHT);
	}
	if (CheckPureTop()){
		return def->GetTexCoordsForEdge(EDGE_PURE_TOP);
	}
	if (CheckPureBottom()){
		return def->GetTexCoordsForEdge(EDGE_PURE_BOTTOM);
	}

	return def->GetTexCoordsForEdge(EDGE_SINGLE_SMALL);
}

bool TileNeighborSet::MatchesEdgeDefinition(Tile * tile) const
{
	if (tile == nullptr){
		return false;
	}

	if (tile->HasTerrainDefinition(m_edgeToLookFor)){
		return true;
	}
	return false;
}

bool TileNeighborSet::CheckConcaveTopLeft() const
{
	if (MatchesEdgeDefinition(GetLeft()) && MatchesEdgeDefinition(GetTop()) && MatchesEdgeDefinition(GetTopLeft())){
		if (!MatchesEdgeDefinition(GetRight()) && !MatchesEdgeDefinition(GetBottom()) && !MatchesEdgeDefinition(GetBottomRight())){
			return true;
		}
	}
	return false;
}

bool TileNeighborSet::CheckConcaveTopRight() const
{
	if (MatchesEdgeDefinition(GetRight()) && MatchesEdgeDefinition(GetTop()) && MatchesEdgeDefinition(GetTopRight())){
		if (!MatchesEdgeDefinition(GetLeft()) && !MatchesEdgeDefinition(GetBottom()) && !MatchesEdgeDefinition(GetBottomLeft())){
			return true;
		}
	}
	return false;
}

bool TileNeighborSet::CheckConcaveBottomLeft() const
{
	if (MatchesEdgeDefinition(GetLeft()) && MatchesEdgeDefinition(GetBottom()) && MatchesEdgeDefinition(GetBottomLeft())){
		if (!MatchesEdgeDefinition(GetRight()) && !MatchesEdgeDefinition(GetTop()) && !MatchesEdgeDefinition(GetTopRight())){
			return true;
		}
	}
	return false;
}

bool TileNeighborSet::CheckConcaveBottomRight() const
{
	if (MatchesEdgeDefinition(GetRight()) && MatchesEdgeDefinition(GetBottom()) && MatchesEdgeDefinition(GetBottomRight())){
		if (!MatchesEdgeDefinition(GetLeft()) && !MatchesEdgeDefinition(GetTop()) && !MatchesEdgeDefinition(GetTopLeft())){
			return true;
		}
	}
	return false;
}

bool TileNeighborSet::CheckConvexTopLeft() const
{
	if (MatchesEdgeDefinition(GetBottomRight())){
		if (!MatchesEdgeDefinition(GetRight()) && !MatchesEdgeDefinition(GetBottom())){
			return true;
		}
	}
	return false;
}

bool TileNeighborSet::CheckConvexTopRight() const
{
	if (MatchesEdgeDefinition(GetBottomLeft())){
		if (!MatchesEdgeDefinition(GetLeft()) && !MatchesEdgeDefinition(GetBottom())){
			return true;
		}
	}
	return false;
}

bool TileNeighborSet::CheckConvexBottomLeft() const
{
	if (MatchesEdgeDefinition(GetTopRight())){
		if (!MatchesEdgeDefinition(GetRight()) && !MatchesEdgeDefinition(GetTop())){
			return true;
		}
	}
	return false;
}

bool TileNeighborSet::CheckConvexBottomRight() const
{
	if (MatchesEdgeDefinition(GetTopLeft())){
		if (!MatchesEdgeDefinition(GetLeft()) && !MatchesEdgeDefinition(GetTop())){
			return true;
		}
	}
	return false;
}

bool TileNeighborSet::CheckPureLeft() const
{
	if (GetLeft() == nullptr || GetRight() == nullptr){
		return false;
	}
	if (GetRight()->HasTerrainDefinition(m_edgeToLookFor) && !GetLeft()->HasTerrainDefinition(m_edgeToLookFor)){
		return true;
	}
	return false;
}

bool TileNeighborSet::CheckPureRight() const
{
	if (GetLeft() == nullptr || GetRight() == nullptr){
		return false;
	}
	if (GetLeft()->HasTerrainDefinition(m_edgeToLookFor) && !GetRight()->HasTerrainDefinition(m_edgeToLookFor)){
		return true;
	}
	return false;
}

bool TileNeighborSet::CheckPureTop() const
{
	if (GetBottom() == nullptr || GetTop() == nullptr){
		return false;
	}
	if (GetBottom()->HasTerrainDefinition(m_edgeToLookFor) && !GetTop()->HasTerrainDefinition(m_edgeToLookFor)){
		return true;
	}
	return false;
}

bool TileNeighborSet::CheckPureBottom() const
{
	if (GetBottom() == nullptr || GetTop() == nullptr){
		return false;
	}
	if (GetTop()->HasTerrainDefinition(m_edgeToLookFor) && !GetBottom()->HasTerrainDefinition(m_edgeToLookFor)){
		return true;
	}
	return false;
}
