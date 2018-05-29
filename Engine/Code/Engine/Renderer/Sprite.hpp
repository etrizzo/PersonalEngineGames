#pragma once
#include "Engine/Core/EngineCommon.hpp"

class Sprite
{
public:
	Sprite(){};
	Sprite(const Texture* tex, AABB2 uvs, Vector2 pivot, Vector2 dimensions, Vector2 scale = Vector2::ONE);
	~Sprite(){};

	Sprite* GetScaledSprite(Vector2 scale);
	Plane* GetBounds(const Vector3& pos, const Vector3& right, const Vector3& up);

	const Texture* m_tex	= nullptr;
	AABB2 m_uvs				= AABB2::ZERO_TO_ONE;
	Vector2 m_pivot			= Vector2::HALF;
	Vector2 m_dimensions	= Vector2::ONE;
	Vector2 m_scale			= Vector2::ONE;

};