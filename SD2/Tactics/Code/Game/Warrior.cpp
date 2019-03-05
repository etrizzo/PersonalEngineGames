#include "Game/Warrior.hpp"
#include "Game/Game.hpp"
#include "Game/EncounterState.hpp"
#include "Game/Map.hpp"
#include "Engine/Renderer/OrbitCamera.hpp"
#include "Game/IsoSprite.hpp"
#include "Game/Animator.hpp"
#include "Game/AnimSet.hpp"


Warrior::Warrior(IntVector2 initialPosition, Map * entityMap, Faction faction)
	:Actor(initialPosition, entityMap, faction)
{
	m_health		= 100;
	m_maxHealth		= 100;
	m_moveSpeed		= 4;
	m_jumpHeight	= 1;
	m_strength		= IntRange(20,40);

	AnimSet* archerSet = AnimSet::GetAnimSet("knight_f");
	IsoSprite* defaultSprite = IsoSprite::GetIsoSprite("knight_f.idle");
	m_animator = new Animator(archerSet, defaultSprite);
}

void Warrior::Render()
{
	if (IsAlive()){
		Block* block = m_map->GetHighestTileAtXY(m_position.x, m_position.y);
		Vector3 pos = block->GetTopCenter();
		//g_theRenderer->DrawCube(pos, Vector3(BLOCK_WIDTH * .35f, BLOCK_WIDTH * 2.f * (float) m_height, BLOCK_WIDTH * .35f), GetFactionColor(m_faction));
		IsoSprite* iso = m_animator->GetCurrentIsoSprite(); //IsoSprite::GetIsoSprite("archer_f.idle");
		//IsoSprite* iso = IsoSprite::GetIsoSprite("knight_f.idle");
		Sprite* actorSprite = iso->GetSpriteforViewAngle(g_theGame->m_camera->GetForward(), m_forwardDirection);
		g_theRenderer->DrawSprite(m_worldPosition, actorSprite, g_theGame->m_camera->GetRight(), UP);
		g_theRenderer->BindShaderProgram("stroke");
		g_theRenderer->DrawTextAsSprite("Warrior", m_worldPosition + Vector3(0.f, m_height * .9f, 0.f), Vector2::HALF, .2f, g_theGame->m_camera->GetRight(), UP, GetFactionColor(m_faction));
		g_theRenderer->BindShaderProgram("cutout");
		//g_theRenderer->DrawCube(m_worldPosition + Vector3(m_forwardDirection.x, 0.f, m_forwardDirection.y), Vector3::ONE * .1f, GetFactionColor(m_faction));
	}
}

void Warrior::Attack(IntVector2 targetPos)
{
	m_animator->Play("attack");
	Actor::Attack(targetPos);
	m_waitTime+=30.f;
}

void Warrior::TakeDamage(int damage, float critChance, float blockChance)
{
	m_animator->Play("hit");
	Actor::TakeDamage(damage, critChance, blockChance);
	m_isDefending = false;
}

Menu * Warrior::MakeMenu()
{
	AABB2 UIBox = g_theGame->GetUIBounds();

	Menu* menu = new Menu(UIBox.GetPercentageBox(.35f,.45f,.45f,.55f)); 

	menu->AddOption( "MOVE", (int) ACTOR_ACTION_MOVE ); 
	menu->AddOption( "ATTACK", (int) ACTOR_ACTION_ATTACK ); 
	menu->AddOption( "DEFEND", (int) ACTOR_ACTION_DEFEND);
	menu->AddOption( "WAIT", (int) ACTOR_ACTION_WAIT );

	m_currentMenu = menu;
	return menu;
}

void Warrior::ProcessTurn()
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
	case(ACTOR_ACTION_DEFEND):
		m_isDefending = true;
		m_waitTime+= 10.f;
		m_currentMenu->DisableOption((int) newAction);
		g_theGame->GetEncounterState()->EndActorTurn();
		break;
	}

}

bool Warrior::HandleTurnInputAndCheckCursorMovement()
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
	default:
		m_currentMenu->HandleInput();
		return false;
	}
}

float Warrior::GetBlockChance(Actor * target)
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

Sprite * Warrior::GetDisplaySprite() const
{
	return g_theRenderer->GetSprite("knight_f.portrait");
}
