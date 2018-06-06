#pragma once
#include "Game/Entity.hpp"

class Player :public Entity{
public:
	Player(Vector3 position = Vector3::ZERO);
	~Player() {};

	void Update() override;


	void HandleInput();

	Vector3 m_positionXZ;
	float m_speed			= 10.f;
	float m_degPerSecond	= 30.f;
	StopWatch m_rateOfFire;

private:
	void SetWorldPosition();

	float GetHeightAtCurrentPos();

};