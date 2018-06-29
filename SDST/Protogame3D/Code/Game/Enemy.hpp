#include "Game/Entity.hpp"

//basic swarm enemy
class Enemy : public Entity{
public:
	Enemy(Vector2 startPos, GameState_Playing* playState);
	~Enemy();

	Vector2 m_positionXZ;
	Vector2 m_direction;
	float m_speed			= 3.f;
	float m_degPerSecond	= 180.f;
	

	void Update() override;
	void RunEntityPhysics() override;

	void Damage();
	bool IsPointInside(const Vector3& point) const;

private:
	void CheckForPlayerCollision();
	void UpdateDirection();
	void TurnTowardPlayer();
	void SetWorldPosition();
	float GetHeightAtCurrentPos();

	Vector2 GetSeekDirection();
	Vector2 GetSeparateDirection();
	Vector2 GetCohesionDirection();
	Vector2 GetAlignmentDirection();
};