#include "Flower.hpp"

Flower::Flower(Vector2 position, float size)
{
	m_renderable = new Renderable2D();
	m_renderable->SetPosition(position);
	m_size = size;
}

Flower::~Flower()
{
}

void Flower::GenerateFlower()
{
	GeneratePetals();
	GenerateCenter();
}

void Flower::GenerateCenter()
{
}

void Flower::GeneratePetals()
{
}

void Flower::AddPetal(RGBA color)
{
}
