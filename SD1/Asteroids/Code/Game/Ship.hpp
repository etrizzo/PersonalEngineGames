#pragma once
#include "Game/Entity.hpp"

#define MAXIMUM_THRUST 300.f
#define STARBURST_COOLDOWN 5.f

class Ship: public Entity{
public:
	~Ship() {}
	Ship();
	Ship(Vector2 initialPos, Vector2 initialVelocity, Disc2 outerRadius, Disc2 innerRadius, float initialRotation, float rotateSpeed, int numSides = 5);

	Vector2 m_thrustVertices[3];

	bool m_alive;
	int m_turnDir;		//left -1, right 1, no turn 0
	float m_thrustRate;
	Vector2 m_facing;
	int m_controllerID;		//id  of the controller that controls this ship

	bool m_firingStarburst;
	float m_starburstStartDegrees;
	float m_starburstStartedTime;
	float m_starburstCharge;



	void Render(bool devMode);
	void RenderThrust();
	void RenderStarburstCooldown();
	void RenderStarburstCharge();
	void RenderCooldown(float percentage, Vector2 pos, float rotation, float scale, RGBA color );

	void Update(float deltaSeconds);

	void CheckXboxController();

	bool IsTurningLeft();
	bool IsTurningRight();
	bool IsThrusting();
	bool IsThrustingKeyboard();
	bool CanFireStarburst();

	void ChargeStarburst();
	void FireStarburst();

};