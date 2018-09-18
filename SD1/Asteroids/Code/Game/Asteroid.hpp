#pragma once
#include "Game/Entity.hpp"





class Asteroid: public Entity{
public:

	Asteroid(Vector2 initialPos,  Vector2 initialVelocity, Disc2 outerRadius, Disc2 innerRadius, float initialRotation, float rotateSpeed, int numSides, int asteroidSize);

	int m_size;		//2 = happy, 1 = sad, 0 = angry

	void DrawAsteroid();
	void Render(bool devMode);

	void FunkUpVertices();

	void RenderFace();
	void RenderEye(Vector2 pos);
	void RenderSmile();
	void RenderExtra();
	void RenderBlush();
	void RenderTears(Vector2 pos);
	void RenderEyebrows();


};
