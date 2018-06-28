#include "GameState.hpp"
#include "Game/Game.hpp"
#include "Game/Entity.hpp"
#include "Engine/Core/Clock.hpp"
#include "Game/GameState_Playing.hpp"

void GameState::Render()
{
	RenderGame();
	RenderUI();
	RenderTransition();
}

void GameState::RenderTransition()
{
	//do a very simple fade in/out of state - temporary
	//if transitioning out of state, fade out
	if (m_isTransitioning){
		float percThroughTransition = (m_timeInState - m_startTransitionTime) / m_transitionLength;


		RenderTransitionEffect(percThroughTransition);

		if (m_timeInState - m_startTransitionTime > m_transitionLength){
			//move to the new state 
			m_isTransitioning = false;
			g_theGame->TriggerTransition();
		}
	}

	//if transitioning into state, fade in
	if (m_timeInState < m_transitionLength){
		float percThroughTransition =  (m_timeInState) / m_transitionLength;

		RenderTransitionEffect(1 - percThroughTransition);
	}
}

void GameState::RenderTransitionEffect(float t)
{
	g_theGame->SetUICamera();

	RGBA transitionColor = RGBA(0,0,0, (unsigned char) (t * 255.f));
	g_theRenderer->DrawAABB2(g_theGame->GetUIBounds(), transitionColor);

	g_theGame->SetGameCamera();
}

void GameState::EnterState()
{
}

void GameState::StartTransition()
{
	m_startTransitionTime = m_timeInState;
	m_isTransitioning = true;
}

RenderScene * GameState::GetScene()
{
	return nullptr;
}

void GameState::ResetTime()
{
	m_timeInState = 0.f;
}

GameState_Attract::GameState_Attract()
{
	if (g_theGame != nullptr){
		AABB2 box = g_theGame->GetUIBounds();
		m_mainMenu = new Menu(box.GetPercentageBox(Vector2(.3f, .4f), Vector2(.7f, .6f)));
		m_mainMenu->AddOption("Start", 0, true);
		m_mainMenu->AddOption("Quit", 1, true);
	}
	
}

void GameState_Attract::Update(float ds)
{
	m_timeInState+=ds;
	int selection = m_mainMenu->GetFrameSelection();
	if (selection != -1){
		if (selection == 0){
			g_theGame->TransitionToState(new GameState_Select());
		}
		if (selection == 1){
			CommandQuit(Command("text"));
		}
	}
}

void GameState_Attract::RenderUI()
{
	

	AABB2 UIBounds = g_theGame->SetUICamera();
	g_theRenderer->DrawAABB2(UIBounds, RGBA(0, 64, 64));
	m_mainMenu->Render();
	//g_theRenderer->DrawTextInBox2D("Tactics", UIBounds, Vector2(.5f,.5f), .1f);
	//g_theRenderer->DrawTextInBox2D("Press Start", UIBounds, Vector2(.5f,.3f), .05f);

	//float seconds = GetMasterClock()->GetCurrentSeconds();
	//g_theRenderer->DrawTextInBox2D(std::to_string(seconds), UIBounds, Vector2(0.f, .8f), .05f);
	//
	//float gameSeconds = g_theGame->m_gameClock->GetCurrentSeconds();
	//g_theRenderer->DrawTextInBox2D(std::to_string(gameSeconds), UIBounds, Vector2(0.f, .6f), .05f);

	g_theGame->SetGameCamera();
}

void GameState_Attract::HandleInput()
{
	/*if (g_theInput->WasKeyJustPressed(VK_SPACE) || g_theInput->GetController(0)->WasButtonJustPressed(XBOX_START) || AcceptJustPressed()){
 		g_theGame->TransitionToState(new GameState_Playing());
	}*/
	m_mainMenu->HandleInput();

	if (g_theInput->WasKeyJustPressed('T')){
		g_theGame->m_gameClock->SetScale(.1f);
	}
	if (g_theInput->WasKeyJustReleased('T')){
		g_theGame->m_gameClock->SetScale(1.f);
	}

	if (g_theInput->WasKeyJustPressed('Y')){
		GetMasterClock()->SetScale(.1f);
	}
	if (g_theInput->WasKeyJustReleased('Y')){
		GetMasterClock()->SetScale(1.f);
	}
}


