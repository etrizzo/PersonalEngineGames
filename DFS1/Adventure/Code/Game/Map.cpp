#include "Map.hpp"
#include "Game.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Game/MapGenStep.hpp"
#include "Game/ActorDefinition.hpp"
#include "Game/ProjectileDefinition.hpp"
#include "Game/Actor.hpp"
#include "Game/Player.hpp"
#include "Game/Projectile.hpp"
#include "Game/Portal.hpp"
#include "Game/PortalDefinition.hpp"
#include "Game/Item.hpp"
#include "Game/ItemDefinition.hpp"
#include "Game/Adventure.hpp"



Map::~Map()
{
	for (unsigned int i = 0; i < m_allEntities.size(); i++){
		if (m_allEntities[i] != g_theGame->m_player){
			delete m_allEntities[i];
		}
	}
}

Map::Map(std::string name, MapDefinition* mapDef, int difficulty)
{
	m_difficulty = difficulty;
	m_scene = new RenderScene2D();
	m_name = name;
	m_mapDef = mapDef;
	m_allEntities = std::vector<Entity*>();
	m_allActors = std::vector<Actor * >();
	m_allProjectiles = std::vector<Projectile*>();
	m_allPortals = std::vector<Portal*>();
	m_allItems = std::vector<Item*>();
	//m_camera = new Camera2D(0.f, ZOOM_FACTOR);
	g_theGame->m_camera->m_transform.SetLocalPosition(Vector3::ZERO);
	g_theGame->m_camera->m_orthographicSize = ZOOM_FACTOR;

	//find width/height??
	InitializeTiles();
	RunMapGeneration();
	CreateTileRenderable();
}


void Map::Render()
{
	SetCamera();
	//RenderTiles();
	//RenderEntities();
}



void Map::RenderTiles(){
	static std::vector<Vertex3D_PCU> tileVerts;
	tileVerts.clear();
	AABB2 camBounds = GetCameraBounds();
	Vector2 spacing = Vector2(1.f,1.f);
	Tile* minTile = TileAtFloat(camBounds.mins - spacing);
	Tile* maxTile = TileAtFloat(camBounds.maxs + spacing);

	IntVector2 minTileCoords = IntVector2(0,0);
	IntVector2 maxTileCoords = m_dimensions;
	if (minTile != nullptr){
		 minTileCoords= minTile->m_coordinates;
	}
	if (maxTile != nullptr){
		maxTileCoords = maxTile->m_coordinates;
	}
	for ( int x = minTileCoords.x; x < maxTileCoords.x; x++){
		for (int y = minTileCoords.y; y < maxTileCoords.y; y++){
			//push vertices
			Tile* tileToRender = TileAt(x,y);
			AABB2 bounds = tileToRender->GetBounds();
			RGBA color = tileToRender->m_tileDef->m_spriteTint;
			AABB2 texCoords = tileToRender->m_tileDef->GetTexCoords(tileToRender->m_extraInfo->m_variant);
			tileVerts.push_back(Vertex3D_PCU(Vector2(bounds.mins.x, bounds.mins.y), color, Vector2(texCoords.mins.x, texCoords.maxs.y)));
			tileVerts.push_back(Vertex3D_PCU(Vector2(bounds.maxs.x, bounds.mins.y), color, Vector2(texCoords.maxs.x, texCoords.maxs.y)));
			tileVerts.push_back(Vertex3D_PCU(Vector2(bounds.maxs.x, bounds.maxs.y), color, Vector2(texCoords.maxs.x, texCoords.mins.y)));
			tileVerts.push_back(Vertex3D_PCU(Vector2(bounds.mins.x, bounds.maxs.y), color, Vector2(texCoords.mins.x, texCoords.mins.y)));
		}
	}
	Vertex3D_PCU* vertArray = &tileVerts[0];
	const Texture* tileTexture = g_tileSpriteSheet->GetTexture();
	g_theRenderer->BindTexture(*tileTexture);
	g_theRenderer->DrawMeshImmediate(vertArray, tileVerts.size(), PRIMITIVE_QUADS);
	g_theRenderer->ReleaseTexture();
}

