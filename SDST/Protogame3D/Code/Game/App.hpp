#pragma once
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Networking/Net.hpp"

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

void CommandMakeNewLight(Command& cmd);
void CommandSetLightColor(Command& cmd);
void CommandSetLightPosition(Command& cmd);
void CommandSetAmbientLight(Command& cmd);

void CommandRemoveLight(Command& cmd);
void CommandRemoveAllLights(Command& cmd);
void CommandSetLightAttenuation(Command& cmd);

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


void ThreadTestWork( void* ) ;

void CommandConsoleNonThreadedTest( Command& cmd ) ;
void CommandConsoleThreadedTest( Command& cmd  ) ;
void CommandLogThreadTest(Command& cmd);
void CommandLogFlushTest(Command& cmd);
void CommandLogTestWarning(Command& cmd);
void CommandLogTestError(Command& cmd);
void CommandLogTest(Command& cmd);
void CommandLogShowFilter(Command& cmd);
void CommandLogHideFilter(Command& cmd);
void CommandLogToggleWhitelist(Command& cmd);
void CommandLogHideAll(Command& cmd);
void CommandLogShowAll(Command& cmd);


void CommandGetAddress(Command& cmd);
//void CommandSendMessage(Command& cmd);


//void CommandUDPTestStart(Command& cmd);
//void CommandUDPTestStop(Command& cmd);
//void CommandUDPTestSend(Command& cmd);


void CommandAddConnection(Command& cmd);
void CommandAddLocalConnectionAtIndexWithOffset(Command& cmd);
void CommandSendPing(Command& cmd);
void CommandSendAdd(Command& cmd);
void CommandSetHeartbeat(Command& cmd);
void CommandSetSimulatedLoss(Command& cmd);
void CommandSetSimulatedLatency(Command& cmd);
void CommandSetSessionSendRate(Command& cmd);
void CommandSetConnectionSendRate(Command& cmd);
void CommandStartUnreliableTest(Command& cmd);
void CommandStartReliableTest(Command& cmd);
void CommandStartInorderTest(Command& cmd);

