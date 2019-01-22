#include "EncounterState.hpp"
#include "Game/Encounter.hpp"
#include "Game/Map.hpp"
#include "Game/Actor.hpp"
#include "Game/Game.hpp"
#include "Game/Menu.hpp"
#include "Game/Map.hpp"
#include "Game/GameState.hpp"
#include "Engine/Renderer/OrbitCamera.hpp"
#include "Game/Projectile.hpp"
#include "Game/Fireball.hpp"

EncounterState::EncounterState()
{
	m_actions = std::queue<ActionType>();
}

Encounter* EncounterState::Enter(std::string mapFile)
{
	m_encounter = new Encounter();
	m_currentState = ENCOUNTER_STATE_DEFAULT;
	m_encounter->LoadMap(mapFile.c_str());

	m_encounter->AddActor(IntVector2(4,4), FACTION_BLUE, "Mage");
	m_encounter->AddActor(IntVector2(5,5), FACTION_BLUE, "Archer");
	m_encounter->AddActor(IntVector2(6,6), FACTION_BLUE, "Warrior");
	
	m_encounter->AddActor(IntVector2(10,10), FACTION_RED, "Warrior");
	m_encounter->AddActor(IntVector2(11,10), FACTION_RED, "Archer");
	m_encounter->AddActor(IntVector2(9,10), FACTION_RED, "Mage");
	m_encounter->SortActorsByWaitTime();
	return m_encounter;
}

void EncounterState::Update(float deltaSeconds)
{
	m_encounter->Update(deltaSeconds);
	switch(m_currentState){
	case (ENCOUNTER_STATE_DEFAULT):
		UpdateDefault(deltaSeconds);
		break;
	case (ENCOUNTER_STATE_PROCESSTURN):
		if (HasActions()){
			UpdateActions(deltaSeconds);
		} else {
			UpdateProcessTurn(deltaSeconds);
		}
		break;
	case (ENCOUNTER_STATE_VICTORY):
		UpdateVictory(deltaSeconds);
		break;
	}
}

void EncounterState::Render()
{
	
	m_encounter->Render();
	RenderCursor();
	

	switch(m_currentState){
	case (ENCOUNTER_STATE_DEFAULT):
		//RenderDefault();
		break;
	case (ENCOUNTER_STATE_PROCESSTURN):
		RenderProcessTurn();
		break;
	case (ENCOUNTER_STATE_VICTORY):
		//RenderVictory();
		break;
	}
	m_encounter->RenderEntities();	

	
	//g_theRenderer->ApplyEffect("defaultTest");
	//g_theRenderer->ApplyEffect("SwapRG");
	//g_theRenderer->FinishEffects();
	//g_theGame->SetMainCamera();
}

void EncounterState::RenderUI()
{
	switch(m_currentState){
	case (ENCOUNTER_STATE_DEFAULT):
		RenderDefault();
		break;
	case (ENCOUNTER_STATE_PROCESSTURN):
		RenderProcessTurnUI();
		break;
	case (ENCOUNTER_STATE_VICTORY):
		RenderVictory();
		break;
	}
}

void EncounterState::HandleInput()
{
	
	switch(m_currentState){
	case (ENCOUNTER_STATE_DEFAULT):
		HandleInputDefault();
		break;
	case (ENCOUNTER_STATE_PROCESSTURN):
		if (!HasActions()){
			HandleInputProcessTurn();		//do not allow user input while actions are playing
		}
		break;
	case (ENCOUNTER_STATE_VICTORY):
		HandleInputVictory();
		break;
	}
}

IntVector3 EncounterState::GetMapDimensions()
{
	return m_encounter->GetMap()->m_dimensions; 
}

Vector3 EncounterState::GetCursorPos()
{
	Block* cursorBlock = m_encounter->GetMap()->GetHighestTileAtXY(m_cursorPos.x, m_cursorPos.y);
	return cursorBlock->GetTopCenter();
}

IntVector2 EncounterState::GetCursorPosXY()
{
	return m_cursorPos;
}

