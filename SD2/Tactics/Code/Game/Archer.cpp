#include "Game/Archer.hpp"
#include "Game/Game.hpp"
#include "Game/EncounterState.hpp"
#include "Game/Map.hpp"
#include "Engine/Renderer/OrbitCamera.hpp"
#include "Engine/Math/Trajectory.hpp"
#include "Game/IsoSprite.hpp"
#include "Game/AnimSet.hpp"
#include "Game/Animator.hpp"

Archer::Archer(IntVector2 initialPosition, Map * entityMap, Faction faction)
	:Actor(initialPosition, entityMap, faction)
{
	m_health		= 60;
	m_maxHealth		= 60;
	m_moveSpeed		= 5;
	m_jumpHeight	= 3;
	m_strength		= IntRange(15,30);

	AnimSet* archerSet = AnimSet::GetAnimSet("archer_f");
	IsoSprite* defaultSprite = IsoSprite::GetIsoSprite("archer_f.idle");
 	m_animator = new Animator(archerSet, defaultSprite);

}

void Archer::Render()
{
	if (IsAlive()){
		Block* block = m_map->GetHighestTileAtXY(m_position.x, m_position.y);
		Vector3 pos = block->GetTopCenter();
		//g_theRenderer->DrawCube(pos, Vector3(BLOCK_WIDTH * .35f, BLOCK_WIDTH * 2.f * (float) m_height, BLOCK_WIDTH * .35f), GetFactionColor(m_faction));
		IsoSprite* iso = m_animator->GetCurrentIsoSprite(); //IsoSprite::GetIsoSprite("archer_f.idle");
		Sprite* actorSprite = iso->GetSpriteforViewAngle(g_theGame->m_camera->GetForward(), m_forwardDirection);
		g_theRenderer->DrawSprite(m_worldPosition, actorSprite, g_theGame->m_camera->GetRight(), Vector3::UP);
		g_theRenderer->BindShaderProgram("stroke");
		g_theRenderer->DrawTextAsSprite("Archer", m_worldPosition + Vector3(0.f, m_height * .9f, 0.f), Vector2::HALF, .2f, g_theGame->m_camera->GetRight(), Vector3::UP, GetFactionColor(m_faction));
		g_theRenderer->BindShaderProgram("cutout");
		//g_theRenderer->DrawCube(m_worldPosition + Vector3(m_forwardDirection.x, 0.f, m_forwardDirection.y), Vector3::ONE * .1f, GetFactionColor(m_faction));
	}
}

void Archer::RenderProcessTurn()
{
	if (m_currentAction == ACTOR_ACTION_MOVE || m_currentAction == ACTOR_ACTION_ATTACK || m_currentAction == ACTOR_ACTION_BOW){
		RenderSelectableArea();

		if (m_currentAction == ACTOR_ACTION_BOW && !g_theGame->GetEncounterState()->HasActions()){
			RenderTrajectoryPath();
		}
	}
}

void Archer::RenderHoverData(Actor * target)
{
	AABB2 UIBounds = g_theGame->GetUIBounds();
	AABB2 hoverBox = UIBounds.GetPercentageBox(.85f, 0.f, 1.f, .15f);
	std::string text;
	if (m_currentAction == ACTOR_ACTION_MOVE){
		text = "Health: " + std::to_string(target->GetHealth()) +\
			"\nFaction: " + std::to_string(target->GetFactionInt()) +\
			"\nWait: "	  + std::to_string(target->GetWaitTime());
	} else if (m_currentAction == ACTOR_ACTION_ATTACK || m_currentAction == ACTOR_ACTION_BOW){
		text = "Health: "		+ std::to_string(target->GetHealth())					+\
			"\nFaction: "		+ std::to_string(target->GetFactionInt())				+\
			"\nWait: "			+ std::to_string(target->GetWaitTime())					+\
			"\nCrit Chance: "	+ std::to_string(GetCritChance(target)) +\
			"\nBlock Chance: "	+ std::to_string(GetBlockChance(target));
	}

	g_theRenderer->DrawAABB2(hoverBox, RGBA(64, 64, 128, 128));
	g_theRenderer->DrawAABB2Outline(hoverBox, RGBA::WHITE);
	g_theRenderer->DrawTextInBox2D(text, hoverBox, Vector2::HALF, .5f, TEXT_DRAW_SHRINK_TO_FIT);
}

void Archer::Attack(IntVector2 targetPos)
{
	m_animator->Play("attack");
	Actor::Attack(targetPos);
	m_waitTime+=40.f;
}

void Archer::TakeDamage(int damage, float critChance, float blockChance)
{
	m_animator->Play("hit");
	Actor::TakeDamage(damage, critChance, blockChance);
}

Menu * Archer::MakeMenu()
{
	AABB2 UIBox = g_theGame->GetUIBounds();

	Menu* menu = new Menu(UIBox.GetPercentageBox(.35f,.45f,.45f,.55f)); 

	menu->AddOption( "MOVE", (int) ACTOR_ACTION_MOVE ); 
	menu->AddOption( "ATTACK", (int) ACTOR_ACTION_ATTACK ); 
	menu->AddOption( "BOW", (int) ACTOR_ACTION_BOW);
	menu->AddOption( "WAIT", (int) ACTOR_ACTION_WAIT );

	m_currentMenu = menu;
	return menu;
}

