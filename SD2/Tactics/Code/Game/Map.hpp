#pragma once
#include "Game/Block.hpp"
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

class OldCamera{
public:
	OldCamera(){};
	OldCamera(Vector2 position, float orientation, float zoomFactor);
	Vector2 m_worldPos;
	float m_orientationDegrees;
	float m_numTilesInViewVertically;
	Vector2 m_bottomLeft;
	Vector2 m_topRight;

	void Update(float deltaSeconds, Vector2 playerPos);

};

class Encounter;


class Map{
	

public:
	Map(Encounter* encounter);
	~Map();
	Map(std::string name, MapDefinition* mapDef);

	std::string m_name;
	MapDefinition* m_definition;
	IntVector3 m_dimensions;
	std::vector<Block> m_tiles;
	Camera* m_camera;
	Encounter* m_encounter;

	bool LoadFromHeightMap(std::string fileName, const int height = 32);

	void Render();
	void RenderTiles();

	void Update(float deltaSeconds);


	void UpdateEntities(float deltaSeconds);
	void RenderEntities();
	void CheckEntityInteractions();

	int GetWidth() const;
	int GetHeight() const;
	Block* TileAt(int x, int y, int z);
	Block* TileAt(IntVector3 coordinates);
	Block* TileAtFloat(float x, float y, float z);
	Block* TileAtFloat(Vector3 floatCoordinates);
	Block* GetHighestTileAtXY(int x, int y);
	Block* GetHighestTileAtXY(IntVector2 coords);
	float GetHeightAtXY(IntVector2 coords);
	Block GetRandomTileOfType(BlockDefinition* tileType) const;
	Block GetRandomBaseTile() const;
	int GetNumBlocks() const { return m_numTiles; };

	bool HasLineOfSight(const Vector2& startPos, const Vector2& endPos, float maxDistance = 20.f);
	RaycastResult2D Map::Raycast(const Vector2& startPos, const Vector2& direction, float maxDistance) ;

	void AddEntityToMap(Entity* entity);
	void RemoveEntityFromMap(Entity* entity);
	void RemoveEntities();
	void SetCamera();
	
	//make a "remove from map" method to keep this private
	std::vector<Entity*> m_allEntities;

	//void MoveCursor(const IntVector2 direction);
	/*Vector3 GetCursorPos();
	IntVector2 GetCursorPosXY() {return m_cursorPos; };*/
	//void SetCursorPos(IntVector2 newPos);

	void FillSeededDistanceMap(Heatmap& distMpa, const Block* destinationBlock = nullptr, const int& maxRange = 999, const int& jumpHeight = 2, bool blockActors = true);
	void GetManhattanNeighbors(IntVector2 XYpos, Block (&neighbors)[4]);
	Entity* GetEntityInColumn(const IntVector2& pos);

	Vector3 GetWorldPosition(Vector2 xyPos);
	Vector3 GetWorldPosition(IntVector2 xyCoords);
	
	bool IsOnBoard(IntVector2 xyPos);

	std::vector<IntVector2> GetPath(const IntVector2& startPos, const IntVector2& endPos, const Heatmap& distanceMap);

	void ApplyDamageAOE(IntVector2 targetLocation, int areaRange, int verticalRange, int damageToApply);

private:
	int m_numTiles;
	//IntVector2 m_cursorPos = IntVector2(0,5);

	int GetHeightFromHeightMap(Image* img, int x, int y);

	void ClampCameraToMap();
	void InitializeTiles();
	
	void RunMapGeneration();
	//void RenderCursor();
	

};



