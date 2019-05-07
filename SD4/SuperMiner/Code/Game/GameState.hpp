#pragma once
#include "Game/GameCommon.hpp"
#include "Game/Menu.hpp"


class GameState_Playing;
class RenderScene;



class GameState
{
public:
	GameState() {};
	GameState(float transitionLength) { m_transitionLength = transitionLength; };
	virtual ~GameState() {};
	// How many of these should be pure virutal??
	virtual void Render();		//renders game, then UI, then transitions
	virtual void Update(float ds = 0) { m_timeInState+=ds; }; 
	virtual void RenderUI() {};
	virtual void RenderGame() {};
	virtual void RenderTransition();
	virtual void RenderTransitionEffect(float t);
	virtual void HandleInput() = 0;
	
	virtual void EnterState(); //do any setup for when you immediately enter the state 
	
	virtual void StartTransition();
	virtual RenderScene* GetScene();
	void ResetTime();

protected:
	float m_timeInState = 0.f;
	bool m_isTransitioning = false;
	float m_startTransitionTime = 0.f;
	float m_transitionLength = .5f;
};

class GameState_Loading: public GameState{
public:
	//GameState_Loading();
	void Render() override;
	void Update(float ds) override;
	void HandleInput();

	bool m_firstFrame = true;
	//void RenderUI();
};

class GameState_Attract: public GameState{
public:
	GameState_Attract();
	Menu* m_mainMenu;
	void EnterState() override;
	void Update(float ds);
	void RenderUI();
	void HandleInput();
};

class GameState_Select: public GameState{
public:
	GameState_Select();
	Menu* m_selectMenu;
	void Update(float ds);
	void RenderUI();
	void HandleInput();
};


class GameState_Paused: public GameState{
public:
	GameState_Paused(GameState_Playing* playstate);
	void Update(float ds) override;
	void RenderGame() override;
	void RenderUI() override;
	void RenderTransitionEffect(float t) override;
	void HandleInput() override;

	void EnterState() override;

	GameState_Playing* m_encounterGameState;
};


class GameState_Victory: public GameState{
public:
	GameState_Victory(GameState_Playing* playState);
	void Update(float ds) override;
	void RenderGame() override;
	void RenderUI() override;
	void HandleInput() override;

	GameState_Playing* m_encounterGameState;
};

class GameState_Defeat: public GameState{
public:
	GameState_Defeat(GameState_Playing* playState);
	void Update(float ds) override;
	void RenderGame() override;
	void RenderUI() override;
	void HandleInput() override;

	GameState_Playing* m_encounterGameState;
};