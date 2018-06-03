#pragma once
#include "Game/Map.hpp"

struct Room;

class MapGenStep
{
public:
	MapGenStep( const tinyxml2::XMLElement& genStepXmlElement );
	~MapGenStep();
	void RunIterations(Map& map);
	virtual void Run( Map& map ) = 0; // "pure virtual", MUST be overridden by subclasses

public:
	static MapGenStep* CreateMapGenStep( const tinyxml2::XMLElement& genStepXmlElement );

protected:
	std::string		m_name;
	IntRange m_iterations = IntRange(1);
	float m_chanceToRun = 1.f;
};


class MapGenStep_FillAndEdge: public MapGenStep{
public:
	MapGenStep_FillAndEdge( const tinyxml2::XMLElement& generationStepElement);

	void Run( Map& map );

	TileDefinition*		m_fillTileDef = nullptr;
	TileDefinition*		m_edgeTileDef = nullptr;
	int					m_edgeThickness = 1;
	float				m_chanceToChangeEachTile = 1.f;
};

class MapGenStep_SpawnActor: public MapGenStep{
public:
	MapGenStep_SpawnActor(const tinyxml2::XMLElement& generationStepElement);
	void Run(Map& map);

	std::string m_actorName				= std::string();
	TileDefinition* m_spawnOnTileDef	= nullptr;
};

class MapGenStep_SpawnItem: public MapGenStep{
public:
	MapGenStep_SpawnItem(const tinyxml2::XMLElement& generationStepElement);
	void Run(Map& map);

	std::string m_itemName				= std::string();
	TileDefinition* m_spawnOnTileDef	= nullptr;
};


class MapGenStep_FromFile: public MapGenStep{
public:
	MapGenStep_FromFile(const tinyxml2::XMLElement& generationStepElement);
	void Run( Map& map );

	std::string m_fileName = (std::string) "";
	IntVector2 m_startCoordinates = IntVector2(0,0);
};

class MapGenStep_Mutate:  public MapGenStep{
public:
	MapGenStep_Mutate(const tinyxml2::XMLElement& generationStepElement);
	void Run(Map& map);

	TileDefinition* m_ifType = nullptr;
	TileDefinition* m_toType = nullptr;
	float m_chanceToMutate = 1.f;
};

class MapGenStep_PerlinNoise:  public MapGenStep{
public:
	MapGenStep_PerlinNoise(const tinyxml2::XMLElement& generationStepElement);
	void Run(Map& map);

	TileDefinition* m_ifType = nullptr;
	TileDefinition* m_toType = nullptr;
	FloatRange m_noiseRange = FloatRange(0.f);
	//IntRange m_gridRange	= IntRange(20);
	float m_gridSize = 20.f;

protected:
	bool CheckPerlinNoiseAtTile(Tile* tile);
};

class MapGenStep_CellularAutomata: public MapGenStep{
public:
	MapGenStep_CellularAutomata(const tinyxml2::XMLElement& generationStepElement);
	void Run(Map& map);

	float m_chanceToMutate = 1.f;
	IntRange m_requiredNeighborCount = IntRange(1,8);
	TileDefinition* m_ifTypeDef			= nullptr;
	TileDefinition* m_changeToTypeDef	= nullptr;
	TileDefinition* m_ifNeighborTypeDef	= nullptr;
};

class MapGenStep_RoomAndPath: public MapGenStep{
public:
	MapGenStep_RoomAndPath(const tinyxml2::XMLElement& generationStepElement);
	void Run(Map& map);

	IntRange	m_numRooms		= IntRange(1);
	IntRange	m_roomWidth		= IntRange(3);
	IntRange	m_roomHeight	= IntRange(3);
	IntRange	m_numOverlaps	= IntRange(0);

	IntRange	m_extraPaths			= IntRange(0);
	bool		m_loopPaths				= true;
	float		m_straightPathChance	= 0.f;

	TileDefinition* m_floorTileDef	= nullptr;
	TileDefinition* m_wallTileDef	= nullptr;
	TileDefinition* m_pathTileDef	= nullptr;
private:
	std::vector<Room*> m_rooms;
	void GenerateRooms(Map& map);
	void AddRoomsToMap(Map& map);
	void GeneratePaths(Map& map);
	void MakeDijkstraPathBetweenRooms(Room* startRoom, Room* endRoom, Map& map);
	void MakePathBetweenRooms(Room* startRoom, Room* endRoom, Map& map);
};

class MapGenStep_Conway: public MapGenStep{
public:
	MapGenStep_Conway(const tinyxml2::XMLElement& generationStepElement);
	void Run(Map& map);

	int m_birthLimit;
	int m_deathLimit;
	TileDefinition* m_liveTileDefinition;
	TileDefinition* m_deadTileDefintion;
};



struct Room{
public:
	Room(){};
	Room(IntVector2 center, int roomWidth, int roomHeight);
	Room(IntVector2 center, IntVector2 dimensions);

	IntVector2 m_center;
	IntVector2 m_minCoords;
	IntVector2 m_maxCoords;
	int m_width;
	int m_height;

	bool DoesRoomOverlap(Room* overlapRoom) const;
	AABB2 GetBoundsAsAABB2() const;
	AABB2 GetBoundsWithPadding(int padding = 1) const;
};