bool Map::RunBubbleSortPassOnEntities()
{
	bool sorted = true;
	if (m_allEntities.size() > 1){
		for (unsigned int checkIndex = 0; checkIndex < m_allEntities.size() - 1; checkIndex++){
			Entity* currentEntity = m_allEntities[checkIndex];
			Entity* nextEntity = m_allEntities[checkIndex+1];
			if (nextEntity != nullptr && currentEntity != nullptr){
				if (currentEntity->GetPosition().y < nextEntity->GetPosition().y){
					sorted = false;
					Entity* temp = m_allEntities[checkIndex];
					m_allEntities[checkIndex] = m_allEntities[checkIndex+1];
					m_allEntities[checkIndex+1] = temp;
				}
			}
		}
	}
	return sorted;
}

void Map::SortAllEntities()
{
	while (!RunBubbleSortPassOnEntities());
}


void Map::Update(float deltaSeconds)
{
	

	UpdateEntities(deltaSeconds);

	RunPhysics();

	CheckEntityInteractions();
	ResetPortals();

}

void Map::UpdateEntities(float deltaSeconds)
{
	for (unsigned int entityIndex = 0; entityIndex < m_allEntities.size(); entityIndex++){
		Entity* currentEntity = m_allEntities.at(entityIndex);
		if (currentEntity != nullptr && !currentEntity->m_aboutToBeDeleted){
			m_allEntities.at(entityIndex)->Update(deltaSeconds);
		}
	}
	

}

void Map::RenderEntities()
{
	RunBubbleSortPassOnEntities();
	for (unsigned int entityIndex = 0; entityIndex < m_allEntities.size(); entityIndex++){
		Entity* currentEntity = m_allEntities[entityIndex];
		if (currentEntity != nullptr && !currentEntity->m_aboutToBeDeleted){
			m_allEntities.at(entityIndex)->Render();
		}
	}
}

void Map::RunPhysics()
{
	for (unsigned int entityIndex = 0; entityIndex < m_allEntities.size(); entityIndex++){
		Entity* currentEntity = m_allEntities[entityIndex];
		if (currentEntity != nullptr && !currentEntity->m_aboutToBeDeleted){
			currentEntity->RunCorrectivePhysics();
		}
	}

}

void Map::CheckEntityInteractions()
{
	for (unsigned int projIndex = 0; projIndex < m_allProjectiles.size(); projIndex++){
		Projectile* projectile = m_allProjectiles[projIndex];
		for (unsigned int actorIndex = 0; actorIndex < m_allActors.size(); actorIndex++){
			Actor* actor = m_allActors[actorIndex];
			if (projectile->m_faction.compare(actor->m_faction) && DoDiscsOverlap(projectile->m_physicsDisc, actor->m_physicsDisc)){
				projectile->m_aboutToBeDeleted = true;
				actor->TakeDamage(projectile->m_damage);
			}
		}
	}

	for (unsigned int portalIndex = 0; portalIndex < m_allPortals.size(); portalIndex++){
		Portal* portal = m_allPortals[portalIndex];
		for (unsigned int actorIndex = 0; actorIndex < m_allActors.size(); actorIndex++){
			Actor* actor = m_allActors[actorIndex];
			if (DoDiscsOverlap(portal->m_physicsDisc, actor->m_physicsDisc)){
			//if (portal->m_physicsDisc.IsPointInside(actor->m_position)){				//need to fix how portals reset to do center teleport. 
				if (portal->m_isReadyToTeleport && portal->IsSameFaction(actor)){
					portal->Teleport(actor);
				}
			}
		}
	}


	if (g_theGame->m_player != nullptr){
		for (unsigned int itemIndex = 0; itemIndex < m_allItems.size(); itemIndex++){
			Item* item = m_allItems[itemIndex];
			if (DoDiscsOverlap(item->m_physicsDisc, g_theGame->m_player->m_physicsDisc)){
				//Item* copyItem = new Item(*item);		//add a copy so that the one on the map can get deleted safely
				g_theGame->m_player->AddItemToInventory(item);
				m_allItems[itemIndex] = nullptr;
				item->m_aboutToBeDeleted = true;
			}

			//for all actors being able to pick things up:

			//for (unsigned int actorIndex = 0; actorIndex < m_allActors.size(); actorIndex++){
			//	Actor* actor = m_allActors[actorIndex];
			//	if (item->m_faction.compare(actor->m_faction) && DoDiscsOverlap(item->m_physicsDisc, actor->m_physicsDisc)){
			//		Item* copyItem = new Item(*item);		
			//		m_player->AddItemToInventory(copyItem);
			//		item->m_aboutToBeDeleted = true;
			//	}
			//}
		}
	}

}

