#pragma once
#include "Game/GameCommon.hpp"

struct Ray3D
{
	Ray3D(Vector3 pos, Vector3 dir);
	Vector3 m_position;
	Vector3 m_direction;

	Vector3 Evaluate(float t);
};


struct Contact3D
{
	Vector3 m_position;
	Vector3 m_normal;
};