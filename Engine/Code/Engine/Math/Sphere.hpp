#pragma once
#include "Engine/Math/Vector3.hpp"

class Sphere{
public:
	Sphere(){};
	Sphere(Vector3 center, float radius, int wedges = 10, int slices = 10);
	Vector3 m_center;
	float m_radius;
	int m_wedges;
	int m_slices;

	void SetPosition(const Vector3& pos);
	bool IsPointInside(const Vector3& point) const;
	bool DoSpheresOverlap(const Sphere& sphere) const;
};