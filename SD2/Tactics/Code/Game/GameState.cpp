#include "GameState.hpp"
#include "Game/Game.hpp"
#include "Game/EncounterState.hpp"
#include "Engine/Core/Clock.hpp"

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

	g_theGame->SetMainCamera();
}

void GameState::StartTransition()
{
	m_startTransitionTime = m_timeInState;
	m_isTransitioning = true;
}

void GameState::ResetTime()
{
	m_timeInState = 0.f;
}

void GameState_Attract::RenderUI()
{
	

	AABB2 UIBounds = g_theGame->SetUICamera();
	g_theRenderer->DrawAABB2(UIBounds, RGBA(0, 64, 64));
	g_theRenderer->DrawTextInBox2D("Tactics", UIBounds, Vector2(.5f,.5f), .1f);
	g_theRenderer->DrawTextInBox2D("Press Start", UIBounds, Vector2(.5f,.3f), .05f);

	//float seconds = GetMasterClock()->GetCurrentSeconds();
	//g_theRenderer->DrawTextInBox2D(std::to_string(seconds), UIBounds, Vector2(0.f, .8f), .05f);
	//
	//float gameSeconds = g_theGame->m_gameClock->GetCurrentSeconds();
	//g_theRenderer->DrawTextInBox2D(std::to_string(gameSeconds), UIBounds, Vector2(0.f, .6f), .05f);

	g_theGame->SetMainCamera();
}

void GameState_Attract::HandleInput()
{
	if (g_theInput->WasKeyJustPressed(VK_SPACE) || g_theInput->GetController(0)->WasButtonJustPressed(XBOX_START) || AcceptJustPressed()){
 		g_theGame->TransitionToState(new GameState_Encounter());
	}

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

GameState_Encounter::GameState_Encounter(std::string lvlToLoad)
{
	g_theGame->m_encounterState->Enter(lvlToLoad);
}

void GameState_Encounter::Update(float ds)
{
	m_timeInState+=ds;
	g_theGame->m_encounterState->Update(ds);
}

void GameState_Encounter::RenderGame()
{
	g_theRenderer->ClearScreen( RGBA::BLACK ); 
	g_theRenderer->ClearDepth( 1.0f ); 
	g_theRenderer->EnableDepth( COMPARE_LESS, true ); 

	if (g_theGame->IsDevMode()){		//draw cube at origin
		g_theRenderer->DrawCube(Vector3::ZERO,Vector3::ONE, RGBA::RED);
	}

	g_theGame->m_encounterState->Render();
}

void GameState_Encounter::RenderUI()
{
	g_theGame->m_encounterState->RenderUI();
}

void GameState_Encounter::HandleInput()
{
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

	g_theGame->m_encounterState->HandleInput();
}

GameState_Paused::GameState_Paused(GameState_Encounter* encounter)
{
	m_encounterGameState = encounter;
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
	g_theRenderer->ClearScreen( RGBA::BLACK ); 
	g_theRenderer->ClearDepth( 1.0f ); 
	g_theRenderer->EnableDepth( COMPARE_LESS, true ); 

	if (g_theGame->IsDevMode()){		//draw cube at origin
		g_theRenderer->DrawCube(Vector3::ZERO,Vector3::ONE, RGBA::RED);
	}

	g_theGame->m_encounterState->Render();
}

void GameState_Paused::RenderUI()
{
	m_encounterGameState->RenderUI();
	g_theRenderer->ApplyEffect("timeeffect");
	g_theRenderer->FinishEffects();
	g_theRenderer->ClearDepth(1.f);
	
	AABB2 UIBounds = g_theGame->SetUICamera(); //AABB2(-1.f,-1.f, 1.f,1.f);
	//g_theRenderer->DrawAABB2(UIBounds, RGBA(0, 32, 32, 64));
	if (!m_isTransitioning){
		g_theRenderer->DrawTextInBox2D("Paused", UIBounds, Vector2(.5f,.5f), .1f);
	}
	
}

void GameState_Paused::RenderTransitionEffect()
{

}

void GameState_Paused::HandleInput()
{
	if (g_theInput->WasKeyJustPressed('P') || g_theInput->GetController(0)->WasButtonJustPressed(XBOX_START) || AcceptJustPressed()){
		g_theGame->TransitionToState(m_encounterGameState);
	}
}
