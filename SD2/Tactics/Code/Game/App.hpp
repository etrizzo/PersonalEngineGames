#pragma once
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"

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


void CommandSpawnActor(Command& cmd);
void CommandRecompileShaders(Command& cmd);