#include "Raycast3.hpp"

Ray3D::Ray3D(Vector3 pos, Vector3 dir)
{
	m_position = pos;
	m_direction = dir;
}

Vector3 Ray3D::Evaluate(float t)
{
	return (m_position + (m_direction * t));
}
