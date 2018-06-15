#pragma once
#include "Game/Entity.hpp"
#include "Game/MapDefinition.hpp"
#include "Game/Chunk.hpp"


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
	Vector3 GetVertexWorldPos(IntVector2 xy);		//index of vertex
	float GetHeightAtCoord(Vector2 xzCoord);
	Vector3 GetPositionAtCoord(Vector2 xzCoord);
	Vector3 GetNormalAtTile(Vector2 xzCoord);
	Vector3 GetNormalForVertex(IntVector2 vertCoords);
	Vector3 GetNormalForVertex(int x, int y);
	IntVector2 GetVertexCoordsFromWorldPos(Vector2 xzPos);


	std::vector<Chunk*> m_chunks;
	std::vector<float> m_heights;
	std::vector<Vector3> m_normals;
	Renderable* m_renderable;

private:
	int m_numTiles;
	Vector2 m_tileSize;
	Vector2 m_heightRange;
	IntVector2 m_chunkDimensions;
	float m_tilesPerChunk;

	//void InitializeTiles();
	void RunMapGeneration(const Image& img);
	

};



