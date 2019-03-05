#pragma once
#include "Game/GameCommon.hpp"

class Actor;
class VillageDefinition;
class Map;
class StoryGraph;
class Tile;
class TileDefinition;

class Village
{
public:
	Village(VillageDefinition* definition, Map* map, int numActorsToSpawn);
	~Village();

	void ProgressVillageStoryTime();

	std::string m_villageName;
	VillageDefinition* m_definition;

	std::string m_faction;
	IntVector2 m_center;
	AreaMask* m_area;
	Map* m_map;

	std::vector<Actor*> m_residents;	//villagers also should have a ref to their village 
	StoryGraph* m_villageGraph;

	StoryEdge* m_currentEdge;

private:
	void SpawnResidents(int numToSpawn);
	void ReadGraphData();
	void ConnectResidentsToGraphCharacters();
	void GenerateGraph();
	void SetResidentDialogues();

	Tile* GetSpawnTileOfType(TileDefinition* def);

};