#include "Game.hpp"
#include "Game/TileDefinition.hpp"
#include "Game/MapDefinition.hpp"
#include "Game/ActorDefinition.hpp"
#include "Game/ProjectileDefinition.hpp"
#include "Game/PortalDefinition.hpp"
#include "Game/AdventureDefinition.hpp"
#include "Game/Adventure.hpp"
#include "Game/ItemDefinition.hpp"
#include "Game/Item.hpp"
#include "Game/VictoryCondition.hpp"
#include "Engine/Renderer/SpriteAnimSetDef.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Game/DebugRenderSystem.hpp"

#include "Game/Map.hpp"
#include "Game/Player.hpp"
#include <map>

SpriteAnimSetDef* Game::s_humanoidAnimSetDef;

Game::~Game()
{
	delete m_currentAdventure;
	//delete m_player;
	delete g_tileSpriteSheet;

	//delete all the definitions
}

Game::Game()
{

	m_renderPath = new SpriteRenderPath();
	m_renderPath->m_renderer = g_theRenderer;

	Texture* tileTexture = g_theRenderer->CreateOrGetTexture("Terrain_32x32.png");
	g_tileSpriteSheet = new SpriteSheet(*tileTexture, 32,32);
	Texture* miscTexture = g_theRenderer->CreateOrGetTexture("MiscItems_4x4.png");
	m_miscSpriteSheet = new SpriteSheet(*miscTexture, 4,4);

	m_isPaused = false;
	m_devMode = false;
	m_fullMapMode = false;
	m_gameTime = 0.f;
	

	LoadSounds();
	LoadTileDefinitions();
	LoadMapDefinitions();
	LoadEntityDefinitions();
	LoadAdventureDefinitions();

	m_screenWidth = 10;
	m_camera = new Camera();
	m_camera->SetColorTarget( g_theRenderer->m_defaultColorTarget );
	m_camera->SetDepthStencilTarget( g_theRenderer->m_defaultDepthTarget );
	//m_camera->SetProjectionOrtho(Vector2(0.f,0.f), Vector2((float) m_screenWidth, (float) m_screenWidth));
	m_camera->SetProjectionOrtho(m_screenWidth, g_gameConfigBlackboard.GetValue("windowAspect", 1.f), 0.f,100.f);
	Vector2 center = m_camera->GetBounds().GetCenter();
	m_camera->LookAt(Vector3(center.x, center.y, -1.f), Vector3(center.x, center.y, .5f));


	// Setup ui camera - draws from (0,0) to (1,1) always
	m_uiCamera = new Camera();

	// Setup what it will draw to
	m_uiCamera->SetColorTarget( g_theRenderer->m_defaultColorTarget );
	m_uiCamera->SetDepthStencilTarget( g_theRenderer->m_defaultDepthTarget );

	m_uiCamera->SetProjectionOrtho(1.f, g_gameConfigBlackboard.GetValue("windowAspect", 1.f), 0.f,100.f);
	Vector2 uicenter = m_uiCamera->GetBounds().GetCenter();
	m_uiCamera->LookAt( Vector3( uicenter.x, uicenter.y, -1.f ), Vector3(uicenter.x, uicenter.y, .5f)); 




	g_theRenderer->SetCamera( m_camera ); 

	g_theRenderer->ClearDepth( 1.0f ); 
	g_theRenderer->EnableDepth( COMPARE_LESS, true );
	m_debugRenderSystem = new DebugRenderSystem();
	m_debugRenderSystem->Startup(m_camera);
	m_debugRenderSystem->ToggleInfo();
	m_debugRenderSystem->ToggleScreenGrid();
}

Vector2 Game::GetPlayerPosition() const
{
	if (m_player != nullptr){
		return m_player->GetPosition();
	}
	return Vector2::ZERO;
}

void Game::Update(float deltaSeconds)
{
	m_gameTime+=deltaSeconds;
	if(!m_isPaused){
		m_currentState->Update(deltaSeconds);
		m_currentState->HandleInput();
	}
}

//void Game::UpdateAttract(float deltaSeconds)
//{
//	if (!m_isFinishedTransitioning && m_transitionToState != nullptr){
//		float timeInTranstion = m_gameTime - m_timeEnteredState;
//		float percThroughTransition = timeInTranstion / m_transitionLength;
//		g_theAudio->SetSoundPlaybackVolume(m_attractPlayback, 1.f - percThroughTransition);
//	}
//}

void Game::UpdatePlaying(float deltaSeconds)
{

	m_currentAdventure->Update(deltaSeconds);
}

void Game::UpdatePaused(float deltaSeconds)
{
	deltaSeconds;
	m_currentAdventure->Update(0.f);
}

