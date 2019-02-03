#include "MenuState.hpp"
#include "Game/GameState.hpp"
#include "Game/Game.hpp"
#include "Game/Player.hpp"
#include "Game/Item.hpp"
#include "Game/ItemDefinition.hpp"
#include "Game/Menu.hpp"
#include "Game/VictoryCondition.hpp"
#include "Game/Adventure.hpp"
#include "Game/Quest.hpp"
#include "Game/Party.hpp"

MenuState::MenuState()
{
	m_bounds = AABB2::ZERO_TO_ONE;
}

MenuState::MenuState(GameState_Paused* pauseState, AABB2 bounds)
{
	m_bounds = bounds;
	m_pauseState = pauseState;
}

void MenuState::Update(float ds)
{
	UNUSED(ds);
}

void MenuState::HandleInput()
{
}

MenuState_Paused::MenuState_Paused(GameState_Paused* pauseState,AABB2 bounds)
	:MenuState(pauseState, bounds)
{
}

void MenuState_Paused::RenderBackground()
{
	g_theRenderer->DrawAABB2(m_bounds, RGBA(0, 32, 32, 64));
}

void MenuState_Paused::RenderContent()
{
	g_theRenderer->DrawTextInBox2D("Paused", m_bounds, Vector2(.5f,.7f), .1f);
	
	//g_theRenderer->DrawAABB2Outline(boxToDrawIn, RGBA(255,0,0));
	float fontHeight = m_bounds.GetHeight() * .05f;
	float lineHeight = fontHeight * 3.f;
	/*	AABB2 textBox = AABB2(boxToDrawIn.mins.x, boxToDrawIn.maxs.y - lineHeight, boxToDrawIn.maxs.x, boxToDrawIn.maxs.y);
	AABB2 iconBox = AABB2(textBox.mins, Vector2(textBox.mins.x + fontHeight, textBox.mins.y + fontHeight));
	*///textBox.AddPaddingToSides(fontHeight * -2.f, 0.f);
	
	int numLines = 0;
	for (Quest* quest: m_pauseState->m_encounterGameState->m_currentAdventure->m_quests){
		numLines++;
		for (VictoryCondition* condition : quest->m_conditions){
			if (condition->m_active || condition->m_complete){
				numLines++;
			}
		}
	}
	float heightRatio = 1.f / (float) numLines;
	AABB2 lineBox = m_bounds.GetPercentageBox(.1f, .5f, .9f, .5f + (heightRatio * .4f));		//get the height of the textbox (should be enough to cover half the screen with all lines)
	AABB2 textBox;
	AABB2 iconBox;
	lineBox.SplitAABB2Vertical(.1f, iconBox, textBox );

	//g_theRenderer->DrawAABB2Outline(textBox, RGBA(255,0,0));
	
	fontHeight = textBox.GetHeight() * .3f;
	lineHeight = fontHeight * 3.f;
	iconBox.TrimToSquare();
	iconBox.AddPaddingToSides(-fontHeight * .5f,-fontHeight * .5f);
	float indent = lineHeight;
	RGBA defaultQuestColor = RGBA::WHITE;
	RGBA mainQuestColor = RGBA::YELLOW;

	Texture* buttonTexture = g_theGame->m_miscSpriteSheet->GetTexture();
	for (Quest* quest: m_pauseState->m_encounterGameState->m_currentAdventure->m_quests){
		RGBA tint = RGBA::WHITE;
		if (quest->IsMainQuest()){
			tint = mainQuestColor;
		}
		//draw quest title
		if (quest->IsComplete()){
			tint = tint.GetColorWithAlpha(164);
			AABB2 texCoords = g_theGame->m_miscSpriteSheet->GetTexCoordsForSpriteCoords(IntVector2(1,0));
			g_theRenderer->DrawTexturedAABB2(iconBox, *buttonTexture, texCoords.mins, texCoords.maxs , tint);
		} else {
			AABB2 texCoords = g_theGame->m_miscSpriteSheet->GetTexCoordsForSpriteCoords(IntVector2(0,0));
			g_theRenderer->DrawTexturedAABB2(iconBox, *buttonTexture, texCoords.mins, texCoords.maxs ,  tint);
		}
		std::string victoryText = quest->GetText();
		g_theRenderer->DrawTextInBox2D(victoryText, textBox, Vector2(0.f,.5f), fontHeight, TEXT_DRAW_WORD_WRAP, tint);
		//g_theRenderer->DrawAABB2Outline(iconBox, RGBA::RED);
		//g_theRenderer->DrawAABB2Outline(textBox, RGBA::MAGENTA);
		textBox.Translate(indent, -lineHeight);
		iconBox.Translate(indent, -lineHeight);

		for (VictoryCondition* condition : quest->m_conditions){
			if (condition->m_active || condition->m_complete){
				RGBA tint = RGBA::WHITE;
				if (quest->IsMainQuest()){
					tint = mainQuestColor;
				}
				if (condition->IsComplete()){
					tint = tint.GetColorWithAlpha(164);
					AABB2 texCoords = g_theGame->m_miscSpriteSheet->GetTexCoordsForSpriteCoords(IntVector2(1,0));
					g_theRenderer->DrawTexturedAABB2(iconBox, *buttonTexture, texCoords.mins, texCoords.maxs , tint);
				} else {
					AABB2 texCoords = g_theGame->m_miscSpriteSheet->GetTexCoordsForSpriteCoords(IntVector2(0,0));
					g_theRenderer->DrawTexturedAABB2(iconBox, *buttonTexture, texCoords.mins, texCoords.maxs ,  tint);
				}
				std::string victoryText = condition->GetText();
				g_theRenderer->DrawTextInBox2D(victoryText, textBox, Vector2(0.f,.5f), fontHeight, TEXT_DRAW_WORD_WRAP, tint);
				//g_theRenderer->DrawAABB2Outline(iconBox, RGBA::RED);
				//g_theRenderer->DrawAABB2Outline(textBox, RGBA::MAGENTA);
				textBox.Translate(0.f, -lineHeight);
				iconBox.Translate(0.f, -lineHeight);
			}
		}
		textBox.Translate(-indent, 0.f);
		iconBox.Translate(-indent, 0.f);
	}

}

