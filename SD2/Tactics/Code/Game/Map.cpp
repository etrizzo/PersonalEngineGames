#include "Map.hpp"
#include "Game.hpp"
#include "Game/MapGenStep.hpp"
#include "Game/Actor.hpp"

Map::Map(Encounter* encounter)
{
	m_name = "default";
	m_definition = nullptr;
	m_encounter = encounter;
	//find width/height??
	//InitializeTiles();
	
}

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
	SetCamera();
	RenderTiles();
	
	//RenderCursor();

	//RenderEntities();
	
}



void Map::RenderTiles(){
	g_theRenderer->BindShaderProgram("border");

	for (int tileIndex = 0; tileIndex < m_numTiles; tileIndex++){
		Block blockToRender = m_tiles[tileIndex];
		if (!blockToRender.IsAir()){
			BlockDefinition* definition = blockToRender.m_blockDef;
			const Texture* tileTexture = definition->m_texture;
			if (definition->m_texture == nullptr){
				tileTexture = g_tileSpriteSheet->GetTexture();
			}

			g_theRenderer->BindTexture(*tileTexture);
			g_theRenderer->DrawCube(blockToRender.GetCenter(), blockToRender.GetDimensions(), definition->m_spriteTint, RIGHT, UP, FORWARD, definition->m_uvTop, definition->m_uvSide, definition->m_uvBottom);
			g_theRenderer->ReleaseTexture();
			/*if (g_theGame->m_devMode){
				g_theRenderer->DrawCube(blockToRender.GetTopCenter(), blockToRender.GetDimensions() * .1f, RGBA::YELLOW);
			}*/
		}
	}
	g_theRenderer->ReleaseShader();
}


void Map::Update(float deltaSeconds)
{
	UpdateEntities(deltaSeconds);

	CheckEntityInteractions();

}

void Map::UpdateEntities(float deltaSeconds)
{
	for (unsigned int entityIndex = 0; entityIndex < m_allEntities.size(); entityIndex++){
		m_allEntities.at(entityIndex)->Update(deltaSeconds);
	}

	RemoveEntities();		//removes dying entities
}

void Map::RenderEntities()
{
	g_theRenderer->BindShaderProgram("cutout");
	for (unsigned int entityIndex = 0; entityIndex < m_allEntities.size(); entityIndex++){
		m_allEntities.at(entityIndex)->Render();
	}
	g_theRenderer->ReleaseShader();
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
	return m_dimensions.z;
}


Block* Map::TileAt(int x, int y, int z)
{
	return TileAt(IntVector3(x,y,z));
}

Block* Map::TileAt(IntVector3 coordinates)
{
	IntVector3 clampedCoords = IntVector3(ClampInt(coordinates.x, 0, m_dimensions.x -1), ClampInt(coordinates.y, 0, m_dimensions.y - 1), ClampInt(coordinates.z, 0, m_dimensions.z -1));
	return &m_tiles[GetIndexFromCoordinates(clampedCoords.x, clampedCoords.y, clampedCoords.z, m_dimensions.x, m_dimensions.y, m_dimensions.z)];
}

Block* Map::TileAtFloat(float x, float y, float z)
{
	
	IntVector3 intCoords = IntVector3((int)floor(x), (int)floor(y), (int) floor(z));
	return TileAt(intCoords);
}

Block* Map::TileAtFloat(Vector3 floatCoordinates)
{
	IntVector3 intCoords = IntVector3((int) floor(floatCoordinates.x), (int) floor(floatCoordinates.y), (int) floor(floatCoordinates.z));
	return TileAt(intCoords);
}

Block * Map::GetHighestTileAtXY(int x, int y)
{
	Block* highestBlock = nullptr;
	for (int i = 0; i < m_dimensions.y; i++){
		Block* nextBlock = TileAt(x,i,y);
		if (!nextBlock->IsAir()){
			highestBlock = nextBlock;
		} else {
			break;
		}
	}
	return highestBlock;
}

Block * Map::GetHighestTileAtXY(IntVector2 coords)
{
	return GetHighestTileAtXY(coords.x, coords.y);
}

float Map::GetHeightAtXY(IntVector2 coords)
{
	return GetHighestTileAtXY(coords)->GetTopCenter().y;
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
	for (int entityIndex = 0; entityIndex < (int) m_allEntities.size(); entityIndex++){
		if (m_allEntities[entityIndex] != nullptr && m_allEntities[entityIndex]->IsAboutToBeDeleted()){
			m_allEntities.erase(m_allEntities.begin() + entityIndex);
			//delete(entity);
		}
	}

}