void Map::RemoveDoomedEntities()
{
	for (unsigned int entityIndex = m_allEntities.size()-1; entityIndex < m_allEntities.size(); entityIndex--){
		Entity* entity = m_allEntities.at(entityIndex);
		if (entity == nullptr || entity->IsAboutToBeDeleted()){
			RemoveAtFast(m_allEntities, entityIndex);
			if (entity!= nullptr){
				g_theGame->m_currentState->m_currentAdventure->GetScene()->RemoveRenderable(entity->m_renderable);
			}
		}
	}

	for (unsigned int entityIndex = m_allActors.size()-1; entityIndex < m_allActors.size(); entityIndex--){
		Actor* entity = m_allActors.at(entityIndex);
		if (entity->IsAboutToBeDeleted()){
 			RemoveAtFast(m_allActors, entityIndex);
			delete(entity);
		}
	}

	for (unsigned int entityIndex = m_allProjectiles.size()-1; entityIndex < m_allProjectiles.size(); entityIndex--){
		Projectile* entity = m_allProjectiles.at(entityIndex);
		if (entity->IsAboutToBeDeleted()){
			RemoveAtFast(m_allProjectiles, entityIndex);
			delete(entity);
		}
	}

	for (unsigned int entityIndex = 0; entityIndex < m_allPortals.size(); entityIndex++){
		Entity* entity = m_allPortals.at(entityIndex);
		if (entity->IsAboutToBeDeleted()){
			m_allPortals.erase(m_allPortals.begin() + entityIndex);
			delete(entity);
		}
	}

	for (unsigned int itemIndex = 0; itemIndex < m_allItems.size(); itemIndex++){
		Entity* entity = m_allItems.at(itemIndex);
		if (entity == nullptr){
			m_allItems.erase(m_allItems.begin() + itemIndex);
		}else if (entity->IsAboutToBeDeleted()){
			m_allItems.erase(m_allItems.begin() + itemIndex);
			delete(entity);
		}
	}
}

void Map::StartMusic()
{
	m_musicPlayback = g_theAudio->PlaySound(m_mapDef->m_mapMusic);
	g_theGame->m_currentState->m_soundtrackPlayback = m_musicPlayback;
}

void Map::StopMusic()
{
	g_theAudio->StopSound(m_musicPlayback);
}


std::string Map::GetName() const
{
	return m_name;
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
	int index = GetIndexFromCoordinates(coordinates.x, coordinates.y, m_dimensions.x, m_dimensions.y);
	if (index >= m_numTiles || index < 0){
		return nullptr;
	}
	return &m_tiles[index];
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
	m_scene->AddRenderable(entity->m_renderable);
}

void Map::RemoveEntityFromMap(Entity* entity)
{
	m_scene->RemoveRenderable(entity->m_renderable);
	for( std::vector<Entity*>::iterator entityIter = m_allEntities.begin(); entityIter != m_allEntities.end(); ++entityIter )
	{
		if( *entityIter == entity )
		{
			m_allEntities.erase( entityIter );
			break;
		}
	}
}

void Map::RemoveActorFromMap(Actor * actor)
{
	m_scene->RemoveRenderable(actor->m_renderable);
	for( std::vector<Actor*>::iterator entityIter = m_allActors.begin(); entityIter != m_allActors.end(); ++entityIter )
	{
		if( *entityIter == actor )
		{
			m_allActors.erase( entityIter );
			break;
		}
	}

	Entity* entityActor = (Entity*) actor;
	for( std::vector<Entity*>::iterator entityIter = m_allEntities.begin(); entityIter != m_allEntities.end(); ++entityIter )
	{
		if( *entityIter == entityActor )
		{
			m_allEntities.erase( entityIter );
			break;
		}
	}

}

void Map::AddActorToMap(Actor * actor)
{
	m_allActors.push_back(actor);
	m_allEntities.push_back(actor);
	m_scene->AddRenderable(actor->m_renderable);
}

void Map::RemoveEntities()
{


}