void MenuState_Paused::HandleInput()
{
	if (WasPauseJustPressed() || g_primaryController->WasButtonJustPressed(XBOX_B)){
		g_theGame->TransitionToState(m_pauseState->m_encounterGameState);
	}
	//if (g_theInput->WasKeyJustPressed(VK_ESCAPE)){
	//	g_theGame->TransitionToState(new GameState_Attract());
	//}

	if (g_theInput->WasKeyJustPressed('I') || g_primaryController->WasButtonJustPressed(XBOX_BACK)){
		m_pauseState->SwitchToInventory();
	}
	if (g_theInput->WasKeyJustPressed('M')){
		m_pauseState->SwitchToMap();
	}

}

MenuState_Inventory::MenuState_Inventory(GameState_Paused * pauseState, AABB2 bounds)
	:MenuState(pauseState, bounds)
{
	float m_screenWidth = m_bounds.GetWidth();
	//AABB2 unequippedBox = AABB2(m_bounds.mins + Vector2((m_screenWidth* .5f) + .5f,.5f), m_bounds.maxs - Vector2(.5f,2.f));
	AABB2 unequippedBox = m_bounds.GetPercentageBox(.55f, .1f, .95f, .8f);
	m_menu = new Menu(unequippedBox);
	for (unsigned int i = 0; i < g_theGame->m_party->m_inventory.size(); i++){
		m_menu->AddOption(g_theGame->m_party->m_inventory[i]->GetName().c_str(), i);
	}
}

void MenuState_Inventory::Update(float ds)
{
	int selection = m_menu->GetFrameSelection();
	if (selection != -1){
		g_theGame->m_party->EquipOrUnequipItem(g_theGame->m_party->m_inventory[selection]);
	}
}

void MenuState_Inventory::RenderBackground()
{
	g_theRenderer->DrawAABB2(m_bounds, RGBA(0, 32, 32, 64));
}