void Game::UpdateInventory(float deltaSeconds)
{
	deltaSeconds;
	m_currentAdventure->Update(0.f);

	m_indexOfSelectedMenuItem = ClampInt(m_indexOfSelectedMenuItem, 0, (int) m_player->m_inventory.size()-1);
	if (g_theInput->WasKeyJustPressed(VK_RETURN) || g_theInput->GetController(0)->WasButtonJustPressed(XBOX_A)){
		m_player->EquipOrUnequipItem(m_player->m_inventory[m_indexOfSelectedMenuItem]);
	}
}

void Game::UpdateVictory(float deltaSeconds)
{
	deltaSeconds;
	m_currentAdventure->Update(0.f);
}

void Game::UpdateDefeat(float deltaSeconds)
{
	deltaSeconds;
}

void Game::UpdateMapMode(float deltaSeconds)
{
	m_currentAdventure->Update(deltaSeconds);
}

void Game::Render()
{
	g_theRenderer->SetCullMode(CULLMODE_NONE);
	g_theRenderer->DisableDepth();
	g_theRenderer->ClearScreen(RGBA::BLACK);
	//g_theRenderer->SetOrtho(m_camera->GetBounds().mins, m_camera->GetBounds().maxs);

	m_currentState->Render();

	m_debugRenderSystem->UpdateAndRender();
	/*if (m_currentState == STATE_PLAYING){
		RenderPlaying();
	}
	if (m_currentState == STATE_ATTRACT){
		RenderAttract();
	}
	if (m_currentState == STATE_PAUSED){
		RenderPaused();
	}
	if (m_currentState == STATE_INVENTORY){
		RenderInventory();
	}
	if (m_currentState == STATE_VICTORY){
		RenderVictory();
	}
	if (m_currentState == STATE_DEFEAT){
		RenderDefeat();
	}



	if (m_currentState == STATE_MAPMODE){
		RenderMapMode();
	}*/

}
//
//void Game::RenderAttract()
//{
//	/*if (m_gameTime - m_timeEnteredState < m_transitionLength){
//		FadeIn();
//	}*/
//	AABB2 screenBounds = AABB2(Vector2(0.f,0.f), Vector2((float) m_screenWidth, (float) m_screenWidth));
//	g_theRenderer->SetOrtho(Vector3(0.f,0.f, 0.f), Vector3((float) m_screenWidth, (float) m_screenWidth, 2.f));
//	g_theRenderer->DrawAABB2(AABB2(0.f,0.f, (float) m_screenWidth, (float) m_screenWidth), RGBA(64,128,0,255));
//	g_theRenderer->DrawTextInBox2D("Adventure", screenBounds, Vector2(.5f,.5f), (float)m_screenWidth * .08f);
//	g_theRenderer->DrawTextInBox2D("Press Start", screenBounds, Vector2(.5f,.3f), (float)m_screenWidth * .03f);
//
//	/*if (m_transitionToState != NO_STATE){
//		float percThroughTransition = (m_gameTime - m_timeEnteredState)/m_transitionLength;
//		g_theRenderer->DrawAABB2(AABB2(0.f,0.f, m_screenWidth,m_screenWidth), m_startFadeColor.GetColorWithAlpha((int) (percThroughTransition * 255)));
//	}*/
//	if (!m_isFinishedTransitioning){
//		if (m_transitionToState != NO_STATE){
//			float percThroughTransition = (m_gameTime - m_timeEnteredState)/m_transitionLength;
//			g_theRenderer->DrawAABB2(AABB2(0.f,0.f,(float) m_screenWidth, (float) m_screenWidth), m_startFadeColor.GetColorWithAlpha((unsigned char) (percThroughTransition * 255)));
//		} else if (m_gameTime - m_timeEnteredState < m_transitionLength){
//			FadeIn();
//		}
//	}
//
//}

//void Game::RenderPlaying()
//{
//	m_currentAdventure->Render();
//	//m_currentAdventure->m_currentMap->SetCamera()
//	m_renderPath->RenderSceneForCamera(m_camera, m_currentAdventure->GetScene());
//	//m_currentAdventure->Render();
//	if (!m_isFinishedTransitioning){
//		if (m_transitionToState != NO_STATE){
//			float percThroughTransition = (m_gameTime - m_timeEnteredState)/m_transitionLength;
//			m_currentFadeColor = Interpolate( m_startFadeColor.GetColorWithAlpha(0), m_startFadeColor, percThroughTransition);
//			g_theRenderer->DrawAABB2(m_camera->GetBounds(), m_currentFadeColor);
//		} else if (m_gameTime - m_timeEnteredState < m_transitionLength){
//			FadeIn();
//		}
//	}
//}

