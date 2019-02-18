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
TileDefinition * TileNeighborSet::FindEdgeTileDefinition()
{
	std::vector<TileDefinition*> defsHigherLevelThanMe;
	std::vector<int> defCount;
	for (Tile* t : m_neighbors){
		if (t != nullptr && t->m_tileDef->m_isTerrain){
			if (CheckForEdge(t)){
				TileDefinition* terrainDef = t->m_extraInfo->m_terrainDef;
				if (m_compareMode > COMPARE_GROUND && t->m_extraInfo->m_cosmeticBaseDefinition != nullptr){
					terrainDef = t->m_extraInfo->m_cosmeticBaseDefinition;
				}
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
	if (highestCountDef != nullptr && highestCountDef->m_isTerrain){
		m_edgeToLookFor = highestCountDef;
	} else {
		highestCountDef = nullptr;
	}
	return highestCountDef;
}

TileDefinition * TileNeighborSet::FindShoreEdge()
{
	//we're finding which tile type shoudl be the base underneath the shore edge
	std::vector<TileDefinition*> defsHigherLayerThanMe;
	std::vector<int> defCount;
	for (Tile* t : m_neighbors){
		if (t != nullptr){
			if (t->GetTerrainLayer() > m_center->GetTerrainLayer()){
				TileDefinition* terrainDef = t->m_extraInfo->m_terrainDef;
				//Add the terrain def to the list, or increment existing value
				bool containsDef = false;
				for (int i = 0; i < defsHigherLayerThanMe.size(); i++){
					if (defsHigherLayerThanMe[i] == terrainDef){
						//one more tile of that type :)
						defCount[i] = defCount[i] + 1;
						containsDef = true;
					}
				}

				if (!containsDef){
					//if the definition hadn't been found already, add it w/ count 1
					defsHigherLayerThanMe.push_back(terrainDef);
					defCount.push_back(1);
				}
			}
		}
	}

	//get the higher-layer definition that has the highest number of neighbors around this tile
	TileDefinition* highestCountDef = nullptr;
	int highestCount = 0;
	for (int i = 0; i < defsHigherLayerThanMe.size(); i++){
		if (defCount[i] > highestCount){
			highestCountDef = defsHigherLayerThanMe[i];
			highestCount = defCount[i];
		}
	}
	if (highestCountDef != nullptr){
		m_edgeToLookFor = highestCountDef;
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

AABB2 TileNeighborSet::GetDownflowingEdge(TileDefinition * edgeTileDef)
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

	//single tile - swap to base definition
	TileEdgeDefinition* centerEdgeDef = m_center->m_tileDef->m_edgeDefinition;
	if (centerEdgeDef != nullptr){
		m_center->m_extraInfo->m_cosmeticBaseDefinition = m_edgeToLookFor;
		m_center->AddOverlaySpriteFromTileSheet(m_edgeToLookFor->GetTexCoords(), SPRITE_COSMETIC_BASE);
		return centerEdgeDef->GetTexCoordsForEdge(EDGE_SINGLE_SMALL);
	}

	//last resort :(
	//try swapping the base def to the edge tile
	//m_center->SetType(edgeTileDef);
	//ConsolePrintf(RGBA::RED, "No suitable tile edge found.");

	//m_center->m_extraInfo->m_cosmeticBaseDef = m_edgeToLookFor;
	//m_center->AddOverlaySpriteFromTileSheet(m_center->m_tileDef->GetTexCoords(), SPRITE_COSMETIC_BASE);
	return def->GetTexCoordsForEdge(EDGE_SINGLE_SMALL);
}

AABB2 TileNeighborSet::GetUpflowingEdge(TileDefinition * upwardEdge)
{
	//wanna return the opposite edge from what we think
	m_edgeToLookFor = upwardEdge;
	TileEdgeDefinition* def = m_center->m_tileDef->m_edgeDefinition;
	//check every configuration :))))))
	if (CheckConcaveTopLeft()){
		return def->GetTexCoordsForEdge(EDGE_CONVEX_TOPLEFT);
	}
	if (CheckConcaveTopRight()){
		return def->GetTexCoordsForEdge(EDGE_CONVEX_TOPRIGHT);
	}
	if (CheckConcaveBottomLeft()){
		return def->GetTexCoordsForEdge(EDGE_CONVEX_BOTTOMLEFT);
	}
	if (CheckConcaveBottomRight()){
		return def->GetTexCoordsForEdge(EDGE_CONVEX_BOTTOMRIGHT);
	}

	if (CheckConvexTopLeft()){
		return def->GetTexCoordsForEdge(EDGE_CONCAVE_TOPLEFT);
	}
	if (CheckConvexTopRight()){
		return def->GetTexCoordsForEdge(EDGE_CONCAVE_TOPRIGHT);
	}
	if (CheckConvexBottomLeft()){
		return def->GetTexCoordsForEdge(EDGE_CONCAVE_BOTTOMLEFT);
	}
	if (CheckConvexBottomRight()){
		return def->GetTexCoordsForEdge(EDGE_CONCAVE_BOTTOMRIGHT);
	}

	if (CheckPureLeft()){
		return def->GetTexCoordsForEdge(EDGE_PURE_RIGHT);
	}
	if (CheckPureRight()){
		return def->GetTexCoordsForEdge(EDGE_PURE_LEFT);
	}
	if (CheckPureTop()){
		return def->GetTexCoordsForEdge(EDGE_PURE_BOTTOM);
	}
	if (CheckPureBottom()){
		return def->GetTexCoordsForEdge(EDGE_PURE_TOP);
	}

	//single tile - swap to base definition
	TileEdgeDefinition* centerEdgeDef = m_center->m_tileDef->m_edgeDefinition;
	if (centerEdgeDef != nullptr){
		//ConsolePrintf(RGBA::RED, "No suitable tile edge found.");
		m_center->m_extraInfo->m_cosmeticBaseDefinition = m_edgeToLookFor;
		m_center->AddOverlaySpriteFromTileSheet(m_edgeToLookFor->GetTexCoords(), SPRITE_COSMETIC_BASE);
		if (CheckRandomChance(.8f)){
			return def->GetTexCoordsForEdge(EDGE_SINGLE_LARGE);
		} else {
			return centerEdgeDef->GetTexCoordsForEdge(EDGE_SINGLE_SMALL);
		}
	}

	//last resort :(
	ConsolePrintf(RGBA::RED, "No suitable tile edge found.");
	
	return def->GetTexCoordsForEdge(EDGE_SINGLE_SMALL);
}

void TileNeighborSet::SetCompareMode(eComparisonMode mode)
{
	m_compareMode = mode;
}

bool TileNeighborSet::CheckForEdge(Tile * tileToCheck)
{
	switch (m_compareMode){
	case (COMPARE_GROUND):		//grass to dirt
		if (tileToCheck->GetTerrainLayer() == m_center->GetTerrainLayer()){
			if (tileToCheck->GetGroundLayer() > m_center->GetGroundLayer()){
				if (tileToCheck->m_tileDef != m_center->m_tileDef){
					return true;
				}
			}
			//return tileToCheck->GetGroundLayer() > m_center->GetGroundLayer();
		} else {
			//if (m_center->m_extraInfo->m_cosmeticBaseDefinition != nullptr ){
			//	if (m_center->m_extraInfo->m_cosmeticBaseDefinition->m_terrainLayer == tileToCheck->GetTerrainLayer()){
			//		//if you're not on the same terrain layer but you have the same cosmetic def, still edge it
			//		if (tileToCheck->GetGroundLayer() > m_center->GetGroundLayer()){
			//			if (tileToCheck->m_tileDef != m_center->m_tileDef){
			//				return true;
			//			}
			//		}
			//	}
			//}
			return false;
		}
		break;
	case (COMPARE_TERRAIN):		//water to ground
		return tileToCheck->GetTerrainLayer() > m_center->GetTerrainLayer();
		break;
	case (COMPARE_DEFINITION): //internal
		TileDefinition* checkDefinition = tileToCheck->m_extraInfo->m_terrainDef;
		if (tileToCheck->m_extraInfo->m_cosmeticBaseDefinition != nullptr){
			checkDefinition = tileToCheck->m_extraInfo->m_cosmeticBaseDefinition;
		}
		TileDefinition* centerDefinition = m_center->m_extraInfo->m_terrainDef;
		if (m_center->m_extraInfo->m_cosmeticBaseDefinition != nullptr){
			centerDefinition = m_center->m_extraInfo->m_cosmeticBaseDefinition;
		}

		if (checkDefinition == nullptr || centerDefinition == nullptr){
			return false;
		}
		if (checkDefinition->m_terrainLayer < TERRAIN_GROUND || centerDefinition->m_terrainLayer < TERRAIN_GROUND){		//we don't really wanna fuck with water anymore?
			return false;
		}

		//if on the same ground layer, compare as usual
		if (checkDefinition->m_groundLayer == centerDefinition->m_groundLayer){
			return checkDefinition->m_terrainLevel > centerDefinition->m_terrainLevel;
			//otherwise, compare the cosmetic layers
		/*} else if (tileToCheck->GetCosmeticTerrainLayer() == m_center->GetCosmeticTerrainLayer()){
			ConsolePrintf("Comparing cosmetic terrain");
			return tileToCheck->GetCosmeticTerrainLevel() > m_center->GetCosmeticTerrainLevel();
			*/
		} else {
			return false;
		}
		break;
	}
	return false;
}

bool TileNeighborSet::IsTileInvalid() const
{
	//TileDefinition* centerDefinition = m_center->m_extraInfo->m_terrainDef;
	//int numMatchingCenter = 0;
	//int numHigherLevel = 0;
	//for(Tile* neighbor : m_neighbors){
	//	if (neighbor != nullptr){
	//		if (neighbor->HasTerrainDefinition(centerDefinition)){
	//			numMatchingCenter++;
	//		//} else if (neighbor->HasTerrainDefinition(m_edgeToLookFor)){
	//		} else if (neighbor->GetTerrainLevel() < 0 || neighbor->GetTerrainLevel() > centerDefinition->m_terrainLevel){
	//			numHigherLevel++;
	//		}
	//	}
	//}
	//if (numMatchingCenter <= 2 && numHigherLevel > 5){
	//	return true;
	//} else {
	//	return false;
	//}

	TileEdgeDefinition* def = m_edgeToLookFor->m_edgeDefinition;
	//check every configuration :))))))
	if (CheckConcaveTopLeft()){
		return false;
	}
	if (CheckConcaveTopRight()){
		return false;
	}
	if (CheckConcaveBottomLeft()){
		return false;
	}
	if (CheckConcaveBottomRight()){
		return false;
	}

	if (CheckConvexTopLeft()){
		return false;
	}
	if (CheckConvexTopRight()){
		return false;
	}
	if (CheckConvexBottomLeft()){
		return false;
	}
	if (CheckConvexBottomRight()){
		return false;
	}

	if (CheckPureLeft()){
		return false;
	}
	if (CheckPureRight()){
		return false;
	}
	if (CheckPureTop()){
		return false;
	}
	if (CheckPureBottom()){
		return false;
	}

	//last resort :(
	//try swapping the base def to the edge tile
	//m_center->SetType(edgeTileDef);
	//ConsolePrintf(RGBA::RED, "No suitable tile edge found.");

	//m_center->m_extraInfo->m_cosmeticBaseDef = m_edgeToLookFor;
	//m_center->AddOverlaySpriteFromTileSheet(m_center->m_tileDef->GetTexCoords(), SPRITE_COSMETIC_BASE);
	return true;
}

bool TileNeighborSet::IsTuft()
{
	int numLowerThanMe = 0;
	for (Tile* t : m_neighbors){
		if (t != nullptr){
			if (t->GetTerrainLayer() < m_center->GetTerrainLayer()){
				numLowerThanMe++;
			} else if (t->GetGroundLayer() < m_center->GetGroundLayer()){
				numLowerThanMe++;
			} else if (t->GetTerrainLevel() < m_center->GetTerrainLayer()){
				numLowerThanMe++;
			}
		}
	}
	if (numLowerThanMe >= 7){
		return true;
	} else {
		return false;
	}
}

TileDefinition * TileNeighborSet::FindMostSignificantNeighbor()
{
	std::vector<TileDefinition*> neighborDefs;
	std::vector<int> defCount;
	for (Tile* t : m_neighbors){
		if (t != nullptr){
			TileDefinition* terrainDef = t->m_extraInfo->m_terrainDef;
			//Add the terrain def to the list, or increment existing value
			bool containsDef = false;
			for (int i = 0; i < neighborDefs.size(); i++){
				if (neighborDefs[i] == terrainDef){
					//one more tile of that type :)
					defCount[i] = defCount[i] + 1;
					containsDef = true;
				}
			}

			if (!containsDef){
				//if the definition hadn't been found already, add it w/ count 1
				neighborDefs.push_back(terrainDef);
				defCount.push_back(1);
			}
		}
	}

	TileDefinition* highestCountDef = nullptr;
	int highestCount = 0;
	for (int i = 0; i < neighborDefs.size(); i++){
		if (defCount[i] > highestCount){
			highestCountDef = neighborDefs[i];
			highestCount = defCount[i];
		}
	}
	if (highestCountDef != nullptr){
		m_edgeToLookFor = highestCountDef;
	}
	return highestCountDef;
}

bool TileNeighborSet::MatchesEdgeDefinition(Tile * tile) const
{
	if (tile == nullptr){
		return false;
	}
	switch (m_compareMode){
	case COMPARE_DEFINITION:
		if (tile->HasTerrainDefinition(m_edgeToLookFor)){
			return true;
		} else if (tile->m_extraInfo->m_cosmeticBaseDefinition == m_edgeToLookFor){
			return true;
		} else{
			return false;
		}
		break;
	case COMPARE_TERRAIN:
		if (tile->GetTerrainLayer() == m_edgeToLookFor->m_terrainLayer){
			return true;
		} else {
			return false;
		}
		break;
	case COMPARE_GROUND:
		if (tile->m_tileDef->m_groundLayer == m_edgeToLookFor->m_groundLayer){
			return true;
		} else {
			return false;
		}
		break;
	}

	
	return false;
}

bool TileNeighborSet::CheckConcaveTopLeft() const
{
	if (!MatchesEdgeDefinition(GetRight()) && !MatchesEdgeDefinition(GetBottom()) && !MatchesEdgeDefinition(GetBottomRight())){
		if (MatchesEdgeDefinition(GetLeft()) && MatchesEdgeDefinition(GetTop()) /*&& MatchesEdgeDefinition(GetTopLeft())*/){
			return true;
		}
		
		if (MatchesEdgeDefinition(GetLeft()) && MatchesEdgeDefinition(GetTopRight())){
			return true;
		}

		if (MatchesEdgeDefinition(GetTop()) && MatchesEdgeDefinition(GetBottomLeft())){
			return true;
		}
	}
	return false;
}

bool TileNeighborSet::CheckConcaveTopRight() const
{
	if (!MatchesEdgeDefinition(GetLeft()) && !MatchesEdgeDefinition(GetBottom()) && !MatchesEdgeDefinition(GetBottomLeft())){
		if (MatchesEdgeDefinition(GetRight()) && MatchesEdgeDefinition(GetTop()) /*&& MatchesEdgeDefinition(GetTopRight())*/){
			return true;
		}

		if (MatchesEdgeDefinition(GetRight()) && MatchesEdgeDefinition(GetTopLeft())){
			return true;
		}

		if (MatchesEdgeDefinition(GetTop()) && MatchesEdgeDefinition(GetBottomRight())){
			return true;
		}
	}
	return false;
}

bool TileNeighborSet::CheckConcaveBottomLeft() const
{
	if (!MatchesEdgeDefinition(GetRight()) && !MatchesEdgeDefinition(GetTop()) && !MatchesEdgeDefinition(GetTopRight())){
		if (MatchesEdgeDefinition(GetLeft()) && MatchesEdgeDefinition(GetBottom()) /*&& MatchesEdgeDefinition(GetBottomLeft())*/){
			return true;
		}
		if (MatchesEdgeDefinition(GetLeft()) && MatchesEdgeDefinition(GetBottomRight())){
			return true;
		}
		if (MatchesEdgeDefinition(GetBottom()) && MatchesEdgeDefinition(GetTopLeft())){
			return true;
		}
	}
	return false;
}

bool TileNeighborSet::CheckConcaveBottomRight() const
{
	
	if (!MatchesEdgeDefinition(GetLeft()) && !MatchesEdgeDefinition(GetTop()) && !MatchesEdgeDefinition(GetTopLeft())){
		//normal case
		if (MatchesEdgeDefinition(GetRight()) && MatchesEdgeDefinition(GetBottom()) /*&& MatchesEdgeDefinition(GetBottomRight())*/){
			return true;
		}
		//assuming that the other tiles will edge correctly too
		if (MatchesEdgeDefinition(GetRight()) && MatchesEdgeDefinition(GetBottomLeft())){
			return true;
		}
		
		if (MatchesEdgeDefinition(GetBottom()) && MatchesEdgeDefinition(GetTopRight())){
			return true;
		}
	}
	

	return false;
}

bool TileNeighborSet::CheckConvexTopLeft() const
{
	if (MatchesEdgeDefinition(GetBottomRight())){
		if (!MatchesEdgeDefinition(GetRight()) && !MatchesEdgeDefinition(GetBottom())){
			//ONLY MATCH the one
			if (!MatchesEdgeDefinition(GetLeft()) && !MatchesEdgeDefinition(GetTop()) && !MatchesEdgeDefinition(GetTopRight()) && !MatchesEdgeDefinition(GetTopLeft())  && !MatchesEdgeDefinition(GetBottomLeft()) ) {
				return true;
			}
		}
	}
	return false;
}

bool TileNeighborSet::CheckConvexTopRight() const
{
	if (MatchesEdgeDefinition(GetBottomLeft())){
		if (!MatchesEdgeDefinition(GetLeft()) && !MatchesEdgeDefinition(GetBottom())){
			//ONLY MATCH the one
			if (!MatchesEdgeDefinition(GetRight()) && !MatchesEdgeDefinition(GetTop()) && !MatchesEdgeDefinition(GetTopRight()) && !MatchesEdgeDefinition(GetTopLeft())  && !MatchesEdgeDefinition(GetBottomRight()) ) {
				return true;
			}
		}
	}
	return false;
}

bool TileNeighborSet::CheckConvexBottomLeft() const
{
	if (MatchesEdgeDefinition(GetTopRight())){
		if (!MatchesEdgeDefinition(GetRight()) && !MatchesEdgeDefinition(GetTop())){
			//ONLY MATCH the one
			if (!MatchesEdgeDefinition(GetLeft()) && !MatchesEdgeDefinition(GetBottom()) && !MatchesEdgeDefinition(GetBottomRight()) && !MatchesEdgeDefinition(GetTopLeft())  && !MatchesEdgeDefinition(GetBottomLeft()) ) {
				return true;
			}
		}
	}
	return false;
}

bool TileNeighborSet::CheckConvexBottomRight() const
{
	if (MatchesEdgeDefinition(GetTopLeft())){
		if (!MatchesEdgeDefinition(GetLeft()) && !MatchesEdgeDefinition(GetTop())){
			//ONLY MATCH the one
			if (!MatchesEdgeDefinition(GetBottom()) && !MatchesEdgeDefinition(GetRight()) && !MatchesEdgeDefinition(GetTopRight()) && !MatchesEdgeDefinition(GetBottomRight())  && !MatchesEdgeDefinition(GetBottomLeft()) ) {
				return true;
			}
		}
	}
	return false;
}

bool TileNeighborSet::CheckPureLeft() const
{
	if (GetLeft() == nullptr || GetRight() == nullptr){
		return false;
	}
	if ( !MatchesEdgeDefinition(GetRight()) && !MatchesEdgeDefinition(GetTop()) && !MatchesEdgeDefinition(GetBottom())){
		//pure left version
		if (MatchesEdgeDefinition(GetLeft())){
			return true;
		} else {
			//assuming that the one to the left will become a single tile
			if (MatchesEdgeDefinition(GetTopLeft()) && MatchesEdgeDefinition(GetBottomLeft())){
				return true;
			}
		}
		
	}
	//if (GetRight()->HasTerrainDefinition(m_edgeToLookFor) && !GetLeft()->HasTerrainDefinition(m_edgeToLookFor)){
	//	return true;
	//}
	return false;
}

bool TileNeighborSet::CheckPureRight() const
{
	if (GetLeft() == nullptr || GetRight() == nullptr){
		return false;
	}
	if ( !MatchesEdgeDefinition(GetLeft()) && !MatchesEdgeDefinition(GetTop()) && !MatchesEdgeDefinition(GetBottom())){
		if (MatchesEdgeDefinition(GetRight())){
			return true;
		} else {
		//assuming that the one to the left will become a single tile
		if (MatchesEdgeDefinition(GetTopRight()) && MatchesEdgeDefinition(GetBottomRight())){
			return true;
		}
	}
	}
	/*if (GetLeft()->HasTerrainDefinition(m_edgeToLookFor) && !GetRight()->HasTerrainDefinition(m_edgeToLookFor)){
		return true;
	}*/
	return false;
}

bool TileNeighborSet::CheckPureTop() const
{
	if (GetBottom() == nullptr || GetTop() == nullptr){
		return false;
	}
	if ( !MatchesEdgeDefinition(GetBottom()) && !MatchesEdgeDefinition(GetLeft()) && !MatchesEdgeDefinition(GetRight())){
		if (MatchesEdgeDefinition(GetTop())){
			return true;
		} else {
			if (MatchesEdgeDefinition(GetTopRight()) && MatchesEdgeDefinition(GetTopLeft())){
				return true;
			}
		}

	}
	/*if (GetBottom()->HasTerrainDefinition(m_edgeToLookFor) && !GetTop()->HasTerrainDefinition(m_edgeToLookFor)){
		return true;
	}*/
	return false;
}

bool TileNeighborSet::CheckPureBottom() const
{
	if (GetBottom() == nullptr || GetTop() == nullptr){
		return false;
	}
	if ( !MatchesEdgeDefinition(GetTop()) && !MatchesEdgeDefinition(GetLeft()) && !MatchesEdgeDefinition(GetRight())){
		if (MatchesEdgeDefinition(GetBottom())){
			return true;
		} else {
			if (MatchesEdgeDefinition(GetBottomRight()) && MatchesEdgeDefinition(GetBottomLeft())){
				return true;
			}
		}
	}
	/*if (GetTop()->HasTerrainDefinition(m_edgeToLookFor) && !GetBottom()->HasTerrainDefinition(m_edgeToLookFor)){
		return true;
	}*/
	return false;
}
