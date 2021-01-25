#pragma once
#include "Game/GameCommon.hpp"
#include "Game/Menu.hpp"


class GameState_Playing;
class RenderScene2D;



class GameState
{
public:
	GameState() {};
	GameState(float transitionLength) { m_transitionLength = transitionLength; };
	~GameState() {};
	// How many of these should be pure virutal??
	virtual void Render();		//renders game, then UI, then transitions
	virtual void Update(float ds = 0) { m_timeInState+=ds; }; 
	virtual void RenderUI() {};
	virtual void RenderGame() {};
	virtual void RenderTransition();
	virtual void RenderTransitionEffect(float t);
	virtual void HandleInput() = 0;

	virtual void StartTransition();
	virtual RenderScene2D* GetScene();
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
	void Update(float ds);
	void RenderUI();
	void HandleInput();
};

class GameState_Paused: public GameState{
public:
	GameState_Paused(GameState_Playing* playstate);
	void Update(float ds);
	void RenderGame();
	void RenderUI();
	void RenderTransitionEffect();
	void HandleInput();

	GameState_Playing* m_encounterGameState;
};