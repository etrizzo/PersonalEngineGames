#pragma once
#include "Game/GameCommon.hpp"
#include "Game/MenuState.hpp"

class Adventure;

class GameState
{
public:
	GameState() {};
	GameState(float transitionLength, SoundID soundtrack = NULL) ;
	~GameState() {};

	SoundPlaybackID m_soundtrackPlayback;
	Adventure* m_currentAdventure;
	// How many of these should be pure virutal??
	virtual void Render();		//renders game, then UI, then transitions
	virtual void Update(float ds = 0) { m_timeInState+=ds; }; 
	virtual void Transition() {};		//does any state-specific set-up
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

	SoundID m_soundtrackID;
	
};



class GameState_Attract: public GameState{
public:
	GameState_Attract();
	void Update(float ds);
	
	void RenderUI();
	void HandleInput();
};


class GameState_Encounter: public GameState{
public:
	GameState_Encounter(std::string adventureDefName, int difficulty = 0);
	~GameState_Encounter();

	void Update(float ds);
	void Transition() override;
	void RenderTransition() override;
	void RenderGame();
	void RenderUI();
	void HandleInput();

	
};


class GameState_Paused: public GameState{
public:
	GameState_Paused(GameState_Encounter* encounter);
	void Update(float ds) override;
	void RenderGame() override;
	void RenderUI() override;
	void RenderTransition() override;
	void RenderTransitionEffect(float t) override;
	void HandleInput() override;

	void SwitchToPaused();
	void SwitchToInventory();
	void SwitchToMap();

	GameState_Encounter* m_encounterGameState;
	MenuState* m_menuState;
};

class GameState_Victory: public GameState{
public:
	GameState_Victory(GameState_Encounter* encounter);
	void Update(float ds) override;
	void RenderGame() override;
	void RenderUI() override;
	void RenderTransition() override;
	void RenderTransitionEffect(float t) override;
	void HandleInput() override;

	GameState_Encounter* m_encounterGameState;
	MenuState* m_menuState;
};

class GameState_Defeat: public GameState{
public:
	GameState_Defeat(GameState* encounter);
	void Update(float ds) override;
	void RenderGame() override;
	void RenderUI() override;
	//void RenderTransition() override;
	void RenderTransitionEffect(float t) override;
	void HandleInput() override;

	GameState* m_encounterGameState;

};


class GameState_MapViewer : public GameState{
public:
	GameState_MapViewer();
	void Update(float ds) override;
	void RenderGame() override;
	void RenderUI() override;

	void HandleInput() override;

};