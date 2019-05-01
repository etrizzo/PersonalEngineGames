#pragma once
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/Renderable.hpp"

class Map;

class Grass
{
public:
	//a tuft of the grass for the map.
	Grass(Vector3 m_position, Map* map);
	~Grass();


	Vector3 m_position;
	Map* m_map = nullptr;
	Renderable* m_renderable = nullptr;

};