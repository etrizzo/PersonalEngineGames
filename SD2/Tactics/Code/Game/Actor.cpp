#include "Actor.hpp"
#include "Game/Map.hpp"
#include "Game/Game.hpp"
#include "Engine/Renderer/OrbitCamera.hpp"
#include "Game/EncounterState.hpp"
#include "Game/AnimSet.hpp"
#include "Game/Animator.hpp"


Actor::Actor(IntVector2 initialPosition, Map * entityMap, Faction faction)
{
	
	m_map = entityMap;
	m_faction = faction;
	m_health = m_maxHealth;
	m_distanceMap = new Heatmap(IntVector2(m_map->GetWidth(), m_map->GetHeight()), 9999.f);
	m_selectableArea = m_distanceMap;
	SetDistanceMap();
	SetPosition(initialPosition);


	//Block* block = m_map->GetHighestTileAtXY(m_position.x, m_position.y);
	//Vector3 pos = block->GetTopCenter();
	//g_theGame->AddFlyoutText("Im alive", pos + Vector3(0.f, m_height, 0.f), .2f, 2.f);
	
}

void Actor::Update(float deltaSeconds)
{
	Entity::Update(deltaSeconds);
	if (m_animator != nullptr){
		m_animator->Update(deltaSeconds);
	}
}

void Actor::UpdateWalkToTile(float deltaSeconds)
{


	Vector2 tilePos = m_pathToDestination[m_pathIndex].GetVector2();
	Vector2 worldPos = Vector2(m_worldPosition.x, m_worldPosition.z);
	if (GetDistance(worldPos, tilePos) > .1f){		//if the next tile has NOT been reached, head @ it
		Vector2 direction = tilePos - worldPos;
		direction.NormalizeAndGetLength();
		Vector3 dir3D = Vector3(direction.x, m_map->GetHeightAtXY(m_position), direction.y);
		dir3D = dir3D * deltaSeconds * 2.f;
		SetPosition(m_worldPosition + dir3D);
	} else {		//if the tile HAS been reached (approximately) set position (actually) and update next coordinate 
		IntVector2 spot = m_pathToDestination[m_pathIndex];
		Vector3 world = m_map->GetWorldPosition(spot);
		SetPosition(spot);
		m_forwardDirection = m_pathToDestination[ClampInt(m_pathIndex + 1, 0, m_pathToDestination.size() - 1)].GetVector2() - m_pathToDestination[m_pathIndex].GetVector2();
		m_pathIndex++;
	}

	if (m_pathIndex >= (int) m_pathToDestination.size()){
		//destination has been reached!!!!
		SetPosition(m_destinationCoords);
		SetDistanceMap();		//when done
		g_theGame->GetEncounterState()->FinishAction();
		int index = ClampInt(m_pathIndex, 0, m_pathToDestination.size() - 1);
		m_forwardDirection = m_pathToDestination[index].GetVector2() - m_pathToDestination[index-1].GetVector2();
		if (m_animator!= nullptr){
			m_animator->Play("idle");
		}
	}
	g_theGame->SetDollyCamTarget(m_worldPosition);
}


void Actor::Render()
{
	if (IsAlive()){
		Block* block = m_map->GetHighestTileAtXY(m_position.x, m_position.y);
		Vector3 pos = block->GetTopCenter();
		//g_theRenderer->DrawCube(pos, Vector3(BLOCK_WIDTH * .35f, BLOCK_WIDTH * 2.f * (float) m_height, BLOCK_WIDTH * .35f), GetFactionColor(m_faction));
		Sprite* actorSprite = g_theRenderer->GetSprite("archer_f.tr.idle");
		g_theRenderer->DrawSprite(m_worldPosition, actorSprite, g_theGame->m_camera->GetRight(), Vector3::UP, GetFactionColor(m_faction));
		g_theRenderer->DrawTextAsSprite("the boi", m_worldPosition + Vector3(0.f, (float) m_height, 0.f), Vector2::HALF, .1f, g_theGame->m_camera->GetRight(), Vector3::UP, GetFactionColor(m_faction));
		//g_theRenderer->DrawCube(m_worldPosition + Vector3(m_forwardDirection.x, 0.f, m_forwardDirection.y), Vector3::ONE * .1f, GetFactionColor(m_faction));
	}
	if (g_theGame->m_devMode){
		RenderDistanceMap();
	}
}

float Actor::GetCritChance(Actor * target)
{
	float dot = DotProduct(target->GetForwardDirection(), m_position.GetVector2() - target->GetPosition().GetVector2());
	float forwardDot = .71f;		//~45 degrees
	if (dot >= forwardDot){
		//attacker is fully in front of target
		return .05f;
	} else {
		if (abs(dot) >= forwardDot){
			//attacker is fully behind target
			return .25f;
		} else {
			//attacker is on the side of target
			return .15f;
		}
	}
}

