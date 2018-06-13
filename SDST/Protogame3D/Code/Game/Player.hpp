#pragma once
#include "Game/Entity.hpp"

class Player :public Entity{
public:
	Player(Vector3 position = Vector3::ZERO);
	~Player() {};

	void Update() override;


	void HandleInput();

	Vector2 m_positionXZ;
	float m_speed			= 10.f;
	float m_degPerSecond	= 30.f;
	StopWatch m_rateOfFire;
	Transform* m_cameraTarget;
	Renderable* m_turretRenderable;

private:
	void SetWorldPosition();
	void MoveTurretTowardTarget();

	float GetHeightAtCurrentPos();

};