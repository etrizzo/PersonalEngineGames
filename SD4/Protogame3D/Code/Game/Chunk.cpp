#include "Game/Chunk.hpp"

void Chunk::Setup(Map * map, IntVector2 chunkCoords)
{
	m_chunkIndex = chunkCoords;
	m_map = map;
	m_renderable = new Renderable();
}

void Chunk::SetMesh(Mesh * mesh)
{
	m_renderable->SetMesh(mesh);
}

void Chunk::SetMaterial(Material * mat)
{
	m_renderable->SetMaterial(mat);
}