void EncounterState::MoveCursor(const IntVector2 & direction)
{
	IntVector2 newNorth = FindNewNorth();		//figure out which direction is the new "North" (away from camera)
	IntVector2 newDir = MapDirectionToNorth(direction, newNorth);		//given the original direction (north, east, south, west) return the actual direction to move

	IntVector2 newPos = m_cursorPos + newDir;
	IntVector2 mapDimensions = IntVector2(m_encounter->GetMap()->GetWidth(), m_encounter->GetMap()->GetHeight());
	newPos = IntVector2(ClampInt(newPos.x, 0, mapDimensions.x - 1), ClampInt(newPos.y, 0, mapDimensions.y - 1));
	//if (IsPositionInSelectableArea(newPos)){
		SetCursorPos(newPos);
	//}
}

void EncounterState::SetCursorPos(IntVector2 newPos)
{
	m_cursorPos = newPos;
}

void EncounterState::UpdateDefault(float ds)
{
	UNUSED(ds);
	if (m_encounter->CheckForVictory()){
		m_currentState = ENCOUNTER_STATE_VICTORY;
	} else {
		m_currentActor = m_encounter->AdvanceToNextReadyActor();
		StartActorTurn();
		
	}
}

void EncounterState::UpdateProcessTurn(float ds)
{
	UNUSED(ds);
	//if self is dead, end the turn
	if (!m_currentActor->IsAlive()){
		m_currentActor->m_waitTime = MAX_WAIT_TIME;
		m_currentState = ENCOUNTER_STATE_DEFAULT;
	} else {
		m_currentActor->ProcessTurn();
	}

}

void EncounterState::UpdateActions(float ds)
{
	ActionType action = m_actions.front();
	switch (action){
	case (ACTION_WALK_TO_TILE):
		m_currentActor->UpdateWalkToTile(ds);
		break;
	case (ACTION_FIRE_PROJECTILE):
		m_currentProjectile->Update(ds);
		if (m_currentProjectile->IsAboutToBeDeleted()){
			delete m_currentProjectile;
			m_currentProjectile = nullptr;
			FinishAction();
		}
		break;
	case (ACTION_PLAY_ATTACK_ANIM):
		//m_currentActor->PlayAnim(ds);			//???
		FinishAction();
		break;
	}
}

void EncounterState::UpdateVictory(float ds)
{
	UNUSED(ds);
}

void EncounterState::RenderDefault()
{
}

void EncounterState::RenderProcessTurn()
{
	m_currentActor->RenderProcessTurn();
	if (m_currentProjectile != nullptr){
		m_currentProjectile->Render();
	}
}

void EncounterState::RenderProcessTurnUI()
{
	if (m_currentActor->m_currentAction == INVALID_ACTION && !HasActions()){
		m_currentActor->m_currentMenu->Render();
		
		
		//g_theGame->SetMainCamera();
	} else {
		Actor* targetActor = m_encounter->GetActorInColumn(m_cursorPos);
		if (targetActor != nullptr){
			m_currentActor->RenderHoverData(targetActor);
		}
	}

	m_encounter->RenderTurnWidget();
	
}

void EncounterState::RenderVictory()
{
	AABB2 uiBounds = g_theGame->SetUICamera();
	g_theRenderer->DrawAABB2(uiBounds, RGBA(0,65,65,200));
	int winningTeam = (int) m_encounter->GetActor(0)->m_faction;
	RGBA winningColor = GetFactionColor(m_encounter->GetActor(0)->m_faction);
	g_theRenderer->DrawTextInBox2D("TEAM " + std::to_string(winningTeam) + " WINS", uiBounds, Vector2(.5f,.5f), .06f, TEXT_DRAW_SHRINK_TO_FIT, winningColor);
	g_theRenderer->DrawTextInBox2D("Press Start to Return to Main Menu",uiBounds, Vector2(.5f,.3f), .03f, TEXT_DRAW_SHRINK_TO_FIT);
	g_theGame->SetMainCamera();
}

