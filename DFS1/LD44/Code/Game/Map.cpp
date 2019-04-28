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
	watermb.AppendPlane(Vector3(0.f,0.f,radius), FORWARD, RIGHT, Vector2(-radius * 4.f, radius * 5.f), RGBA::WHITE.GetColorWithAlpha(200), Vector2::ZERO, (Vector2::ONE * 128));
	watermb.End();
	m_waterRenderable->SetMesh(watermb.CreateMesh());
	m_waterRenderable->SetMaterial(Material::GetMaterial("water"));
	m_waterRenderable->SetPosition(Vector3(0.f, waterHeight, 0.0f));
	g_theGame->m_playState->m_scene->AddRenderable(m_waterRenderable);

	//set up background
	GenerateBackgroundTerrain();
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
	if (fabs(xCoord) > (m_collider.m_radius * WALKABLE_AREA_AS_PERCENTAGE_OF_RADIUS))
	{
		//we're off the orbb
		return m_waterRenderable->GetPosition().y;
	}
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

float Map::GetWalkableRadius() const
{
	return m_collider.m_radius * WALKABLE_AREA_AS_PERCENTAGE_OF_RADIUS;
}

void Map::GenerateBackgroundTerrain()
{

	float x = m_collider.m_radius * 2.f;
	float y = m_collider.m_radius;
	dimensions = IntVector2((int)x * 2.f, (int)y * 2.f);
	float radius = m_collider.m_radius;
	float tileSize = 1.f;

	//get the heights/normals
	heights.resize(dimensions.x * dimensions.y);
	//read vertex heights from image data
	for (int x = 0; x < dimensions.x; x++) {
		for (int y = 0; y < dimensions.y; y++) {
			//Vector2 xzPos = Vector2(x * tileSize, y * tileSize);
			float height = Compute2dPerlinNoise(x, y, 20.f, 1);
			int idx = GetIndexFromCoordinates(x, y, dimensions.x, dimensions.y);
			heights[idx] = RangeMapFloat(height,-1.f, 1.f, 0.f, radius * .8f);
		}
	}




	MeshBuilder mb = MeshBuilder();
	Material* terrainMat = Material::GetMaterial("default_lit");
	//generate mesh
	mb.Clear();
	mb.Begin(PRIMITIVE_TRIANGLES, true);

	for (int x = 0; x < dimensions.x; x++) {
		for (int y = 0; y < dimensions.y; y++) {
			//for each vertex, append plane (as bottom right)


			Vector3 blPos = GetVertexWorldPos(x, y);
			Vector3 brPos = GetVertexWorldPos(x + 1, y);
			Vector3 tlPos = GetVertexWorldPos(x, y + 1);
			Vector3 trPos = GetVertexWorldPos(x + 1, y + 1);

			Vector3 right = (brPos - blPos).GetNormalized();
			Vector3 up = (tlPos - blPos).GetNormalized();
			int idx = GetIndexFromCoordinates(x, y, dimensions.x, dimensions.y);
			if ((x + 1) < dimensions.x && (y + 1) < dimensions.y) {
				mb.AppendPlane(blPos, brPos, tlPos, trPos, RGBA::GREEN, Vector2::ZERO, Vector2::ONE);
			}
		}
	}
	mb.End();
	m_background = new Renderable();
	m_background->SetMesh(mb.CreateMesh());
	m_background->SetMaterial(terrainMat);

	m_background->m_transform.TranslateLocal(Vector3(0.f, radius * .1f, radius * .7f));
	m_background->m_transform.RotateByEuler(0.f, 15.f, 0.f);
	g_theGame->m_playState->m_scene->AddRenderable(m_background);
}

Vector3 Map::GetVertexWorldPos(int x, int y) const
{
	Vector2 xyPos =(Vector2( x - (dimensions.x * .5f),  y));
	//float height = GetHeightForVertex(x, y);
	float height = 0.f;
	int index = GetIndexFromCoordinates(x, y, dimensions.x, dimensions.y);
	if (index >= 0) {
		height = heights[index];
	}

	return Vector3(xyPos.x, height, xyPos.y);
}









