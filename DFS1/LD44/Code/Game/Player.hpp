#pragma once
#include "Game/Entity.hpp"

class FlowerPot;

class Player :public Entity{
public:
	Player(GameState_Playing* playState, Vector3 position = Vector3::ZERO);
	~Player();

	void Update() override;
	void Render() override;
	void HandleInput();


	
	void TakeDamage() override;
	void Respawn();

	float GetPercentageOfHealth() const;


	float m_health			= PLAYER_MAX_HEALTH;
	float m_speed			= 8.f;
	float m_regenerationSpeed = PLAYER_REGEN_RATE;
	float m_cameraDegPerSeconds = 60.0f;
	StopWatch m_rateOfAttack;
	Transform* m_cameraTarget;
	Transform* m_shadowCameraTransform;
	Vector3 m_shadowCameraOffset;

	//SpriteAnimSet* m_animSet = nullptr;
protected:
	void UpdateAnimation() override;
	void HandleMovementInput();
	void HandleActionInput();
	void TryToLoadCannon();
	void RecoverHealth();
	void BeginAttack() override;
	void ExecuteAttack() override;
	void ExecuteReload() override;

	FlowerPot* m_reloadPot = nullptr;

	//bool m_isMoving = false;
	//bool m_isAttacking = false;

	Vector2 m_positionXY;
	Vector3 m_worldPos;
	void SetWorldPosition();
	void MoveTurretTowardTarget();

	float GetHeightAtCurrentPos();

};