//void Game::RenderPaused()
//{
//	m_currentAdventure->Render();
//	g_theRenderer->DrawAABB2(m_camera->GetBounds(), m_startFadeColor);
//	
//	RGBA textColor = RGBA::WHITE;
//	if (!m_isFinishedTransitioning){
//		float percThroughTransition = (m_gameTime - m_timeEnteredState)/m_transitionLength;
//		if (m_transitionToState == NO_STATE){
//			textColor = Interpolate( textColor.GetColorWithAlpha(0), textColor, percThroughTransition);
//		} else {
//			textColor = Interpolate( textColor, textColor.GetColorWithAlpha(0), percThroughTransition);
//		}
//	}
//
//	//Vector2 center = m_currentAdventure->m_currentMap->m_camera->m_worldPos;
//	//		
//	//m_screenWidth = m_currentAdventure->m_currentMap->m_camera->m_numTilesInViewVertically;
//	AABB2 camBounds = m_camera->GetBounds();
//	Vector2 center = camBounds.GetCenter();
//	m_screenWidth = camBounds.GetHeight();		//num tiles in view vertically
//	g_theRenderer->DrawTextInBox2D("Paused", camBounds, Vector2(.5f,.9f), 1.f);
//	camBounds.AddPaddingToSides(m_screenWidth * -.1f, m_screenWidth * -.2f);
//	camBounds.Translate(0.f, -.1f * m_screenWidth);
//	RenderVictoryConditionsInBox(camBounds);
//	
//}

//void Game::RenderInventory()
//{
//	m_screenWidth = m_camera->GetBounds().GetHeight();
//	m_currentAdventure->Render();
//	g_theRenderer->DrawAABB2( m_camera->GetBounds(), m_startFadeColor);
//
//	RGBA textColor = RGBA::WHITE;
//	if (!m_isFinishedTransitioning){
//		float percThroughTransition = (m_gameTime - m_timeEnteredState)/m_transitionLength;
//		if (m_transitionToState == NO_STATE){
//			textColor = Interpolate( textColor.GetColorWithAlpha(0), textColor, percThroughTransition);
//		} else {
//			textColor = Interpolate( textColor, textColor.GetColorWithAlpha(0), percThroughTransition);
//		}
//	}
//	DrawCurrentInventory();	
//}

//void Game::RenderVictory()
//{
//	m_currentAdventure->Render();
//	g_theRenderer->DrawAABB2( m_camera->GetBounds(), m_startFadeColor);
//
//	RGBA textColor = RGBA::WHITE;
//	if (!m_isFinishedTransitioning){
//		float percThroughTransition = (m_gameTime - m_timeEnteredState)/m_transitionLength;
//		if (m_transitionToState == NO_STATE){
//			textColor = Interpolate( textColor.GetColorWithAlpha(0), textColor, percThroughTransition);
//		} else {
//			textColor = Interpolate( textColor, textColor.GetColorWithAlpha(0), percThroughTransition);
//		}
//	}
//
//	AABB2 camBounds = m_currentAdventure->m_currentMap->GetCameraBounds();
//	g_theRenderer->DrawTextInBox2D("Victory!!", camBounds, Vector2(.5f,.5f), 1.f);
//	g_theRenderer->DrawTextInBox2D("Press 'start' or 'p' to return to main menu", camBounds, Vector2(.5f,.3f), .2f, TEXT_DRAW_WORD_WRAP);
//	g_theRenderer->DrawAABB2Outline(camBounds, RGBA::WHITE);
//}

//void Game::RenderDefeat()
//{
//	m_currentAdventure->Render();
//	g_theRenderer->DrawAABB2( m_camera->GetBounds(), m_startFadeColor);
//
//	RGBA textColor = RGBA::WHITE;
//	if (!m_isFinishedTransitioning){
//		float percThroughTransition = (m_gameTime - m_timeEnteredState)/m_transitionLength;
//		if (m_transitionToState == NO_STATE){
//			textColor = Interpolate( textColor.GetColorWithAlpha(0), textColor, percThroughTransition);
//		} else {
//			textColor = Interpolate( textColor, textColor.GetColorWithAlpha(0), percThroughTransition);
//		}
//	}
//
//	AABB2 camBounds = m_currentAdventure->m_currentMap->GetCameraBounds();
//	g_theRenderer->DrawTextInBox2D("Defeat :(", camBounds, Vector2(.5f,.5f), 1.f);
//	g_theRenderer->DrawTextInBox2D("Press 'start' or 'n' to respawn", camBounds, Vector2(.5f,.3f), .5f, TEXT_DRAW_SHRINK_TO_FIT);
//	g_theRenderer->DrawAABB2Outline(camBounds, RGBA::WHITE);
//}