void Map::FillSeededDistanceMap(Heatmap& distMap,const Tile* destinationTile, const float& maxRange, const float& straightChance, const float& walkingPenalty, const float& swimPenalty, const float flyPenalty, const float& unWalkablePenalty)
{
	std::vector<Tile> openTiles;
	for (int i = 0; i < m_numTiles; i++){
		if ( distMap.GetHeat(i) == 0){
			openTiles.push_back(m_tiles[i]);
		}
	}

	Tile currentTile;
	Tile neighbors[4];
	const Tile firstSeed = openTiles[0];
	eManhattanDir currentDirection; //= GetMostSignificantDirection(firstSeed.m_coordinates, destinationTile->m_coordinates);
	while (!openTiles.empty()){
		currentTile = openTiles.back();
		openTiles.pop_back();
		if (destinationTile != nullptr && destinationTile->m_coordinates == currentTile.m_coordinates){
			break;
		}
		GetManhattanNeighbors(currentTile.m_coordinates, neighbors);

		if (distMap.GetHeat(currentTile.m_coordinates) > 0.f){
			eManhattanDir minDirection = currentDirection;
			float minCost = 9999.f;
			for (int direction = MANHATTAN_NORTH; direction < NUM_MANHATTAN_DIRS; direction++){
				Tile currentNeighbor = neighbors[direction];
				if (currentNeighbor.m_tileDef != nullptr){
					if (distMap.GetHeat(currentNeighbor.m_coordinates) < minCost){
						minDirection = (eManhattanDir) direction;
						minCost = distMap.GetHeat(currentNeighbor.m_coordinates);
					}
				}
			}
			currentDirection =  ReverseDirection(minDirection);
		} else {
			if (destinationTile != nullptr){
				currentDirection = GetMostSignificantDirection(firstSeed.m_coordinates, destinationTile->m_coordinates);
			} else {
				currentDirection = MANHATTAN_EAST;
			}
		}
		bool straightenPath = CheckRandomChance(straightChance);
		for (int direction = MANHATTAN_NORTH; direction < NUM_MANHATTAN_DIRS; direction++){
			Tile currentNeighbor = neighbors[direction];
			if (currentNeighbor.m_tileDef != nullptr){
				float costToAdd = 9999.f;
				if (currentNeighbor.m_tileDef->m_allowsWalking){
					costToAdd = walkingPenalty;
				}else if (currentNeighbor.m_tileDef->m_allowsSwimming){
					costToAdd = swimPenalty;
				} else if (currentNeighbor.m_tileDef->m_allowsFlying){
					costToAdd = flyPenalty;
				} else if (distMap.GetHeat(currentNeighbor.m_coordinates) >= 9999.f){
					costToAdd = unWalkablePenalty;
				}
				
				if (costToAdd < 9999.f){
					/*if (straightenPath){
						if (currentDirection == direction){
							costToAdd-=.3f;
						} else {
							costToAdd+=.3f;
						}
					}*/
					float oldHeatVal = distMap.GetHeat(currentNeighbor.m_coordinates);
					float newHeatVal = Min(oldHeatVal, distMap.GetHeat(currentTile.m_coordinates) + costToAdd);
					if (newHeatVal < oldHeatVal && newHeatVal < maxRange){
						distMap.SetHeat(currentNeighbor.m_coordinates, newHeatVal);
						openTiles.insert(openTiles.begin(), currentNeighbor);
					}
				}
			}
		}
	}
}

eManhattanDir Map::GetMostSignificantDirection(const IntVector2 startCoords, const IntVector2 endCoords)
{
	IntVector2 distance = endCoords - startCoords;
	if (abs(distance.x) > abs(distance.y)){		//more on east/west
		if (distance.x > 0){
			return MANHATTAN_EAST;
		} else {
			return MANHATTAN_WEST;
		}
	} else {
		if (distance.y > 0){
			return MANHATTAN_NORTH;
		} else {
			return MANHATTAN_SOUTH;
		}
	}
}

eManhattanDir Map::ReverseDirection(eManhattanDir incomingDir)
{
	switch (incomingDir){
	case (MANHATTAN_NORTH):
		return MANHATTAN_SOUTH;
		break;
	case (MANHATTAN_SOUTH):
		return MANHATTAN_NORTH;
		break;
	case (MANHATTAN_EAST):
		return MANHATTAN_WEST;
		break;
	case (MANHATTAN_WEST):
		return MANHATTAN_EAST;
		break;
	default:
		return NUM_MANHATTAN_DIRS;
		break;
		
	}
}

