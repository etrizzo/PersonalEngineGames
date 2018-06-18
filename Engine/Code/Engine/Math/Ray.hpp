#pragma once
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/AABB3.hpp"

struct Ray3D
{
	Ray3D(Vector3 pos, Vector3 dir);
	Vector3 m_position;
	Vector3 m_direction;

	Vector3 Evaluate(float t) const;
};

bool DoesIntersect(Ray3D const& ray, AABB3 const& bounds);

struct Contact3D
{
	Vector3 m_position;
	Vector3 m_normal;
};