#include "Game/Adventure.hpp"
#include "Game/Map.hpp"
#include "Game/AdventureDefinition.hpp"
#include "Game/VictoryCondition.hpp"
#include "Game/Game.hpp"
#include "Game/Player.hpp"
#include "Game/Actor.hpp"
#include "Game/Item.hpp"
#include "Game/Quest.hpp"
#include "Game/Party.hpp"

Adventure::Adventure(AdventureDefinition * adventureDef, int difficulty)
{
	m_definition = adventureDef;
	m_adventureBegun = false;
	m_difficulty = difficulty;
	
}

Adventure::~Adventure()
{
	g_theGame->m_party->MovePartyToMap(nullptr);
	for (Map* m : m_mapsByIndex){
		delete(m);
	}

	for (Quest* q : m_quests){
		delete(q);
	}



}

void Adventure::Clear()
{
	g_theGame->m_party->MovePartyToMap(nullptr);
	for (Map* m : m_mapsByIndex){
		delete(m);
	}

	for (Quest* q : m_quests){
		delete(q);
	}
}

void Adventure::Begin()
{
	m_adventureBegun = true;
	
	GenerateMaps();
	//if (g_theGame->m_currentState == STATE_MAPMODE){
	//	m_startingMap = m_mapsByIndex[0];
	//} else {
	//	m_startingMap = GetMap(m_definition->m_startMapName);
	//}

	m_startingMap = GetMap(m_definition->m_startMapName);

	SetCurrentMap(m_startingMap);
	m_quests = std::vector<Quest*>();
	for(QuestDefinition* q :m_definition->m_quests){
		m_quests.push_back(new Quest(q, this));		//i hate this
	}

	if (g_theGame->m_party->IsEmpty()){
		g_theGame->m_party->AddActorAndSetAsPlayer(SpawnPlayer());
		g_theGame->m_party->SetMap(m_currentMap);
	} else {
		g_theGame->m_party->VoidMaps();
		Tile spawnTile = m_startingMap->GetRandomTileOfType(m_definition->m_startTileDef);
		g_theGame->m_party->MovePartyToMap( m_currentMap, spawnTile.GetCenter());
	}
	//if(g_theGame->m_transitionToState != STATE_MAPMODE){
	//	g_theGame->m_player = SpawnPlayer();
	//} else {
	//	g_theGame->m_player = nullptr;
	//}
}

void Adventure::Update(float deltaSeconds)
{
	
	m_currentMap->Update(deltaSeconds);
	g_theGame->m_party->Update(deltaSeconds);
	CheckForVictory();
	m_currentMap->RemoveDoomedEntities();
}

void Adventure::Render()
{
	m_currentMap->Render();
	//RenderUI();
}

void Adventure::PostRender()
{
	m_currentMap->PostRender();
}

void Adventure::RenderUI()
{
	//adventure title - top left
	AABB2 cameraBounds = g_theGame->GetUIBounds();
	float screenWidth = cameraBounds.GetWidth();
	float screenHeight = cameraBounds.GetHeight();
	//g_theRenderer->DrawTextInBox2D(m_definition->m_title, cameraBounds, Vector2(0.02f,0.98f), screenHeight * .02f, TEXT_DRAW_SHRINK_TO_FIT);
	g_theRenderer->DrawTextInBox2D(std::to_string(m_difficulty), cameraBounds, Vector2(0.98f,0.98f), screenHeight * .02f, TEXT_DRAW_SHRINK_TO_FIT);

	m_currentMap->RenderUI();

	if (!g_theGame->m_fullMapMode){
		//player stats - bottom left
		Vector2 boxSize = Vector2(screenWidth * .3f, screenWidth * .2f);
		AABB2 statBox = cameraBounds.GetPercentageBox(0.f, .9f, .4f, 1.f);
		g_theGame->m_party->RenderPartyUI(statBox);
		//g_theRenderer->DrawAABB2(statBox, RGBA(200,200,100,200));
		//g_theGame->m_party->GetPlayerCharacter()->RenderStatsInBox(statBox, RGBA(0,0,0));

		////currently equipped weapon - bottom right
		//Item* weapon = g_theGame->m_party->GetPlayerCharacter()->m_equippedItems[EQUIP_SLOT_WEAPON];
		//if (weapon != nullptr){
		//	AABB2 weaponBox = AABB2(cameraBounds.maxs.x - (boxSize.y * .5f), cameraBounds.mins.y, cameraBounds.maxs.x, cameraBounds.mins.y + (boxSize.y * .5f));
		//	//weaponBox.AddPaddingToSides(-.05f, -.05f);
		//	weapon->RenderImageInBox(weaponBox);
		//}
	}

}

