#include "Game/GameState_Playing.hpp"
#include "Game/Game.hpp"
#include "Game/Player.hpp"
#include "Game/DebugRenderSystem.hpp"
#include "Engine/Renderer/ForwardRenderPath.hpp"
#include "Engine/Renderer/PerspectiveCamera.hpp"


GameState_Playing::GameState_Playing()
{
	m_renderPath = new ForwardRenderPath(g_theRenderer);
	//m_renderPath->m_renderer = g_theRenderer;
	m_scene = new RenderScene(g_theRenderer);

}

void GameState_Playing::EnterState()
{
	m_gameWon = false;
	if (!m_playing){
		//initialize map
		m_playing = true;
		m_gameLost = false;

	}
}

void GameState_Playing::Update(float ds)
{
	PROFILE_PUSH_FUNCTION_SCOPE();
	m_timeInState+=ds;
	//UpdateGraph();
	
}

void GameState_Playing::RenderUI()
{
	AABB2 bounds = g_theGame->SetUICamera();
	RenderGraph();
	
}

void GameState_Playing::HandleInput()
{
	
	g_theGame->m_debugRenderSystem->HandleInput();

	if (g_theInput->WasKeyJustPressed(VK_F1)){
		g_theGame->ToggleDevMode();
	}

	if (g_theInput->WasKeyJustPressed('T')){
		g_theGame->m_gameClock->SetScale(.1f);
	}
	if (g_theInput->WasKeyJustReleased('T')){
		g_theGame->m_gameClock->SetScale(1.f);
	}


}

void GameState_Playing::RenderGraph() const
{
	//AABB2 bounds = g_theGame->GetUIBounds();
	//std::string graphText = g_theGame->m_graph.ToString();
	//g_theRenderer->DrawTextInBox2D(graphText, bounds, Vector2(0.f, 1.f), .01f, TEXT_DRAW_SHRINK_TO_FIT);
	g_theGame->m_graph.RenderGraph();
}

void GameState_Playing::UpdateGraph()
{
	g_theGame->m_graph.UpdateNodePositions();
}