void EncounterState::RenderCursor()
{
	RGBA tint = RGBA::YELLOW;
	if (!IsPositionInSelectableArea(m_cursorPos)){
		tint = RGBA::RED;
	}
	Block* cursorBlock = m_encounter->GetMap()->GetHighestTileAtXY(m_cursorPos.x, m_cursorPos.y);
	Plane cursorPlane = cursorBlock->GetTopPlane();
	g_theRenderer->DrawPlane(cursorPlane, tint.GetColorWithAlpha(128));
	g_theRenderer->EnableDepth(COMPARE_GREATER, false);
	g_theRenderer->DrawPlane(cursorPlane, RGBA::MAGENTA.GetColorWithAlpha(64));		//cursor overlay
	g_theRenderer->EnableDepth(COMPARE_LESS, true);
}


void EncounterState::HandleInputDefault()
{
	HandleCameraInput();
}

void EncounterState::HandleInputProcessTurn()
{
	//handles turn input for an actor and checks to see if cursor should be moveable
	if (m_currentActor->HandleTurnInputAndCheckCursorMovement()){
		HandleCursorArrowInput();
	}
	/*switch (m_currentAction){
	case (ENCOUNTER_ACTION_MOVE):
		HandleCursorArrowInput();
		if (AcceptJustPressed()){
			IntVector2 newPos = GetCursorPosXY();		
			m_currentActor->Move(newPos);
			m_currentAction = INVALID_ACTION;
		}
		if (BackJustPressed()){
			m_currentMenu->EnableOption(ENCOUNTER_ACTION_MOVE);
			SetCursorPos(m_currentActor->m_position);
			m_currentAction = INVALID_ACTION;
		}
		break;
	case (ENCOUNTER_ACTION_ATTACK):
		HandleCursorArrowInput();
		if (AcceptJustPressed()){
			IntVector2 newPos = GetCursorPosXY();		
			m_currentActor->Attack(newPos);
			m_currentAction = INVALID_ACTION;
		}
		if (BackJustPressed()){
			m_currentMenu->EnableOption(ENCOUNTER_ACTION_ATTACK);
			SetCursorPos(m_currentActor->m_position);
			m_currentAction = INVALID_ACTION;
		}
		break;
	default:
		m_currentMenu->HandleInput();
	}*/
	HandleCameraInput();
	
}

void EncounterState::HandleInputVictory()
{
	if (AcceptJustPressed() || g_theInput->GetController(0)->WasButtonJustPressed(XBOX_START)){
		g_theGame->TransitionToState(new GameState_Attract());
	}
}

void EncounterState::HandleCursorArrowInput()
{
	if (RightJustPressed()){
		MoveCursor(STEP_EAST);
	}
	if (UpJustPressed()){
		MoveCursor(STEP_NORTH);
	}
	if (LeftJustPressed()){
		MoveCursor(STEP_WEST);
	}
	if (DownJustPressed()){
		MoveCursor(STEP_SOUTH);
	}
}

void EncounterState::HandleCameraInput()
{
	XboxController* controller = g_theInput->GetController(0);
	Vector2 thumbstickPos = controller->GetRightThumbstickCoordinates();
	//Vector2 zoomPos = g_theInput->GetController(0)->GetRightThumbstickCoordinates();
	float zoomIn =  controller->GetRightTrigger();
	float zoomOut = controller->GetLeftTrigger();
	g_theGame->RotateCamera(thumbstickPos, zoomIn, zoomOut);		

	
	if (controller->WasButtonJustPressed(XBOX_BUMPER_RIGHT)){
		g_theGame->m_camera->SnapToNextCorner(-1);
	}
	if (controller->WasButtonJustPressed(XBOX_BUMPER_LEFT)){
		g_theGame->m_camera->SnapToNextCorner(1);
	}
}

void EncounterState::StartActorTurn()
{
	
	m_currentActor->m_turnsSinceLastMove = 0;
	m_currentState = ENCOUNTER_STATE_PROCESSTURN;
	g_theGame->SetCameraTarget(m_currentActor->GetBlockPosition());

	//AABB2 UIBox = g_theGame->m_uiCamera->GetBounds();

	//m_currentMenu = new Menu(UIBox.GetPercentageBox(.35f,.45f,.45f,.55f)); 

	//m_currentMenu->AddOption( "MOVE", (int) ENCOUNTER_ACTION_MOVE ); 
	//m_currentMenu->AddOption( "ATTACK", (int) ENCOUNTER_ACTION_ATTACK ); 
	//m_currentMenu->AddOption( "WAIT", (int) ENCOUNTER_ACTION_WAIT );

	m_currentActor->MakeMenu();

	SetCursorPos(m_currentActor->m_position);
}

