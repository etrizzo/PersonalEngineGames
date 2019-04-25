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
	m_currentAdventure = encounter->m_currentAdventure;

	m_cameraAspect = g_gameConfigBlackboard.GetValue("windowAspect", 1.f);

	//set up the graph camera
	m_graphCamera = new Camera();

	// Setup what it will draw to
	m_graphCamera->SetColorTarget(g_theRenderer->m_defaultColorTarget);
	m_graphCamera->SetDepthStencilTarget(g_theRenderer->m_defaultDepthTarget);
	m_graphCamera->SetProjectionOrtho(m_cameraOrtho, m_cameraAspect, 0.f, 100.f);
}

GameState_Graph::~GameState_Graph()
{
}

void GameState_Graph::Update(float ds)
{
	m_timeInState+=ds;
	UpdateGraph();
	UpdateCamera();
}

void GameState_Graph::RenderUI()
{
	AABB2 bounds = SetGraphCamera();
	RenderGraph(bounds);
	g_theRenderer->DrawTextInBox2D(m_graphName, bounds, Vector2::ALIGN_CENTER_TOP, bounds.GetHeight() * .015f);
	//g_theRenderer->DrawTextInBox2D(bounds.ToString(), bounds, Vector2::ONE, bounds.GetHeight() * .02f);
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
			if (m_graphIndex < (int) m_encounterMap->m_allVillages.size()){
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

	m_currentGraph->HandleInput(m_graphCamera->GetBounds());

	HandleCameraInput();
}

AABB2 GameState_Graph::SetGraphCamera()
{
	g_theRenderer->SetCamera(m_graphCamera);

	g_theRenderer->DisableDepth();
	return m_graphCamera->GetBounds();
}


void GameState_Graph::RenderGraph(const AABB2& bounds) const
{
	g_theRenderer->ClearScreen(RGBA::BLACK);
	//AABB2 bounds = g_theGame->GetUIBounds();
	//std::string graphText = m_currentGraph.ToString();
	//g_theRenderer->DrawTextInBox2D(graphText, bounds, Vector2(0.f, 1.f), .01f, TEXT_DRAW_SHRINK_TO_FIT);
	m_currentGraph->RenderGraph(bounds);

	if (m_encounterMap != nullptr){
		for (Village* village : m_encounterMap->m_allVillages)
		{
			if (village->m_villageGraph == m_currentGraph)
			{
				m_currentGraph->RenderEdge(village->m_currentEdge, RGBA::MAGENTA);
			}
		}
	}
}

void GameState_Graph::UpdateGraph()
{
	m_currentGraph->UpdateNodePositions();
}

void GameState_Graph::UpdateCamera()
{
	m_graphCamera->SetProjectionOrtho(m_cameraOrtho, m_cameraAspect, 0.f, 100.f, m_cameraPosition);
	//m_graphCamera->SetPosition(m_cameraPosition);
}

void GameState_Graph::HandleCameraInput()
{
	float ds = g_theGame->GetDeltaSeconds();
	float speed = m_cameraOrtho;
	Vector2 translation = Vector2::ZERO;
	Vector2 right = Vector2(1.f, 0.f);
	Vector2 up = Vector2(0.f, 1.f);
	if (IsRightKeyDown())
	{
		translation += right * speed * ds;
	}
	if (IsLeftKeyDown())
	{
		translation += right * -speed * ds;
	}

	if (IsUpKeyDown())
	{
		translation += up * speed * ds;
	}
	if (IsDownKeyDown())
	{
		translation += up * -speed * ds;
	}
	m_cameraPosition += translation;

	if (g_theInput->WasKeyJustPressed('0'))
	{
		m_cameraOrtho -= .1f;
	}
	if (g_theInput->WasKeyJustPressed('9'))
	{
		m_cameraOrtho += .1f;
	}

	m_cameraOrtho = ClampFloat(m_cameraOrtho, .5f, 10.f);
}
