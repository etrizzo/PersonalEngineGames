#include "Engine/Core/Clock.hpp"

#include "GameState.hpp"
#include "Game/Game.hpp"
#include "Game/Adventure.hpp"
#include "Game/AdventureDefinition.hpp"
#include "Game/Player.hpp"
#include "Game/Party.hpp"
#include "Game/GameState_Encounter.hpp"
#include "Game/GameState_Graph.hpp"

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
			g_theAudio->StopSound(m_soundtrackPlayback);
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

	g_theGame->SetGameCamera();
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
		//m_soundtrackPlayback = g_theAudio->PlaySound(m_soundtrackID);
	}
}

void GameState_Attract::Update(float ds)
{
	m_timeInState+=ds;
	if (m_isTransitioning){
		//float percThroughTransition =  (m_timeInState) / m_transitionLength;
		//g_theAudio->SetSoundPlaybackVolume(g_theGame->m_attractPlayback, 1.f - percThroughTransition);
	}
}



void GameState_Attract::RenderUI()
{
	

	AABB2 screenBounds = g_theGame->SetUICamera();
	//g_theRenderer->SetOrtho(Vector3(0.f,0.f, 0.f), Vector3((float) m_screenWidth, (float) m_screenWidth, 2.f));
	g_theRenderer->DrawAABB2(screenBounds, RGBA(64,128,0,255));
	g_theRenderer->DrawTextInBox2D("Procedural Narrative Structures", screenBounds, Vector2(.5f,.5f), screenBounds.GetHeight() * .04f, TEXT_DRAW_SHRINK_TO_FIT);
	g_theRenderer->DrawTextInBox2D("Press Enter", screenBounds, Vector2(.5f,.3f), screenBounds.GetHeight() * .025f);

	//Texture* dispTexture = g_theRenderer->CreateOrGetTexture("Noise/heat_displacements.png");
	//g_theRenderer->BindTexture(*dispTexture, 4);
	//g_theRenderer->ApplyEffect("watercolor");
	//g_theRenderer->ReleaseTexture(4);
	//g_theRenderer->FinishEffects();
	//g_theGame->SetMainCamera();
}

