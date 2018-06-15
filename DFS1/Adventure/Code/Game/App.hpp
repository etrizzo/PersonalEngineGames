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
