#include "Game/Bullet.hpp"

Bullet::Bullet(Vector2 initialPos, Vector2 initialVelocity, Disc2 outerRadius, Disc2 innerRadius, float initialRotation, float rotateSpeed, int numSides, RGBA color):Entity(initialPos, initialVelocity, outerRadius, innerRadius, initialRotation, rotateSpeed, numSides, color)
{
	float theta = 360.f /  numSides;		//internal angle of the polygon
	for (int i = 0; i < numSides; i++){
		float degrees = (theta * i);
		m_vertices[i] = Vector2(CosDegreesf(degrees), SinDegreesf(degrees));
	}

}

