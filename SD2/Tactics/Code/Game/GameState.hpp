#pragma once
#include "Game/GameCommon.hpp"



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
	void ResetTime();

protected:
	float m_timeInState = 0.f;
	bool m_isTransitioning = false;
	float m_startTransitionTime = 0.f;
	float m_transitionLength = .5f;
};



class GameState_Attract: public GameState{
	void RenderUI();
	void HandleInput();
};


class GameState_Encounter: public GameState{
public:
	GameState_Encounter(std::string lvlToLoad = DEFAULT_LVL);

	void Update(float ds);
	void RenderGame();
	void RenderUI();
	void HandleInput();
};


class GameState_Paused: public GameState{
public:
	GameState_Paused(GameState_Encounter* encounter);
	void Update(float ds);
	void RenderGame();
	void RenderUI();
	void RenderTransitionEffect();
	void HandleInput();

	GameState_Encounter* m_encounterGameState;
};