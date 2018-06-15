#include "AdventureDefinition.hpp"
#include "Game/Game.hpp"
#include "Game/Map.hpp"
#include "Game/MapDefinition.hpp"
#include "Game/Actor.hpp"
#include "Game/ActorDefinition.hpp"
#include "Game/Portal.hpp"
#include "Game/PortalDefinition.hpp"
#include "Game/VictoryCondition.hpp"
#include "Game/Adventure.hpp"
#include "Game/ItemDefinition.hpp"
#include "Game/Item.hpp"


std::map<std::string, AdventureDefinition*>	AdventureDefinition::s_definitions;

AdventureDefinition::AdventureDefinition(tinyxml2::XMLElement* defElement)
{
	m_name = ParseXmlAttribute(*defElement, "name", "NONAME");
	m_title = ParseXmlAttribute(*defElement, "title", m_name);
	
	tinyxml2::XMLElement* startElement = defElement->FirstChildElement("StartConditions");
	m_startMapName = ParseXmlAttribute(*startElement, "startMap", "");
	m_startTileDef = ParseXmlAttribute(*startElement, "startOnTileType", (TileDefinition*) nullptr);

	tinyxml2::XMLElement* victoryElement = defElement->FirstChildElement("VictoryConditions");
	ParseVictoryConditions(victoryElement);

	m_mapsToGenerate = std::vector<MapToGenerate*>();
	for (tinyxml2::XMLElement* mapElement = defElement->FirstChildElement("Map"); mapElement != NULL; mapElement = mapElement->NextSiblingElement("Map")){
		MapToGenerate* newMap = new MapToGenerate(mapElement);
		m_mapsToGenerate.push_back(newMap);
	}
}

AdventureDefinition::~AdventureDefinition()
{
}


AdventureDefinition * AdventureDefinition::GetAdventureDefinition(std::string definitionName)
{
	auto containsDef = s_definitions.find((std::string)definitionName); 
	AdventureDefinition* adventureDef = nullptr;
	if (containsDef != s_definitions.end()){
		adventureDef = containsDef->second;
	} else {
		ERROR_AND_DIE("No adventure named: " + definitionName);
	}
	return adventureDef;
}

void AdventureDefinition::ParseVictoryConditions(tinyxml2::XMLElement * victoryElement)
{
	m_victoryConditions = std::vector<VictoryCondition*>();
	for (tinyxml2::XMLElement* victoryConditionElement = victoryElement->FirstChildElement(); victoryConditionElement != NULL; victoryConditionElement = victoryConditionElement->NextSiblingElement()){
		VictoryCondition* newCondition = VictoryCondition::CreateVictoryCondition(victoryConditionElement);
		m_victoryConditions.push_back(newCondition);
	}
}

MapToGenerate::MapToGenerate(tinyxml2::XMLElement* mapElement)
{
	m_name = ParseXmlAttribute(*mapElement, "name", m_name);
	std::string mapDefinitionName = ParseXmlAttribute(*mapElement, "mapDefinition", "");
	m_mapDefinition = MapDefinition::GetMapDefinition(mapDefinitionName);

	m_portalsToSpawn = std::vector<PortalToSpawn*>();
	m_actorsToSpawn	 = std::vector<ActorToSpawn*>();
	m_itemsToSpawn   = std::vector<ItemToSpawn*>();

	for (tinyxml2::XMLElement* portalElement = mapElement->FirstChildElement("Portal"); portalElement != NULL; portalElement = portalElement->NextSiblingElement("Portal")){
		PortalToSpawn* newPortal = new PortalToSpawn(portalElement);
		m_portalsToSpawn.push_back(newPortal);
	}

	for (tinyxml2::XMLElement* actorElement = mapElement->FirstChildElement("Actor"); actorElement != NULL; actorElement = actorElement->NextSiblingElement("Actor")){
		ActorToSpawn* newActor = new ActorToSpawn(actorElement);
		m_actorsToSpawn.push_back(newActor);
	}

	for (tinyxml2::XMLElement* itemElement = mapElement->FirstChildElement("Item"); itemElement != NULL; itemElement = itemElement->NextSiblingElement("Item")){
		ItemToSpawn* newItem = new ItemToSpawn(itemElement);
		m_itemsToSpawn.push_back(newItem);
	}

}