void Game::DrawCurrentInventory()
{
	

	AABB2 camBounds = m_currentAdventure->m_currentMap->GetCameraBounds();
	float fontSize = m_screenWidth * .02f;
	//g_theRenderer->DrawTextInBox2D("Inventory", camBounds, Vector2(.5f,.95f), 1.f);


	//------ Draw raw inventory box -------//

	AABB2 unequippedBox = AABB2(camBounds.mins + Vector2((m_screenWidth* .5f) + .5f,.5f), camBounds.maxs - Vector2(.5f,2.f));
	g_theRenderer->DrawAABB2(unequippedBox, RGBA(64,64,64,200));
	g_theRenderer->DrawAABB2Outline(unequippedBox, RGBA(255,255,255,128));
	g_theRenderer->DrawTextInBox2D("Inventory", unequippedBox, Vector2(.5f, .99f),fontSize * 1.5f);
	Vector2 textHeight = Vector2(0.f,fontSize);
	Vector2 unequippedDrawStart = Vector2(0.05f, .94f);
	
	AABB2 drawItemBox = AABB2(unequippedBox.mins.x, unequippedBox.maxs.y - textHeight.y, unequippedBox.maxs.x, unequippedBox.maxs.y);
	drawItemBox.Translate(0.f, fontSize * -2.f);
	for (unsigned int i = 0; i < m_player->m_inventory.size(); i++){
		Item* item = m_player->m_inventory[i];
		if (i == m_indexOfSelectedMenuItem){
			item->RenderInMenu(drawItemBox, true);
		} else {
			item->RenderInMenu(drawItemBox, false);
		}
		drawItemBox.Translate(textHeight * -1.f);
	}

	//------ Draw equipped box -------//

	AABB2 equippedBox = AABB2(camBounds.mins + Vector2(.5f, m_screenWidth * .35f), camBounds.maxs - Vector2((m_screenWidth* .5f) + .5f,2.f));
	g_theRenderer->DrawAABB2(equippedBox, RGBA(64,64,64,200));
	g_theRenderer->DrawAABB2Outline(equippedBox, RGBA(255,255,255,128));
	g_theRenderer->DrawTextInBox2D("Equipped", equippedBox, Vector2(.5f, .97f),fontSize * 1.5f);

	float eqWidth = equippedBox.GetWidth();
	AABB2 equippedItemBox = AABB2(equippedBox.mins.x, equippedBox.maxs.y - textHeight.y - .02f, equippedBox.mins.x + (eqWidth * .4f), equippedBox.maxs.y - .02f);
	equippedItemBox.Translate(fontSize, fontSize * -2.5f);
	float eqItemHeight = equippedItemBox.GetHeight();
	AABB2 itemIconBox = AABB2(equippedItemBox.maxs.x, equippedItemBox.mins.y, equippedItemBox.maxs.x + eqItemHeight, equippedItemBox.maxs.y);
	for ( int i = 0; i < NUM_EQUIP_SLOTS; i++){
		Item* item = m_player->m_equippedItems[i];
		std::string drawText = Actor::GetEquipSlotByID((EQUIPMENT_SLOT) i) + ": ";
		std::string itemText = "NONE";
		Texture* buttonTexture = m_miscSpriteSheet->GetTexture();
		float padding = eqItemHeight * .2f;
		itemIconBox.AddPaddingToSides(padding, padding);
		if (item != nullptr){
			//itemText= item->m_definition->m_name;
			AABB2 texCoords = m_miscSpriteSheet->GetTexCoordsForSpriteCoords(IntVector2(0,0));
			g_theRenderer->DrawTexturedAABB2(itemIconBox, *buttonTexture, texCoords.mins, texCoords.maxs , RGBA::WHITE);
			itemIconBox.AddPaddingToSides(-padding, -padding);
			item->RenderImageInBox(itemIconBox);
		} else {
			AABB2 texCoords = m_miscSpriteSheet->GetTexCoordsForSpriteCoords(IntVector2(1,0));
			g_theRenderer->DrawTexturedAABB2(itemIconBox, *buttonTexture, texCoords.mins, texCoords.maxs , RGBA::WHITE);
			itemIconBox.AddPaddingToSides(-padding, -padding);
		}
		g_theRenderer->DrawTextInBox2D(drawText, equippedItemBox, Vector2(.05f, .5f), fontSize, TEXT_DRAW_SHRINK_TO_FIT);
		//g_theRenderer->DrawTextInBox2D(itemText, equippedItemBox, )
		equippedItemBox.Translate(textHeight * -1.5f);
		itemIconBox.Translate(textHeight * -1.5f);
	}

	float eqHeight = equippedBox.GetHeight();
	equippedBox.AddPaddingToSides(0.f, eqHeight * -.2f);
	equippedBox.Translate(0.f, eqHeight * -.2f);
	m_player->RenderStatsInBox(equippedBox);


	//------ Draw selected item stats box -------//

	Item* selectedItem = m_player->m_inventory[m_indexOfSelectedMenuItem];
	AABB2 statsBox = AABB2(camBounds.mins + Vector2(.5f,  .5f), camBounds.maxs - Vector2((m_screenWidth* .5f) + .5f,2.f + (m_screenWidth * .5f)));
	g_theRenderer->DrawAABB2(statsBox, RGBA(64,64,64,200));
	g_theRenderer->DrawAABB2Outline(statsBox, RGBA(255,255,255,128));
	//g_theRenderer->DrawTextInBox2D(selectedItem->m_definition->m_name, statsBox, Vector2(.5f, .97f),fontSize);
	selectedItem->RenderStats(statsBox);
}

