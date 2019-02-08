#include "GameState_Graph.hpp"
#include "Game/Game.hpp"

GameState_Graph::GameState_Graph(GameState*  encounter)
{
	m_previousState = encounter;
}

GameState_Graph::~GameState_Graph()
{
}

void GameState_Graph::Update(float ds)
{
	m_timeInState+=ds;
	g_theGame->m_graph.UpdateNodePositions();
}

void GameState_Graph::RenderUI()
{
	AABB2 bounds = g_theGame->SetUICamera();
	RenderGraph();
}

void GameState_Graph::HandleInput()
{

	if (g_theInput->WasKeyJustPressed(VK_F1)){
		g_theGame->ToggleDevMode();
	}

	if (g_theInput->WasKeyJustPressed(VK_F7)){
		g_theGame->TransitionToState(m_previousState);
	}

	g_theGame->m_graph.HandleInput();
}


void GameState_Graph::RenderGraph() const
{
	g_theRenderer->ClearScreen(RGBA::BLACK);
	//AABB2 bounds = g_theGame->GetUIBounds();
	//std::string graphText = g_theGame->m_graph.ToString();
	//g_theRenderer->DrawTextInBox2D(graphText, bounds, Vector2(0.f, 1.f), .01f, TEXT_DRAW_SHRINK_TO_FIT);
	g_theGame->m_graph.RenderGraph();
}

void GameState_Graph::UpdateGraph()
{
	g_theGame->m_graph.UpdateNodePositions();
}