float Actor::GetBlockChance(Actor * target)
{
	float dot = DotProduct(target->GetForwardDirection(), m_position.GetVector2() - target->GetPosition().GetVector2());
	float forwardDot = .71f;		//~45 degrees
	if (dot >= forwardDot){
		//attacker is fully in front of target
		return .2f;
	} else {
		if (abs(dot) >= forwardDot){
			//attacker is fully behind target
			return 0.f;
		} else {
			//attacker is on the side of target
			return .05f;
		}
	}
}

void Actor::RenderDistanceMap()
{
	for (int x = 0; x < m_map->GetWidth(); x++){
		for (int y = 0; y < m_map->GetHeight(); y++){
			if (m_distanceMap->GetHeat(x,y) < 9999.f){
				Block* block = m_map->GetHighestTileAtXY(x,y);
				Plane highlightPlane = block->GetTopPlane();
				g_theRenderer->DrawPlane(highlightPlane, RGBA::GREEN.GetColorWithAlpha(100));
				g_theRenderer->EnableDepth(COMPARE_ALWAYS, false);
				g_theRenderer->DrawPlane(highlightPlane, RGBA::CYAN.GetColorWithAlpha(64));		//cursor overlay
				g_theRenderer->EnableDepth(COMPARE_LESS, true);
			}
		}
	}
}

void Actor::RenderSelectableArea()
{
	for (int x = 0; x < m_map->GetWidth(); x++){
		for (int y = 0; y < m_map->GetHeight(); y++){
			if (m_selectableArea->GetHeat(x,y) < 9999.f){
				Block* block = m_map->GetHighestTileAtXY(x,y);
				Plane highlightPlane = block->GetTopPlane();
				g_theRenderer->DrawPlane(highlightPlane, RGBA::GREEN.GetColorWithAlpha(100));
				g_theRenderer->EnableDepth(COMPARE_GREATER, false);
				g_theRenderer->DrawPlane(highlightPlane, RGBA::CYAN.GetColorWithAlpha(64));		//cursor overlay
				g_theRenderer->EnableDepth(COMPARE_LESS, true);
				g_theRenderer->DrawTextInBox2D(std::to_string((int) m_distanceMap->GetHeat(x,y)), highlightPlane.GetAABB2(), Vector2(.5f,.5f), BLOCK_WIDTH * .25f, TEXT_DRAW_SHRINK_TO_FIT);
			}
		}
	}
}

void Actor::RenderHoverData(Actor* target)
{
	AABB2 UIBounds = g_theGame->GetUIBounds();
	AABB2 hoverBox = UIBounds.GetPercentageBox(.85f, 0.f, 1.f, .15f);
	std::string text = "";
	if (m_currentAction == ACTOR_ACTION_MOVE){
		text = "Health: " + std::to_string(target->GetHealth()) +\
			"\nFaction: " + std::to_string(target->GetFactionInt()) +\
			"\nWait: "	  + std::to_string(target->GetWaitTime());
	} else if (m_currentAction == ACTOR_ACTION_ATTACK){
		text = "Health: "		+ std::to_string(target->GetHealth())					+\
			"\nFaction: "		+ std::to_string(target->GetFactionInt())				+\
			"\nWait: "			+ std::to_string(target->GetWaitTime())					+\
			"\nCrit Chance: "	+ std::to_string(GetCritChance(target)) +\
			"\nBlock Chance: "	+ std::to_string(GetBlockChance(target));
	}

	if (text != ""){
		g_theRenderer->DrawAABB2(hoverBox, RGBA(64, 64, 128, 128));
		g_theRenderer->DrawAABB2Outline(hoverBox, RGBA::WHITE);
		g_theRenderer->DrawTextInBox2D(text, hoverBox, Vector2::HALF, .5f, TEXT_DRAW_SHRINK_TO_FIT);
	}
}

void Actor::RenderProcessTurn()
{
	if (m_currentAction == ACTOR_ACTION_MOVE || m_currentAction == ACTOR_ACTION_ATTACK){
		RenderSelectableArea();
	}
}

void Actor::Wait()
{
	if (m_waitTime < MIN_WAIT_TIME){
		m_waitTime = MIN_WAIT_TIME;
	}
}

void Actor::Move(IntVector2 m_newPos)
{
	m_pathToDestination = m_map->GetPath(m_position, m_newPos, *m_distanceMap);
	m_destinationCoords = m_newPos;
	m_pathIndex = 0;
	m_forwardDirection = m_pathToDestination[ClampInt(m_pathIndex + 1, 0, m_pathToDestination.size() - 1)].GetVector2() - m_pathToDestination[m_pathIndex].GetVector2();

	//if (m_pathToDestination.size() > 1){		//if pos != newPos, update direction
	//	m_forwardDirection = m_pathToDestination[0].GetVector2() - m_pathToDestination[1].GetVector2();
	//}
	//m_position = m_newPos;
	m_waitTime += 40.f;

	g_theGame->GetEncounterState()->AddAction(ACTION_WALK_TO_TILE);
	if (m_animator != nullptr){
		m_animator->Play("walk");
	}
	
	
	//Block* block = m_map->GetHighestTileAtXY(m_position.x, m_position.y);
	//Vector3 pos = block->GetTopCenter();
	//g_theGame->AddFlyoutText("Moving", pos + Vector3(0.f, m_height, 0.f), .2f, 2.f);
}