GameState_Select::GameState_Select()
{
	if (g_theGame != nullptr){
		AABB2 box = g_theGame->GetUIBounds();
		m_selectMenu = new Menu(box.GetPercentageBox(Vector2(.3f, .4f), Vector2(.7f, .6f)));
		m_selectMenu->AddOption("Press A", 0, true);
	}
	
}

void GameState_Select::Update(float ds)
{
	m_timeInState+=ds;
	int selection = m_selectMenu->GetFrameSelection();
	if (selection != -1){
		if (selection == 0){
			g_theGame->TransitionToState(g_theGame->m_playState);
		}
	}
}

void GameState_Select::RenderUI()
{
	

	AABB2 UIBounds = g_theGame->SetUICamera();
	g_theRenderer->DrawAABB2(UIBounds, RGBA(64, 64, 0));
	m_selectMenu->Render();

	g_theGame->SetGameCamera();
}

void GameState_Select::HandleInput()
{

	m_selectMenu->HandleInput();

	if (g_theInput->WasKeyJustPressed('T')){
		g_theGame->m_gameClock->SetScale(.1f);
	}
	if (g_theInput->WasKeyJustReleased('T')){
		g_theGame->m_gameClock->SetScale(1.f);
	}

	if (g_theInput->WasKeyJustPressed('Y')){
		GetMasterClock()->SetScale(.1f);
	}
	if (g_theInput->WasKeyJustReleased('Y')){
		GetMasterClock()->SetScale(1.f);
	}
	if (BackJustPressed()){
		g_theGame->TransitionToState(new GameState_Attract());
	}
}




GameState_Paused::GameState_Paused(GameState_Playing* playState)
{
	m_encounterGameState = playState;
	m_transitionLength = .2f;
	//g_theGame->m_gameClock->SetScale(0.f);
}

void GameState_Paused::Update(float ds)
{
	m_encounterGameState->Update(0.f);
	m_timeInState+=ds;
}

void GameState_Paused::RenderGame()
{
	//GameState_Encounter::RenderGame();
	//m_encounterGameState->RenderGame();
	//g_theRenderer->ClearScreen( RGBA::BLACK ); 
	//g_theRenderer->ClearDepth( 1.0f ); 
	//g_theRenderer->EnableDepth( COMPARE_LESS, true ); 

	//if (g_theGame->IsDevMode()){		//draw cube at origin
	//	g_theRenderer->DrawCube(Vector3::ZERO,Vector3::ONE, RGBA::RED);
	//}

	m_encounterGameState->RenderGame();
}

void GameState_Paused::RenderUI()
{
	m_encounterGameState->RenderUI();
	g_theRenderer->ClearDepth(1.f);
	
	AABB2 UIBounds = g_theGame->SetUICamera(); //AABB2(-1.f,-1.f, 1.f,1.f);
	g_theRenderer->DrawAABB2(UIBounds, RGBA(0, 32, 32, 64));
	if (!m_isTransitioning){
		g_theRenderer->DrawTextInBox2D("Paused", UIBounds, Vector2(.5f,.5f), .1f);
	}
	
}

void GameState_Paused::RenderTransitionEffect( float t)
{

}

void GameState_Paused::HandleInput()
{
	if (g_theInput->WasKeyJustPressed('P') || g_theInput->GetController(0)->WasButtonJustPressed(XBOX_START) || AcceptJustPressed()){
		g_theGame->m_gameClock->SetScale(1.f);
		g_theGame->TransitionToState(m_encounterGameState);
	}
}

void GameState_Paused::EnterState()
{
	g_theGame->m_gameClock->SetScale(0.f);
}

void GameState_Loading::Render()
{
	//DON"T render transition and shit
	AABB2 x = g_theGame->SetUICamera();
	g_theRenderer->DrawAABB2(x, RGBA(100,100,100));
	g_theRenderer->DrawTextInBox2D("Loading...", x, Vector2::HALF, .2f, TEXT_DRAW_SHRINK_TO_FIT);
}

void GameState_Loading::Update(float ds)
{
	UNUSED(ds);
	if (m_firstFrame){
		//do nothing first frame
		m_firstFrame = false;
	} else {
		//postStartup
		g_theGame->PostStartup();
	}
}

void GameState_Loading::HandleInput()
{
}

GameState_Victory::GameState_Victory(GameState_Playing * playState)
{
	m_encounterGameState = playState;
}

void GameState_Victory::Update(float ds)
{
}

void GameState_Victory::RenderGame()
{
}

void GameState_Victory::RenderUI()
{
}

void GameState_Victory::HandleInput()
{
}