RaycastResult2D Map::Raycast(const Vector2& startPos, const Vector2& direction, float maxDistance) 
{
	UNIMPLEMENTED();
	UNUSED(startPos);
	UNUSED(direction);
	UNUSED(maxDistance);
	//int numSteps = (int)(maxDistance * RAYCAST_STEPS_PER_TILE);
	//Vector2 singleStep = direction/RAYCAST_STEPS_PER_TILE;
	//Block previousTile = *TileAtFloat(startPos);
	//Block currentTile = previousTile;
	//Vector2 currentPosition = startPos;
	//if (previousTile.m_blockDef->m_allowsSight){			//TODO: maybe need to think about this more (clear solid blocks? non-solid vision blocking blocks?)
	//	return RaycastResult2D(true, startPos, previousTile.m_coordinates, 0.f,Vector2(0.f,0.f));
	//}
	//for (int stepNumber = 0; stepNumber < numSteps; stepNumber++){
	//	currentPosition = startPos + (singleStep * (float) stepNumber);
	//	currentTile = *TileAtFloat(currentPosition);
	//	if (currentTile.m_coordinates == previousTile.m_coordinates){
	//		continue;
	//	} else {
	//		if (currentTile.m_tileDef->m_allowsSight){
	//			float distanceFromStart = (currentPosition - startPos).GetLength();
	//			return RaycastResult2D(true, currentPosition, currentTile.m_coordinates, distanceFromStart, (currentTile.m_coordinates - previousTile.m_coordinates).GetVector2().GetNormalized());
	//		} else {
	//			previousTile = currentTile;
	//		}
	//	}
	//}
	//return RaycastResult2D(false, currentPosition, currentTile.m_coordinates, maxDistance, Vector2(0.f,0.f));
	return RaycastResult2D();
}

Block  Map::GetRandomTileOfType(BlockDefinition* tileType) const
{
	int randomTileX = GetRandomIntInRange(1,m_dimensions.x-2);
	int randomTileY = GetRandomIntInRange(1,m_dimensions.y-2);
	int randomTileIndex = GetIndexFromCoordinates(randomTileX, randomTileY, m_dimensions.x, m_dimensions.y);
	while (m_tiles[randomTileIndex].m_blockDef->m_name != tileType->m_name){
		randomTileX = GetRandomIntInRange(1,m_dimensions.x-2);
		randomTileY = GetRandomIntInRange(1,m_dimensions.y-2);
		randomTileIndex = GetIndexFromCoordinates(randomTileX, randomTileY, m_dimensions.x, m_dimensions.y);
	}
	return m_tiles[randomTileIndex];
}

Block Map::GetRandomBaseTile() const
{
	//return GetRandomTileOfType(m_baseTile);
	return m_tiles[0];
}


void Map::SetCamera()
{
	if (m_camera != nullptr){
		g_theRenderer->SetCamera(m_camera);
	}
}


void Map::FillSeededDistanceMap(Heatmap & distMap, const Block * destinationBlock, const int & maxRange, const int & jumpHeight, bool blockActors)
{
	std::vector<Block> openBlocks;
	for (int x = 0; x < m_dimensions.x; x++){
		for (int y = 0; y < m_dimensions.y; y++){
			if ( distMap.GetHeat(x,y) == 0.f){
				openBlocks.push_back(*GetHighestTileAtXY(x,y));
			}
		}
	}

	Block currentBlock;
	Block neighbors[4];
	const Block firstSeed = openBlocks[0];
	while (!openBlocks.empty()){
		currentBlock = openBlocks.back();
		openBlocks.pop_back();
		//if you've already found destination, break
		if (destinationBlock != nullptr && destinationBlock->m_coordinates == currentBlock.m_coordinates){
			break;
		}

		//otherwise, get 4 neighbors
		GetManhattanNeighbors(currentBlock.GetXYCoords(), neighbors);


		for (int direction = MANHATTAN_NORTH; direction < NUM_MANHATTAN_DIRS; direction++){
			Block currentNeighbor = neighbors[direction];
			if (currentNeighbor.m_blockDef != nullptr){
				float costToAdd = 1.f;
				int heightDifference = abs(currentNeighbor.m_coordinates.y - currentBlock.m_coordinates.y);
				if (heightDifference > jumpHeight){
					costToAdd = 9999.f;
				}
				if (blockActors){
					Entity* ent = GetEntityInColumn(currentNeighbor.GetXYCoords());
					if (ent != nullptr){
						costToAdd = 9999.f;		//can't move on other actors
					}
				}

				float oldHeatVal = distMap.GetHeat(currentNeighbor.GetXYCoords());
				float newHeatVal = Min(oldHeatVal, distMap.GetHeat(currentBlock.GetXYCoords()) + costToAdd);
				if (newHeatVal < oldHeatVal && newHeatVal < (float) maxRange){
					distMap.SetHeat(currentNeighbor.GetXYCoords(), newHeatVal);
					openBlocks.insert(openBlocks.begin(), currentNeighbor);
				}
				//}
			}
		}
	}
}