void EncounterState::AddAction(ActionType newAction)
{
	m_actions.push(newAction);
}

bool EncounterState::HasActions() const
{
	return (m_actions.size() > 0);
}

void EncounterState::FinishAction()
{
	ActionType action = m_actions.front();
	switch (action){
	case (ACTION_FIRE_PROJECTILE):
		EndActorTurn();
	case (ACTION_PLAY_ATTACK_ANIM):
		EndActorTurn();
	}
	m_actions.pop();
}

//asdfjdkflajf;ajf
void EncounterState::AddProjectile(IntVector2 launchPos, IntVector2 targetPos, int damage, float critChance, float blockChance, float maxRange, std::string type,  int effectRange, int verticalRange, float cosmeticOffset, float gravity)
{
	Map* map = m_encounter->GetMap();
	Vector3 launchPos3 = map->GetHighestTileAtXY(launchPos)->GetTopCenter();
	Vector3 targetPos3 = map->GetHighestTileAtXY(targetPos)->GetTopCenter();
	Projectile* proj = nullptr;
	if (type =="arrow"){
	//ConsolePrintf("Launched projectile at: (%i, %i) from (%i, %i).", targetPos.x, targetPos.y, launchPos.x, launchPos.y);
		proj = new Projectile(map, launchPos3, targetPos3, damage, critChance, blockChance, maxRange, gravity, cosmeticOffset);
	}
	if (type == "fireball"){
		proj = (Projectile*) new Fireball(map, launchPos3, targetPos3, damage, maxRange, effectRange, verticalRange, gravity, .3f);
	}
	AddProjectile(proj);
	
}

void EncounterState::AddProjectile(Projectile * proj)
{
	m_currentProjectile = proj;
	if (proj != nullptr){
		AddAction(ACTION_FIRE_PROJECTILE);
	}
}	

void EncounterState::EndActorTurn()
{
	m_currentState = ENCOUNTER_STATE_DEFAULT;
	m_encounter->SortActorsByWaitTime();
}

void EncounterState::TurnUpdate()
{
}

bool EncounterState::IsPositionInSelectableArea(IntVector2 pos) const
{
	//check against a current heat map
	if (m_currentActor->m_selectableArea->GetHeat(pos) < 9999.f){
		return true;
	}
	return false;
}


IntVector2 EncounterState::FindNewNorth()
{
	IntVector2 newNorth;
	Vector3 camForward = g_theGame->m_camera->GetForward();
	float maxDot = 0.f;

	float northDot	= DotProduct(camForward, Vector3(0,0,1));
	float eastDot	= DotProduct(camForward, Vector3(1,0,0));
	float southDot	= DotProduct(camForward, Vector3(0,0,-1));
	float westDot	= DotProduct(camForward, Vector3(-1,0,0));

	//there is a better way to do this.
	if ( northDot > maxDot){
		maxDot = northDot;
		newNorth = STEP_NORTH;
	}
	if (eastDot > maxDot){
		maxDot = eastDot;
		newNorth = STEP_EAST;
	}
	if (southDot > maxDot){
		maxDot = southDot;
		newNorth = STEP_SOUTH;
	}
	if (westDot > maxDot){
		maxDot = westDot;
		newNorth = STEP_WEST;
	}
	return newNorth;
}

IntVector2 EncounterState::MapDirectionToNorth(IntVector2 direction, IntVector2 newNorth)
{
	IntVector2 newDirection = newNorth;
	if (direction == STEP_EAST){
		newDirection = IntVector2(newNorth.y, -newNorth.x);
	} else if (direction == STEP_SOUTH){
		newDirection = IntVector2(-newNorth.x, -newNorth.y);
	} else if (direction == STEP_WEST){
		newDirection = IntVector2(-newNorth.y, newNorth.x);
	}
	return newDirection;
}