#include "GameState_Graph.hpp"
#include "Game/Game.hpp"
#include "Game/Adventure.hpp"
#include "Game/Map.hpp"
#include "Game/Village.hpp"

GameState_Graph::GameState_Graph(GameState*  encounter, bool isEncounter)
{
	m_previousState = encounter;
	m_currentGraph = g_theGame->m_graph;
	m_graphName = "Game Graph";
	m_isInEncounter = isEncounter;
	if (isEncounter)
	{
		m_encounterMap = ((GameState_Encounter*) encounter)->m_currentAdventure->m_currentMap;
	}
}

GameState_Graph::~GameState_Graph()
{
}

void GameState_Graph::Update(float ds)
{
	m_timeInState+=ds;
	UpdateGraph();
}

void GameState_Graph::RenderUI()
{
	AABB2 bounds = g_theGame->SetUICamera();
	RenderGraph();
	g_theRenderer->DrawTextInBox2D(m_graphName, bounds, Vector2::ALIGN_CENTER_TOP, bounds.GetHeight() * .03f);
}

void GameState_Graph::HandleInput()
{

	if (g_theInput->WasKeyJustPressed(VK_F1)){
		g_theGame->ToggleDevMode();
	}

	if (g_theInput->WasKeyJustPressed(VK_F7)){
		g_theGame->TransitionToState(m_previousState);
	}
	
	if (m_isInEncounter)
	{
		if (g_theInput->WasKeyJustPressed('M')){
			m_graphIndex++;
			if (m_graphIndex < m_encounterMap->m_allVillages.size()){
				//go to the next village graph
				m_currentGraph = m_encounterMap->m_allVillages[m_graphIndex]->m_villageGraph;
				m_graphName = "Village: " + m_encounterMap->m_allVillages[m_graphIndex]->m_villageName;
			} else {
				//wrap back to the game graph
				m_graphIndex = -1;
				m_currentGraph = g_theGame->m_graph;
				m_graphName = "Game Graph";
			}
		}

		if (g_theInput->WasKeyJustPressed('N')){
			m_graphIndex--;
			if (m_graphIndex >= 0){
				//go to the next village graph
				m_currentGraph = m_encounterMap->m_allVillages[m_graphIndex]->m_villageGraph;
				m_graphName = "Village: " + m_encounterMap->m_allVillages[m_graphIndex]->m_villageName;
			} else if (m_graphIndex == -1){
				//set to the game graph
				m_graphIndex = -1;
				m_currentGraph = g_theGame->m_graph;
				m_graphName = "Game Graph";
			} else {
				//wrap back to the village graphs
				m_graphIndex = m_encounterMap->m_allVillages.size() - 1;
				m_currentGraph = m_encounterMap->m_allVillages[m_graphIndex]->m_villageGraph;
				m_graphName = "Village: " + m_encounterMap->m_allVillages[m_graphIndex]->m_villageName;
			}
		}
	}
	m_currentGraph->HandleInput();
}


void GameState_Graph::RenderGraph() const
{
	g_theRenderer->ClearScreen(RGBA::BLACK);
	//AABB2 bounds = g_theGame->GetUIBounds();
	//std::string graphText = m_currentGraph.ToString();
	//g_theRenderer->DrawTextInBox2D(graphText, bounds, Vector2(0.f, 1.f), .01f, TEXT_DRAW_SHRINK_TO_FIT);
	m_currentGraph->RenderGraph();
}

void GameState_Graph::UpdateGraph()
{
	m_currentGraph->UpdateNodePositions();
}