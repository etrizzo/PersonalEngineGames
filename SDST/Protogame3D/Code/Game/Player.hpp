#pragma once
#include "Game/Entity.hpp"

class Player :public Entity{
public:
	Player(GameState_Playing* playState, Vector3 position = Vector3::ZERO);
	~Player() {};

	void Update() override;


	void HandleInput();

	Vector2 m_positionXZ;
	float m_speed			= 5.f;
	float m_degPerSecond	= 60.f;
	StopWatch m_rateOfFire;
	Transform* m_cameraTarget;
	Renderable* m_turretRenderable;
	Renderable* m_laserSightRenderable;
	Renderable* m_targetRenderable;
	Transform* m_barrelPosition;
	Vector3 m_target;

private:
	void SetWorldPosition();
	void MoveTurretTowardTarget();
	void UpdateTarget();

	float GetHeightAtCurrentPos();

};