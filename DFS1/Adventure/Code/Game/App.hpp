#pragma once
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"

class App{
public:
	~App();											
	App() {};	
	App(HINSTANCE applicationInstanceHandle);

	Vector2 m_bottomLeft;
	Vector2 m_topRight;
	RGBA m_backgroundColor;


	double m_appTime;
	float m_deltaTime;

	
	


public:
	void RunFrame();

	void Update();

	void Render();

	bool IsQuitting();

private:
	//place to register game-specific commands
	void RegisterCommands();
	void HandleInput();
};

void CommandRecompileShaders(Command& cmd);
void CommandShowStats(Command& cmd);	//render stats by actors, eventually
void CommandToggleGodMode(Command& cmd);
void CommandSetDifficulty(Command& cmd);
void CommandGoToMap(Command& cmd);
void CommandPrintMapNames(Command& cmd);
void CommandWinAdventure(Command& cmd);
void CommandCompleteQuest(Command& cmd);
void CommandSetSpeed(Command& cmd);

void CommandSpawnActor(Command& cmd);
void CommandSpawnItem(Command& cmd);
void CommandToggleEdges(Command& cmd);
void CommandRemoveInvalidTiles(Command& cmd);
void CommandAddTufts(Command& cmd);
void CommandEdgeShore(Command& cmd);
void CommandEdgeHighPriority(Command& cmd);
void CommandEdgeLowPriority(Command& cmd);
void CommandEdgeTiles(Command& cmd);
void CommandRerollPlayerAppearance(Command& cmd);


void CommandSetSeed(Command& cmd);
void CommandPrintGraph(Command& cmd);
void CommandGenerateGraph(Command& cmd);
void CommandFindPath(Command& cmd);
void CommandPrintStory(Command& cmd);
void CommandFindBranches(Command& cmd);
void CommandSetBranchChance(Command& cmd);

void CommandResetGraph(Command& cmd);
void CommandGenerateSkeleton(Command& cmd);
void CommandAddDetails(Command& cmd);
void CommandGeneratePairs(Command& cmd);

void CommandReadDefaultData(Command& cmd);
void CommandReadMurderData(Command& cmd);
void CommandReadDialogueData(Command& cmd);

void CommandProgressStory(Command& cmd);