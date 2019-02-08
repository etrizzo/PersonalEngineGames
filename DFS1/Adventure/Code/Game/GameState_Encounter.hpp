#pragma once
#include "Game/GameState.hpp"

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