void Game::RenderSelectArrow(AABB2 boxToDrawIn)
{
	g_theRenderer->DrawTextInBox2D(">", boxToDrawIn, Vector2(.5f,.5f), boxToDrawIn.GetHeight() * .8f);
}

void Game::RenderMapMode()
{
	m_currentAdventure->m_currentMap->Render();
	float textHeight = m_currentAdventure->m_currentMap->GetCameraBounds().GetHeight() * .03f;
	g_theRenderer->DrawTextInBox2D(m_currentAdventure->m_currentMap->m_name, m_currentAdventure->m_currentMap->GetCameraBounds(), Vector2(.95f,.95f), textHeight, TEXT_DRAW_SHRINK_TO_FIT);
}

void Game::RenderVictoryConditionsInBox(AABB2 boxToDrawIn)
{
	//g_theRenderer->DrawAABB2Outline(boxToDrawIn, RGBA(255,0,0));
	float fontHeight = boxToDrawIn.GetHeight() * .05f;
	float lineHeight = fontHeight * 3.f;
/*	AABB2 textBox = AABB2(boxToDrawIn.mins.x, boxToDrawIn.maxs.y - lineHeight, boxToDrawIn.maxs.x, boxToDrawIn.maxs.y);
	AABB2 iconBox = AABB2(textBox.mins, Vector2(textBox.mins.x + fontHeight, textBox.mins.y + fontHeight));
	*///textBox.AddPaddingToSides(fontHeight * -2.f, 0.f);
	AABB2 lineBox = boxToDrawIn.GetPercentageBox(.1f, .4f, .9f, .5f);
	AABB2 textBox;
	AABB2 iconBox;
	lineBox.SplitAABB2Vertical(.1f, iconBox, textBox );
	fontHeight = textBox.GetHeight() * .3f;
	lineHeight = fontHeight * 3.f;
	iconBox.TrimToSquare();
	
	Texture* buttonTexture = m_miscSpriteSheet->GetTexture();
	for (VictoryCondition* objective: m_currentAdventure->m_victoryConditions){
		RGBA tint = RGBA(255,255,255);
		if (objective->CheckIfComplete()){
			tint = RGBA(200,200,200, 200);
			AABB2 texCoords = m_miscSpriteSheet->GetTexCoordsForSpriteCoords(IntVector2(1,0));
			g_theRenderer->DrawTexturedAABB2(iconBox, *buttonTexture, texCoords.mins, texCoords.maxs , tint);
		} else {
			AABB2 texCoords = m_miscSpriteSheet->GetTexCoordsForSpriteCoords(IntVector2(0,0));
			g_theRenderer->DrawTexturedAABB2(iconBox, *buttonTexture, texCoords.mins, texCoords.maxs ,  tint);
		}
		std::string victoryText = objective->GetText();
		g_theRenderer->DrawTextInBox2D(victoryText, textBox, Vector2(0.f,.5f), fontHeight, TEXT_DRAW_WORD_WRAP, tint);
		//g_theRenderer->DrawAABB2Outline(iconBox, RGBA::RED);
		//g_theRenderer->DrawAABB2Outline(textBox, RGBA::MAGENTA);
		textBox.Translate(0.f, -lineHeight);
		iconBox.Translate(0.f, -lineHeight);
	}
}

