#pragma once
#include "Game/GameCommon.hpp"
#include "Game/Entity.hpp"
#include "Game/Encounter.hpp"
#include "Game/Menu.hpp"


const float MIN_WAIT_TIME = 30.f;
const float MAX_WAIT_TIME = 100.f;

enum ActorAction{
	INVALID_ACTION = -1,
	ACTOR_ACTION_MOVE,
	ACTOR_ACTION_WAIT,
	ACTOR_ACTION_ATTACK,
	ACTOR_ACTION_DEFEND,
	ACTOR_ACTION_BOW,
	ACTOR_ACTION_FIREBALL,
	ACTOR_ACTION_HEAL,
	NUM_ACTOR_ACTIONS
};

class Animator;

class Actor : public Entity{
public:
	Actor(){};
	Actor(IntVector2 initialPosition, Map* entityMap, Faction faction);
	~Actor() {};

	virtual void Update(float deltaSeconds);
	void UpdateWalkToTile(float deltaSeconds);
	virtual void Render();
	
	void RenderSelectableArea();
	virtual void RenderHoverData(Actor* target);
	virtual void RenderProcessTurn();

	//later this will be pulled from a definition, so for now just sticking with default values
	Faction m_faction	;
	int m_health		;
	int m_maxHealth		;
	int m_moveSpeed		;
	int m_jumpHeight	;
	IntRange m_strength	= IntRange(0);
	int m_height = 2;
	float m_waitTime	= MAX_WAIT_TIME;
	float m_actionSpeed	= 40.f;
	int m_turnsSinceLastMove = 0;
	Vector2 m_forwardDirection	 = DIRECTION_NORTH;

	int m_pathIndex = 0;
	std::vector<IntVector2> m_pathToDestination = std::vector<IntVector2>();
	IntVector2 m_destinationCoords;

	Heatmap* m_distanceMap = new Heatmap();
	Heatmap* m_selectableArea = new Heatmap();			//updates for both move and attack for easy checking for the selection cursor

	Menu* m_currentMenu = nullptr;
	ActorAction m_currentAction = INVALID_ACTION;

	Animator* m_animator;


	void Wait();
	virtual void Move(IntVector2 m_newPos);
	virtual void Attack(IntVector2 m_posToAttack);

	virtual void TakeDamage(int damage, float critChance = 0.f, float blockChance = 0.f);
	bool IsAlive() { return m_health > 0; };

	void SetDistanceMap();
	virtual void SetMoveArea();
	virtual void SetAttackArea();

	virtual Menu* MakeMenu() { return nullptr; }
	virtual void ProcessTurn();
	virtual bool HandleTurnInputAndCheckCursorMovement();

	Vector2 GetForwardDirection() const;

	void PassTime(float waitTime);
	Vector3 GetBlockPosition();
	Vector3 GetWorldPosition();
	int GetHealth() { return m_health; };
	int GetFactionInt() { return (int) m_faction; }
	Faction GetFaction() { return m_faction; }
	float GetWaitTime() { return m_waitTime; }
	IntVector2 GetPosition() { return m_position; }

	virtual Sprite* GetDisplaySprite() const = 0;

	virtual float GetCritChance(Actor* target);
	virtual float GetBlockChance(Actor* target);
	virtual bool IsDefending() const {return false; }

protected:
	void RenderDistanceMap();
	
};



RGBA GetFactionColor(Faction faction);

