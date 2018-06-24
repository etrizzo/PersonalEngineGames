#pragma once
#include "Game/Entity.hpp"


class Bullet : public Entity{
public:
	Bullet(const Transform& t, GameState_Playing* playState);
	~Bullet();

	void Update() override;
	void CheckPhysics();
	//checks for collision with an entity
	//void CheckForCollision(Enemy* e);


	float m_lifeTime = 2.f;
	float m_speed = 25.f;

	RGBA m_tint = RGBA(255,200,100, 240);
	Light* m_light;
private:
	bool IsAboveTerrain();
	void Destroy();

};