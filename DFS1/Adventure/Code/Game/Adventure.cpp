#include "Game/Adventure.hpp"
#include "Game/Map.hpp"
#include "Game/AdventureDefinition.hpp"
#include "Game/VictoryCondition.hpp"
#include "Game/Game.hpp"
#include "Game/Player.hpp"
#include "Game/Item.hpp"

Adventure::Adventure(AdventureDefinition * adventureDef)
{
	m_definition = adventureDef;
	
}

Adventure::~Adventure()
{
	for (Map* m : m_mapsByIndex){
		delete(m);
	}

	for (VictoryCondition* victoryCondition : m_victoryConditions){
		delete(victoryCondition);
	}

}

void Adventure::Begin()
{
	GenerateMaps();
	if (g_theGame->m_currentState == STATE_MAPMODE){
		m_startingMap = m_mapsByIndex[0];
	} else {
		m_startingMap = GetMap(m_definition->m_startMapName);
	}
	SetCurrentMap(m_startingMap);
	m_victoryConditions = std::vector<VictoryCondition*>();
	for(VictoryCondition* condition :m_definition->m_victoryConditions){
		m_victoryConditions.push_back(condition->Clone());		//i hate this
	}

	if(g_theGame->m_transitionToState != STATE_MAPMODE){
		g_theGame->m_player = SpawnPlayer();
	} else {
		g_theGame->m_player = nullptr;
	}
}

void Adventure::Update(float deltaSeconds)
{
	
	m_currentMap->Update(deltaSeconds);
	CheckForVictory();
	m_currentMap->RemoveDoomedEntities();
}

void Adventure::Render()
{
	m_currentMap->Render();
	RenderUI();
}

void Adventure::RenderUI()
{
	//adventure title - top left
	AABB2 cameraBounds = m_currentMap->GetCameraBounds();
	float screenWidth = cameraBounds.GetWidth();
	float screenHeight = cameraBounds.GetHeight();
	g_theRenderer->DrawTextInBox2D(m_definition->m_title, cameraBounds, Vector2(0.02f,0.98f), screenHeight * .02f, TEXT_DRAW_SHRINK_TO_FIT);

	if (!g_theGame->m_fullMapMode){
		//player stats - bottom left
		Vector2 boxSize = Vector2(screenWidth * .3f, screenWidth * .2f);
		AABB2 statBox = AABB2(cameraBounds.mins, cameraBounds.mins + boxSize);
		g_theRenderer->DrawAABB2(statBox, RGBA(200,200,100,200));
		g_theGame->m_player->RenderStatsInBox(statBox, RGBA(0,0,0));

		//currently equipped weapon - bottom right
		Item* weapon = g_theGame->m_player->m_equippedItems[EQUIP_SLOT_WEAPON];
		if (weapon != nullptr){
			AABB2 weaponBox = AABB2(cameraBounds.maxs.x - (boxSize.y * .5f), cameraBounds.mins.y, cameraBounds.maxs.x, cameraBounds.mins.y + (boxSize.y * .5f));
			weaponBox.AddPaddingToSides(-.05f, -.05f);
			weapon->RenderImageInBox(weaponBox);
		}
	}

}

Map * Adventure::GetMap(std::string mapName)
{
	auto containsMap = m_mapsByName.find((std::string)mapName); 
	Map* foundMap = nullptr;
	if (containsMap != m_mapsByName.end()){
		foundMap = containsMap->second;
	} else {
		ERROR_AND_DIE("No map named: " + mapName);
	}
	return foundMap;
}

void Adventure::CheckForVictory()
{
	if (!m_hasWon){
		bool finishedAllConditions = true;
		for(VictoryCondition* condition : m_victoryConditions){
			if (!condition->CheckIfComplete()){
				finishedAllConditions = false;
			}
		}
		m_hasWon = finishedAllConditions;
		if (m_hasWon){
			g_theGame->StartStateTransition(STATE_VICTORY, .5f, RGBA(0,0,0,128));
		}
	}
	
}

Player * Adventure::SpawnPlayer()
{
	Tile spawnTile = m_startingMap->GetRandomTileOfType(m_definition->m_startTileDef);
	Player* newPlayer = m_currentMap->SpawnNewPlayer(spawnTile.GetCenter());
	return newPlayer;
}

void Adventure::SetCurrentMap(Map * newMap)
{
	if (m_currentMap != nullptr){
		m_currentMap->StopMusic();
	}
	m_currentMap = newMap;
	m_currentMap->StartMusic();
}

void Adventure::MoveToMapIndex(int index)
{
	m_currentMapIndex = ClampInt(index, 0, (int) m_mapsByIndex.size()-1);
	SetCurrentMap(m_mapsByIndex[m_currentMapIndex]);
}

void Adventure::RegenerateCurrentMap()
{
	MapToGenerate* mapToGenerate = nullptr;
	for (MapToGenerate* mapInList : m_definition->m_mapsToGenerate){
		if (!m_currentMap->m_name.compare(mapInList->m_name)){
			mapToGenerate = mapInList;
			break;
		}
	}
	Map* newMap = mapToGenerate->GenerateMap();
	mapToGenerate->SpawnEntities(newMap);
	//m_mapsByName.insert(std::pair<std::string, Map*>(m_currentMap->m_name, newMap));
	m_mapsByName[m_currentMap->m_name] = newMap;
	m_mapsByIndex[m_currentMapIndex] = newMap;
	delete(m_currentMap);
	m_currentMap = newMap;

}

void Adventure::GenerateMaps()
{
	m_mapsByName = std::map<std::string, Map*>();
	m_mapsByIndex = std::vector<Map*>();
	for(MapToGenerate* mapToGenerate : m_definition->m_mapsToGenerate){
		Map* newMap = mapToGenerate->GenerateMap();
		m_mapsByName.insert(std::pair<std::string, Map*>(newMap->m_name, newMap));
	}

	for(MapToGenerate* mapToGenerate : m_definition->m_mapsToGenerate){
		Map* existingMap = GetMap(mapToGenerate->m_name);
		mapToGenerate->SpawnEntities(existingMap);
		m_mapsByIndex.push_back(existingMap);
		existingMap->SortAllEntities();
	}
}
