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
	AABB2 SetGraphCamera();

	int m_graphIndex = -1;
	StoryGraph* m_currentGraph = nullptr;

	bool m_isInEncounter = false;
	GameState* m_previousState = nullptr;
	Map* m_encounterMap = nullptr;
	Camera* m_graphCamera = nullptr;	//for scrolling/moving the graph around
	std::string m_graphName = "NO_GRAPH";

	float m_transitionLength = 0.f;

	float m_cameraAspect;
	float m_cameraOrtho = 1.f;
	Vector2 m_cameraPosition = Vector2::ZERO;

private:
	void RenderGraph() const;
	void UpdateGraph();

	void UpdateCamera();
	void HandleCameraInput();
};