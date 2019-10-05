#pragma once
#include "Game/Entity.hpp"

class Player :public Entity{
public:
	Player(Vector2 position);
	~Player() {};

	void Update() override;


	void HandleInput();

	Vector2 m_position;
	float m_speed			= 10.f;
	float m_degPerSecond	= 30.f;
	StopWatch m_rateOfFire;

private:


};