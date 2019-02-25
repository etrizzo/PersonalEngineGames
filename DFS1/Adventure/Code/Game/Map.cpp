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
#include "Game/Decoration.hpp"
#include "Game/DecorationDefinition.hpp"
#include "Game/Adventure.hpp"
#include "Game/DialogueSet.hpp"
#include "Game/Party.hpp"
#include "Game/Village.hpp"
#include "Game/VillageDefinition.hpp"


Map::~Map()
{
	for (unsigned int i = 0; i < m_allEntities.size(); i++){
		delete m_allEntities[i];
		//if (m_allEntities[i] != g_theGame->m_party->){
		//	
		//}
	}
	m_tiles.clear();
	delete m_scene;
	if (m_fullMap != m_generationMask){
		delete m_generationMask;
	}
	delete m_fullMap;
	delete m_tileRenderable;
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
	m_allDecorations = std::vector<Decoration*>();
	m_allVillages = std::vector<Village*>();
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
	
	RenderTiles();
	//RenderEntities();
}

void Map::PostRender()
{
	RenderQuestIcons();
	if (g_theGame->m_devMode){
		RenderTileTags();
	}
}

void Map::RenderQuestIcons()
{
	AABB2 texCoords = g_theGame->m_speechBubbleSpriteSheet->GetTexCoordsForSpriteCoords(IntVector2(3,4));
	Sprite* bubble = g_theGame->m_speechBubbleSpriteSheet->GetSprite(3, 4, g_theRenderer);
	for (Actor* actor : m_allActors){
		if (actor->m_questGiven != nullptr){
			float size = actor->m_localDrawingBox.GetHeight() * .025f;
			Vector2 drawPos = actor->GetPosition() + Vector2(0.f, actor->m_localDrawingBox.GetHeight() * .9f);
			//AABB2 box = AABB2(actor->GetPosition() + Vector2::ONE * size, actor->GetPosition() + Vector2::ONE * size);
			//box.TrimToSquare();
			//box.Translate(0.f, actor->m_localDrawingBox.GetHeight() * .05f);
			//g_theGame->m_debugRenderSystem->MakeDebugRenderQuad()
			
			g_theRenderer->DrawSprite(Vector3(drawPos, -.1f), bubble, Vector2::EAST * size, Vector2::NORTH * size);
			//g_theRenderer->DrawTexturedAABB2(box, *g_theGame->m_speechBubbleSpriteSheet->GetTexture(), texCoords.mins, texCoords.maxs , RGBA::WHITE);
		}
	}
}

void Map::RenderUI()
{
	
	if (m_activeDialogueSet != nullptr){
		m_activeDialogueSet->Render(g_theGame->m_dialogueBox);
	}
}

void Map::RenderTileTags()
{
	
	Vector2 playerPos = g_theGame->m_party->GetPlayerCharacter()->GetPosition();
	Vector2 spacing = Vector2(1.f,1.f);
	Tile* minTile = TileAtFloat(playerPos - (spacing * 10.f));
	Tile* maxTile = TileAtFloat(playerPos + (spacing * 10.f));

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
			Tile* tileToRender = TileAt(x,y);
			tileToRender->RenderTag();
		}
	}
}



