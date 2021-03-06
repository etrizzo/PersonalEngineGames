#pragma once
#include "Game/Entity.hpp"
#include "Game/RaycastResult.hpp"

class Player :public Entity{
public:
	Player(GameState_Playing* playState, Vector3 position = Vector3::ZERO);
	~Player();

	void Update() override;
	void Render() override;
	void HandleInput();
	
	void Damage();
	void Respawn();
	void RemoveRenderable();
	void AddRenderable();

	float GetPercentageOfHealth() const;

	uchar m_currentPlaceBlockType = 1;

	Vector2 m_positionXZ;
	float m_speed				= 5.f;
	float m_controlMultiplier		= 6.f;
	float m_degPerSecond	= 60.f;

	float m_digDistance = 8.f;

	RaycastResult m_digRaycast;

	//Transform* m_cameraTarget;

private:

	void HandleInputNoClip();
	void HandleInputWalking();
	void HandleInputFlying();

	void HandleRotationInputMouse();

	void HandleInputDigPlace();
};