void Map::GetManhattanNeighbors(Tile * tile, Tile(&neighbors)[4])
{
	IntVector2 coords = tile->m_coordinates;
	GetManhattanNeighbors(coords, neighbors);
}

void Map::GetManhattanNeighbors(IntVector2 coords, Tile(&neighbors)[4])
{
	Tile* checkTile;
	checkTile = TileAt(coords + STEP_NORTH);
	if (checkTile != nullptr){
		neighbors[MANHATTAN_NORTH] = *(checkTile);
	}
	checkTile = TileAt(coords + STEP_WEST);
	if (checkTile != nullptr){
		neighbors[MANHATTAN_WEST] = *(checkTile);
	}

	checkTile = TileAt(coords + STEP_EAST);
	if (checkTile != nullptr){
		neighbors[MANHATTAN_EAST] = *(checkTile);
	}
	checkTile = TileAt(coords + STEP_SOUTH);
	if (checkTile != nullptr){
		neighbors[MANHATTAN_SOUTH] = *(checkTile);
	}
}

void Map::GetNeighbors(Tile * tile, Tile (&neighbors)[8])
{
	IntVector2 coords = tile->m_coordinates;
	GetNeighbors(coords, neighbors);
}

void Map::GetNeighbors(IntVector2 coords, Tile (&neighbors)[8])
{
	Tile* checkTile;
	checkTile = TileAt(coords + STEP_NORTHWEST);
	if (checkTile != nullptr){
		neighbors[0] = *(checkTile);
	}

	checkTile = TileAt(coords + STEP_NORTH);
	if (checkTile != nullptr){
		neighbors[1] = *(checkTile);
	}

	checkTile = TileAt(coords + STEP_NORTHEAST);
	if (checkTile != nullptr){
		neighbors[2] = *(checkTile);
	}

	checkTile = TileAt(coords + STEP_WEST);
	if (checkTile != nullptr){
		neighbors[3] = *(checkTile);
	}

	checkTile = TileAt(coords + STEP_EAST);
	if (checkTile != nullptr){
		neighbors[4] = *(checkTile);
	}


	checkTile = TileAt(coords + STEP_SOUTHWEST);
	if (checkTile != nullptr){
		neighbors[5] = *(checkTile);
	}

	checkTile = TileAt(coords + STEP_SOUTH);
	if (checkTile != nullptr){
		neighbors[6] = *(checkTile);
	}

	checkTile = TileAt(coords + STEP_SOUTHEAST);
	if (checkTile != nullptr){
		neighbors[7] = *(checkTile);
	}
}