void Game::RenderXboxStartButton(AABB2 boxToDrawIn)
{
	const Texture* buttonTexture = m_miscSpriteSheet->GetTexture();
	AABB2 texCoords = m_miscSpriteSheet->GetTexCoordsForSpriteCoords(IntVector2(3,0));
	g_theRenderer->DrawTexturedAABB2(boxToDrawIn, *buttonTexture, texCoords.mins, texCoords.maxs, RGBA::WHITE );
}

void Game::RenderXboxBackButton(AABB2 boxToDrawIn)
{

	const Texture* buttonTexture = m_miscSpriteSheet->GetTexture();
	AABB2 texCoords = m_miscSpriteSheet->GetTexCoordsForSpriteCoords(IntVector2(2,0));
	g_theRenderer->DrawTexturedAABB2(boxToDrawIn, *buttonTexture, texCoords.mins, texCoords.maxs, RGBA::WHITE );

}

void Game::TogglePause()
{
	if ( !m_isPaused){
		m_isPaused = true;
	} else {
		m_isPaused = false;
	}
}

void Game::ToggleDevMode()
{
	m_devMode = !m_devMode;
}

void Game::TransitionToState(GameState* newState)
{
	/*if (newState == GAME_STATE_ATTRACT){
	m_encounterState->Enter("heightMap.png");
	}*/
	m_transitionToState = newState;
	m_currentState->StartTransition();
	//m_startTransitionTime = m_gameTime;
}

void Game::TriggerTransition()
{
	m_currentState = m_transitionToState;
	m_transitionToState = nullptr;
	m_timeEnteredState = m_gameTime;
}

AABB2 Game::SetUICamera()
{
	g_theRenderer->SetCamera( m_uiCamera ); 

	//g_theRenderer->ClearDepth( 1.0f ); 
	//g_theRenderer->EnableDepth( COMPARE_ALWAYS, false );
	g_theRenderer->DisableDepth();
	return m_uiCamera->GetBounds();
}

AABB2 Game::SetGameCamera()
{
	g_theRenderer->SetCamera( m_camera ); 

	g_theRenderer->ClearDepth( 1.0f ); 
	g_theRenderer->DisableDepth();
	return m_camera->GetBounds();		//not really gonna be necessary, probably
}

AABB2 Game::GetUIBounds()
{
	return m_uiCamera->GetBounds();
}

AABB2 Game::GetMainCameraBounds()
{
	return m_camera->GetBounds();
}

//Map * Game::CreateMap(std::string mapName, std::string mapType)
//{
//	MapDefinition* newMapDef = MapDefinition::GetMapDefinition(mapType);
//	if (newMapDef == nullptr){
//		ERROR_AND_DIE("No such map");
//	}
//	Map* newMap = new Map(mapName, newMapDef);
//	m_mapsByName.insert(std::pair<std::string, Map*> (mapName, newMap));
//	return newMap;
//}

void Game::UpdateMenuSelection(int direction)
{
	m_indexOfSelectedMenuItem+=direction;
}

//void Game::StartStateTransition(GAME_STATE newState, float transitionLength, RGBA startColor)
//{
//	
//	m_transitionToState = newState;
//	m_timeEnteredState = m_gameTime;
//	m_isFinishedTransitioning = false;
//	m_transitionLength = transitionLength;
//	m_startFadeColor = startColor;
//}

//void Game::Transition()
//{
//	if (m_currentState == STATE_ATTRACT){
//		g_theAudio->StopSound(m_attractPlayback);
//		if (m_transitionToState != STATE_ATTRACT){
//			m_currentAdventure->Begin();
//		}
//	}
//	if (m_currentState == STATE_VICTORY){
//		g_theAudio->StopSound(m_victoryPlayback);
//	}
//	if (m_transitionToState == STATE_VICTORY){
//		m_currentAdventure->m_currentMap->StopMusic();
//		m_victoryPlayback = g_theAudio->PlaySound(m_victoryMusicID);
//	}
//	if (m_transitionToState == STATE_ATTRACT){
//		if (m_currentAdventure != nullptr){
//			m_currentAdventure->m_currentMap->StopMusic();
//		}
//		m_attractPlayback = g_theAudio->PlaySound(m_attractMusicID);
//		g_theAudio->SetSoundPlaybackVolume(m_attractPlayback, 1.f);
//	}
//	m_currentState = m_transitionToState;
//	m_timeEnteredState = m_gameTime;
//	m_transitionToState = NO_STATE;
//	//m_isFinishedTransitioning = true;
//}

//void Game::FadeIn()
//{
//	AABB2 bounds =  m_camera->GetBounds();
//	float t = (m_gameTime - m_timeEnteredState)/m_transitionLength;
//	m_currentFadeColor = Interpolate(m_startFadeColor, m_startFadeColor.GetColorWithAlpha(0), t);
//	g_theRenderer->DrawAABB2(bounds, m_currentFadeColor);
//}

