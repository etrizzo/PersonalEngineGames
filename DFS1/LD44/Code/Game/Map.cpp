#include "Map.hpp"
#include "Game.hpp"
#include "DebugRenderSystem.hpp"
#include "Game/Grass.hpp"

Map::~Map()
{
	for (Grass* grass : m_grass)
	{
		delete grass;
	}
}

Map::Map(Vector3 position, float radius)
{
	//set up the collider?
	m_collider = Sphere(position, radius, 30, 30);

	m_renderable = new Renderable();
	//set up sphere
	MeshBuilder mb = MeshBuilder();
	mb.Begin(PRIMITIVE_TRIANGLES, true);
	mb.AppendSphere(position, radius, 40, 40, RGBA(0, 128, 60));
	mb.End();

	m_renderable->SetMesh(mb.CreateMesh());
	m_renderable->SetMaterial(Material::GetMaterial("default_lit"));
	g_theGame->m_playState->m_scene->AddRenderable(m_renderable);
	//CreateSphereMesh();


	//set up water
	float waterHeight = GetHeightFromXPosition(radius * WALKABLE_AREA_AS_PERCENTAGE_OF_RADIUS);
	Vector3 planeCenter = Vector3(position.x, waterHeight, position.z);
	m_waterRenderable = new Renderable();
	MeshBuilder watermb = MeshBuilder();
	watermb.Begin(PRIMITIVE_TRIANGLES, true);
	RGBA color = RGBA::WHITE.GetColorWithAlpha(200);
	Vector2 planeSize = Vector2(1.0f,1.0f);
	int numPlanes = 80;
	float numPlanesF = (float) numPlanes;
	float halfnumPlanes = numPlanes * .5f;
	for (int x = -numPlanes; x < numPlanes; x++){
		for (int y = -numPlanes; y < numPlanes; y++){
			float xPos = ((float) x * planeSize.x) + (planeSize.x * .5f);
			float yPos = ((float) y * planeSize.y) + (planeSize.y * .5f);

			float xPerc = ((float)x + halfnumPlanes) / numPlanesF;
			float yPerc = ((float)y + halfnumPlanes) / numPlanesF;
			float nextxPerc = ((float)x + 1.f + halfnumPlanes) / numPlanesF;
			float nextyPerc = ((float)y + 1.f + halfnumPlanes) / numPlanesF;

			watermb.AppendPlane(Vector3(xPos, 0.0f, yPos), FORWARD, RIGHT, planeSize, color, Vector2(xPerc, yPerc), Vector2(nextxPerc, nextyPerc));
		}
		//watermb.AppendPlane(Vector3(0.f,0.f,radius), FORWARD, RIGHT, Vector2(-radius * 4.f, radius * 5.f), color, Vector2::ZERO, (Vector2::ONE * 128));
	}
	
	watermb.End();
	m_waterRenderable->SetMesh(watermb.CreateMesh());
	m_waterRenderable->SetMaterial(Material::GetMaterial("water"));
	m_waterRenderable->SetPosition(Vector3(0.f, waterHeight, 0.0f));
	g_theGame->m_playState->m_scene->AddRenderable(m_waterRenderable);

	//set up background
	GenerateBackgroundTerrain();


	AddGrassToSphere();
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

void Map::CreateSphereMesh()
{
	AABB2 uvs = AABB2::ZERO_TO_ONE;
	int wedges = 30.f;
	int slices = 30.f;
	float radius = m_collider.m_radius;
	Vector3 center = m_collider.m_center;
	RGBA grassColor = RGBA(0, 128, 60);
	RGBA pathColor = RGBA::BLANCHEDALMOND;

	MeshBuilder mb;
	mb.Begin(PRIMITIVE_TRIANGLES, true);

	//Vector2 uvSize = Vector2(uvs.GetWidth() / (float) wedges, uvs.GetHeight() / (float) slices);
	Vector2 uvWidth = Vector2(uvs.GetWidth() / (float)wedges, 0.f);
	Vector2 uvHeight = Vector2(0.f, uvs.GetHeight() / (float)slices);
	float xTheta = 360.f / (float)wedges;
	float yTheta = 180.f / (float)slices;
	unsigned int idx;
	for (int y = 0; y < slices; y++) {
		float azimuth = RangeMapFloat((float)y, 0.f, (float)slices, -90.f, 90.f);
		for (int x = 0; x < wedges; x++) {
			float rotation = RangeMapFloat((float)x, 0.f, (float)slices, 0.f, 360.f);

			float nextAzimuth = ClampFloat(azimuth + yTheta, -90.f, 90.f);

			Vector2 bl_uv = (uvWidth * (float)x) + (uvHeight * (float)y);
			Vector3 botLeft = center + SphericalToCartesian(radius, rotation, azimuth);
			//Vector3 bl_tan = Vector3(-cosA * sinR * radius, 0.f, cosA * cosR * radius);
			Vector3 bl_tan = GetSphereTangentForVertex(botLeft, radius, rotation, azimuth);

			Vector2 br_uv = bl_uv + uvWidth;
			Vector3 botRight = center + SphericalToCartesian(radius, rotation + xTheta, azimuth);
			Vector3 br_tan = GetSphereTangentForVertex(botLeft, radius, rotation + xTheta, azimuth);

			Vector2 tl_uv = bl_uv + uvHeight;
			Vector3 topLeft = center + SphericalToCartesian(radius, rotation, nextAzimuth);
			Vector3 tl_tan = GetSphereTangentForVertex(botLeft, radius, rotation, nextAzimuth);

			Vector2 tr_uv = bl_uv + uvWidth + uvHeight;
			Vector3 topRight = center + SphericalToCartesian(radius, rotation + xTheta, nextAzimuth);
			Vector3 tr_tan = GetSphereTangentForVertex(botLeft, radius, rotation + xTheta, nextAzimuth);



			if (IsOnCenterPath(topLeft))
			{
				mb.SetColor(pathColor);
			}
			else {
				mb.SetColor(grassColor);
			}
			mb.SetUV(tl_uv);
			mb.SetNormal((topLeft - center).GetNormalized());
			mb.SetTangent(tl_tan);
			idx = mb.PushVertex(topLeft);

			if (IsOnCenterPath(topRight))
			{
				mb.SetColor(pathColor);
			}
			else {
				mb.SetColor(grassColor);
			}
			mb.SetUV(tr_uv);
			mb.SetNormal((topRight - center).GetNormalized());
			mb.SetTangent(tr_tan);
			mb.PushVertex(topRight);

			if (IsOnCenterPath(botLeft))
			{
				mb.SetColor(pathColor);
			}
			else {
				mb.SetColor(grassColor);
			}
			mb.SetUV(bl_uv);
			mb.SetNormal((botLeft - center).GetNormalized());
			mb.SetTangent(bl_tan);
			mb.PushVertex(botLeft);

			if (IsOnCenterPath(botRight))
			{
				mb.SetColor(pathColor);
			}
			else {
				mb.SetColor(grassColor);
			}
			mb.SetUV(br_uv);
			mb.SetNormal((botRight - center).GetNormalized());
			mb.SetTangent(br_tan);
			mb.PushVertex(botRight);



			mb.AddTriIndices(idx + 0, idx + 1, idx + 2);
			mb.AddTriIndices(idx + 2, idx + 1, idx + 3);
		}
	}
	mb.End();

	m_renderable = new Renderable();
	m_renderable->SetMesh(mb.CreateMesh());
	m_renderable->SetMaterial(Material::GetMaterial("default_lit"));
	g_theGame->m_playState->m_scene->AddRenderable(m_renderable);
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

void Map::AddGrassToSphere()
{
	m_grassBuilder;
	m_grassBuilder.Begin(PRIMITIVE_TRIANGLES, true);
	for (int i = 0; i < 1000; i++)
	{
		Vector3 pos = GetRandomGrassPosition();
		AddGrassAtPosition(pos);
		//Grass* grass = new Grass(pos, this);
		//m_grass.push_back(grass);
	}
	m_grassBuilder.End();

	Material* grassMat = Material::GetMaterial("grass");

	m_grassRenderable = new Renderable();
	m_grassRenderable->SetMesh(m_grassBuilder.CreateMesh());
	m_grassRenderable->SetMaterial(grassMat);

	g_theGame->m_playState->m_scene->AddRenderable(m_grassRenderable);
}

Vector3 Map::GetRandomGrassPosition() const
{
	float minAzimuth = 90.f;
	float maxAzimuth = 124.f;
	float theta = GetRandomFloatInRange(0.f, 360.f);
	float azimuth = GetRandomFloatInRange(minAzimuth, maxAzimuth);
	Vector3 pos = SphericalToCartesian(m_collider.m_radius, theta, azimuth);
	float minZ = m_collider.m_radius * CENTER_PATH_RATIO;

	//push tufts off the central path
	if (pos.z >= 0.f)
	{
		while (pos.z < minZ)
		{
			if (CheckRandomChance(.7f)) {
				float theta = GetRandomFloatInRange(0.f, 360.f);
				float azimuth = GetRandomFloatInRange(minAzimuth, maxAzimuth);
				pos = SphericalToCartesian(m_collider.m_radius, theta, azimuth);
			}
			else
			{
				pos.z = minZ + GetRandomFloatInRange(0.f, .03f);
			}
		}
	}
	else
	{
		while (pos.z > -minZ)
		{
			if (CheckRandomChance(.7f)) {
				float theta = GetRandomFloatInRange(0.f, 360.f);
				float azimuth = GetRandomFloatInRange(minAzimuth, maxAzimuth);
				pos = SphericalToCartesian(m_collider.m_radius, theta, azimuth);
			}
			else
			{
				pos.z = minZ + GetRandomFloatInRange(0.f, .03f);
			}
		}
	}
	return pos;
}

void Map::AddGrassAtPosition(const Vector3 & position)
{
	Vector3 normal = (position - GetCenter()).GetNormalized();
	Vector3 center = position + (normal * GRASS_HEIGHT * .5f);

	Vector3 someRight = Cross(normal, GetRandomNormalizedDirection());
	someRight.NormalizeAndGetLength();
	Vector3 otherRight = Cross(normal, someRight);
	//create renderable at position - two quads pointing up along normal

	m_grassBuilder.AppendPlane(center, normal, someRight, Vector2(GRASS_WIDTH, GRASS_HEIGHT), RGBA::WHITE, Vector2::ZERO, Vector2(1.f, .98f));
	m_grassBuilder.AppendPlane(center, normal, otherRight, Vector2(GRASS_WIDTH, GRASS_HEIGHT), RGBA::WHITE, Vector2::ZERO, Vector2(1.f, .98f));
}


bool Map::IsOnCenterPath(const Vector3 & position) const
{
	float minZ = m_collider.m_radius * CENTER_PATH_RATIO;

	if (position.z > 0.f && position.z < minZ)
	{
		return true;
	}
	if (position.z <= 0.f && position.z > minZ)
	{
		return true;
	}

	return false;
}









