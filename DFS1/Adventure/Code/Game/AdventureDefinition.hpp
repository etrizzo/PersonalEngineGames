#pragma once;
#include "Game/GameCommon.hpp"

class PortalDefinition;
class ActorDefinition;
class ItemDefinition;
class MapDefinition;
class TileDefinition;
class VictoryCondition;
class Map;

struct ActorToSpawn;
struct PortalToSpawn;
struct ItemToSpawn;
struct MapToGenerate;


class AdventureDefinition{
public:
	AdventureDefinition(){};
	AdventureDefinition(tinyxml2::XMLElement* defElement);
	~AdventureDefinition();

	std::string m_name;
	std::string m_title;
	std::vector<MapToGenerate*> m_mapsToGenerate;
	std::vector<Quest*> m_victoryConditions;
	std::string m_startMapName;
	TileDefinition* m_startTileDef;


	static std::map< std::string, AdventureDefinition* >		s_definitions;
	static AdventureDefinition* GetAdventureDefinition(std::string definitionName);
private:
	void ParseQuests(tinyxml2::XMLElement* victoryElement);
};


struct MapToGenerate{
public:
	MapToGenerate(tinyxml2::XMLElement* mapElement);
	~MapToGenerate();

	Map* GenerateMap(int difficulty);
	void SpawnEntities(Map* generatedMap);

	MapDefinition* m_mapDefinition;
	std::string m_name;
	std::vector<ActorToSpawn*> m_actorsToSpawn;
	std::vector<PortalToSpawn*> m_portalsToSpawn;
	std::vector<ItemToSpawn*> m_itemsToSpawn;
};

struct PortalToSpawn{
public:
	PortalToSpawn(tinyxml2::XMLElement* portalElement);
	~PortalToSpawn();

	void Spawn(Map* mapToSpawnOn);

	PortalDefinition* m_portalDefinition = nullptr;
	std::string m_toMapName;
	TileDefinition* m_onTileDefinition = nullptr;
	TileDefinition* m_toTileDefinition = nullptr;
	PortalDefinition* m_reciprocalType = nullptr;
};

struct ActorToSpawn{
public:
	ActorToSpawn(tinyxml2::XMLElement* actorElement);
	~ActorToSpawn();

	void Spawn(Map* mapToSpawnOn, int difficulty);

	ActorDefinition* m_actorDef= nullptr;
	TileDefinition* m_onTileDefinition = nullptr;
};

struct ItemToSpawn{
public:
	ItemToSpawn(tinyxml2::XMLElement* itemElement);
	~ItemToSpawn();

	void Spawn(Map* mapToSpawnOn);

	ItemDefinition* m_itemDef = nullptr;
	TileDefinition* m_onTileDefintion = nullptr;
};