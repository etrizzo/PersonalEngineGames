#pragma once
#include "Grass.hpp"
#include "Game/Map.hpp"
#include "Game/Game.hpp"



Grass::Grass(Vector3 m_position, Map * map)
{
	Vector3 normal = (m_position - map->GetCenter()).GetNormalized();
	Vector3 center = m_position + (normal * GRASS_HEIGHT * .5f);

	Vector3 someRight = Cross(normal, GetRandomNormalizedDirection());
	someRight.NormalizeAndGetLength();
	Vector3 otherRight = Cross(normal, someRight);
	//create renderable at position - two quads pointing up along normal

	MeshBuilder mb;
	mb.Begin(PRIMITIVE_TRIANGLES, true);

	mb.AppendPlane(center, normal, someRight, Vector2(GRASS_WIDTH, GRASS_HEIGHT), RGBA::WHITE, Vector2::ZERO, Vector2::ONE);
	mb.AppendPlane(center, normal, otherRight, Vector2(GRASS_WIDTH, GRASS_HEIGHT), RGBA::WHITE, Vector2::ZERO, Vector2::ONE);

	mb.End();

	Material* grassMat = Material::GetMaterial("grass");

	m_renderable = new Renderable();
	m_renderable->SetMesh(mb.CreateMesh());
	m_renderable->SetMaterial(grassMat);

	g_theGame->m_playState->m_scene->AddRenderable(m_renderable);
}

Grass::~Grass()
{
	g_theGame->m_playState->m_scene->RemoveRenderable(m_renderable);
	delete m_renderable;
}