void Archer::ProcessTurn()
{
	//Checks for menu selection and triggers appropriate action
	ActorAction newAction = (ActorAction) m_currentMenu->GetFrameSelection();
	switch (newAction){
	case (ACTOR_ACTION_WAIT):
		Wait();
		g_theGame->GetEncounterState()->EndActorTurn();
		break;
	case (ACTOR_ACTION_MOVE):
		m_currentAction = newAction;
		m_currentMenu->DisableOption((int) newAction);
		SetMoveArea();
		g_theGame->SetCursorPos(m_position);
		break;
	case (ACTOR_ACTION_ATTACK):
		m_currentAction = newAction;
		m_currentMenu->DisableOption((int) newAction);
		SetAttackArea();
		g_theGame->SetCursorPos(m_position);
		break;
	case (ACTOR_ACTION_BOW):
		m_currentAction = newAction;
		m_currentMenu->DisableOption((int) newAction);
		SetBowArea();
		g_theGame->SetCursorPos(m_position);
		break;
	}

}

bool Archer::HandleTurnInputAndCheckCursorMovement()
{
	switch (m_currentAction){
	case (ACTOR_ACTION_MOVE):
		if (AcceptJustPressed() && g_theGame->GetEncounterState()->IsCursorInSelectableArea()){
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
		if (AcceptJustPressed() && g_theGame->GetEncounterState()->IsCursorInSelectableArea()){
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
	case (ACTOR_ACTION_BOW):
		if (AcceptJustPressed() && g_theGame->GetEncounterState()->IsCursorInSelectableArea()){
			IntVector2 newPos = g_theGame->GetCursorPosXY();		
			BowAttack(newPos);
			m_currentAction = INVALID_ACTION;
		}
		if (BackJustPressed()){
			m_currentMenu->EnableOption(ACTOR_ACTION_BOW);
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

float Archer::GetBlockChance(Actor * target)
{
	if (target->IsDefending()){
		return 1.f;
	} else{
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
}

void Archer::SetBowArea()
{
	float gravity = 9.8f;
	m_selectableArea->ClearHeat(9999.f);
	for (int x = 0; x < m_map->GetWidth(); x++){
		for (int y = 0; y < m_map->GetHeight(); y++){
			IntVector2 coords = IntVector2(x,y);
			Vector2 thetas = Vector2::ZERO;
			Vector2 dir2D = coords.GetVector2() - m_position.GetVector2();
			Vector3 direction = Vector3(dir2D.x, 0.f, dir2D.y);
			float distance = direction.NormalizeAndGetLength();
			float launchSpeed= Trajectory::GetMinimumLaunchSpeed(9.8f, m_maxRange + .5f);
			if (distance >= m_minRange - .5f && distance <= m_maxRange + .5f){
				float height = m_map->GetHeightAtXY(coords) - m_worldPosition.y;
				if (Trajectory::GetLaunchAngles(&thetas, gravity, launchSpeed, distance, height)){
					m_selectableArea->SetHeat(coords, distance);
				}
				//float theta = max(thetas.x, thetas.y);
				//Vector2 traj = Trajectory::Evaluate(gravity, launchSpeed, theta, t);
				
			}
		}
	}
}

void Archer::BowAttack(IntVector2 targetPos)
{
	m_animator->Play("attack");
	m_waitTime+=40.f;
	Actor* toAttack = m_map->m_encounter->GetActorInColumn(targetPos);
	float critChance   = 0.f;
	float blockChance  = 0.f;
	if (toAttack != nullptr){
		critChance = GetCritChance(toAttack);
		blockChance = GetBlockChance(toAttack);
	}
	g_theGame->GetEncounterState()->AddProjectile(m_position, targetPos, m_strength.GetRandomInRange(), critChance, blockChance, m_maxRange, "arrow", 0, 1, m_height * .5f, 9.8f);
}

Sprite * Archer::GetDisplaySprite() const
{
	return g_theRenderer->GetSprite("archer_f.portrait");
}

void Archer::RenderTrajectoryPath()
{
	
	float gravity = 9.8f;
	float tStep = .06f;
	float t = 0.f;
	Vector2 thetas = Vector2::ZERO;
	Vector2 dir2D = g_theGame->GetCursorPosXY().GetVector2() - m_position.GetVector2();
	Vector3 direction = Vector3(dir2D.x, 0.f, dir2D.y);
	float distance = direction.NormalizeAndGetLength();
	float launchSpeed= Trajectory::GetMinimumLaunchSpeed(9.8f, m_maxRange + .5f);
	if (distance >= m_minRange - .5f && distance <= m_maxRange + .5f){
		float height = m_map->GetHeightAtXY(g_theGame->GetCursorPosXY()) - m_worldPosition.y;
		Trajectory::GetLaunchAngles(&thetas, gravity, launchSpeed, distance, height);
		float theta = max(thetas.x, thetas.y);
		Vector2 traj = Trajectory::Evaluate(gravity, launchSpeed, theta, t);
		while (traj.x < distance){
			
			t+=tStep;
			//Vector2  displacementWorld = GetTransformedOutOfBasis(traj, direction, Vector2(-direction.y, direction.x));
			Vector3 pos3D = m_worldPosition + (direction * traj.x); //????
			pos3D.y = m_worldPosition.y + traj.y + (m_height * .5f);		//display is from half height up
			
			//if inside a block, break
			if (m_map->GetHeightAtXY(IntVector2(RoundToNearestInt(pos3D.x), RoundToNearestInt(pos3D.z))) > pos3D.y){
				break;
			}

			g_theRenderer->DrawCube(pos3D, Vector3::ONE * .03f, RGBA::YELLOW);
			traj = Trajectory::Evaluate(gravity, launchSpeed, theta, t);
			
				
		}
	}
}
