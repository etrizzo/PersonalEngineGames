#include "Map.hpp"
#include "Game.hpp"
#include "Game/MapGenStep.hpp"

Map::~Map()
{
	
	for (unsigned int i = 0; i < m_allEntities.size(); i++){
		delete m_allEntities.at(i);
	}

}

Map::Map(std::string name, MapDefinition* mapDef)
{
	m_name = name;
	m_definition = mapDef;
	//find width/height??
	InitializeTiles();
	RunMapGeneration();
}


void Map::Render()
{
	RenderTiles();
	//RenderEntities();
	
}



void Map::RenderTiles(){
	for (int tileIndex = 0; tileIndex < m_numTiles; tileIndex++){
		Tile tileToRender = m_tiles[tileIndex];
		TileDefinition* definition = tileToRender.m_tileDef;
		const Texture* tileTexture = g_tileSpriteSheet->GetTexture();
		if (!definition->m_isDestructible){
			//m_tileSprites->DrawSpriteAtTexCoords(tileToRender.GetBounds(), definition->m_texCoords);
			g_theRenderer->DrawTexturedAABB2(tileToRender.GetBounds(), *tileTexture, definition->m_texCoords.mins, definition->m_texCoords.maxs, definition->m_spriteTint);
			g_theRenderer->DrawAABB2Outline(tileToRender.GetBounds(),RGBA(0,0,0,32));
		} else {
			AABB2 texCoords = definition->GetTexCoordsAtHealth((int) tileToRender.m_health);
			g_theRenderer->DrawTexturedAABB2(tileToRender.GetBounds(), *tileTexture, texCoords.mins, texCoords.maxs, definition->m_spriteTint);
			g_theRenderer->DrawAABB2Outline(tileToRender.GetBounds(),RGBA(0,0,0,32));
		}
		

	}
}


void Map::Update(float deltaSeconds)
{
	UpdateEntities(deltaSeconds);

	RunPhysics();

	CheckEntityInteractions();

}

void Map::UpdateEntities(float deltaSeconds)
{
	UNUSED(deltaSeconds);
	for (unsigned int entityIndex = 0; entityIndex < m_allEntities.size(); entityIndex++){
		m_allEntities.at(entityIndex)->Update();
	}
}

//void Map::RenderEntities()
//{
//	for (unsigned int entityIndex = 0; entityIndex < m_allEntities.size(); entityIndex++){
//		m_allEntities.at(entityIndex)->Render();
//	}
//}

void Map::RunPhysics()
{
	for (unsigned int entityIndex = 0; entityIndex < m_allEntities.size(); entityIndex++){
		m_allEntities.at(entityIndex)->RunCorrectivePhysics();
	}
}

void Map::CheckEntityInteractions()
{
	

}


int Map::GetWidth() const 
{
	return m_dimensions.x;
}

int Map::GetHeight() const 
{
	return m_dimensions.y;
}


Tile* Map::TileAt(int x, int y) 
{
	return TileAt(IntVector2(x,y));
}

Tile* Map::TileAt(IntVector2 coordinates) 
{
	IntVector2 clampedCoords = IntVector2(ClampInt(coordinates.x, 0, m_dimensions.x -1), ClampInt(coordinates.y, 0, m_dimensions.y - 1));
	return &m_tiles[GetIndexFromCoordinates(clampedCoords.x, clampedCoords.y, m_dimensions.x, m_dimensions.y)];
}

Tile* Map::TileAtFloat(float x, float y)
{
	
	IntVector2 intCoords = IntVector2((int)floor(x), (int)floor(y));
	return TileAt(intCoords);
}

Tile* Map::TileAtFloat(Vector2 floatCoordinates)
{
	IntVector2 intCoords = IntVector2((int) floor(floatCoordinates.x), (int) floor(floatCoordinates.y));
	return TileAt(intCoords);
}

bool Map::HasLineOfSight(const Vector2 & startPos, const Vector2 & endPos, float maxDistance)
{
	Vector2 direction = (endPos - startPos);
	if (direction.GetLength() > maxDistance){
		return false;
	} else {
		RaycastResult2D raycastResult = Raycast(startPos, direction.GetNormalized(), maxDistance);
		if (raycastResult.m_didImpact){
			if (raycastResult.m_impactDistance > direction.GetLength()){
				return true;
			} else {
				return false;
			}
		}
		
	}
	return true;
}

