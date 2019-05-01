#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/Mesh.hpp"

const std::string OBJ_DIRECTORY = "Data/Models/";

class MeshBuilder{
public:
	MeshBuilder();
	~MeshBuilder();

	void Clear();
	void ReserveVerts(int numVertsToReserve);

	void SetUV(Vector2 uv);
	void SetColor(RGBA color);
	void SetNormal(Vector3 normal);
	void SetTangent(Vector3 tangent);

	unsigned int PushVertex( Vector3 position);
	unsigned int PushIndex( unsigned int index);


	/*
	What should begin and end do??
	to work, the mesh needs, vertices, indices, and the draw instruction.
	Begin and End is where the draw instructions are coming from.
	*/

	//called before mesh builder does stuff to set up draw instruction
	void Begin(eDrawPrimitiveType type, bool useIndices);


	//gets called when the mesh builder is done being set up to save end index
	void End();

	Mesh* CreateMesh(eVertexType vertType = VERTEX_TYPE_LIT);
	SubMesh* CreateSubMesh(eVertexType vertType = VERTEX_TYPE_LIT);
	
	//LINE
	void AppendLine(Vector3 start, Vector3 end, RGBA startColor, RGBA endColor);

	//PLANE
	void AppendPlane(Vector3 nbl, Vector3 nbr, Vector3 ftl, Vector3 ftr, const RGBA& color, Vector2 uvMins, Vector2 uvMaxs );
	void AppendPlane(const Vector3& center, const Vector3& up, const Vector3& right, const Vector2 & size, const RGBA & color, Vector2 uvMins, Vector2 uvMaxs);
	void AppendPlane2D(const AABB2& plane, const RGBA& color, const AABB2& uvs, float z = 0.f);

	// CUBE
	void AppendCube(Vector3 position, Vector3 size, RGBA color,  const Vector3& right, const Vector3& up, const Vector3& forward, AABB2 UV_TOP = AABB2::ZERO_TO_ONE, AABB2 UV_SIDE = AABB2::ZERO_TO_ONE, AABB2 UV_BOTTOM = AABB2::ZERO_TO_ONE);

	// SPHERE
	void AppendSphere(Vector3 center, float radius, int wedges, int slices, RGBA color, AABB2 uvs = AABB2::ZERO_TO_ONE);


	void AppendCircle(Vector2 center, float radius, int wedges, RGBA color, AABB2 uvs = AABB2::ZERO_TO_ONE);
	
	void AppendTriangle(Vector2 v1, Vector2 v2, Vector2 v3, RGBA color, AABB2 uvs = AABB2::ZERO_TO_ONE);

	//ARRAY OF VERTS
	void AppendVertices(std::vector<Vertex3D_PCU> verts, Transform t);

	void AppendQuad(const Vector3& botLeft, const Vector3& botRight, const Vector3& topLeft, const Vector3& topRight, RGBA color = RGBA::WHITE, AABB2 uvs = AABB2::ZERO_TO_ONE);

	void AddTriIndices(unsigned int idx1, unsigned int idx2, unsigned int idx3);


public:

	std::vector<VertexMaster> m_vertices;			//vertex builder???
	std::vector<unsigned int> m_indices;

	VertexMaster m_stamp;

	DrawInstruction m_drawInstruction;

private:
	// Create mesh functions for each vertex type
	// get a template working #later
	SubMesh* CreateSubMesh3DPCU();
	SubMesh* CreateSubMeshLit();
};



SubMesh* CreateCube( Vector3 position, Vector3 size, RGBA color, const Vector3& right, const Vector3& up, const Vector3& forward, AABB2 UV_TOP = AABB2::ZERO_TO_ONE, AABB2 UV_SIDE = AABB2::ZERO_TO_ONE, AABB2 UV_BOTTOM = AABB2::ZERO_TO_ONE);
SubMesh* CreateSphere(Vector3 position, float radius, int wedges, int slices, RGBA color, AABB2 uvs = AABB2::ZERO_TO_ONE);
SubMesh* CreatePlane(const Vector3& center, const Vector3& up, const Vector3& right, const Vector2 & size, const RGBA & color, Vector2 uvMins, Vector2 uvMaxs);

Mesh* CreateCubeMesh( Vector3 position, Vector3 size, RGBA color, const Vector3& right, const Vector3& up, const Vector3& forward, AABB2 UV_TOP = AABB2::ZERO_TO_ONE, AABB2 UV_SIDE = AABB2::ZERO_TO_ONE, AABB2 UV_BOTTOM = AABB2::ZERO_TO_ONE);
Mesh* CreateSphereMesh(Vector3 position, float radius, int wedges, int slices, RGBA color, AABB2 uvs = AABB2::ZERO_TO_ONE);
Mesh* CreatePlaneMesh(const Vector3& center, const Vector3& up, const Vector3& right, const Vector2 & size, const RGBA & color, Vector2 uvMins, Vector2 uvMaxs);