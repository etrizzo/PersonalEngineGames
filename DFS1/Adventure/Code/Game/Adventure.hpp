#pragma once
#include "Game/GameCommon.hpp"

class AdventureDefinition;
class Map;
class VictoryCondition;
class Player;
class Quest;

class Adventure{
public:
	Adventure(){};
	Adventure(AdventureDefinition* adventureDef, int difficulty = 0);
	~Adventure();

	void Clear();	//empties the adventure
	void Begin();

	void Update(float deltaSeconds);
	void Render();
	void RenderUI();

	Map* GetMap(std::string mapName);
	void CheckForVictory();

	void DebugWinAdventure();
	void DebugCompleteQuest(int index);

	Player* SpawnPlayer();

	void SetCurrentMap(Map* newMap);
	void MoveToMapIndex(int index);
	void RegenerateCurrentMap();

	RenderScene2D* GetScene();

	void SetDifficulty(int difficulty);
	void GoToMap(std::string mapName);



	AdventureDefinition* m_definition = nullptr;
	Map* m_startingMap = nullptr;
	Map* m_currentMap = nullptr;
	std::map<std::string, Map*> m_mapsByName;
	std::vector<Map*>			m_mapsByIndex;		//for map testing mode
	int m_currentMapIndex;
	std::vector<Quest*> m_quests;
	//std::vector<VictoryCondition*> m_victoryConditions;

	bool m_adventureBegun = false;
	bool m_hasWon = false;
	int m_difficulty = 0;

private:
	void GenerateMaps();
};