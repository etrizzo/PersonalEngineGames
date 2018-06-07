#pragma once
#include "Game/Entity.hpp"
#include "Game/MapDefinition.hpp"


const float RAYCAST_STEPS_PER_TILE = 100.f;

//struct RaycastResult2D{
//public:
//	RaycastResult2D(){};
//	RaycastResult2D(bool didImpact, Vector2 impactPos, IntVector2 impactTileCoords, float impactDistance, Vector2 impactNormal);
//	bool m_didImpact;
//	Vector2 m_impactPosition;
//	IntVector2 m_impactTileCoordinates;
//	float m_impactDistance;
//	Vector2 m_impactNormal;
//};


//class GameMapChunk{
//public:
//	GameMapChunk();
//
//	Renderable* m_renderable;
//	IntVector2 m_chunkIndex;
//	Map* m_map;
//};
//


class Map{
	

public:
	Map(){}
	~Map();
	Map(std::string imageFile, AABB2 const &extents = AABB2(Vector2::ZERO, Vector2::ONE * 100.f), float minHeight = 0.f, float maxHeight = 5.f, IntVector2 chunks = IntVector2(5,5), float tilesPerChunk = 10.f);

	std::string m_name;
	//MapDefinition* m_definition;
	AABB2 m_extents;
	IntVector2 m_dimensions;
	//std::vector<Tile> m_tiles;

	void Render();
	//void RenderTiles();

	void Update(float deltaSeconds);


	//void UpdateEntities(float deltaSeconds);
	////void RenderEntities();
	//void RunPhysics();
	void CheckEntityInteractions();

	int GetWidth() const;
	int GetHeight() const;

	float GetHeightForVertex(int x, int y);
	Vector3 GetVertexWorldPos(int x, int y);		//index of vertex
	float GetHeightAtCoord(Vector2 xzCoord);
	IntVector2 GetVertexCoordsFromWorldPos(Vector2 xzPos);
	//Tile* TileAt(int x, int y);
	//Tile* TileAt(IntVector2 coordinates);
	//Tile* TileAtFloat(float x, float y);
	//Tile* TileAtFloat(Vector2 floatCoordinates);
	//Tile  GetRandomTileOfType(TileDefinition* tileType) const;
	//Tile  GetRandomBaseTile() const;

	//bool HasLineOfSight(const Vector2& startPos, const Vector2& endPos, float maxDistance = 20.f);
	//RaycastResult2D Map::Raycast(const Vector2& startPos, const Vector2& direction, float maxDistance) ;

	//void AddEntityToMap(Entity* entity);
	//void RemoveEntityFromMap(Entity* entity);
	//void RemoveEntities();
	
	//make a "remove from map" method to keep this private
	//std::vector<Entity*> m_allEntities;
	//std::vector<GameChunk*> m_chunks;
	std::vector<float> m_heights;
	Renderable* m_renderable;

private:
	int m_numTiles;
	Vector2 m_tileSize;
	Vector2 m_heightRange;

	//void InitializeTiles();
	void RunMapGeneration(const Image& img);
	

};



