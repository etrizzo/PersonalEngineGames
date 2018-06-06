#pragma once
#include "Game/Tile.hpp"
#include "Game/Entity.hpp"
#include "Game/MapDefinition.hpp"

class Actor;
class Player;
class Projectile;
class ProjectileDefinition;
class ActorDefinition;
class Portal;
class PortalDefinition;
class Item;
class ItemDefinition;

const float RAYCAST_STEPS_PER_TILE = 100.f;
const bool USE_ALL_ENTITIES = false;



struct RaycastResult2D{
public:
	RaycastResult2D(){};
	RaycastResult2D(bool didImpact, Vector2 impactPos, IntVector2 impactTileCoords, float impactDistance, Vector2 impactNormal);
	bool m_didImpact;
	Vector2 m_impactPosition;
	IntVector2 m_impactTileCoordinates;
	float m_impactDistance;
	Vector2 m_impactNormal;
};

//class Camera2D{
//public:
//	Camera2D();
//	Camera2D(float orientation, float zoomFactor);
//	Vector2 m_worldPos;
//	float m_orientationDegrees;
//	int m_numTilesInViewVertically;
//	Vector2 m_bottomLeft;
//	Vector2 m_topRight;
//
//	void Update(float deltaSeconds);
//
//};


class Map{
public:
	Map(){}
	~Map();
	Map(std::string name, MapDefinition* mapDef);

	std::string m_name;
	MapDefinition* m_mapDef;
	IntVector2 m_dimensions;
	std::vector<Tile> m_tiles;
	//Camera* m_camera;
	int m_numTiles;
	SoundPlaybackID m_musicPlayback;

	RenderScene2D* m_scene;
	Renderable2D* m_tileRenderable;

	void Update(float deltaSeconds);
	void UpdateEntities(float deltaSeconds);
	void RunPhysics();
	void CheckEntityInteractions();

	void Render();
	void RenderTiles();
	void RenderEntities();
	bool RunBubbleSortPassOnEntities();
	void SortAllEntities();
	
	void RemoveDoomedEntities();

	void StartMusic();
	void StopMusic();

	int GetWidth() const;
	int GetHeight() const;
	Tile* TileAt(int x, int y);
	Tile* TileAt(IntVector2 coordinates);
	Tile* TileAtFloat(float x, float y);
	Tile* TileAtFloat(Vector2 floatCoordinates);
	Tile GetRandomTileOfType(TileDefinition* tileType) const;
	Tile GetSpawnTileOfType(TileDefinition* tileType) const;
	Tile GetRandomBaseTile() const;
	Tile GetRandomTile() const;
	IntVector2 GetRandomTileCoords() const;
	bool IsCoordinateOnMap(IntVector2 coordinate);

	bool HasLineOfSight(const Vector2& startPos, const Vector2& endPos, float maxDistance = 20.f);
	RaycastResult2D Map::Raycast(const Vector2& startPos, const Vector2& direction, float maxDistance) ;
	AABB2 GetCameraBounds() const;

	Player* SpawnNewPlayer(Vector2 spawnPosition);
	Actor* SpawnNewActor(std::string actorName, Vector2 spawnPosition, float spawnRotation = 0.f);
	Actor* SpawnNewActor(ActorDefinition* actorDef, Vector2 spawnPosition, float spawnRotation = 0.f);
	Projectile* SpawnNewProjectile(std::string projectileName, Vector2 spawnPosition, float spawnRotation, std::string faction, int bonusStrength);
	Projectile* SpawnNewProjectile(ProjectileDefinition* projectileDef, Vector2 spawnPosition, float spawnRotation, std::string faction, int bonusStrength);
	Portal* SpawnNewPortal(std::string portalName, Vector2 spawnPosition, Map* destinationMap, Vector2 toPos, float spawnRotation, bool spawnReciprocal = true);
	Portal* SpawnNewPortal(std::string portalName, Vector2 spawnPosition, std::string destinationMapName, Vector2 toPos, float spawnRotation, bool spawnReciprocal = true);
	Portal* SpawnNewPortal(PortalDefinition* portalDef, Vector2 spawnPosition, Map* destinationMap, Vector2 toPos, float spawnRotation, bool spawnReciprocal = true);
	Item* SpawnNewItem(std::string itemName, Vector2 spawnPosition);
	Item* SpawnNewItem(ItemDefinition* itemDef, Vector2 spawnPosition);

	void AddEntityToMap(Entity* entity);
	void RemoveEntityFromMap(Entity* entity);
	void RemoveActorFromMap(Actor* actor);
	void AddActorToMap(Actor* actor);
	void RemoveEntities();
	
	void FillSeededDistanceMap(Heatmap& distMap,const Tile* destinationTile = nullptr, const float& maxRange = 9999.f, const float& straightChance = 0.f, const float& walkingPenalty = 1.f, const float& swimPenalty = 10.f, const float flyPenalty = 1.f, const float& unWalkablePenalty = 9999.f);
	eManhattanDir GetMostSignificantDirection(const IntVector2 startCoords, const IntVector2 endCoords);
	eManhattanDir ReverseDirection(eManhattanDir incomingDir);
	void GetManhattanNeighbors(Tile* tile, Tile (&neighbors)[4]);
	void GetManhattanNeighbors(IntVector2 coords, Tile (&neighbors)[4]);
	void GetNeighbors(Tile* tile, Tile (&neighbors)[8]);
	void GetNeighbors(IntVector2 coords, Tile (&neighbors)[8]);

	//SURELY these can be private somehow
	std::vector<Entity*> m_allEntities;
	std::vector<Actor*> m_allActors;
	std::vector<Projectile*> m_allProjectiles;
	std::vector<Portal*> m_allPortals;
	std::vector<Item*> m_allItems;

private:
	void SetCamera();
	void ClampCameraToMap();
	void InitializeTiles();
	void CreateTileRenderable();
	void RunMapGeneration();

	void ResetPortals();
	

};