void Game::StartAdventure(std::string adventureDefName)
{
	AdventureDefinition* adventureDef = AdventureDefinition::GetAdventureDefinition(adventureDefName);
	m_currentAdventure = new Adventure(adventureDef);
	//m_currentAdventure->Begin();
	
}

void Game::ToggleState(bool & stateToToggle)
{
	stateToToggle = !stateToToggle;
}

void Game::LookAtNextMap(int direction)
{
	m_currentAdventure->MoveToMapIndex(m_currentAdventure->m_currentMapIndex + direction);
}

void Game::LoadSounds()
{
	std::string attractPath = g_gameConfigBlackboard.GetValue("AttractMusic", std::string());
	std::string victoryPath = g_gameConfigBlackboard.GetValue("VictoryMusic", std::string());

	m_attractMusicID = g_theAudio->CreateOrGetSound(attractPath);
	m_victoryMusicID = g_theAudio->CreateOrGetSound(victoryPath);

}

void Game::LoadTileDefinitions()
{
	tinyxml2::XMLDocument tileDefDoc;
	tileDefDoc.LoadFile("Data/Data/Tiles.xml");


	tinyxml2::XMLElement* root = tileDefDoc.FirstChildElement("TileDefinitions");
	for (tinyxml2::XMLElement* tileDefElement = root->FirstChildElement("TileDefinition"); tileDefElement != NULL; tileDefElement = tileDefElement->NextSiblingElement("TileDefinition")){
		TileDefinition* newDefinition = new TileDefinition(tileDefElement);
		TileDefinition::s_definitions.insert(std::pair<std::string, TileDefinition*>(newDefinition->m_name, newDefinition));
	}


}

void Game::LoadMapDefinitions()
{
	tinyxml2::XMLDocument mapDefDoc;
	mapDefDoc.LoadFile("Data/Data/Maps.xml");


	tinyxml2::XMLElement* root = mapDefDoc.FirstChildElement("MapDefinitions");
	for (tinyxml2::XMLElement* tileDefElement = root->FirstChildElement("MapDefinition"); tileDefElement != NULL; tileDefElement = tileDefElement->NextSiblingElement("MapDefinition")){
		//had to make make MapDefinition take in pointer because was getting inaccessible error when trying to derference
		MapDefinition* newDefinition = new MapDefinition(tileDefElement);

		MapDefinition::s_definitions.insert(std::pair<std::string, MapDefinition*>(newDefinition->m_name, newDefinition));
	}
}

void Game::LoadEntityDefinitions()
{
	tinyxml2::XMLDocument humanoidDefDoc;
	humanoidDefDoc.LoadFile("Data/Data/Humanoid.xml");
	tinyxml2::XMLElement* spriteAnimDefElement = humanoidDefDoc.FirstChildElement("SpriteAnimSet");
	s_humanoidAnimSetDef = new SpriteAnimSetDef(spriteAnimDefElement, *g_theRenderer);

	LoadItemDefinitions();
	LoadActorDefinitions();
	LoadProjectileDefinitions();
	LoadPortalDefinitions();
}

void Game::LoadActorDefinitions()
{
	tinyxml2::XMLDocument actorDefDoc;
	actorDefDoc.LoadFile("Data/Data/Actors.xml");

	tinyxml2::XMLElement* root = actorDefDoc.FirstChildElement("ActorDefinitions");
	for (tinyxml2::XMLElement* tileDefElement = root->FirstChildElement("ActorDefinition"); tileDefElement != NULL; tileDefElement = tileDefElement->NextSiblingElement("ActorDefinition")){
		//had to make make MapDefinition take in pointer because was getting inaccessible error when trying to dereference
		ActorDefinition* newDefinition = new ActorDefinition(tileDefElement);
		ActorDefinition::s_definitions.insert(std::pair<std::string, ActorDefinition*>(newDefinition->m_name, newDefinition));
	}
}

void Game::LoadProjectileDefinitions()
{
	tinyxml2::XMLDocument projectileDefDoc;
	projectileDefDoc.LoadFile("Data/Data/Projectiles.xml");

	tinyxml2::XMLElement* root = projectileDefDoc.FirstChildElement("ProjectileDefinitions");
	for (tinyxml2::XMLElement* projectileDefElement = root->FirstChildElement("ProjectileDefinition"); projectileDefElement != NULL; projectileDefElement = projectileDefElement->NextSiblingElement("ProjectileDefinition")){
		ProjectileDefinition* newDefinition = new ProjectileDefinition(projectileDefElement);
		ProjectileDefinition::s_definitions.insert(std::pair<std::string, ProjectileDefinition*>(newDefinition->m_name, newDefinition));
	}
}

