#include "Sphere.hpp"
#pragma once

Sphere::Sphere(Vector3 center, float radius, int wedges, int slices)
{
	m_center = center;
	m_radius = radius;
	m_wedges = wedges;
	m_slices = slices;
}
