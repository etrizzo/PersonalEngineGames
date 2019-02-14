#pragma once
#include "Game/Entity.hpp"

class Flower : public Entity
{
public:
	Flower(Vector2 position, float size);
	~Flower();
	void GenerateFlower();

private:
	

	void GenerateCenter();
	void GeneratePetals();
	void AddPetal(RGBA baseColor, Vector2 forward);

	float m_size = 1.f;
	int m_numPetals = 24;
	int m_maxPetalDepth = 10;
	Vector2 m_center;

	MeshBuilder m_cpuMesh;
	int m_centerWedges = 15;
	RGBA m_petalColor = RGBA(255, 204, 0);
	RGBA m_darkPetalColor;
	RGBA m_centerColor = RGBA(97,19,9);
};