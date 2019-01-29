#pragma once
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/Renderable.hpp"
//#include "Game/Map.hpp"

class Map;

class Chunk{
public:
	void Setup(Map* map, IntVector2 chunkCoords);
	void SetMesh(Mesh* mesh);
	void SetMaterial(Material* mat);

	IntVector2 m_chunkIndex = IntVector2::INVALID_INDEX;
	Map* m_map;
	Renderable* m_renderable = nullptr;
};