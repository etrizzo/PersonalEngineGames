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
	void HandleInput();
	void PostStartup();

	//place to register game-specific commands
	void RegisterCommands();

	void RegisterGameCommands();
	void RegisterDebugSystemCommands();
	void RegisterNetCommands();
	void RegisterLogSystemCommands();
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

