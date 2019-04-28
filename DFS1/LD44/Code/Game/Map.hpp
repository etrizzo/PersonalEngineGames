#pragma once
#include "Game/Entity.hpp"


#define WALKABLE_AREA_AS_PERCENTAGE_OF_RADIUS (.5f)

class Map{
	

public:
	Map(){}
	~Map();
	Map(Vector3 position, float radius);

	std::string m_name;

	Sphere m_collider;

	void Render();

	void Update(float deltaSeconds);

	float GetRadius() const;
	Vector3 GetCenter() const;

	float GetHeightFromXPosition(float xCoord) const;
	Vector3 GetPositionAtXCoord(float x) const;

	bool Raycast( Contact3D& contact, int maxHits, const Ray3D& ray, float maxDistance = 1000.f);
	bool IsPointAboveTerrain(const Vector3& point) const;
	bool HitRaycastTarget(const Vector3& point) const;
	float GetVerticalDistanceFromTerrain(const Vector3& point) const;

	float GetWalkableRadius() const;

	Renderable* m_renderable;
	Renderable* m_waterRenderable;
	Renderable* m_background;

private:
	std::vector<float> heights = std::vector<float>();
	IntVector2 dimensions;
	void GenerateBackgroundTerrain();
	Vector3 GetVertexWorldPos(int x, int y) const;

};



