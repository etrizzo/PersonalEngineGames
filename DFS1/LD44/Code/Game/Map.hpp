#pragma once
#include "Game/Entity.hpp"

class Grass;

#define WALKABLE_AREA_AS_PERCENTAGE_OF_RADIUS (.5f)
#define CENTER_PATH_RATIO (.07f)

constexpr float GRASS_WIDTH = .7f;
constexpr float GRASS_HEIGHT = .4f;

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
	Material * m_grassMaterial = nullptr;
	MeshBuilder m_grassBuilder;
	//Mesh* m_grassMesh = nullptr;
	Renderable* m_grassRenderable = nullptr;

	void CreateSphereMesh();

	std::vector<float> heights = std::vector<float>();
	IntVector2 dimensions;
	void GenerateBackgroundTerrain();
	Vector3 GetVertexWorldPos(int x, int y) const;

	void AddGrassToSphere();
	Vector3 GetRandomGrassPosition() const;
	void AddGrassAtPosition(const Vector3& position);
	bool IsOnCenterPath(const Vector3& position) const;


	std::vector<Grass*> m_grass;
};



