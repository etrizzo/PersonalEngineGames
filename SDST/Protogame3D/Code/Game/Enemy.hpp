#include "Game/Entity.hpp"

//basic swarm enemy
class Enemy : public Entity{
public:
	Enemy(Vector2 startPos, GameState_Playing* playState);

	Vector2 m_positionXZ;
	Vector2 m_direction;
	float m_speed			= 3.f;
	float m_degPerSecond	= 180.f;

	void Update() override;

private:
	void UpdateDirection();
	void TurnTowardPlayer();
	void SetWorldPosition();
	float GetHeightAtCurrentPos();

	Vector2 GetSeekDirection();
	Vector2 GetSeparateDirection();
};