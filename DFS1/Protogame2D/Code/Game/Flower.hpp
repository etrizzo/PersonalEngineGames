#pragma once
#include "Game/Entity.hpp"

class Flower : public Entity
{
public:
	Flower(Vector2 position, float size);
	~Flower();

private:
	void GenerateFlower();

	void GenerateCenter();
	void GeneratePetals();
	void AddPetal(RGBA color);

	float m_size = 1.f;
	int m_numPetals = 6.f;
};