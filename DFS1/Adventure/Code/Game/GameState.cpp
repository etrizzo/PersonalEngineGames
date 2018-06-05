#include "GameState.hpp"
#include "Game/Game.hpp"
#include "Game/Adventure.hpp"
#include "Engine/Core/Clock.hpp"
#include "Game/Player.hpp"

GameState::GameState(float transitionLength, SoundID soundtrack)
{
	m_transitionLength = transitionLength;
	m_soundtrackID = soundtrack;
	m_soundtrackPlayback = g_theAudio->PlaySound(m_soundtrackID);
}

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
		if (m_soundtrackID != NULL){
			g_theAudio->SetSoundPlaybackVolume(m_soundtrackPlayback, 1.f - percThroughTransition);
		}
	}

	//if transitioning into state, fade in
	if (m_timeInState < m_transitionLength){
		float percThroughTransition =  (m_timeInState) / m_transitionLength;

		RenderTransitionEffect(1 - percThroughTransition);
		if (m_soundtrackID != NULL){
			g_theAudio->SetSoundPlaybackVolume(m_soundtrackPlayback, 1.f - percThroughTransition);
		}
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

GameState_Attract::GameState_Attract()
{
	if (g_theGame != nullptr){
		m_soundtrackID = g_theGame->m_attractMusicID;
	}
}

void GameState_Attract::Update(float ds)
{
	m_timeInState+=ds;
	if (m_isTransitioning){
		float percThroughTransition =  (m_timeInState) / m_transitionLength;
		g_theAudio->SetSoundPlaybackVolume(g_theGame->m_attractPlayback, 1.f - percThroughTransition);
	}
}

void GameState_Attract::RenderUI()
{
	

	AABB2 screenBounds = g_theGame->SetUICamera();
	//g_theRenderer->SetOrtho(Vector3(0.f,0.f, 0.f), Vector3((float) m_screenWidth, (float) m_screenWidth, 2.f));
	g_theRenderer->DrawAABB2(screenBounds, RGBA(64,128,0,255));
	g_theRenderer->DrawTextInBox2D("Adventure", screenBounds, Vector2(.5f,.5f), screenBounds.GetHeight() * .08f);
	g_theRenderer->DrawTextInBox2D("Press Start", screenBounds, Vector2(.5f,.3f), screenBounds.GetHeight() * .03f);

	//g_theGame->SetMainCamera();
}

void GameState_Attract::HandleInput()
{
	if (WasStartJustPressed()){
		//g_theGame->StartStateTransition(STATE_PLAYING, 1.f);
		g_theGame->TransitionToState(new GameState_Encounter("Balrog"));
		g_theGame->m_currentAdventure->Begin();
		//g_theGame->StartAdventure("Balrog");
	}

	if (WasExitJustPressed()){
		CommandRun("quit");
	}

	/*if (g_theInput->WasKeyJustPressed('M') || g_primaryController->WasButtonJustPressed(XBOX_Y)){
		g_theGame->StartStateTransition(STATE_MAPMODE, 1.f);
		g_theGame->StartAdventure("Balrog");
		g_theGame->m_fullMapMode = true;
	}*/

	CheckArrowKeys();
}

GameState_Encounter::GameState_Encounter(std::string lvlToLoad)
{
	g_theGame->StartAdventure(lvlToLoad);
}

void GameState_Encounter::Update(float ds)
{
	m_timeInState+=ds;
	g_theGame->m_currentAdventure->Update(ds);
}

void GameState_Encounter::RenderGame()
{
	g_theRenderer->ClearScreen( RGBA::BLACK ); 
	g_theRenderer->ClearDepth( 1.0f ); 
	g_theRenderer->EnableDepth( COMPARE_LESS, true ); 



	g_theGame->m_currentAdventure->Render();
	g_theGame->m_renderPath->RenderSceneForCamera(g_theGame->m_camera, g_theGame->m_currentAdventure->GetScene());
}

void GameState_Encounter::RenderUI()
{
	g_theGame->m_currentAdventure->RenderUI();
}

void GameState_Encounter::HandleInput()
{
	if (g_theInput->WasKeyJustPressed(VK_F1)){
		g_theGame->ToggleDevMode();
	}
	if (g_theInput->WasKeyJustPressed('P') || g_theInput->GetController(0)->WasButtonJustPressed(XBOX_START) ){
		g_theGame->TransitionToState(new GameState_Paused(this));
	}

	if (WasPauseJustPressed() || g_theInput->WasKeyJustPressed(VK_ESCAPE)){
		//g_theGame->StartStateTransition(STATE_PAUSED, .15f, RGBA(0,0,0,200));
		g_theGame->TransitionToState(new GameState_Paused(this));
	}
	/*if (g_theInput->WasKeyJustPressed('I') || g_primaryController->WasButtonJustPressed(XBOX_BACK)){
		g_theGame->StartStateTransition(STATE_INVENTORY, .15f, RGBA(0,0,0,200));
	}*/

	if (g_theInput->WasKeyJustPressed('M')){
		g_theGame->ToggleState(g_theGame->m_fullMapMode);		//change later to open map screen
	}

	if (g_theInput->WasKeyJustPressed(VK_F1)){
		g_theGame->ToggleDevMode();
	}

	if (g_primaryController != nullptr){
		if ((g_primaryController->WasButtonJustPressed(XBOX_A) || g_theInput->WasKeyJustPressed(VK_SPACE))){
			g_theGame->m_player->StartFiringArrow();
		}
	}



	//if (g_theInput->WasKeyJustPressed('T')){
	//	g_theGame->m_gameClock->SetScale(.1f);
	//}
	//if (g_theInput->WasKeyJustReleased('T')){
	//	g_theGame->m_gameClock->SetScale(1.f);
	//}

	//g_theGame->m_currentAdventure->HandleInput();
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

	//if (g_theGame->IsDevMode()){		//draw cube at origin
	//	g_theRenderer->DrawCube(Vector3::ZERO,Vector3::ONE, RGBA::RED);
	//}

	m_encounterGameState->Render();
}

void GameState_Paused::RenderUI()
{
	m_encounterGameState->RenderUI();
	//g_theRenderer->ApplyEffect("timeeffect");
	//g_theRenderer->FinishEffects();
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
	/*if (g_theInput->WasKeyJustPressed('P') || g_theInput->GetController(0)->WasButtonJustPressed(XBOX_START) || WasAcceptJustPressed()){
		g_theGame->TransitionToState(m_encounterGameState);
	}
*/
	if (WasPauseJustPressed() || g_primaryController->WasButtonJustPressed(XBOX_B)){
		g_theGame->TransitionToState(m_encounterGameState);
	}
	if (g_theInput->WasKeyJustPressed('I') || g_primaryController->WasButtonJustPressed(XBOX_BACK)){
		TODO("Add inventory state");
		//g_theGame->TransitionToState(new GameState_Inventory);
	}

	CheckArrowKeys();

	if (g_theInput->WasKeyJustPressed(VK_ESCAPE)){
		g_theGame->TransitionToState(new GameState_Attract());
	}
}