void MenuState_Inventory::RenderContent()
{
	float m_screenWidth = m_bounds.GetWidth();
	float fontSize = m_bounds.GetHeight() * .05f;

	//------ Draw raw inventory box -------//

	AABB2 unequippedBox = m_menu->m_menuBox;
	g_theRenderer->DrawAABB2(unequippedBox, RGBA(64,64,64,200));
	g_theRenderer->DrawAABB2Outline(unequippedBox, RGBA(255,255,255,128));
	g_theRenderer->DrawTextInBox2D("Inventory", unequippedBox, Vector2(.5f, .99f),fontSize * 1.5f);
	Vector2 textHeight = Vector2(0.f,fontSize);
	Vector2 unequippedDrawStart = Vector2(0.05f, .94f);

	AABB2 drawItemBox = AABB2(unequippedBox.mins.x, unequippedBox.maxs.y - textHeight.y, unequippedBox.maxs.x, unequippedBox.maxs.y);
	drawItemBox.Translate(0.f, fontSize * -2.f);
	for (unsigned int i = 0; i < g_theGame->m_party->m_inventory.size(); i++){
		Item* item = g_theGame->m_party->m_inventory[i];
		if (i == m_menu->m_currentIndex){
			item->RenderInMenu(drawItemBox, true);
		} else {
			item->RenderInMenu(drawItemBox, false);
		}
		drawItemBox.Translate(textHeight * -1.f);
	}

	//------ Draw equipped box -------//

	//AABB2 equippedBox = AABB2(m_bounds.mins + Vector2(.5f, m_screenWidth * .35f), m_bounds.maxs - Vector2((m_screenWidth* .5f) + .5f,2.f));
	AABB2 equippedBox = m_bounds.GetPercentageBox(.05f, .1f, .45f, .8f);
	fontSize = equippedBox.GetHeight() * .32f * .125f;		//shrink font size to be the size we need for drawing the text stuff
	textHeight = Vector2(0.f,fontSize);

	g_theRenderer->DrawAABB2(equippedBox, RGBA(64,64,64,200));
	g_theRenderer->DrawAABB2Outline(equippedBox, RGBA(255,255,255,128));
	g_theRenderer->DrawTextInBox2D("Equipped", equippedBox, Vector2(.5f, .97f),fontSize * 1.5f);

	float eqWidth = equippedBox.GetWidth();
	AABB2 equippedItemBox = AABB2(equippedBox.mins.x, equippedBox.maxs.y - textHeight.y -.02f, equippedBox.mins.x + (eqWidth * .4f), equippedBox.maxs.y - .02f);
	equippedItemBox.Translate(fontSize, fontSize * -2.5f);
	float eqItemHeight = equippedItemBox.GetHeight();
	AABB2 itemIconBox = AABB2(equippedItemBox.maxs.x, equippedItemBox.mins.y, equippedItemBox.maxs.x + eqItemHeight, equippedItemBox.maxs.y);
	for ( int i = 0; i < NUM_EQUIP_SLOTS; i++){
		Item* item = g_theGame->m_party->GetPlayerCharacter()->m_equippedItems[i];
		std::string drawText = Actor::GetEquipSlotByID((EQUIPMENT_SLOT) i) + ": ";
		std::string itemText = "NONE";
		Texture* buttonTexture = g_theGame->m_miscSpriteSheet->GetTexture();
		float padding = fontSize * .1f;
		
		if (item != nullptr){
			//itemText= item->m_definition->m_name;
			AABB2 texCoords = g_theGame->m_miscSpriteSheet->GetTexCoordsForSpriteCoords(IntVector2(0,0));
			g_theRenderer->DrawTexturedAABB2(itemIconBox, *buttonTexture, texCoords.mins, texCoords.maxs , RGBA::WHITE);
			itemIconBox.AddPaddingToSides(-padding, -padding);
			item->RenderImageInBox(itemIconBox);
		} else {
			AABB2 texCoords = g_theGame->m_miscSpriteSheet->GetTexCoordsForSpriteCoords(IntVector2(1,0));
			g_theRenderer->DrawTexturedAABB2(itemIconBox, *buttonTexture, texCoords.mins, texCoords.maxs , RGBA::WHITE);
			itemIconBox.AddPaddingToSides(-padding, -padding);
		}
		itemIconBox.AddPaddingToSides(padding, padding);
		g_theRenderer->DrawTextInBox2D(drawText, equippedItemBox, Vector2(.05f, .5f), fontSize, TEXT_DRAW_SHRINK_TO_FIT);
		//g_theRenderer->DrawTextInBox2D(itemText, equippedItemBox, )
		equippedItemBox.Translate(textHeight * -1.5f);
		itemIconBox.Translate(textHeight * -1.5f);
	}

	//// ------ Render Player Paperdoll ------ ////

	float eqHeight = equippedBox.GetHeight();
	equippedBox.AddPaddingToSides(0.f, eqHeight * -.2f);
	equippedBox.Translate(0.f, eqHeight * -.2f);
	g_theRenderer->DrawAABB2Outline(equippedBox, RGBA::YELLOW);
	g_theGame->m_party->GetPlayerCharacter()->RenderStatsInBox(equippedBox.GetPercentageBox(0.f,0.4f, 1.f, 1.f));


	////------ Draw selected item stats box -------//

	Item* selectedItem = g_theGame->m_party->m_inventory[m_menu->m_currentIndex];
	//AABB2 statsBox = AABB2(m_bounds.mins + Vector2(.5f,  .5f), m_bounds.maxs - Vector2((m_screenWidth* .5f) + .5f,2.f + (m_screenWidth * .5f)));
	AABB2 statsBox = equippedBox.GetPercentageBox(0.f,0.f, 1.f, .4f);
	g_theRenderer->DrawAABB2(statsBox, RGBA(64,64,64,200));
	g_theRenderer->DrawAABB2Outline(statsBox, RGBA(255,255,255,128));
	//g_theRenderer->DrawTextInBox2D(selectedItem->m_definition->m_name, statsBox, Vector2(.5f, .97f),fontSize);
	selectedItem->RenderStats(statsBox);
}

