#pragma once
#include "Game/GameState_Encounter.hpp"

class StoryGraph;
class Map;

class GameState_Graph: public GameState{
public:
	GameState_Graph(GameState* encounter, bool isEncounter = false);
	~GameState_Graph();

	void Update(float ds);
	void RenderUI();
	void HandleInput();

	int m_graphIndex = -1;
	StoryGraph* m_currentGraph = nullptr;

	bool m_isInEncounter = false;
	GameState* m_previousState = nullptr;
	Map* m_encounterMap = nullptr;
	std::string m_graphName = "NO_GRAPH";

	float m_transitionLength = 0.f;

private:
	void RenderGraph() const;
	void UpdateGraph();
};