void Map::RenderTiles(){
	//an optimization would be to only bind the thing if it's different from the previous bind.
	g_theRenderer->BindMaterial(m_tileRenderable->GetEditableMaterial());
	g_theRenderer->BindModel(m_tileRenderable->m_transform.GetWorldMatrix());
	g_theRenderer->DrawMesh(m_tileRenderable->m_mesh->m_subMeshes[0]);


	//static std::vector<Vertex3D_PCU> tileVerts;
	//tileVerts.clear();
	//AABB2 camBounds = GetCameraBounds();
	//Vector2 spacing = Vector2(1.f,1.f);
	//Tile* minTile = TileAtFloat(camBounds.mins - spacing);
	//Tile* maxTile = TileAtFloat(camBounds.maxs + spacing);

	//IntVector2 minTileCoords = IntVector2(0,0);
	//IntVector2 maxTileCoords = m_dimensions;
	//if (minTile != nullptr){
	//	 minTileCoords= minTile->m_coordinates;
	//}
	//if (maxTile != nullptr){
	//	maxTileCoords = maxTile->m_coordinates;
	//}
	//for ( int x = minTileCoords.x; x < maxTileCoords.x; x++){
	//	for (int y = minTileCoords.y; y < maxTileCoords.y; y++){
	//		//push vertices
	//		Tile* tileToRender = TileAt(x,y);
	//		AABB2 bounds = tileToRender->GetBounds();
	//		RGBA color = tileToRender->m_tileDef->m_spriteTint;
	//		AABB2 texCoords = tileToRender->m_tileDef->GetTexCoords(tileToRender->m_extraInfo->m_variant);
	//		tileVerts.push_back(Vertex3D_PCU(Vector2(bounds.mins.x, bounds.mins.y), color, Vector2(texCoords.mins.x, texCoords.maxs.y)));
	//		tileVerts.push_back(Vertex3D_PCU(Vector2(bounds.maxs.x, bounds.mins.y), color, Vector2(texCoords.maxs.x, texCoords.maxs.y)));
	//		tileVerts.push_back(Vertex3D_PCU(Vector2(bounds.maxs.x, bounds.maxs.y), color, Vector2(texCoords.maxs.x, texCoords.mins.y)));
	//		tileVerts.push_back(Vertex3D_PCU(Vector2(bounds.mins.x, bounds.maxs.y), color, Vector2(texCoords.mins.x, texCoords.mins.y)));
	//	}
	//}
	//Vertex3D_PCU* vertArray = &tileVerts[0];
	//const Texture* tileTexture = g_tileSpriteSheet->GetTexture();
	//g_theRenderer->BindTexture(*tileTexture);
	//g_theRenderer->DrawMeshImmediate(vertArray, tileVerts.size(), PRIMITIVE_QUADS);
	//g_theRenderer->ReleaseTexture();
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
	
	PROFILE_PUSH("Map::UpdateEntities");
	UpdateEntities(deltaSeconds);
	PROFILE_POP();
	//PROFILE_PUSH("Map::RunPhysics");
	RunPhysics();
	//PROFILE_POP();
	//PROFILE_PUSH("Map::CheckInteractions");
	CheckEntityInteractions();
	//PROFILE_POP();
	ResetPortals();

	

}

