#include "Sphere.hpp"
#pragma once

Sphere::Sphere(Vector3 center, float radius, int wedges, int slices)
{
	m_center = center;
	m_radius = radius;
	m_wedges = wedges;
	m_slices = slices;

	
	
}

void Sphere::SetPosition(const Vector3 & pos)
{
	m_center = pos;
}

bool Sphere::IsPointInside(const Vector3 & point) const
{
	Vector3 displacement = m_center - point;
	if (displacement.GetLengthSquared() > (m_radius * m_radius)){
		return false;
	} else {
		return true;
	}
}

bool Sphere::DoSpheresOverlap(const Sphere & sphere) const
{
	Vector3 displacement = m_center - sphere.m_center;
	float sumRadii = m_radius + sphere.m_radius;
	//check distance to centers against sum of the radii
	if (displacement.GetLengthSquared() > (sumRadii * sumRadii)){
		return false;
	} else {
		return true;
	}
}
