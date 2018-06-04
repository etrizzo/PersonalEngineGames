#pragma once
#include "Game/GameCommon.hpp"

class AdventureDefinition;
class Map;
class VictoryCondition;
class Player;

class Adventure{
public:
	Adventure(){};
	Adventure(AdventureDefinition* adventureDef);
	~Adventure();

	void Begin();

	void Update(float deltaSeconds);
	void Render();
	void RenderUI();

	Map* GetMap(std::string mapName);
	void CheckForVictory();

	Player* SpawnPlayer();

	void SetCurrentMap(Map* newMap);
	void MoveToMapIndex(int index);
	void RegenerateCurrentMap();

	RenderScene* GetScene();



	AdventureDefinition* m_definition = nullptr;
	Map* m_startingMap = nullptr;
	Map* m_currentMap = nullptr;
	std::map<std::string, Map*> m_mapsByName;
	std::vector<Map*>			m_mapsByIndex;		//for map testing mode
	int m_currentMapIndex;
	std::vector<VictoryCondition*> m_victoryConditions;

	bool m_hasWon = false;

private:
	void GenerateMaps();
};