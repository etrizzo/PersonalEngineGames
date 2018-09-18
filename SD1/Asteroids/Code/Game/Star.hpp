#pragma once
#include "Game/GameCommon.hpp"
#include "Game/Entity.hpp"

class Star : public Entity{
public:
	Star(){};
	Star(Vector2 pos, RGBA color = RGBA::WHITE, float size = 10.f, float duration = 1.f, float initialRotation = 0.f, float rotateSpeed = 90.f);
	~Star(){};

	void Update(float deltaSeconds);
	void Render();

	void FillVerts();

	float m_duration = 1.f;
	//float m_scale = 1.f;
	float m_size = 10.f;
	Transform m_transform;
	RGBA m_currentColor = RGBA::WHITE;
	bool m_aboutToBeDeleted = false;

	std::vector<Vertex3D_PCU> m_vertices = std::vector<Vertex3D_PCU>();

};