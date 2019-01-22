#pragma once
#include "Game/GameCommon.hpp"
#include "Game/Actor.hpp"

class Encounter;
class Menu;
class Projectile;

enum EncounterStateType{
	ENCOUNTER_STATE_DEFAULT,
	ENCOUNTER_STATE_PROCESSTURN,
	ENCOUNTER_STATE_VICTORY,
	NUM_ENCOUNTER_STATES
};

enum ActionType{
	ACTION_WALK_TO_TILE,
	ACTION_FIRE_PROJECTILE,
	ACTION_PLAY_ATTACK_ANIM,		//will i need a separate enum for each kind of attack? probs not right?
	NUM_ACTIONS
};



class EncounterState{
public:
	EncounterState();
	~EncounterState(){};

	Encounter* Enter(std::string mapFile);

	void Update(float deltaSeconds);
	void Render();
	void RenderUI();
	void HandleInput();

	Actor* GetCurrentActor() { return m_currentActor; };
	IntVector2 GetCurrentActorPos() {return m_currentActor->m_position; };

	IntVector3 GetMapDimensions();
	Vector3 GetCursorPos();
	IntVector2 GetCursorPosXY();
	void MoveCursor(const IntVector2& direction);
	void SetCursorPos(IntVector2 newPos);

	bool IsCursorInSelectableArea() const { return IsPositionInSelectableArea(m_cursorPos); }

	void EndActorTurn();

	void AddAction(ActionType newAction);
	bool HasActions() const;
	void FinishAction();

	//sets current projectile and begins projectile action
	void AddProjectile(IntVector2 launchPos, IntVector2 targetPos, int damage, float critChance, float blockChance, float maxRange, std::string type = "arrow", int effectRange = 1, int verticalRange = 1, float cosmeticOffset = .5f, float gravity = 9.8f);
	void AddProjectile(Projectile* proj);


	Encounter* m_encounter			  = nullptr;
	EncounterStateType m_currentState = ENCOUNTER_STATE_DEFAULT;
	//Menu* m_currentMenu;


private:
	void UpdateDefault(float ds);
	void UpdateProcessTurn(float ds);
	void UpdateActions(float ds);
	void UpdateVictory(float ds);

	void RenderDefault();
	void RenderProcessTurn();
	void RenderProcessTurnUI();
	void RenderVictory();
	void RenderCursor();

	void HandleInputDefault();
	void HandleInputProcessTurn();
	void HandleInputVictory();
	void HandleCursorArrowInput();	
	void HandleCameraInput();

	void StartActorTurn();



	void TurnUpdate();

	bool IsPositionInSelectableArea(IntVector2 pos) const;

	IntVector2 FindNewNorth();
	IntVector2 MapDirectionToNorth(IntVector2 direction, IntVector2 newNorth);

	Actor* m_currentActor = nullptr;
	IntVector2 m_cursorPos = IntVector2(0,5);

	std::queue<ActionType> m_actions = std::queue<ActionType>();

	//EncounterAction m_currentAction = INVALID_ACTION;

	Projectile* m_currentProjectile = nullptr;
};