void Actor::Attack(IntVector2 posToAttack)
{
	Actor* toAttack = m_map->m_encounter->GetActorInColumn(posToAttack);
	if (toAttack != nullptr){
		float critChance = GetCritChance(toAttack);
		float blockChance = GetBlockChance(toAttack);
		toAttack->TakeDamage(m_strength.GetRandomInRange(), critChance, blockChance);

		m_forwardDirection = posToAttack.GetVector2() - m_position.GetVector2();
	}
	//g_theGame->GetEncounterState()->AddAction(ACTION_PLAY_ATTACK_ANIM);
}

void Actor::TakeDamage(int damage, float critChance, float blockChance)
{

	Block* block = m_map->GetHighestTileAtXY(m_position.x, m_position.y);
	Vector3 pos = block->GetTopCenter();
	RGBA tint = RGBA::WHITE;
	std::string text = std::to_string(damage) ;
	if (g_theGame->m_devMode){
		text = std::to_string(damage) + " crit:" + std::to_string(critChance) + " block:" + std::to_string(blockChance);
	}
	if( CheckRandomChance(critChance)){
		damage*=2;
		tint = RGBA::RED;
		text = std::to_string(damage);
	}
	if( CheckRandomChance(blockChance)){
		damage = 0;
		tint = RGBA::WHITE;
		text = "BLOCKED";
	}
	if (damage < 0){
		text = std::to_string(-damage) ;
		tint = RGBA::GREEN;
	}
	m_health = ClampInt(m_health - damage, 0, m_maxHealth);
	if (!IsAlive()){
		m_aboutToBeDeleted = true;
	}

	g_theGame->AddFlyoutText(text, pos + Vector3(0.f, (float) m_height, 0.f), .2f, 2.5f, tint, .5f);
}

void Actor::SetDistanceMap()
{
	m_distanceMap->ClearHeat(9999.f);
	m_distanceMap->SetHeat(m_position, 0.f);
	m_map->FillSeededDistanceMap(*m_distanceMap, nullptr, m_moveSpeed, m_jumpHeight);
}

void Actor::SetMoveArea()
{
	SetDistanceMap();		//this shouldn't be necessary but idgaf
	m_selectableArea = m_distanceMap;
}

void Actor::SetAttackArea()
{
	m_selectableArea->ClearHeat(9999.f);
	m_selectableArea->SetHeat(m_position, 1.f);		//for targeting self
	m_selectableArea->SetHeat(m_position + STEP_EAST, 1.f);
	m_selectableArea->SetHeat(m_position + STEP_NORTH, 1.f);
	m_selectableArea->SetHeat(m_position + STEP_WEST, 1.f);
	m_selectableArea->SetHeat(m_position + STEP_SOUTH, 1.f);

}

void Actor::ProcessTurn()
{
}

bool Actor::HandleTurnInputAndCheckCursorMovement()
{
	switch (m_currentAction){
	case (ACTOR_ACTION_MOVE):
		if (AcceptJustPressed()){
			IntVector2 newPos = g_theGame->GetCursorPosXY();		
			Move(newPos);
			m_currentAction = INVALID_ACTION;
		}
		if (BackJustPressed()){
			m_currentMenu->EnableOption(ACTOR_ACTION_MOVE);
			g_theGame->SetCursorPos(m_position);
			m_currentAction = INVALID_ACTION;
		}
		return true;
		break;
	case (ACTOR_ACTION_ATTACK):
		if (AcceptJustPressed()){
			IntVector2 newPos = g_theGame->GetCursorPosXY();		
			Attack(newPos);
			m_currentAction = INVALID_ACTION;
		}
		if (BackJustPressed()){
			m_currentMenu->EnableOption(ACTOR_ACTION_ATTACK);
			g_theGame->SetCursorPos(m_position);
			m_currentAction = INVALID_ACTION;
		}
		return true;
		break;
	default:
		m_currentMenu->HandleInput();
		return false;
	}
}

Vector2 Actor::GetForwardDirection() const
{
	return m_forwardDirection;
}

void Actor::PassTime(float waitTime)
{
	m_waitTime -= waitTime;
	ClampFloat(m_waitTime, 0.f, MAX_WAIT_TIME);
}

Vector3 Actor::GetBlockPosition()
{
	Block* block = m_map->GetHighestTileAtXY(m_position.x, m_position.y);
	return block->GetTopCenter();
}

Vector3 Actor::GetWorldPosition()
{
	Block* block = m_map->GetHighestTileAtXY(m_position.x, m_position.y);
	return block->GetTopCenter();
}

RGBA GetFactionColor(Faction faction)
{
	switch (faction){
	case FACTION_BLUE:
		return RGBA::BLUE;
	case FACTION_RED:
		return RGBA::RED;
	default:
		return RGBA::WHITE;
	}
}
