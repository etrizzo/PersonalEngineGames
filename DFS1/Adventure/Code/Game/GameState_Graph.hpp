#pragma once
#include "Game/GameState_Encounter.hpp"

class GameState_Graph: public GameState{
public:
	GameState_Graph(GameState* encounter);
	~GameState_Graph();

	void Update(float ds);
	void RenderUI();
	void HandleInput();

	GameState* m_previousState = nullptr;
	float m_transitionLength = 0.f;

private:
	void RenderGraph() const;
	void UpdateGraph();
};