void Map::UpdateEntities(float deltaSeconds)
{
	float cameraUpdateRadius = ZOOM_FACTOR * 5.f;
	Vector2 playerPosition = g_theGame->m_party->m_currentPlayer->GetPosition();
	for (unsigned int entityIndex = 0; entityIndex < m_allEntities.size(); entityIndex++){
		Entity* currentEntity = m_allEntities.at(entityIndex);
		Vector2 entityPosition = currentEntity->GetPosition();
		if (currentEntity != nullptr && !currentEntity->m_aboutToBeDeleted){
			if ((entityPosition - playerPosition).GetLengthSquared() < (cameraUpdateRadius * cameraUpdateRadius)){
				currentEntity->Update(deltaSeconds);
			}
			
		}
	}

	//if (g_theGame->m_player != nullptr){
	//	g_theGame->m_player->Update(deltaSeconds);
	//}
	

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

	if (g_theGame->m_party != nullptr){
		g_theGame->m_party->RunCorrectivePhysics();
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
		//for (unsigned int actorIndex = 0; actorIndex < m_allActors.size(); actorIndex++){
		//	Actor* actor = m_allActors[actorIndex];
		//	if (DoDiscsOverlap(portal->m_physicsDisc, actor->m_physicsDisc)){
		//	//if (portal->m_physicsDisc.IsPointInside(actor->m_position)){				//need to fix how portals reset to do center teleport. 
		//		if (portal->m_isReadyToTeleport && portal->IsSameFaction(actor)){
		//			portal->Teleport(actor);
		//		}
		//	}
		//}
		
		if (DoDiscsOverlap(portal->m_physicsDisc, g_theGame->m_party->GetPlayerCharacter()->m_physicsDisc)){
			//if (portal->m_physicsDisc.IsPointInside(actor->m_position)){				//need to fix how portals reset to do center teleport. 
			if (portal->m_isReadyToTeleport && portal->IsSameFaction( g_theGame->m_party->GetPlayerCharacter())){
				portal->Teleport( g_theGame->m_party);
			}
		}
	}


	if (g_theGame->m_party->GetPlayerCharacter() != nullptr){
		for (unsigned int itemIndex = 0; itemIndex < m_allItems.size(); itemIndex++){
			Item* item = m_allItems[itemIndex];
			if (DoDiscsOverlap(item->m_physicsDisc, g_theGame->m_party->GetPlayerCharacter()->m_physicsDisc)){
				//Item* copyItem = new Item(*item);		//add a copy so that the one on the map can get deleted safely
				g_theGame->m_party->AddItemToInventory(item);
				//m_allItems[itemIndex] = nullptr;
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

void Map::ProgressVillageStories()
{
	//lmao
	for (Village* village : m_allVillages){
		village->ProgressVillageStory();
	}
}

void Map::RemoveDoomedEntities()
{
	g_theGame->m_party->CheckForKilledPlayers();
	for (Actor* actor : m_allActors){
		actor->CheckTargetStatus();		//voids targets if they're about to be deleted
	}
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
			if (entity!= nullptr){
				g_theGame->m_currentState->m_currentAdventure->GetScene()->RemoveRenderable(entity->m_healthRenderable);
			}
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

	//for (unsigned int itemIndex = 0; itemIndex < m_allItems.size(); itemIndex++){
	//	Entity* entity = m_allItems.at(itemIndex);
	//	if (entity == nullptr){
	//		m_allItems.erase(m_allItems.begin() + itemIndex);
	//	}else if (entity->IsAboutToBeDeleted()){
	//		m_allItems.erase(m_allItems.begin() + itemIndex);
	//		delete(entity);
	//	}
	//}

	for (unsigned int itemIndex = m_allItems.size()-1; itemIndex < m_allItems.size(); itemIndex--){
		Item* entity = m_allItems.at(itemIndex);
		if (entity->IsAboutToBeDeleted()){
			RemoveAtFast(m_allItems, itemIndex);
			//delete(entity);
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
	//m_scene->AddRenderable(actor->m_healthRenderable);
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
	eManhattanDir currentDirection = NUM_MANHATTAN_DIRS; //= GetMostSignificantDirection(firstSeed.m_coordinates, destinationTile->m_coordinates);
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
		//bool straightenPath = CheckRandomChance(straightChance);
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
	if (tile != nullptr){
		IntVector2 coords = tile->m_coordinates;
		GetNeighbors(coords, neighbors);
	}
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

TileNeighborSet* Map::GetNeighborSet(Tile * tile)
{
	return new TileNeighborSet(tile, this);
}


void Map::StartDialogue(DialogueSet * d)
{
	m_activeDialogueSet = d;
	g_theGame->Pause();
	ProgressDialogueAndCheckFinish();
}

bool Map::ProgressDialogueAndCheckFinish()
{
	//progress dialogue - if the dialogue finishes, set active dialogue set to nullptr
	if (m_activeDialogueSet->ProgressAndCheckFinish()){
		m_activeDialogueSet = nullptr;
		g_theGame->Unpause();
		return true;
	}
	return false;
}

bool Map::IsDialogueOpen()
{
	return m_activeDialogueSet != nullptr;
}

void Map::ReCreateRenderable(bool edge)
{
	m_scene->RemoveRenderable(m_tileRenderable);
	delete m_tileRenderable;
	CreateTileRenderable(edge);
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
	while (((randomTile.m_tileDef->m_name != tileType->m_name) || (randomTile.HasBeenSpawnedOn())) && tries < 3000){
		randomTileX = GetRandomIntInRange(1,m_dimensions.x-2);
		randomTileY = GetRandomIntInRange(1,m_dimensions.y-2);
		randomTileIndex = GetIndexFromCoordinates(randomTileX, randomTileY, m_dimensions.x, m_dimensions.y);
		randomTile = m_tiles[randomTileIndex];
		tries++;
	}
	//if (tries >= 3000){
	//	randomTile
	//}
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

Tile * Map::GetRandomTileWithTag(std::string tag)
{
	std::vector<Tile*> tilesWithTag = std::vector<Tile*>();
	for (int i = 0; i < m_numTiles; i++){
		if (m_tiles[i].HasTag(tag)){
			tilesWithTag.push_back(&m_tiles[i]);
		}
	}
	if (tilesWithTag.size() > 0){
		int index = GetRandomIntLessThan(tilesWithTag.size());
		return tilesWithTag[index];
	} else {
		return nullptr;
	}
}

Tile * Map::GetRandomTileWithoutTag(std::string tag)
{
	std::vector<Tile*> tilesWithTag = std::vector<Tile*>();
	for (int i = 0; i < m_numTiles; i++){
		if (!m_tiles[i].HasTag(tag)){
			tilesWithTag.push_back(&m_tiles[i]);
		}
	}
	if (tilesWithTag.size() > 0){
		int index = GetRandomIntLessThan(tilesWithTag.size());
		return tilesWithTag[index];
	} else {
		return nullptr;
	}
}

Tile * Map::GetTaggedTileOfType(TileDefinition * def, std::string tag)
{
	std::vector<Tile*> tilesWithTag = std::vector<Tile*>();
	for (int i = 0; i < m_numTiles; i++){
		if (m_tiles[i].HasTag(tag)){
			tilesWithTag.push_back(&m_tiles[i]);
		}
	}
	if (tilesWithTag.size() > 0){
		//get tiles of type from tag list
		std::vector<Tile*> possibleTiles = std::vector<Tile*>();
		for (Tile* tile : tilesWithTag){
			if (tile->m_tileDef == def){
				possibleTiles.push_back(tile);
			}
		}
		if (possibleTiles.size() > 0){
			int index = GetRandomIntLessThan(possibleTiles.size());
			return possibleTiles[index];
		} else {
			return nullptr;
		}
	} else {
		return nullptr;
	}
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

void Map::MarkTileForSpawn(IntVector2 pos)
{
	int index = GetIndexFromCoordinates(pos.x, pos.y, m_dimensions.x, m_dimensions.y);
	if (index >= m_numTiles || index < 0){
		return;
	} else {
		m_tiles[index].MarkAsSpawned();
	}
}

AABB2 Map::GetCameraBounds() const
{
	return g_theGame->m_camera->GetBounds();
}

Actor * Map::SpawnNewPlayer(Vector2 spawnPosition)
{

	ActorDefinition* actorDef = ActorDefinition::GetActorDefinition("Player");
	Actor* newPlayer = new Actor(actorDef, this, spawnPosition);
	newPlayer->m_isPlayer = true;
	m_allEntities.push_back(newPlayer);
	m_allActors.push_back(newPlayer);
	m_scene->AddRenderable(newPlayer->m_renderable);
	//m_scene->AddRenderable(newPlayer->m_healthRenderable);
	newPlayer->EquipItemsInInventory();
	return newPlayer;
}

Actor * Map::SpawnNewActor(std::string actorName, Vector2 spawnPosition, float spawnRotation)
{
	ActorDefinition* actorDef = ActorDefinition::GetActorDefinition(actorName);
	if (actorDef == nullptr){
		std::string s = "No actor called: " + actorName + ". Not spawned.";
		ConsolePrintf(RGBA::RED, s.c_str() );
		return nullptr;
	}
	return SpawnNewActor(actorDef, spawnPosition, spawnRotation, m_difficulty);
}

Actor * Map::SpawnNewActor(ActorDefinition * actorDef, Vector2 spawnPosition, float spawnRotation, int difficulty)
{
	Actor* newActor = new Actor(actorDef, this, spawnPosition, spawnRotation, difficulty);
	m_allEntities.push_back(newActor);
	m_allActors.push_back(newActor);
	m_scene->AddRenderable(newActor->m_renderable);
	if (newActor->m_faction != "good"){
		m_scene->AddRenderable(newActor->m_healthRenderable);
	}
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
	if (itemDef == nullptr){
		std::string s = "No item called: " + itemName + ". Not spawned.";
		ConsolePrintf(RGBA::RED, s.c_str() );
		return nullptr;
	}
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

Decoration * Map::SpawnNewDecoration(std::string decoName, Vector2 spawnPosition)
{
	DecorationDefinition* decoDef = DecorationDefinition::GetDecorationDefinition(decoName);
	return SpawnNewDecoration(decoDef, spawnPosition);
}

Decoration * Map::SpawnNewDecoration(DecorationDefinition * decoDef, Vector2 spawnPosition)
{
	Decoration* newDecoration = new Decoration(decoDef, this, spawnPosition);
	//m_allEntities.push_back( newDecoration);
	m_allDecorations.push_back(newDecoration);
	m_scene->AddRenderable(newDecoration->m_renderable);
	return newDecoration;
}

Village * Map::SpawnNewVillage(std::string villageDefName, int numResidentsToSpawn)
{
	VillageDefinition* villageDef = VillageDefinition::GetVillageDefinition(villageDefName);

	Village* newVillage = new Village(villageDef, this, numResidentsToSpawn);
	m_allVillages.push_back(newVillage);
	return newVillage;
}

Actor * Map::GetActorOfType(ActorDefinition * actorDef)
{
	//collect all actors of the type
	std::vector<Actor*> actorsOfType = std::vector<Actor*>();
	for (Actor* actor : m_allActors){
		if (actor->m_definition == actorDef){
			actorsOfType.push_back(actor);
		}
	}
	Actor* returnActor = nullptr;
	//if there are actors of that type, get a random actor from the saved list
	if (actorsOfType.size() > 0){
		int index = GetRandomIntLessThan(actorsOfType.size());
		returnActor = actorsOfType[index];
	}
	return returnActor;
}





void Map::SetCamera()
{
	float aspect = g_gameConfigBlackboard.GetValue("windowAspect", 1.f);
	if (g_theGame->m_fullMapMode){
		int ortho = m_dimensions.y;
		g_theGame->m_party->GetPlayerCharacter()->SetScale(m_dimensions.x * .05f);
		float screenWidth = aspect * ortho;
		g_theGame->m_camera->SetProjectionOrtho((float) ortho + 1.f, aspect, 0.f, 100.f);
		g_theGame->m_camera->SetPosition(Vector3(screenWidth * -.25f,0.f, -1.f));
	} else {
		
		float viewWidth = aspect * ZOOM_FACTOR;
		Vector2 halfDimensions = Vector2(viewWidth, ZOOM_FACTOR) * .5f;
		Vector2 positionToCenter = g_theGame->m_party->GetPlayerCharacter()->GetPosition() - halfDimensions;
		g_theGame->m_party->GetPlayerCharacter()->SetScale(1.f);


		float minX = 0.f;
		float maxX = m_dimensions.x - viewWidth;
		float minY = 0.f;
		float maxY = m_dimensions.y - ZOOM_FACTOR;


		positionToCenter.x = ClampFloat(positionToCenter.x, minX, maxX);
		positionToCenter.y = ClampFloat(positionToCenter.y, minY, maxY);

		
		//g_theGame->m_camera->SetProjectionOrtho(ZOOM_FACTOR, aspect, -.1f, 100.f, positionToCenter);

		g_theGame->m_camera->SetProjectionOrtho(ZOOM_FACTOR, aspect, -.1f, 100.f);
		g_theGame->m_camera->SetPosition(Vector3(positionToCenter, -1.f));
		
		//ClampCameraToMap();
	}

	g_theRenderer->ClearScreen( RGBA::BLACK ); 
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
		//if (CheckRandomChance(.1f)){
		//	m_tiles[i].AddOverlaySpriteFromTileSheet(g_tileSpriteSheet->GetTexCoordsForSpriteCoords(IntVector2(7,28)));
		//}
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

void Map::CreateTileRenderable(bool edge)
{
	m_tileRenderable = new Renderable2D();
	Material* tileMat = Material::GetMaterial("tile");
	MeshBuilder mb =  MeshBuilder();
	mb.Begin(PRIMITIVE_TRIANGLES, true);
	static std::vector<Vertex3D_PCU> tileVerts;
	tileVerts.clear();
	AABB2 camBounds = GetCameraBounds();
	Vector2 spacing = Vector2(1.f,1.f);
	//Tile* minTile = TileAtFloat(camBounds.mins - spacing);
	//Tile* maxTile = TileAtFloat(camBounds.maxs + spacing);

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
			AABB2 texCoords;
			//if (edge && tileToRender->m_extraInfo->m_cosmeticBaseDef != nullptr){
			//	texCoords = tileToRender->m_extraInfo->m_cosmeticBaseDef->GetTexCoords();
			//} else {
			//	texCoords = tileToRender->m_tileDef->GetTexCoords(tileToRender->m_extraInfo->m_variant);
			//}
			
			if (edge){
				for (int i = 0; i < NUM_SPRITE_LAYERS; i++){
					if (tileToRender->m_extraInfo->m_spriteCoords[i] != nullptr){
						AABB2 overlayCoords = *tileToRender->m_extraInfo->m_spriteCoords[i];
						mb.AppendPlane2D(bounds, color, overlayCoords, .01f);
					}
				}
			} else {
				texCoords = tileToRender->m_tileDef->GetTexCoords(tileToRender->m_extraInfo->m_variant);
				mb.AppendPlane2D(bounds, color, texCoords, .01f);
			}
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
	//m_scene->AddRenderable(m_tileRenderable);
}

void Map::RunMapGeneration()
{
	for (MapGenStep* genStep : m_mapDef->m_generationSteps){
		genStep->RunIterations(*this);
	}

	//EdgeTiles();
	EdgeTilesThreeSteps();
}

void Map::EdgeTiles()
{
	//remove invalid tiles from the map
	//we want them gone before we start edging things so that they don't interfere with the actuall edges
	for (int i = 0; i < m_numTiles; i++){
		Tile* tile = &m_tiles[i];
		if (tile->m_tileDef->m_isTerrain){
			TileNeighborSet* neighborSet = new TileNeighborSet(tile, this);
			
			TileDefinition* edgeTileDefinition = neighborSet->FindEdgeTileDefinition();
			if (edgeTileDefinition != nullptr){
				if (neighborSet->IsTileInvalid()){
					//if the placement of the tile doesn't work with our spritesheet,
					//replace it with the edgeTileDefinition
					//tile->m_tileDef = edgeTileDefinition;
					tile->SetType(edgeTileDefinition);
				}
			}
		}
	}

	//actually do the edges now
	for (int i = 0; i < m_numTiles; i++){
		Tile* tile = &m_tiles[i];
		if (tile->m_tileDef->m_isTerrain){
			TileNeighborSet* neighborSet = new TileNeighborSet(tile, this);

			TileDefinition* edgeTileDefinition = neighborSet->FindEdgeTileDefinition();
			if (edgeTileDefinition != nullptr){
				AABB2 overlayUVs = neighborSet->GetDownflowingEdge(edgeTileDefinition);
				tile->AddOverlaySpriteFromTileSheet(overlayUVs, SPRITE_LOW_PRIORITY_EDGE);
			}
		}
	}
}

void Map::EdgeTilesThreeSteps()
{
	RemoveInvalidTiles();		//A single pass 
	RemoveInvalidTiles();

	AddTufts();		//if a high-level edge is surrounded by lower-level edges, fuck it up
	EdgeShoreline();
	EdgeGrassToDirt();
	EdgeLowPriority();
}

void Map::RemoveInvalidTiles()
{
	//remove invalid tiles from the map
	//we want them gone before we start edging things so that they don't interfere with the actuall edges
	for (int i = 0; i < m_numTiles; i++){
		Tile* tile = &m_tiles[i];
		if (tile->m_tileDef->m_isTerrain){
			TileNeighborSet* neighborSet = new TileNeighborSet(tile, this);

			TileDefinition* edgeTileDefinition = neighborSet->FindEdgeTileDefinition();
			if (edgeTileDefinition != nullptr){
				if (neighborSet->IsTileInvalid()){
					//if the placement of the tile doesn't work with our spritesheet,
					//replace it with the edgeTileDefinition
					//tile->m_tileDef = edgeTileDefinition;
					tile->SetType(edgeTileDefinition);
				}
			}
		}
	}
}

void Map::AddTufts()
{
	for (int i = 0; i < m_numTiles; i++){
		Tile* tile = &m_tiles[i];
		if (tile->m_tileDef->m_isTerrain){
			TileNeighborSet* neighborSet = new TileNeighborSet(tile, this);


			if (neighborSet->IsTuft()){
				TileDefinition* centerDef = tile->m_tileDef;
				TileDefinition* edgeDef = neighborSet->FindMostSignificantNeighbor();
				//if the placement of the tile doesn't work with our spritesheet,
				//replace it with the edgeTileDefinition
				//tile->m_tileDef = edgeTileDefinition;
				tile->SetType(edgeDef);
				if (CheckRandomChance(TUFT_CHANCE)){
					if (CheckRandomChance(.7f)){
						tile->AddOverlaySpriteFromTileSheet(centerDef->m_edgeDefinition->GetTexCoordsForEdge(EDGE_SINGLE_SMALL), SPRITE_HIGH_PRIORITY_EDGE);
					} else {
						tile->AddOverlaySpriteFromTileSheet(centerDef->m_edgeDefinition->GetTexCoordsForEdge(EDGE_SINGLE_LARGE), SPRITE_HIGH_PRIORITY_EDGE);
					}
				}
			}
			
		}
	}
}

void Map::EdgeShoreline()
{
	//Do shoreline edges now
	for (int i = 0; i < m_numTiles; i++){
		Tile* tile = &m_tiles[i];
		if (tile->m_tileDef->m_isTerrain){
			TileNeighborSet* neighborSet = new TileNeighborSet(tile, this);
			neighborSet->SetCompareMode(COMPARE_TERRAIN);
			TileDefinition* edgeTileDefinition = neighborSet->FindShoreEdge();
			if (edgeTileDefinition != nullptr && edgeTileDefinition != tile->m_tileDef){
				//set base
				AABB2 baseUVs = edgeTileDefinition->GetRandomTexCoords();		//base is the shore's base
				tile->AddOverlaySpriteFromTileSheet(baseUVs, SPRITE_COSMETIC_BASE);
				tile->m_extraInfo->m_cosmeticBaseDefinition = edgeTileDefinition;
				//set edge
				AABB2 edgeUVs = neighborSet->GetUpflowingEdge(edgeTileDefinition);		//get the edge from the actual water 
				tile->AddOverlaySpriteFromTileSheet(edgeUVs, SPRITE_SHORE_EDGE);
			}
		}
	}
}

void Map::EdgeGrassToDirt()
{
	for (int iteration = 0; iteration < 2 ; iteration++){
		//actually do the edges now
		for (int i = 0; i < m_numTiles; i++){
			Tile* tile = &m_tiles[i];
			if (tile->m_tileDef->m_isTerrain){
				TileNeighborSet* neighborSet = new TileNeighborSet(tile, this);
				neighborSet->SetCompareMode(COMPARE_GROUND);
				TileDefinition* edgeTileDefinition = neighborSet->FindEdgeTileDefinition();
				if (edgeTileDefinition != nullptr){
					AABB2 overlayUVs = neighborSet->GetDownflowingEdge(edgeTileDefinition);
					tile->AddOverlaySpriteFromTileSheet(overlayUVs, SPRITE_HIGH_PRIORITY_EDGE);
				}
			}
		}
	}
}

void Map::EdgeLowPriority()
{
	//actually do the edges now
	for (int i = 0; i < m_numTiles; i++){
		Tile* tile = &m_tiles[i];
		if (tile->m_tileDef->m_isTerrain){
			TileNeighborSet* neighborSet = new TileNeighborSet(tile, this);
			neighborSet->SetCompareMode(COMPARE_DEFINITION);
			TileDefinition* edgeTileDefinition = neighborSet->FindEdgeTileDefinition();
			if (edgeTileDefinition != nullptr && edgeTileDefinition->m_terrainLayer > TERRAIN_WATER){
				AABB2 overlayUVs = neighborSet->GetDownflowingEdge(edgeTileDefinition);
				tile->AddOverlaySpriteFromTileSheet(overlayUVs, SPRITE_LOW_PRIORITY_EDGE);
			}
		}
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