void Game::LoadPortalDefinitions()
{
	tinyxml2::XMLDocument projectileDefDoc;
	projectileDefDoc.LoadFile("Data/Data/Portals.xml");

	tinyxml2::XMLElement* root = projectileDefDoc.FirstChildElement("PortalDefinitions");
	for (tinyxml2::XMLElement* projectileDefElement = root->FirstChildElement("PortalDefinition"); projectileDefElement != NULL; projectileDefElement = projectileDefElement->NextSiblingElement("PortalDefinition")){
		PortalDefinition* newDefinition = new PortalDefinition(projectileDefElement);
		PortalDefinition::s_definitions.insert(std::pair<std::string, PortalDefinition*>(newDefinition->m_name, newDefinition));
	}
}

void Game::LoadItemDefinitions()
{
	tinyxml2::XMLDocument itemDefDoc;
	itemDefDoc.LoadFile("Data/Data/Items.xml");

	tinyxml2::XMLElement* root = itemDefDoc.FirstChildElement("ItemDefinitions");
	for (tinyxml2::XMLElement* itemDefElement = root->FirstChildElement("ItemDefinition"); itemDefElement != NULL; itemDefElement = itemDefElement->NextSiblingElement("ItemDefinition")){
		ItemDefinition* newDefinition = new ItemDefinition(itemDefElement);
		ItemDefinition::s_definitions.insert(std::pair<std::string, ItemDefinition*>(newDefinition->m_name, newDefinition));
	}
}

void Game::LoadAdventureDefinitions()
{
	tinyxml2::XMLDocument projectileDefDoc;
	projectileDefDoc.LoadFile("Data/Data/Adventures.xml");

	tinyxml2::XMLElement* root = projectileDefDoc.FirstChildElement("AdventureDefinitions");
	for (tinyxml2::XMLElement* projectileDefElement = root->FirstChildElement("AdventureDefinition"); projectileDefElement != NULL; projectileDefElement = projectileDefElement->NextSiblingElement("AdventureDefinition")){
		AdventureDefinition* newDefinition = new AdventureDefinition(projectileDefElement);
		AdventureDefinition::s_definitions.insert(std::pair<std::string, AdventureDefinition*>(newDefinition->m_name, newDefinition));
	}
}





void Game::SpawnRandomTestGoblin()
{
	Tile baseTile = m_currentAdventure->m_currentMap->GetRandomBaseTile();
	m_currentAdventure->m_currentMap->SpawnNewActor("Goblin", baseTile.GetApproximateCenter(), 0.f);
}




Game* g_theGame = nullptr;

bool WasStartJustPressed()
{
	return g_theInput->WasKeyJustPressed(VK_RETURN) || g_theInput->WasKeyJustPressed('P') || g_primaryController->WasButtonJustPressed(XBOX_START) || g_primaryController->WasButtonJustPressed(XBOX_A);
}

bool WasPauseJustPressed()
{
	return g_theInput->WasKeyJustPressed('P') || g_primaryController->WasButtonJustPressed(XBOX_START);
}

bool WasBackJustPressed()
{
	return g_theInput->WasKeyJustPressed(VK_ESCAPE) || g_primaryController->WasButtonJustPressed(XBOX_B);

}

bool WasSelectJustPressed()
{
	return g_theInput->WasKeyJustPressed(VK_RETURN) || g_primaryController->WasButtonJustPressed(XBOX_A);

}
bool WasExitJustPressed()
{
	return g_theInput->WasKeyJustPressed(VK_ESCAPE);
}

bool WasUpJustPressed()
{
	return g_theInput->WasKeyJustPressed(VK_UP) || g_primaryController->WasButtonJustPressed(XBOX_D_UP);
}

bool WasDownJustPressed()
{
	return g_theInput->WasKeyJustPressed(VK_DOWN) || g_primaryController->WasButtonJustPressed(XBOX_D_DOWN);
}

bool WasRightJustPressed()
{
	return g_theInput->WasKeyJustPressed(VK_RIGHT) || g_primaryController->WasButtonJustPressed(XBOX_D_RIGHT);
}

bool WasLeftJustPressed()
{
	return g_theInput->WasKeyJustPressed(VK_LEFT) || g_primaryController->WasButtonJustPressed(XBOX_D_LEFT);
}

void CheckArrowKeys()
{
	if (WasUpJustPressed()){
		g_theGame->UpdateMenuSelection(-1);
	}
	if (WasDownJustPressed()){
		g_theGame->UpdateMenuSelection(1);
	}
}
