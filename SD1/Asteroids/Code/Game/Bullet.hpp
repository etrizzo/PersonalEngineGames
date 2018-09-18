#pragma once
#include "Game/Entity.hpp"


class Bullet: public Entity{
public:
	~Bullet() {}
	Bullet() {}
	Bullet(Vector2 initialPos,  Vector2 initialVelocity, Disc2 outerRadius, Disc2 innerRadius, float initialRotation = 0.f, float rotateSpeed = 0.f, int numSides = 20, RGBA color = RGBA());

};