void Map::GetManhattanNeighbors(IntVector2 XYpos, Block(&neighbors)[4])
{
	IntVector2 stepNorth = XYpos + STEP_NORTH;
	IntVector2 stepEast = XYpos + STEP_EAST;
	IntVector2 stepSouth = XYpos + STEP_SOUTH;
	IntVector2 stepWest = XYpos + STEP_WEST;
	if (IsOnBoard(stepNorth)){
		neighbors[0] = *GetHighestTileAtXY(stepNorth);
	}
	if (IsOnBoard(stepEast)){
		neighbors[1] = *GetHighestTileAtXY(stepEast);
	}
	if (IsOnBoard(stepSouth)){
		neighbors[2] = *GetHighestTileAtXY(stepSouth);
	}
	if (IsOnBoard(stepWest)){
		neighbors[3] = *GetHighestTileAtXY(stepWest);
	}
}

Entity * Map::GetEntityInColumn(const IntVector2 & pos)
{
	for(Entity* entity : m_allEntities){
		if (entity->m_position == pos){
			return entity;
		}
	}
	return nullptr;
}

Vector3 Map::GetWorldPosition(Vector2 xyPos)
{
	IntVector2 coords = IntVector2((int) xyPos.x, (int) xyPos.y);
	float height = GetHeightAtXY(coords) ;
	return Vector3(xyPos.x,  height, xyPos.y);
}

Vector3 Map::GetWorldPosition(IntVector2 xyCoords)
{
	float height = GetHeightAtXY(xyCoords) ;
	return Vector3((float)xyCoords.x, height, (float)xyCoords.y);
}

bool Map::IsOnBoard(IntVector2 xyPos)
{
	if (xyPos.x < 0 || xyPos.x >= m_dimensions.z){
		return false;
	}
	if (xyPos.y < 0 || xyPos.y >= m_dimensions.y){
		return false;
	}
	return true;
}

std::vector<IntVector2> Map::GetPath(const IntVector2& startPos, const IntVector2& endPos, const Heatmap& distanceMap)
{
	std::vector<IntVector2> path = std::vector<IntVector2>();
	IntVector2 blockPos = endPos;
	while (blockPos != startPos && distanceMap.GetHeat(blockPos) != 0.f ){		//do we need to account for multiple seeds?
		path.push_back(blockPos);
		blockPos = distanceMap.GetCoordsOfMinNeighbor(blockPos);
	}
	//add starting pos??
	path.push_back(startPos);

	std::reverse(path.begin(), path.end());
	return path;
}

void Map::ApplyDamageAOE(IntVector2 targetLocation, int range, int verticalRange, int damageToApply)
{
	float height = GetHeightAtXY(targetLocation);
	for (int x = targetLocation.x - range; x <= targetLocation.x + range; x++){
		for (int y = targetLocation.y - range; y <= targetLocation.y + range; y++){
			IntVector2 coords = IntVector2(x,y);
			float targetHeight = GetHeightAtXY(coords);
			if (abs(height - targetHeight) <= (float) verticalRange){
				Actor* actor = (Actor*) GetEntityInColumn(coords);
				if (actor != nullptr){
					actor->TakeDamage(damageToApply);
				}
				g_theGame->AddFlyoutText(std::to_string(damageToApply), Vector3((float) x, GetHeightAtXY(coords), (float) y), .2f, 2.5f, RGBA::WHITE, .5f);
			}
		}
	}
}

