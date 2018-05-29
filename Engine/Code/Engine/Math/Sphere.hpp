#pragma once
#include "Engine/Math/Vector3.hpp"

class Sphere{
public:
	Sphere(){};
	Sphere(Vector3 center, float radius, int wedges, int slices);
	Vector3 m_center;
	float m_radius;
	int m_wedges;
	int m_slices;
};