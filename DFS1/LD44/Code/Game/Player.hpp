#pragma once
#include "Game/Entity.hpp"

constexpr float PLAYER_MAX_HEALTH = 6.f;
constexpr float RELOAD_HEALTH_COST = 2.f;

class Player :public Entity{
public:
	Player(GameState_Playing* playState, Vector3 position = Vector3::ZERO);
	~Player();

	void Update() override;
	void HandleInput();
	
	void Damage();
	void Respawn();

	float GetPercentageOfHealth() const;


	float m_health			= PLAYER_MAX_HEALTH;
	float m_speed			= 5.f;
	float m_regenerationSpeed = .5f;
	float m_cameraDegPerSeconds = 60.0f;
	StopWatch m_rateOfAttack;
	Transform* m_cameraTarget;
	Transform* m_shadowCameraTransform;
	Vector3 m_shadowCameraOffset;

protected:
	void HandleMovementInput();
	void HandleActionInput();
	void TryToLoadCannon();
	void RecoverHealth();

	Vector2 m_positionXY;
	Vector3 m_worldPos;
	void SetWorldPosition();
	void MoveTurretTowardTarget();

	float GetHeightAtCurrentPos();

};