int Map::GetHeightFromHeightMap(Image* img, int x, int y)
{
	float color = img->GetTexel(x,y).r;		//gets the r val from the texel at x,y
	color = color / 255.f;		//gets from 0 to 1
	float floatHeight = color * m_dimensions.y;		//gets height as  percentage of max height y
	return (int) floorf(floatHeight);
}

void Map::ClampCameraToMap()
{
	/*float minX = m_camera.m_numTilesInViewVertically*WINDOW_ASPECT * .5f;
	float maxX = m_width - minX;
	float minY = m_camera.m_numTilesInViewVertically * .5f;
	float maxY = m_height - minY;*/

	//float minX = 0.f;
	//float maxX = (float) m_dimensions.x;
	//float minY = 0.f;
	//float maxY = (float) m_dimensions.y;

	//if (m_camera.m_bottomLeft.x < minX){
	//	m_camera.m_bottomLeft.x = minX;
	//}
	//if (m_camera.m_bottomLeft.y < minY){
	//	m_camera.m_bottomLeft.y = minY;
	//}
	//if (m_camera.m_topRight.x > maxX){
	//	m_camera.m_topRight.x = maxX;
	//}
	//if (m_camera.m_topRight.y > maxY){
	//	m_camera.m_topRight.y = maxY;
	//}

}

void Map::InitializeTiles()
{
	//m_dimensions = g_gameConfigBlackboard.GetValue("mapDimensions", IntVector3(5,5,5));
	Image* heightMap = new Image("Data/Images/heightMap.png");
	IntVector2 imageDims = heightMap->GetDimensions();
	m_dimensions = IntVector3(imageDims.x, 32, imageDims.y);		
	m_numTiles = m_dimensions.x * m_dimensions.y * m_dimensions.z;
	m_tiles.resize(m_numTiles);
	BlockDefinition* grassDef = new BlockDefinition();
	grassDef->MakeGrassBlockDefinition();
	for (int i = 0; i < m_numTiles; i++){
		IntVector3 coords = GetCoordinatesFromIndex(i, m_dimensions.x, m_dimensions.y);
		int coordHeight = GetHeightFromHeightMap(heightMap, coords.x, coords.z);
		if (coords.y < coordHeight){
			m_tiles[i] = Block(coords, grassDef);
		} else {
			m_tiles[i] = Block(coords);
		}
	}
}

bool Map::LoadFromHeightMap(std::string fileName, const int height)
{
	//m_dimensions = g_gameConfigBlackboard.GetValue("mapDimensions", IntVector3(5,5,5));
	Image* heightMap = new Image("Data/Images/" + fileName);
	IntVector2 imageDims = heightMap->GetDimensions();

	if (imageDims == IntVector2(0,0)){		//did not find the images
		return false;
	}

	m_dimensions = IntVector3(imageDims.x, height, imageDims.y);		
	m_numTiles = m_dimensions.x * m_dimensions.y * m_dimensions.z;
	m_tiles.resize(m_numTiles);
	BlockDefinition* grassDef = new BlockDefinition();
	grassDef->MakeGrassBlockDefinition();
	for (int i = 0; i < m_numTiles; i++){
		IntVector3 coords = GetCoordinatesFromIndex(i, m_dimensions.x, m_dimensions.y);
		int coordHeight = GetHeightFromHeightMap(heightMap, coords.x, coords.z);
		if (coords.y < coordHeight){
			m_tiles[i] = Block(coords, grassDef);
		} else {
			m_tiles[i] = Block(coords);
		}
	}
	return true;
}

void Map::RunMapGeneration()
{
	for (MapGenStep* genStep : m_definition->m_generationSteps){
		genStep->Run(*this);
	}
}

OldCamera::OldCamera(Vector2 position, float orientation, float zoomFactor)
{
	m_worldPos = position;
	m_orientationDegrees = orientation;
	m_numTilesInViewVertically = zoomFactor;
}

void OldCamera::Update(float deltaSeconds, Vector2 playerPosition)
{
	deltaSeconds;
	m_worldPos = playerPosition;
	m_orientationDegrees = 0.f;
	m_numTilesInViewVertically = 7.f;
}

RaycastResult2D::RaycastResult2D(bool didImpact, Vector2 impactPos, IntVector2 impactTileCoords, float impactDistance, Vector2 impactNormal)
{
	m_didImpact = didImpact;
	m_impactPosition = impactPos;
	m_impactTileCoordinates = impactTileCoords;
	m_impactDistance = impactDistance;
	m_impactNormal = impactNormal;
}