MapToGenerate::~MapToGenerate()
{
}

Map* MapToGenerate::GenerateMap(int difficulty)
{
	Map* generatedMap = new Map(m_name, m_mapDefinition, difficulty);
	return generatedMap;
}

void MapToGenerate::SpawnEntities(Map* generatedMap)
{
	for(ActorToSpawn* actor : m_actorsToSpawn){
		actor->Spawn(generatedMap, generatedMap->m_difficulty);
		for (int i = 0; i < generatedMap->m_difficulty; i++){		//random chance to spawn extra baddies
			if (CheckRandomChance((.05f))){
				actor->Spawn(generatedMap, generatedMap->m_difficulty);
			}
		}
	}

	for (PortalToSpawn* portal : m_portalsToSpawn){
		portal->Spawn(generatedMap);
	}

	for (ItemToSpawn* item : m_itemsToSpawn){
		item->Spawn(generatedMap);
	}
}

PortalToSpawn::PortalToSpawn(tinyxml2::XMLElement* portalElement)
{
	std::string portalDefName		= ParseXmlAttribute(*portalElement, "type", "");
	m_toMapName						= ParseXmlAttribute(*portalElement, "toMap", "");
	m_onTileDefinition				= ParseXmlAttribute(*portalElement, "onTileType", (TileDefinition*) nullptr);
	m_toTileDefinition				= ParseXmlAttribute(*portalElement, "toTileType", (TileDefinition*) nullptr);
	std::string reciprocalTypeName	= ParseXmlAttribute(*portalElement, "reciprocalType", "");
	
	m_portalDefinition = PortalDefinition::GetPortalDefinition(portalDefName);
	if (!reciprocalTypeName.compare("")){
		m_reciprocalType = PortalDefinition::GetPortalDefinition(reciprocalTypeName);
	}
}

PortalToSpawn::~PortalToSpawn()
{
	delete m_portalDefinition;
	delete m_reciprocalType;
	delete m_toTileDefinition;
	delete m_onTileDefinition;
}

void PortalToSpawn::Spawn(Map * mapToSpawnOn)
{
	Tile tileToSpawnOn = mapToSpawnOn->GetRandomTileOfType(m_onTileDefinition);
	Map* toMap = g_theGame->m_currentState->m_currentAdventure->GetMap(m_toMapName);
	Tile reciprocalTile = toMap->GetRandomTileOfType(m_toTileDefinition);
	mapToSpawnOn->SpawnNewPortal(m_portalDefinition, tileToSpawnOn.GetCenter(), toMap, reciprocalTile.GetCenter(), 0.f, true);
}

ActorToSpawn::ActorToSpawn(tinyxml2::XMLElement* actorElement)
{
	std::string actorDefName	= ParseXmlAttribute(*actorElement, "type", "");
	m_onTileDefinition			= ParseXmlAttribute(*actorElement, "onTileType", (TileDefinition*) nullptr);
	
	m_actorDef = ActorDefinition::GetActorDefinition(actorDefName);
}

ActorToSpawn::~ActorToSpawn()
{
	delete m_onTileDefinition;
	delete m_actorDef;
}

void ActorToSpawn::Spawn(Map * mapToSpawnOn, int difficulty)
{
	Tile spawnTile = mapToSpawnOn->GetRandomTileOfType(m_onTileDefinition);
	mapToSpawnOn->SpawnNewActor(m_actorDef, spawnTile.GetCenter(), 0.f, difficulty);
}

ItemToSpawn::ItemToSpawn(tinyxml2::XMLElement * itemElement)
{
	std::string itemDefName	= ParseXmlAttribute(*itemElement, "type", "");
	m_onTileDefintion			= ParseXmlAttribute(*itemElement, "onTileType", (TileDefinition*) nullptr);

	m_itemDef = ItemDefinition::GetItemDefinition(itemDefName);
}

ItemToSpawn::~ItemToSpawn()
{
	delete m_onTileDefintion;
	delete m_itemDef;
}

void ItemToSpawn::Spawn(Map * mapToSpawnOn)
{
	Tile spawnTile = mapToSpawnOn->GetRandomTileOfType(m_onTileDefintion);
	mapToSpawnOn->SpawnNewItem(m_itemDef, spawnTile.GetCenter());
}
