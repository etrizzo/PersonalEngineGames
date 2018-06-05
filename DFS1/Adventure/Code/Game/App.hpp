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

	//void CheckKeys();
	//void CheckAttractKeys();
	//void CheckPlayingKeys();
	//void CheckPauseKeys();
	//void CheckInventoryKeys();
	//void CheckMapmodeKeys();
	//void CheckVictoryKeys();
	//void CheckDefeatKeys();

	//bool WasBackJustPressed();		//'back' functionality - esc, b
	//bool WasStartJustPressed();		//BROAD 'start' functionality - p, enter, start, a
	//bool WasSelectJustPressed();		//enter or a ONLY
	//bool WasPauseJustPressed();			//p or start ONLY
	//bool WasExitJustPressed();			//ESC only
	//bool WasUpJustPressed();
	//bool WasDownJustPressed();
	//bool WasRightJustPressed();
	//bool WasLeftJustPressed();
	//void CheckArrowKeys();
};

void CommandRecompileShaders(Command& cmd);
