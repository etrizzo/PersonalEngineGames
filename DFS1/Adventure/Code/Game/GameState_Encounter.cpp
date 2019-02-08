#include "Engine/Core/Clock.hpp"

#include "Game/GameState_Graph.hpp"
#include "Game/GameState_Encounter.hpp"
#include "Game/Game.hpp"
#include "Game/Adventure.hpp"
#include "Game/AdventureDefinition.hpp"
#include "Game/Player.hpp"
#include "Game/Party.hpp"


GameState_Encounter::GameState_Encounter(std::string lvlToLoad, int difficulty)
{
	//g_theGame->StartAdventure(lvlToLoad);
	AdventureDefinition* adventureDef = AdventureDefinition::GetAdventureDefinition(lvlToLoad);
	m_currentAdventure = new Adventure(adventureDef, difficulty);
	//m_currentAdventure->Begin();
}

GameState_Encounter::~GameState_Encounter()
{
	delete m_currentAdventure;
}


void GameState_Encounter::Update(float ds)
{
	m_timeInState+=ds;
	m_currentAdventure->Update(ds);
}

void GameState_Encounter::Transition()
{
	if (!m_currentAdventure->m_adventureBegun){
		m_currentAdventure->Begin();
	}
}

void GameState_Encounter::RenderTransition()
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
	}
}


void GameState_Encounter::RenderGame()
{
	g_theRenderer->ClearScreen( RGBA::BLACK ); 
	//g_theRenderer->ClearDepth( 1.0f ); 
	//g_theRenderer->EnableDepth( COMPARE_LESS, true ); 



	m_currentAdventure->Render();

	//Texture* dispTexture = g_theRenderer->CreateOrGetTexture("Noise/heat_displacements.png");
	//g_theRenderer->BindTexture(*dispTexture, 4);



	AABB2 camBounds = g_theGame->m_camera->GetBounds();
	AABB2 worldBounds = AABB2(Vector2::ZERO, m_currentAdventure->m_currentMap->m_dimensions.GetVector2());
	AABB2 worldUVs = AABB2(worldBounds.GetPercentageOfPoint(camBounds.mins), worldBounds.GetPercentageOfPoint(camBounds.maxs));
	g_theRenderer->ApplyEffect("watercolor", worldUVs);
	g_theRenderer->ReleaseTexture(4);
	g_theRenderer->FinishEffects();

	g_theGame->m_renderPath->RenderSceneForCamera(g_theGame->m_camera, m_currentAdventure->GetScene());
	m_currentAdventure->PostRender();


	//g_theRenderer->BindShaderProgram("watercolor");
	//float windowWidthPixels  = (float) g_Window->GetWidth();
	//float windowHeightPixels = (float) g_Window->GetHeight();
	//Vector2 pixelSizeUVS = Vector2( 1.f / windowWidthPixels, 1.f / windowHeightPixels);
	//g_theRenderer->BindUniform("TEXEL_SIZE_X", pixelSizeUVS.x);
	//g_theRenderer->BindUniform("TEXEL_SIZE_Y", pixelSizeUVS.y);

	//bind the displacement texture
	/*Texture* dispTexture = g_theRenderer->CreateOrGetTexture("Noise/noise1.png");
	g_theRenderer->BindTexture(*dispTexture, 4);
	g_theRenderer->ApplyEffect("watercolor");
	g_theRenderer->ReleaseTexture(4);
	g_theRenderer->FinishEffects();*/
}

void GameState_Encounter::RenderUI()
{
	g_theGame->SetUICamera();
	m_currentAdventure->RenderUI();

}

void GameState_Encounter::HandleInput()
{
	if (g_theGame->m_party != nullptr){
		g_theGame->m_party->HandleInput();
	}

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
	if (g_theInput->WasKeyJustPressed('I') || g_primaryController->WasButtonJustPressed(XBOX_BACK)){
		g_theGame->TransitionToState(new GameState_Paused(this));
	}

	if (g_theInput->WasKeyJustPressed('M')){
		g_theGame->ToggleState(g_theGame->m_fullMapMode);		//change later to open map screen
	}

	if (g_theInput->WasKeyJustPressed(VK_F7)){
		g_theGame->TransitionToState(new GameState_Graph(this));
	}

}