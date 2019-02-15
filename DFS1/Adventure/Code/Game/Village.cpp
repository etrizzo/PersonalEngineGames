#include "Village.hpp"
#include "Game/VillageDefinition.hpp"
#include "Game/Map.hpp"
#include "Game/Actor.hpp"
#include "Game/StoryGraph.hpp"

Village::Village(VillageDefinition * definition, Map * map, int numActorsToSpawn)
{
	m_villageName = definition->GetRandomVillageNameAndCrossOff();
	m_definition = definition;
	m_faction = definition->GetRandomFaction();
	m_map = map;

	m_center = m_map->m_generationMask->GetCenter();
	//copy the area in case we wanna do stuff with it later (village owns this mask)
	m_area = m_map->m_generationMask->Clone();

	SpawnResidents(numActorsToSpawn);		//creates the actors
	ReadGraphData();						//load the graph data
	ConnectResidentsToGraphCharacters();	//sync graph characters with the residents of the villages
	GenerateGraph();						//Actually generate the nodes of the graph
	SetResidentDialogues();					//Set the residents dialogues based on the graph state
}

Village::~Village()
{
	delete m_area;
	delete m_villageGraph;
	//doesn't OWN it's residents, they still belong to the map. Just references.
}

void Village::UpdateVillageStory()
{
	//do whatever to update the state of the story according to quests & shit
	SetResidentDialogues();
}

void Village::SpawnResidents(int numToSpawn)
{
	TileDefinition* spawnTileDefinition = m_definition->m_baseTileDefinition ;
	for (int i = 0; i < numToSpawn; i++)
	{
		if (spawnTileDefinition == nullptr){
			spawnTileDefinition = m_map->m_mapDef->m_defaultTile;
		}
		Tile* spawnTile = nullptr;
		
		spawnTile = GetSpawnTileOfType(spawnTileDefinition);
		
		if (spawnTile != nullptr){
			Actor* newActor = m_map->SpawnNewActor(m_definition->GetRandomResidentDefinition(), spawnTile->GetCenter());
			newActor->SetVillage(this);
		}
	}
}

void Village::ReadGraphData()
{
}

void Village::ConnectResidentsToGraphCharacters()
{
}

void Village::GenerateGraph()
{
}

void Village::SetResidentDialogues()
{
}

Tile * Village::GetSpawnTileOfType(TileDefinition * def)
{
	IntVector2 pos = m_area->GetRandomPointInArea();
	Tile* tile = m_map->TileAt(pos);
	int tries = 0;
	while (tries < 1000 && ((tile == nullptr) || ((tile->m_tileDef != def) || tile->HasBeenSpawnedOn()))){
		pos = m_area->GetRandomPointInArea();
		tile = m_map->TileAt(pos);
		tries++;
	}

	if (tile != nullptr){
		if ((tile->m_tileDef != def) || tile->HasBeenSpawnedOn()) {
			return nullptr;
		} else {
			m_map->MarkTileForSpawn(pos);
		}
	}
	return tile;
}
