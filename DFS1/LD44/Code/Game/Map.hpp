#pragma once
#include "Game/Entity.hpp"
#include "Game/MapDefinition.hpp"

#define WALKABLE_AREA_AS_PERCENTAGE_OF_RADIUS (.7f)

class Map{
	

public:
	Map(){}
	~Map();
	Map(Vector3 position, float radius);

	std::string m_name;

	AABB2 m_extents;
	IntVector2 m_dimensions;
	Sphere m_collider;

	void Render();

	void Update(float deltaSeconds);

	void CheckEntityInteractions();

	int GetWidth() const;
	int GetHeight() const;

	float GetHeightFromXPosition(float xCoord) const;
	Vector3 GetPositionAtXCoord(float x) const;

	bool Raycast( Contact3D& contact, int maxHits, const Ray3D& ray, float maxDistance = 1000.f);
	bool IsPointAboveTerrain(const Vector3& point) const;
	bool HitRaycastTarget(const Vector3& point) const;
	float GetVerticalDistanceFromTerrain(const Vector3& point) const;

	Renderable* m_renderable;
	Renderable* m_waterRenderable;

private:

};



