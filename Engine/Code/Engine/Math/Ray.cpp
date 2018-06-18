#include "Engine/Math/Ray.hpp"
#include <vector>

Ray3D::Ray3D(Vector3 pos, Vector3 dir)
{
	m_position = pos;
	m_direction = dir;
}

Vector3 Ray3D::Evaluate(float t) const
{
	return (m_position + (m_direction * t));
}

bool DoesIntersect(Ray3D const & ray, AABB3 const & bounds)
{
	// will use mins/maxs option for AABB3
	// Voronoi regions - but now with 27 options instead of 9
	// Corner cases are the weird ones
	Vector3 min = bounds.mins;
	Vector3 max = bounds.maxs;

	float txmin = (min.x - ray.m_position.x) / ray.m_direction.x; 
	float txmax = (max.x - ray.m_position.x) / ray.m_direction.x; 

	if (txmin > txmax){
		std::swap(txmin, txmax);
	} 

	float tymin = (min.y - ray.m_position.y) / ray.m_direction.y; 
	float tymax = (max.y - ray.m_position.y) / ray.m_direction.y; 

	if (tymin > tymax) {
		std::swap(tymin, tymax); 
	}

	if ((txmin > tymax) || (tymin > txmax)) 
		return false; 

	if (tymin > txmin) 
		txmin = tymin; 

	if (tymax < txmax) 
		txmax = tymax; 

	float tzmin = (min.z - ray.m_position.z) / ray.m_direction.z; 
	float tzmax = (max.z - ray.m_position.z) / ray.m_direction.z; 

	if (tzmin > tzmax){
		std::swap(tzmin, tzmax);
	}

	if ((txmin > tzmax) || (tzmin > txmax)) 
		return false; 

	if (tzmin > txmin) 
		txmin = tzmin; 

	if (tzmax < txmax) 
		txmax = tzmax; 

	return true; 
}
