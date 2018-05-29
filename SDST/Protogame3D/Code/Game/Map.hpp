#pragma once
#include "Game/Tile.hpp"
#include "Game/Entity.hpp"
#include "Game/MapDefinition.hpp"


const float RAYCAST_STEPS_PER_TILE = 100.f;

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



class Map{
	

public:
	Map(){}
	~Map();
	Map(std::string name, MapDefinition* mapDef);

	std::string m_name;
	MapDefinition* m_definition;
	IntVector2 m_dimensions;
	std::vector<Tile> m_tiles;

	void Render();
	void RenderTiles();

	void Update(float deltaSeconds);


	void UpdateEntities(float deltaSeconds);
	//void RenderEntities();
	void RunPhysics();
	void CheckEntityInteractions();

	int GetWidth() const;
	int GetHeight() const;
	Tile* TileAt(int x, int y);
	Tile* TileAt(IntVector2 coordinates);
	Tile* TileAtFloat(float x, float y);
	Tile* TileAtFloat(Vector2 floatCoordinates);
	Tile GetRandomTileOfType(TileDefinition* tileType) const;
	Tile GetRandomBaseTile() const;

	bool HasLineOfSight(const Vector2& startPos, const Vector2& endPos, float maxDistance = 20.f);
	RaycastResult2D Map::Raycast(const Vector2& startPos, const Vector2& direction, float maxDistance) ;

	void AddEntityToMap(Entity* entity);
	void RemoveEntityFromMap(Entity* entity);
	void RemoveEntities();
	
	//make a "remove from map" method to keep this private
	std::vector<Entity*> m_allEntities;

private:
	int m_numTiles;

	void InitializeTiles();
	void RunMapGeneration();
	

};



