#pragma once
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Networking/Net.hpp"
#include "Engine/DataTypes/DirectedGraph.hpp"

class App{
public:
	~App();											
	App() {};	
	App(HINSTANCE applicationInstanceHandle);

	Vector3 m_nearBottomLeft;
	Vector3 m_farTopRight;
	RGBA m_backgroundColor;

	double m_appTime;
	float m_deltaTime;
	bool firstFrame = true;

	


public:
	void RunFrame();

	void Update();

	void Render();

	bool IsQuitting();

	void Startup();

private:
	//place to register game-specific commands
	void RegisterCommands();
	void HandleInput();
	void PostStartup();
	

	
};

void CommandDebugRenderStartup(Command& cmd);
void CommandDebugRenderShutdown(Command& cmd);
void CommandDebugRenderClear(Command& cmd);
void CommandDebugRenderToggle(Command& cmd);
void CommandDebugRenderSetDepth(Command& cmd);
void CommandDebugRenderSetTask(Command& cmd);
void CommandDebugPrintTasks(Command& cmd);
void CommandDebugDrawTask(Command& cmd);

void CommandDebugDetachCamera(Command& cmd);
void CommandDebugReattachCamera(Command& cmd);

void CommandRecompileShaders(Command& cmd);

void CommandSetGodMode(Command& cmd);
void CommandToggleGodMode(Command& cmd);

void CommandToggleProfiler(Command& cmd);
void CommandPrintProfilerReport(Command& cmd);
void CommandProfilePause(Command& cmd);
void CommandProfileResume(Command& cmd);



void AddProfilerFrameAsTreeToConsole();
void AddProfilerFrameAsFlatToConsole();
void PrintTree(ProfilerReportEntry* tree, int depth = 0);

std::string FormatProfilerReport(ProfilerReportEntry* entry, int parentCount = 0);

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