Map * Adventure::GetMap(std::string mapName)
{
	auto containsMap = m_mapsByName.find((std::string)mapName); 
	Map* foundMap = nullptr;
	if (containsMap != m_mapsByName.end()){
		foundMap = containsMap->second;
	}
	return foundMap;
}

void Adventure::CheckForVictory()
{
	if (!m_hasWon){
		bool finishedAllQuests = true;
		for(Quest* quest : m_quests){
			//Check (and update) each quest
			if (!quest->UpdateAndCheckIfComplete()){
				if (quest->IsMainQuest()){		//if you have not completed a main quest, you are not done yet
					finishedAllQuests = false;
				}
			}
		}
		m_hasWon = finishedAllQuests;
		if (m_hasWon){
			g_theGame->TransitionToState(new GameState_Victory((GameState_Encounter*) g_theGame->m_currentState));
		}
	}
	
}

void Adventure::DebugWinAdventure()
{
	for (int i = 0; i < (int) m_quests.size(); i++){
		DebugCompleteQuest(i);
	}
}

void Adventure::DebugCompleteQuest(int index)
{
	if (index >= 0 && index < (int) m_quests.size()){
		m_quests[index]->CompleteQuest();
	} else {
		ConsolePrintf(RGBA::RED, ("No quest at index " + std::to_string(index) + ". There are " +  std::to_string(m_quests.size()) + " active quests").c_str());
	}
}

Actor * Adventure::SpawnPlayer()
{
	Tile spawnTile = m_startingMap->GetSpawnTileOfType(m_definition->m_startTileDef);
	Actor* newPlayer = m_currentMap->SpawnNewPlayer(spawnTile.GetCenter());
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
	Map* newMap = mapToGenerate->GenerateMap(m_difficulty);
	mapToGenerate->SpawnEntities(newMap);
	//m_mapsByName.insert(std::pair<std::string, Map*>(m_currentMap->m_name, newMap));
	m_mapsByName[m_currentMap->m_name] = newMap;
	m_mapsByIndex[m_currentMapIndex] = newMap;
	delete(m_currentMap);
	m_currentMap = newMap;

}

RenderScene2D * Adventure::GetScene()
{
	return m_currentMap->m_scene;
}

void Adventure::SetDifficulty(int difficulty)
{
	m_difficulty = difficulty;
	Clear();
	Begin();
}

void Adventure::GoToMap(std::string mapName)
{
	Map* newMap = GetMap(mapName);
	if (newMap == nullptr){
		ConsolePrintf(RGBA::RED, ("No map called: " + mapName + ". use print_map_names to see current adventure maps").c_str());
	} else {
		Tile t = newMap->GetRandomBaseTile();
		g_theGame->m_party->GetPlayerCharacter()->SetPosition(t.GetCenter(), newMap);
	}
}

void Adventure::GenerateMaps()
{
	m_mapsByName = std::map<std::string, Map*>();
	m_mapsByIndex = std::vector<Map*>();
	for(MapToGenerate* mapToGenerate : m_definition->m_mapsToGenerate){
		Map* newMap = mapToGenerate->GenerateMap(m_difficulty);
		m_mapsByName.insert(std::pair<std::string, Map*>(newMap->m_name, newMap));
	}

	for(MapToGenerate* mapToGenerate : m_definition->m_mapsToGenerate){
		Map* existingMap = GetMap(mapToGenerate->m_name);
		mapToGenerate->SpawnEntities(existingMap);
		m_mapsByIndex.push_back(existingMap);
		existingMap->SortAllEntities();
	}

	
}