void MenuState_Inventory::HandleInput()
{
	if (WasPauseJustPressed() || g_primaryController->WasButtonJustPressed(XBOX_B)){
		m_pauseState->SwitchToPaused();
	}
	if (g_theInput->WasKeyJustPressed('M')){
		m_pauseState->SwitchToMap();
	}
	if (g_theInput->WasKeyJustPressed(VK_ESCAPE)){
		g_theGame->TransitionToState(m_pauseState->m_encounterGameState);
	}
	if (g_theInput->WasKeyJustPressed(VK_OEM_6) || g_theInput->WasKeyJustPressed(XBOX_D_RIGHT)){
		g_theGame->m_party->SwapPlayer(1);
	}
	if (g_theInput->WasKeyJustPressed(VK_OEM_4) || g_theInput->WasKeyJustPressed(XBOX_D_LEFT)){
		g_theGame->m_party->SwapPlayer(-1);
	}
	m_menu->HandleInput();
}

MenuState_Map::MenuState_Map(GameState_Paused * pauseState, AABB2 bounds)
	:MenuState(pauseState, bounds)
{
	g_theGame->m_fullMapMode = true;
}

void MenuState_Map::RenderBackground()
{
	g_theRenderer->DrawAABB2(m_bounds, RGBA(0, 32, 32, 200));
}

void MenuState_Map::RenderContent()
{
	m_pauseState->m_encounterGameState->RenderGame();
}

void MenuState_Map::HandleInput()
{
	if (WasPauseJustPressed() || g_primaryController->WasButtonJustPressed(XBOX_B)){
		m_pauseState->SwitchToPaused();
		g_theGame->m_fullMapMode = false;
	}
	if (g_theInput->WasKeyJustPressed('I') || g_primaryController->WasButtonJustPressed(XBOX_BACK)){
		m_pauseState->SwitchToInventory();
		g_theGame->m_fullMapMode = false;
	}
	if (g_theInput->WasKeyJustPressed(VK_ESCAPE)){
		g_theGame->TransitionToState(m_pauseState->m_encounterGameState);
		g_theGame->m_fullMapMode = false;
	}
}