void Map::AddEntityToMap(Entity * entity)
{
	m_allEntities.push_back(entity);
}

void Map::RemoveEntityFromMap(Entity* entity)
{
	for( std::vector<Entity*>::iterator entityIter = m_allEntities.begin(); entityIter != m_allEntities.end(); ++entityIter )
	{
		if( *entityIter == entity )
		{
			m_allEntities.erase( entityIter );
			break;
		}
	}
}

void Map::RemoveEntities()
{
	

}


RaycastResult2D Map::Raycast(const Vector2& startPos, const Vector2& direction, float maxDistance) 
{
	int numSteps = (int)(maxDistance * RAYCAST_STEPS_PER_TILE);
	Vector2 singleStep = direction/RAYCAST_STEPS_PER_TILE;
	Tile previousTile = *TileAtFloat(startPos);
	Tile currentTile = previousTile;
	Vector2 currentPosition = startPos;
	if (previousTile.m_tileDef->m_allowsSight){			//TODO: maybe need to think about this more (clear solid blocks? non-solid vision blocking blocks?)
		return RaycastResult2D(true, startPos, previousTile.m_coordinates, 0.f,Vector2(0.f,0.f));
	}
	for (int stepNumber = 0; stepNumber < numSteps; stepNumber++){
		currentPosition = startPos + (singleStep * (float) stepNumber);
		currentTile = *TileAtFloat(currentPosition);
		if (currentTile.m_coordinates == previousTile.m_coordinates){
			continue;
		} else {
			if (currentTile.m_tileDef->m_allowsSight){
				float distanceFromStart = (currentPosition - startPos).GetLength();
				return RaycastResult2D(true, currentPosition, currentTile.m_coordinates, distanceFromStart, (currentTile.m_coordinates - previousTile.m_coordinates).GetVector2().GetNormalized());
			} else {
				previousTile = currentTile;
			}
		}
	}
	return RaycastResult2D(false, currentPosition, currentTile.m_coordinates, maxDistance, Vector2(0.f,0.f));
}

Tile  Map::GetRandomTileOfType(TileDefinition* tileType) const
{
	int randomTileX = GetRandomIntInRange(1,m_dimensions.x-2);
	int randomTileY = GetRandomIntInRange(1,m_dimensions.y-2);
	int randomTileIndex = GetIndexFromCoordinates(randomTileX, randomTileY, m_dimensions.x, m_dimensions.y);
	while (m_tiles[randomTileIndex].m_tileDef->m_name != tileType->m_name){
		randomTileX = GetRandomIntInRange(1,m_dimensions.x-2);
		randomTileY = GetRandomIntInRange(1,m_dimensions.y-2);
		randomTileIndex = GetIndexFromCoordinates(randomTileX, randomTileY, m_dimensions.x, m_dimensions.y);
	}
	return m_tiles[randomTileIndex];
}

Tile Map::GetRandomBaseTile() const
{
	//return GetRandomTileOfType(m_baseTile);
	return m_tiles[0];
}






void Map::InitializeTiles()
{
	m_dimensions = IntVector2();
	m_dimensions.x = m_definition->m_width.GetRandomInRange();
	m_dimensions.y = m_definition->m_height.GetRandomInRange();
	int numTiles = m_dimensions.x * m_dimensions.y;
	m_tiles.resize(numTiles);
	for (int i = 0; i < numTiles; i++){
		IntVector2 coords = GetCoordinatesFromIndex(i, m_dimensions.x);
		m_tiles[i] = Tile(coords);
	}
}

void Map::RunMapGeneration()
{
	for (MapGenStep* genStep : m_definition->m_generationSteps){
		genStep->Run(*this);
	}
}


RaycastResult2D::RaycastResult2D(bool didImpact, Vector2 impactPos, IntVector2 impactTileCoords, float impactDistance, Vector2 impactNormal)
{
	m_didImpact = didImpact;
	m_impactPosition = impactPos;
	m_impactTileCoordinates = impactTileCoords;
	m_impactDistance = impactDistance;
	m_impactNormal = impactNormal;
}