void GameState_Attract::HandleInput()
{
	if (!g_theGame->m_thesisMode){
		if (g_theInput->WasKeyJustPressed(VK_F7)){
			//g_theGame->StartStateTransition(STATE_PLAYING, 1.f);
			g_theGame->TransitionToState(new GameState_Encounter("Balrog"));
			
			//g_theGame->StartAdventure("Balrog");
		}
	}
	if (WasStartJustPressed()){
		g_theGame->TransitionToState(new GameState_Graph(this));
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





GameState_Paused::GameState_Paused(GameState_Encounter* encounter)
{
	m_encounterGameState = encounter;
	m_menuState = new MenuState_Paused(this, g_theGame->SetUICamera());
	m_transitionLength = .1f;
}

void GameState_Paused::Update(float ds)
{
	m_encounterGameState->Update(0.f);
	m_menuState->Update(ds);
	m_timeInState+=ds;
}

void GameState_Paused::RenderGame()
{

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
		m_menuState->RenderBackground();
		m_menuState->RenderContent();
		//g_theRenderer->DrawTextInBox2D("Paused", UIBounds, Vector2(.5f,.5f), .1f);
	}
	
}

void GameState_Paused::RenderTransition()
{
	//do a very simple fade in/out of state - temporary
	//if transitioning out of state, fade out
	if (m_isTransitioning){
		float percThroughTransition = (m_timeInState - m_startTransitionTime) / m_transitionLength;

		if (m_timeInState - m_startTransitionTime > m_transitionLength){
			//move to the new state 
			m_isTransitioning = false;
			g_theGame->TriggerTransition();
		} else {
			RenderTransitionEffect(percThroughTransition);
		}
		
		/*if (m_soundtrackID != NULL){
			g_theAudio->SetSoundPlaybackVolume(m_soundtrackPlayback, 1.f - percThroughTransition);
		}*/
	}

	////if transitioning into state, fade in
	//if (m_timeInState < m_transitionLength){
	//	float percThroughTransition =  (m_timeInState) / m_transitionLength;

	//	RenderTransitionEffect(1 - percThroughTransition);
	//	if (m_soundtrackID != NULL){
	//		g_theAudio->SetSoundPlaybackVolume(m_soundtrackPlayback, 1.f - percThroughTransition);
	//	}
	//}
}

void GameState_Paused::RenderTransitionEffect(float t)
{
	g_theGame->SetUICamera();


	RGBA transitionColor = Interpolate(RGBA(0,0,0,0), RGBA(0,32,32,64), t);
	g_theRenderer->DrawAABB2(g_theGame->GetUIBounds(), transitionColor);

	g_theGame->SetGameCamera();
}

void GameState_Paused::HandleInput()
{
	/*if (g_theInput->WasKeyJustPressed('P') || g_theInput->GetController(0)->WasButtonJustPressed(XBOX_START) || WasAcceptJustPressed()){
		g_theGame->TransitionToState(m_encounterGameState);
	}
*/
	//if (WasPauseJustPressed() || g_primaryController->WasButtonJustPressed(XBOX_B)){
	//	g_theGame->TransitionToState(m_encounterGameState);
	//}
	//if (g_theInput->WasKeyJustPressed('I') || g_primaryController->WasButtonJustPressed(XBOX_BACK)){
	//	TODO("Add inventory state");
	//	//g_theGame->TransitionToState(new GameState_Inventory);
	//}

	//CheckArrowKeys();
	if (g_theInput->WasKeyJustPressed(VK_F7)){
		g_theGame->TransitionToState(new GameState_Graph(this));
	}
	
	m_menuState->HandleInput();
}

void GameState_Paused::SwitchToPaused()
{
	m_menuState = new MenuState_Paused(this, g_theGame->GetUIBounds());
}

void GameState_Paused::SwitchToInventory()
{
	m_menuState = new MenuState_Inventory(this, g_theGame->GetUIBounds());
}

void GameState_Paused::SwitchToMap()
{
	m_menuState = new MenuState_Map(this, g_theGame->GetUIBounds());
}

GameState_Victory::GameState_Victory(GameState_Encounter * encounter)
{
	g_theAudio->StopSound(encounter->m_soundtrackPlayback);
	m_encounterGameState = encounter;
	if (g_theGame != nullptr){
		m_soundtrackID = g_theGame->m_victoryMusicID;
		m_soundtrackPlayback = g_theAudio->PlaySound(m_soundtrackID);
	}
}

void GameState_Victory::Update(float ds)
{
	if (m_encounterGameState != nullptr){
		m_encounterGameState->Update(0.f);
	}
	m_timeInState+=ds;
}

void GameState_Victory::RenderGame()
{
	if (m_encounterGameState != nullptr){
		m_encounterGameState->Render();
	}
}

void GameState_Victory::RenderUI()
{
	if (m_encounterGameState != nullptr){
		m_encounterGameState->RenderUI();
	}
	//g_theRenderer->ApplyEffect("timeeffect");
	//g_theRenderer->FinishEffects();
	g_theRenderer->ClearDepth(1.f);

	AABB2 UIBounds = g_theGame->SetUICamera(); //AABB2(-1.f,-1.f, 1.f,1.f);
											   //g_theRenderer->DrawAABB2(UIBounds, RGBA(0, 32, 32, 64));

	if (!m_isTransitioning){
		g_theRenderer->DrawAABB2(UIBounds, RGBA(0,32,32,200));
		g_theRenderer->DrawTextInBox2D("Victory!!", UIBounds, Vector2(.5f,.5f), UIBounds.GetHeight() * .08f);
		g_theRenderer->DrawTextInBox2D("Press Start", UIBounds, Vector2(.5f,.3f), UIBounds.GetHeight() * .03f);

		//g_theRenderer->DrawTextInBox2D("Paused", UIBounds, Vector2(.5f,.5f), .1f);
	}
}

void GameState_Victory::RenderTransition()
{
	if (m_isTransitioning){
		float percThroughTransition = (m_timeInState - m_startTransitionTime) / m_transitionLength;

		if (m_timeInState - m_startTransitionTime > m_transitionLength){
			//move to the new state 
			m_isTransitioning = false;
			g_theGame->TriggerTransition();
		} else {
			RenderTransitionEffect(percThroughTransition);
		}

		/*if (m_soundtrackID != NULL){
		g_theAudio->SetSoundPlaybackVolume(m_soundtrackPlayback, 1.f - percThroughTransition);
		}*/
	}
}

//void GameState_Victory::RenderTransition()
//{
//
//}

void GameState_Victory::RenderTransitionEffect(float t)
{
	g_theGame->SetUICamera();


	RGBA transitionColor = Interpolate(RGBA(0,0,0,0), RGBA(0,32,32,200), t);
	g_theRenderer->DrawAABB2(g_theGame->GetUIBounds(), transitionColor);

	g_theGame->SetGameCamera();
}

void GameState_Victory::HandleInput()
{
		if (WasStartJustPressed()){
			int difficulty = m_encounterGameState->m_currentAdventure->m_difficulty;
			delete m_encounterGameState;
			m_encounterGameState = nullptr;
			g_theGame->TransitionToState(new GameState_Encounter("Balrog", difficulty + 1));
		}


}


GameState_Defeat::GameState_Defeat(GameState* encounter)
{
	m_encounterGameState = encounter;
	//if (g_theGame != nullptr){
	//	m_soundtrackID = g_theGame->m_victoryMusicID;
	//	m_soundtrackPlayback = g_theAudio->PlaySound(m_soundtrackID);
	//}
}

void GameState_Defeat::Update(float ds)
{
	m_timeInState+=ds;
}

void GameState_Defeat::RenderGame()
{
	if (m_encounterGameState != nullptr){
		m_encounterGameState->Render();
	}
}

void GameState_Defeat::RenderUI()
{
	//if (m_encounterGameState != nullptr){
	//	m_encounterGameState->RenderUI();
	//}
	//g_theRenderer->ApplyEffect("timeeffect");
	//g_theRenderer->FinishEffects();
	g_theRenderer->ClearDepth(1.f);

	AABB2 UIBounds = g_theGame->SetUICamera(); //AABB2(-1.f,-1.f, 1.f,1.f);
											   //g_theRenderer->DrawAABB2(UIBounds, RGBA(0, 32, 32, 64));

	if (!m_isTransitioning){
		g_theRenderer->DrawAABB2(UIBounds, RGBA(0,32,32,200));
		g_theRenderer->DrawTextInBox2D("You Lose", UIBounds, Vector2(.5f,.5f), UIBounds.GetHeight() * .08f);
		g_theRenderer->DrawTextInBox2D("Press Start", UIBounds, Vector2(.5f,.3f), UIBounds.GetHeight() * .03f);

		//g_theRenderer->DrawTextInBox2D("Paused", UIBounds, Vector2(.5f,.5f), .1f);
	}
}

//void GameState_Victory::RenderTransition()
//{
//
//}

void GameState_Defeat::RenderTransitionEffect(float t)
{
	g_theGame->SetUICamera();


	RGBA transitionColor = Interpolate(RGBA(0,0,0,0), RGBA(0,32,32,200), t);
	g_theRenderer->DrawAABB2(g_theGame->GetUIBounds(), transitionColor);

	g_theGame->SetGameCamera();
}

void GameState_Defeat::HandleInput()
{
	if (WasStartJustPressed()){
		delete m_encounterGameState;
		m_encounterGameState = nullptr;
		g_theGame->TransitionToState(new GameState_Attract());
	}
}