RaycastResult2D Map::Raycast(const Vector2& startPos, const Vector2& direction, float maxDistance) 
{
	int numSteps = (int)(maxDistance * RAYCAST_STEPS_PER_TILE);
	Vector2 singleStep = direction/RAYCAST_STEPS_PER_TILE;
	Tile previousTile = *TileAtFloat(startPos);
	Tile currentTile = previousTile;
	Vector2 currentPosition = startPos;
	if (!previousTile.m_tileDef->m_allowsSight){			//TODO: maybe need to think about this more (clear solid blocks? non-solid vision blocking blocks?)
		return RaycastResult2D(true, startPos, previousTile.m_coordinates, 0.f,Vector2(0.f,0.f));
	}
	for (int stepNumber = 0; stepNumber < numSteps; stepNumber++){
		currentPosition = startPos + (singleStep * (float) stepNumber);
		Tile* nextTilePointer = TileAtFloat(currentPosition);
		if (nextTilePointer!= nullptr && nextTilePointer->m_coordinates == previousTile.m_coordinates){
			continue;
		} else {
			if (nextTilePointer == nullptr || !nextTilePointer->m_tileDef->m_allowsSight){
				float distanceFromStart = (currentPosition - startPos).GetLength();
				return RaycastResult2D(true, currentPosition, currentTile.m_coordinates, distanceFromStart, (currentTile.m_coordinates - previousTile.m_coordinates).GetVector2().GetNormalized());
			} else {
				currentTile = * nextTilePointer;
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
	int tries = 0;
	while (m_tiles[randomTileIndex].m_tileDef->m_name != tileType->m_name && tries < 1000){
		randomTileX = GetRandomIntInRange(1,m_dimensions.x-2);
		randomTileY = GetRandomIntInRange(1,m_dimensions.y-2);
		randomTileIndex = GetIndexFromCoordinates(randomTileX, randomTileY, m_dimensions.x, m_dimensions.y);
		tries++;
	}
	return m_tiles[randomTileIndex];
}

Tile Map::GetSpawnTileOfType(TileDefinition * tileType) const
{
	int randomTileX = GetRandomIntInRange(1,m_dimensions.x-2);
	int randomTileY = GetRandomIntInRange(1,m_dimensions.y-2);
	int randomTileIndex = GetIndexFromCoordinates(randomTileX, randomTileY, m_dimensions.x, m_dimensions.y);
	int tries = 0;
	Tile randomTile = m_tiles[randomTileIndex];
	while (((randomTile.m_tileDef->m_name != tileType->m_name) || (randomTile.m_extraInfo->m_spawnedOn)) && tries < 1000){
		randomTileX = GetRandomIntInRange(1,m_dimensions.x-2);
		randomTileY = GetRandomIntInRange(1,m_dimensions.y-2);
		randomTileIndex = GetIndexFromCoordinates(randomTileX, randomTileY, m_dimensions.x, m_dimensions.y);
		randomTile = m_tiles[randomTileIndex];
		tries++;
	}
	randomTile.m_extraInfo->m_spawnedOn = true;
	return randomTile;
}

Tile Map::GetRandomBaseTile() const
{
	return GetRandomTileOfType(m_mapDef->m_defaultTile);
	//return m_tiles[0];
}

Tile Map::GetRandomTile() const
{
	IntVector2 coords = GetRandomTileCoords();
	int index = GetIndexFromCoordinates(coords.x, coords.y, m_dimensions.x, m_dimensions.y);
	return m_tiles[index];
}

IntVector2 Map::GetRandomTileCoords() const
{
	int randomTileX = GetRandomIntInRange(1,m_dimensions.x-2);
	int randomTileY = GetRandomIntInRange(1,m_dimensions.y-2);
	return IntVector2(randomTileX, randomTileY);
}

bool Map::IsCoordinateOnMap(IntVector2 coordinate)
{
	if (coordinate.x >= 0 && coordinate.x < m_dimensions.x){
		if (coordinate.y >= 0 && coordinate.y < m_dimensions.y){
			return true;
		}
	}
	return false;
}

AABB2 Map::GetCameraBounds() const
{
	return g_theGame->m_camera->GetBounds();
}

Player * Map::SpawnNewPlayer(Vector2 spawnPosition)
{

	ActorDefinition* actorDef = ActorDefinition::GetActorDefinition("Player");
	//spawnPosition = Vector2(5.f,5.f);
	Player* newPlayer = new Player(actorDef, spawnPosition, this);
	m_allEntities.push_back((Entity*) newPlayer);
	m_allActors.push_back( (Actor*) newPlayer);
	m_scene->AddRenderable(newPlayer->m_renderable);
	return newPlayer;
}

Actor * Map::SpawnNewActor(std::string actorName, Vector2 spawnPosition, float spawnRotation)
{
	ActorDefinition* actorDef = ActorDefinition::GetActorDefinition(actorName);
	return SpawnNewActor(actorDef, spawnPosition, spawnRotation, m_difficulty);
}

Actor * Map::SpawnNewActor(ActorDefinition * actorDef, Vector2 spawnPosition, float spawnRotation, int difficulty)
{
	Actor* newActor = new Actor(actorDef, this, spawnPosition, spawnRotation, difficulty);
	m_allEntities.push_back(newActor);
	m_allActors.push_back(newActor);
	m_scene->AddRenderable(newActor->m_renderable);
	return newActor;
}

Projectile * Map::SpawnNewProjectile(std::string projectileName, Vector2 spawnPosition, float spawnRotation, std::string faction, int bonusStrength)
{
	ProjectileDefinition* projectileDef = ProjectileDefinition::GetProjectileDefinition(projectileName);
	return SpawnNewProjectile(projectileDef, spawnPosition, spawnRotation, faction, bonusStrength);
}

Projectile * Map::SpawnNewProjectile(ProjectileDefinition * projectileDef, Vector2 spawnPosition, float spawnRotation, std::string faction, int bonusStrength)
{
	Projectile* newProjectile = new Projectile(projectileDef, this, spawnPosition, spawnRotation, faction, bonusStrength);
	m_allEntities.push_back( newProjectile);
	m_allProjectiles.push_back(newProjectile);
	m_scene->AddRenderable(newProjectile->m_renderable);
	return newProjectile;
}

Portal * Map::SpawnNewPortal(std::string portalName, Vector2 spawnPosition, Map* destinationMap, Vector2 toPos, float spawnRotation, bool spawnReciprocal)
{
	PortalDefinition* portalDef = PortalDefinition::GetPortalDefinition(portalName);
	return SpawnNewPortal(portalDef, spawnPosition, destinationMap, toPos, spawnRotation, spawnReciprocal);
}

Portal * Map::SpawnNewPortal(std::string portalName, Vector2 spawnPosition, std::string destinationMapName, Vector2 toPos, float spawnRotation, bool spawnReciprocal)
{
	Map* destinationMap = g_theGame->m_currentState->m_currentAdventure->GetMap(destinationMapName);
	return SpawnNewPortal(portalName, spawnPosition, destinationMap, toPos, spawnRotation, spawnReciprocal);
}

Portal * Map::SpawnNewPortal(PortalDefinition * portalDef, Vector2 spawnPosition, Map * destinationMap, Vector2 toPos, float spawnRotation, bool spawnReciprocal)
{
	Portal* newPortal = new Portal(portalDef, this, destinationMap, spawnPosition, toPos, spawnRotation, spawnReciprocal);
	m_allEntities.push_back( newPortal);
	m_allPortals.push_back(newPortal);
	m_scene->AddRenderable(newPortal->m_renderable);
	return newPortal;
}

Item * Map::SpawnNewItem(std::string itemName, Vector2 spawnPosition)
{
	ItemDefinition* itemDef = ItemDefinition::GetItemDefinition(itemName);
	return SpawnNewItem(itemDef, spawnPosition);
}

Item * Map::SpawnNewItem(ItemDefinition * itemDef, Vector2 spawnPosition)
{
	Item* newItem = new Item(itemDef, this, spawnPosition);
	m_allEntities.push_back( newItem);
	m_allItems.push_back(newItem);
	m_scene->AddRenderable(newItem->m_renderable);
	return newItem;
}





void Map::SetCamera()
{
	if (g_theGame->m_fullMapMode){
		int ortho = max(m_dimensions.x, m_dimensions.y);
		g_theGame->m_camera->SetProjectionOrtho((float) ortho + 1.f, g_gameConfigBlackboard.GetValue("windowAspect", 1.f), 0.f, 100.f);
		g_theGame->m_camera->LookAt( Vector3(m_dimensions.x *.5f, m_dimensions.y * .5f, -1.f), Vector3(m_dimensions.x *.5f, m_dimensions.y * .5f, .5f));
	} else {
		float viewWidth = WINDOW_ASPECT * ZOOM_FACTOR;
		Vector2 halfDimensions = Vector2(viewWidth, ZOOM_FACTOR) * .5f;
		Vector2 positionToCenter = g_theGame->m_player->GetPosition();
		


		float minX = ZOOM_FACTOR * WINDOW_ASPECT * .5f;
		float maxX = m_dimensions.x - minX;
		float minY = ZOOM_FACTOR * .5f;
		float maxY = m_dimensions.y - minY;


		positionToCenter.x = ClampFloat(positionToCenter.x, minX, maxX);
		positionToCenter.y = ClampFloat(positionToCenter.y, minY, maxY);

		g_theGame->m_camera->LookAt( Vector3(positionToCenter, -1.f), Vector3(positionToCenter, .5f));

		g_theGame->m_camera->SetProjectionOrtho(ZOOM_FACTOR, g_gameConfigBlackboard.GetValue("windowAspect", 1.f), 0.f, 100.f);
		
		//ClampCameraToMap();
	}


}

void Map::ClampCameraToMap()
{

	float minX = 0.f;
	float maxX = (float) m_dimensions.x;
	float minY = 0.f;
	float maxY = (float) m_dimensions.y;
	Vector2 bottomleft = g_theGame->m_camera->GetBounds().mins;
	Vector2 topRight = g_theGame->m_camera->GetBounds().maxs;
	Vector2 newMins = bottomleft;
	Vector2 newMaxs = topRight;

	if (bottomleft.x < minX){
		newMins.x = minX;
	}			
	if (bottomleft.y < minY){
		newMins.y = minY;
	}			
	if (topRight.x > maxX){
		newMaxs.x = maxX;
	}			
	if (topRight.y > maxY){
		newMaxs.y = maxY;
	}

}

void Map::InitializeTiles()
{
	m_dimensions = IntVector2();
	m_dimensions.x = m_mapDef->m_width.GetRandomInRange();
	m_dimensions.y = m_mapDef->m_height.GetRandomInRange();
	if (m_dimensions.y == 0){
		m_dimensions.y = m_dimensions.x;
	}
	int numTiles = m_dimensions.x * m_dimensions.y;
	m_tiles.resize(numTiles);
	for (int i = 0; i < numTiles; i++){
		IntVector2 coords = GetCoordinatesFromIndex(i, m_dimensions.x);
		m_tiles[i] = Tile(coords, m_mapDef->m_defaultTile);
	}
	m_numTiles = numTiles;

	//init generation mask to be the entire map
	int width = GetWidth();
	int height = GetHeight();

	float centerX = .5f;
	float centerY = .5f;
	IntVector2 center = IntVector2((int) (centerX * width), (int) (centerY * height));
	float dimX = .5f;
	float dimY = .5f;
	IntVector2 halfDims = IntVector2((int) (dimX * width), (int) (dimY * height));
	m_generationMask = (AreaMask*) new AreaMask_Rectangle(center, halfDims);
	m_fullMap = (AreaMask*) new AreaMask_Rectangle(center, halfDims);	//should never change, just for resetting
}

void Map::CreateTileRenderable()
{
	m_tileRenderable = new Renderable2D();
	Material* tileMat = Material::GetMaterial("tile");
	MeshBuilder mb =  MeshBuilder();
	mb.Begin(PRIMITIVE_TRIANGLES, true);
	static std::vector<Vertex3D_PCU> tileVerts;
	tileVerts.clear();
	AABB2 camBounds = GetCameraBounds();
	Vector2 spacing = Vector2(1.f,1.f);
	Tile* minTile = TileAtFloat(camBounds.mins - spacing);
	Tile* maxTile = TileAtFloat(camBounds.maxs + spacing);

	IntVector2 minTileCoords = IntVector2(0,0);
	IntVector2 maxTileCoords = m_dimensions;
	//if (minTile != nullptr){
	//	minTileCoords= minTile->m_coordinates;
	//}
	//if (maxTile != nullptr){
	//	maxTileCoords = maxTile->m_coordinates;
	//}
	for ( int x = minTileCoords.x; x < maxTileCoords.x; x++){
		for (int y = minTileCoords.y; y < maxTileCoords.y; y++){
			//push vertices
			Tile* tileToRender = TileAt(x,y);
			AABB2 bounds = tileToRender->GetBounds();
			RGBA color = tileToRender->m_tileDef->m_spriteTint;
			AABB2 texCoords = tileToRender->m_tileDef->GetTexCoords(tileToRender->m_extraInfo->m_variant);
			mb.AppendPlane2D(bounds, color, texCoords);
			//tileVerts.push_back(Vertex3D_PCU(Vector2(bounds.mins.x, bounds.mins.y), color, Vector2(texCoords.mins.x, texCoords.maxs.y)));
			//tileVerts.push_back(Vertex3D_PCU(Vector2(bounds.maxs.x, bounds.mins.y), color, Vector2(texCoords.maxs.x, texCoords.maxs.y)));
			//tileVerts.push_back(Vertex3D_PCU(Vector2(bounds.maxs.x, bounds.maxs.y), color, Vector2(texCoords.maxs.x, texCoords.mins.y)));
			//tileVerts.push_back(Vertex3D_PCU(Vector2(bounds.mins.x, bounds.maxs.y), color, Vector2(texCoords.mins.x, texCoords.mins.y)));
		}
	}
	mb.End();

	m_tileRenderable->SetMesh(mb.CreateMesh(VERTEX_TYPE_3DPCU));
	m_tileRenderable->SetMaterial(tileMat);
	m_tileRenderable->m_zOrder = 0;
	m_scene->AddRenderable(m_tileRenderable);
}

void Map::RunMapGeneration()
{
	for (MapGenStep* genStep : m_mapDef->m_generationSteps){
		genStep->RunIterations(*this);
	}
}

void Map::ResetPortals()
{
	for (Portal* portal : m_allPortals){
		portal->CheckIfTeleport();
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
