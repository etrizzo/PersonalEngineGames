#include "Map.hpp"
#include "Game.hpp"
#include "DebugRenderSystem.hpp"


Map::~Map()
{

}

Map::Map(Vector3 position, float radius)
{
	//set up the collider?
	m_collider = Sphere(position, radius, 30, 30);

	m_renderable = new Renderable();
	//set up sphere
	MeshBuilder mb = MeshBuilder();
	mb.Begin(PRIMITIVE_TRIANGLES, true);
	mb.AppendSphere(position, radius, 20, 20, RGBA::WHITE);
	mb.End();

	m_renderable->SetMesh(mb.CreateMesh());
	m_renderable->SetMaterial(Material::GetMaterial("terrain_grass"));
	g_theGame->m_playState->m_scene->AddRenderable(m_renderable);



	//set up water
	float waterHeight = GetHeightFromXPosition(radius * WALKABLE_AREA_AS_PERCENTAGE_OF_RADIUS);
	Vector3 planeCenter = Vector3(position.x, waterHeight, position.z);
	m_waterRenderable = new Renderable();
	MeshBuilder watermb = MeshBuilder();
	watermb.Begin(PRIMITIVE_TRIANGLES, true);
	watermb.AppendPlane(planeCenter, FORWARD, RIGHT, Vector2(-100.f, 100.f), RGBA::WHITE.GetColorWithAlpha(200), Vector2::ZERO, (Vector2::ONE * 128));
	watermb.End();
	m_waterRenderable->SetMesh(watermb.CreateMesh());
	m_waterRenderable->SetMaterial(Material::GetMaterial("water"));
	g_theGame->m_playState->m_scene->AddRenderable(m_waterRenderable);


}



void Map::Render()
{
	//RenderTiles();
	//RenderEntities();
	
}

void Map::Update(float deltaSeconds)
{
	UNUSED(deltaSeconds);
	//UpdateEntities(deltaSeconds);

	//RunPhysics();


}

float Map::GetRadius() const
{
	return m_collider.m_radius;
}



Vector3 Map::GetCenter() const
{
	return m_collider.m_center;
}

float Map::GetHeightFromXPosition(float xCoord) const
{
	float xSquared = (xCoord - m_collider.m_center.x) * (xCoord - m_collider.m_center.x);
	float rSquared = m_collider.m_radius * m_collider.m_radius;

	float ySquared = rSquared - xSquared;

	float yMinusOffset = sqrt(ySquared);

	float y = yMinusOffset + m_collider.m_center.y;
	return y;
}

Vector3 Map::GetPositionAtXCoord(float x) const
{
	return Vector3(x, GetHeightFromXPosition(x), m_collider.m_center.z);
}

bool Map::Raycast(Contact3D & contact, int maxHits, const Ray3D & ray, float maxDistance)
{
	UNIMPLEMENTED();
	TODO("This.");
	return false;
}

bool Map::IsPointAboveTerrain(const Vector3 & point) const
{
	float terrainHeight = GetHeightFromXPosition(point.x);

	if (terrainHeight <= point.y){
		return true;
	} else {
		return false;
	}
}



bool Map::HitRaycastTarget(const Vector3 & point) const
{
	bool inTerrain = !IsPointAboveTerrain(point);
	return inTerrain;
}

float Map::GetVerticalDistanceFromTerrain(const Vector3 & point) const
{
	float terrainHeight = GetHeightFromXPosition(point.x);
	float dist = point.y - terrainHeight;
	return fabs(dist);
}








