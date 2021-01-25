#include "Game/GameState_Playing.hpp"
#include "Game/Game.hpp"
#include "Game/Map.hpp"
#include "Game/Entity.hpp"
#include "Game/Player.hpp"
#include "Game/DebugRenderSystem.hpp"
#include "Engine/Renderer/SpriteRenderPath.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Game/GameOfLife.hpp"


GameState_Playing::GameState_Playing()
{
	m_renderScene = new RenderScene2D();

	//straight line
	m_currentMap = new Map(std::vector<IntVector2>{
		IntVector2(0, 0),
		IntVector2(0, 1),
		IntVector2(0, 2)
	});

	//glider
	//m_currentMap = new Map(std::vector<IntVector2>{
	//		IntVector2(3, 6),
	//		IntVector2(4, 5),
	//		IntVector2(5, 5),
	//		IntVector2(5, 6),
	//		IntVector2(5, 7)
	//});

	//r-pentomino
	//m_currentMap = new Map(std::vector<IntVector2>{
	//	IntVector2(3, 6),
	//	IntVector2(3, 7),
	//	IntVector2(2, 7),
	//	IntVector2(3, 8),
	//	IntVector2(4, 8)
	//});

}

void GameState_Playing::Update(float ds)
{
	m_timeInState+=ds;

	float deltaSeconds = ds;

	m_currentMap->Update(ds);

}

void GameState_Playing::RenderGame()
{
	float width = (float)m_currentMap->GetWidth();
	g_theGame->m_mainCamera->SetProjectionOrtho(width, g_gameConfigBlackboard.GetValue("windowAspect", 1.f), 0.f, 100.f);
	g_theGame->m_mainCamera->SetPosition(Vector3(width * -.5f, 0.f, -1.f));
	g_theGame->SetMainCamera();

	m_currentMap->Render();

	//g_theGame->m_debugRenderSystem->UpdateAndRender();
	//g_theRenderer->EnableDepth( COMPARE_LESS, true ); 
	
}

void GameState_Playing::RenderUI()
{
	//AABB2 bounds = g_theGame->SetUICamera();

}

void GameState_Playing::HandleInput()
{
	
	g_theGame->m_debugRenderSystem->HandleInput();

	if (g_theInput->WasKeyJustPressed(VK_F1)){
		g_theGame->ToggleDevMode();
	}
	if (g_theInput->WasKeyJustPressed('P') || g_theInput->GetController(0)->WasButtonJustPressed(XBOX_START) ){
		g_theGame->TransitionToState(new GameState_Paused(this));
	}

	if (g_theInput->WasKeyJustPressed('T')){
		g_theGame->m_gameClock->SetScale(.1f);
	}
	if (g_theInput->WasKeyJustReleased('T')){
		g_theGame->m_gameClock->SetScale(1.f);
	}

	if (g_theInput->WasKeyJustPressed('O')) {
		if (m_currentMap->IsAutoTick()) {
			m_currentMap->SetAutoTick(false);
		}
		else {
			m_currentMap->m_gameOfLife->Tick();
		}
	}

	if (g_theInput->WasKeyJustPressed('S')) {
		m_currentMap->SetAutoTick(!m_currentMap->IsAutoTick());
	}

	if (g_theInput->WasKeyJustPressed('X')) {
		m_currentMap->IncreaseAutoTickRate(0.05f);
	}

	if (g_theInput->WasKeyJustPressed('Z')) {
		m_currentMap->IncreaseAutoTickRate(-0.05f);
	}

}
