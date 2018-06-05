#include "MeshBuilder.hpp"


SubMesh * CreateCube(Vector3 position, Vector3 size, RGBA color, AABB2 UV_TOP, AABB2 UV_SIDE, AABB2 UV_BOTTOM )
{
	MeshBuilder mb;
	mb.Begin( PRIMITIVE_TRIANGLES, true);
	mb.AppendCube(position, size, color, UV_TOP, UV_SIDE, UV_BOTTOM); 		//could put all of this on the mesh builder, so that you can add a cube to the mesh builder and then add more to it.
	mb.End();
	return mb.CreateSubMesh(VERTEX_TYPE_LIT);
}

SubMesh * CreateSphere(Vector3 position, float radius, int wedges, int slices, RGBA color, AABB2 uvs)
{
	MeshBuilder mb;
	mb.Begin( PRIMITIVE_TRIANGLES, true);
	mb.AppendSphere( position, radius, wedges, slices, color, uvs);
	mb.End();
	return mb.CreateSubMesh(VERTEX_TYPE_LIT);
}

SubMesh * CreatePlane(const Vector3& center, const Vector3& up, const Vector3& right, const Vector2 & size, const RGBA & color, Vector2 uvMins, Vector2 uvMaxs)
{
	MeshBuilder mb;
	mb.Begin( PRIMITIVE_TRIANGLES, true);
	mb.AppendPlane(center, up, right, size, color, uvMins, uvMaxs);
	mb.End();
	return mb.CreateSubMesh(VERTEX_TYPE_LIT);
}

Mesh * CreateCubeMesh(Vector3 position, Vector3 size, RGBA color, AABB2 UV_TOP, AABB2 UV_SIDE, AABB2 UV_BOTTOM)
{
	MeshBuilder mb;
	mb.Begin( PRIMITIVE_TRIANGLES, true);
	mb.AppendCube(position, size, color, UV_TOP, UV_SIDE, UV_BOTTOM); 		//could put all of this on the mesh builder, so that you can add a cube to the mesh builder and then add more to it.
	mb.End();
	return mb.CreateMesh(VERTEX_TYPE_LIT);
}

Mesh * CreateSphereMesh(Vector3 position, float radius, int wedges, int slices, RGBA color, AABB2 uvs)
{
	MeshBuilder mb;
	mb.Begin( PRIMITIVE_TRIANGLES, true);
	mb.AppendSphere( position, radius, wedges, slices, color, uvs);
	mb.End();
	return mb.CreateMesh(VERTEX_TYPE_LIT);
}

Mesh * CreatePlaneMesh(const Vector3 & center, const Vector3 & up, const Vector3 & right, const Vector2 & size, const RGBA & color, Vector2 uvMins, Vector2 uvMaxs)
{
	MeshBuilder mb;
	mb.Begin( PRIMITIVE_TRIANGLES, true);
	mb.AppendPlane(center, up, right, size, color, uvMins, uvMaxs);
	mb.End();
	return mb.CreateMesh(VERTEX_TYPE_LIT);
}

MeshBuilder::MeshBuilder()
{
	m_vertices = std::vector<VertexMaster>();	
	m_indices = std::vector<unsigned int>();
}

MeshBuilder::~MeshBuilder()
{
	//delete m_vertices.data();
	//delete m_indices.data();
}

void MeshBuilder::Clear()
{
	m_vertices.clear();
	m_indices.clear();
}

void MeshBuilder::SetUV(Vector2 uv)
{
	m_stamp.m_uvs = uv;
}

void MeshBuilder::SetColor(RGBA color)
{
	m_stamp.m_color = color;
}

void MeshBuilder::SetNormal(Vector3 normal)
{
	m_stamp.m_normal = normal;
}

void MeshBuilder::SetTangent(Vector3 tangent)
{
	m_stamp.m_tangent = Vector4(tangent, 1.f);
}

unsigned int MeshBuilder::PushVertex(Vector3 position)
{
	m_stamp.m_position = position;
	m_vertices.push_back(m_stamp);

	return m_vertices.size() - 1;
}

unsigned int MeshBuilder::PushIndex(unsigned int index)
{
	m_indices.push_back(index);

	return m_indices.size() - 1;
}

void MeshBuilder::Begin(eDrawPrimitiveType type, bool useIndices)
{
	m_drawInstruction.m_useIndices = useIndices;
	m_drawInstruction.m_primitive = type;
	if (useIndices){
		m_drawInstruction.m_start_index = m_indices.size();
	} else {
		m_drawInstruction.m_start_index = m_vertices.size();
	}
}

void MeshBuilder::End()
{
	unsigned int endIndex;
	if (m_drawInstruction.m_useIndices){
		endIndex = m_indices.size();
	} else {
		endIndex = m_vertices.size();
	}
	m_drawInstruction.m_elem_count = endIndex;
}

Mesh * MeshBuilder::CreateMesh(eVertexType vertType)
{
	Mesh* mesh = new Mesh();
	mesh->AddSubMesh(CreateSubMesh(vertType));
	return mesh;
}

SubMesh * MeshBuilder::CreateSubMesh(eVertexType vertType)
{
	SubMesh* m = nullptr;
	switch(vertType){
	case VERTEX_TYPE_3DPCU:
		m = CreateSubMesh3DPCU();
		break;
	case VERTEX_TYPE_LIT:
		m =  CreateSubMeshLit();
		break;
	}
	//m_indices = std::vector<unsigned int>();
	//m_vertices = std::vector<VertexMaster>();
	m_vertices.clear();
	m_indices.clear();
	return m;
}

void MeshBuilder::AppendPlane(const Vector3& center, const Vector3& up, const Vector3& right, const Vector2 & size, const RGBA & color, Vector2 uvMins, Vector2 uvMaxs)
{

	SetColor(color);

	Vector3 nearBottomLeft	= center - (right * size.x) - (up * size.y);
	Vector3 nearBottomRight	= center + (right * size.x)	- (up * size.y);
	Vector3 farTopRight		= center + (right * size.x) + (up * size.y);
	Vector3 farTopLeft		= center - (right * size.x) + (up * size.y);

	Vector3 normal = Cross(up, right).GetNormalized();

	Vector2 bl_UV = uvMins;
	Vector2 br_UV = Vector2(uvMaxs.x, uvMins.y);
	Vector2 tl_UV = Vector2(uvMins.x, uvMaxs.y);
	Vector2 tr_UV = uvMaxs;

	SetNormal(normal);
	SetTangent(right);
	SetUV(bl_UV);
	unsigned int idx = PushVertex(nearBottomLeft);

	SetUV(br_UV);
	PushVertex(nearBottomRight);

	SetUV(tl_UV);
	PushVertex(farTopLeft);

	SetUV(tr_UV);
	PushVertex(farTopRight);

	AddTriIndices(idx + 0, idx + 1, idx + 2);
	AddTriIndices(idx + 2, idx + 1, idx + 3);

}

void MeshBuilder::AppendPlane2D(const AABB2 & plane, const RGBA & color, const AABB2 & uvs)
{
	SetColor(color);

	Vector3 bottomLeft	= Vector3(plane.mins, 0.f);
	Vector3 bottomRight	= Vector3(plane.maxs.x, plane.mins.y, 0.f);
	Vector3 topRight	= Vector3(plane.maxs, 0.f);
	Vector3 topLeft		= Vector3(plane.mins.x, plane.maxs.y, 0.f);

	Vector2 uvMins = uvs.mins;
	Vector2 uvMaxs = uvs.maxs;

	Vector2 bl_UV = uvMins;
	Vector2 br_UV = Vector2(uvMaxs.x, uvMins.y);
	Vector2 tl_UV = Vector2(uvMins.x, uvMaxs.y);
	Vector2 tr_UV = uvMaxs;

	SetNormal(Vector3::FORWARD * -1.f);
	SetTangent(Vector3::RIGHT);
	SetUV(bl_UV);
	unsigned int idx = PushVertex(bottomLeft);

	SetUV(br_UV);
	PushVertex(bottomRight);

	SetUV(tl_UV);
	PushVertex(topLeft);

	SetUV(tr_UV);
	PushVertex(topRight);

	AddTriIndices(idx + 0, idx + 1, idx + 2);
	AddTriIndices(idx + 2, idx + 1, idx + 3);
}

//assumes begin & end happen outside of the function somewhere
void MeshBuilder::AppendCube(Vector3 position, Vector3 size, RGBA color, AABB2 UV_TOP, AABB2 UV_SIDE, AABB2 UV_BOTTOM )
{

	Vector3 halfDimensions = size * .5f;
	Vector3 a = Vector3(position + Vector3(	-halfDimensions.x,	halfDimensions.y,	-halfDimensions.z));
	Vector3 b = Vector3(position + Vector3(	halfDimensions.x,	halfDimensions.y,	-halfDimensions.z));
	Vector3 c = Vector3(position - halfDimensions);
	Vector3 d = Vector3(position + Vector3(	halfDimensions.x,	-halfDimensions.y,	-halfDimensions.z));
	Vector3 e = Vector3(position + Vector3(	-halfDimensions.x,	halfDimensions.y,	halfDimensions.z));
	Vector3 f = Vector3(position + halfDimensions);
	Vector3 g = Vector3(position + Vector3(	-halfDimensions.x,	-halfDimensions.y,	halfDimensions.z));
	Vector3 h =	Vector3(position + Vector3(   halfDimensions.x,	-halfDimensions.y,	halfDimensions.z));

	
	/* 
	   e-------f
	  /|      /|
	 / |     / |
	a--|----b  |
	|  g----|--h
	| /     | /
	c-------d

	*/

	Vector2 TOP_bl_UV = UV_TOP.mins;
	Vector2 TOP_br_UV = Vector2(UV_TOP.maxs.x, UV_TOP.mins.y);
	Vector2 TOP_tl_UV = Vector2(UV_TOP.mins.x, UV_TOP.maxs.y);
	Vector2 TOP_tr_UV = UV_TOP.maxs;

	Vector2 SIDE_bl_UV = UV_SIDE.mins;
	Vector2 SIDE_br_UV = Vector2(UV_SIDE.maxs.x, UV_SIDE.mins.y);
	Vector2 SIDE_tl_UV = Vector2(UV_SIDE.mins.x, UV_SIDE.maxs.y);
	Vector2 SIDE_tr_UV = UV_SIDE.maxs;

	Vector2 BOTTOM_bl_UV = UV_BOTTOM.mins;
	Vector2 BOTTOM_br_UV = Vector2(UV_BOTTOM.maxs.x, UV_BOTTOM.mins.y);
	Vector2 BOTTOM_tl_UV = Vector2(UV_BOTTOM.mins.x, UV_BOTTOM.maxs.y);
	Vector2 BOTTOM_tr_UV = UV_BOTTOM.maxs;

	SetColor(color);

	/* back face
	a - b
	|   |
	c - d
	*/
	SetNormal(-1.f * Vector3::FORWARD);
	SetTangent(1.f * Vector3::RIGHT);
	SetUV(SIDE_bl_UV);
	unsigned int idx = PushVertex(c);

	SetUV(SIDE_br_UV);
	PushVertex(d);

	SetUV(SIDE_tl_UV);
	PushVertex(a);

	SetUV(SIDE_tr_UV);
	PushVertex(b);

	AddTriIndices(idx + 0, idx + 1, idx + 2);
	AddTriIndices(idx + 2, idx + 1, idx + 3);


	/* front face
		f - e
		|   |
		h - g
	*/
	SetNormal(Vector3::FORWARD);
	SetTangent(-1.f * Vector3::RIGHT);
	SetUV(SIDE_bl_UV);
	idx = PushVertex(h);

	SetUV(SIDE_br_UV);
	PushVertex(g);

	SetUV(SIDE_tl_UV);
	PushVertex(f);

	SetUV(SIDE_tr_UV);
	PushVertex(e);

	AddTriIndices(idx + 0, idx + 1, idx + 2);
	AddTriIndices(idx + 2, idx + 1, idx + 3);


	/* right face
	b - f
	|   |
	d - h
	*/
	SetNormal(Vector3::RIGHT);
	SetTangent(Vector3::FORWARD);
	SetUV(SIDE_bl_UV);
	idx = PushVertex(d);

	SetUV(SIDE_br_UV);
	PushVertex(h);

	SetUV(SIDE_tl_UV);
	PushVertex(b);

	SetUV(SIDE_tr_UV);
	PushVertex(f);

	AddTriIndices(idx + 0, idx + 1, idx + 2);
	AddTriIndices(idx + 2, idx + 1, idx + 3);

	/* left face
	e - a
	|   |
	g - c
	*/
	SetNormal(-1.f * Vector3::RIGHT);
	SetTangent(-1.f * Vector3::FORWARD);
	SetUV(SIDE_bl_UV);
	idx = PushVertex(g);

	SetUV(SIDE_br_UV);
	PushVertex(c);

	SetUV(SIDE_tl_UV);
	PushVertex(e);

	SetUV(SIDE_tr_UV);
	PushVertex(a);

	AddTriIndices(idx + 0, idx + 1, idx + 2);
	AddTriIndices(idx + 2, idx + 1, idx + 3);

	/* top face
	e - f
	|   |
	a - b
	*/
	SetNormal(Vector3::UP);
	SetTangent(Vector3::RIGHT);
	SetUV(TOP_bl_UV);
	idx = PushVertex(a);

	SetUV(TOP_br_UV);
	PushVertex(b);

	SetUV(TOP_tl_UV);
	PushVertex(e);

	SetUV(TOP_tr_UV);
	PushVertex(f);

	AddTriIndices(idx + 0, idx + 1, idx + 2);
	AddTriIndices(idx + 2, idx + 1, idx + 3);

	/* bottom face
	h - g
	|   |
	d - c

	c - d
	|   |
	g - h
	*/
	SetNormal(-1.f * Vector3::UP);
	SetTangent( Vector3::RIGHT);
	SetUV(BOTTOM_bl_UV);
	idx = PushVertex(g);

	SetUV(BOTTOM_br_UV);
	PushVertex(h);

	SetUV(BOTTOM_tl_UV);
	PushVertex(c);

	SetUV(BOTTOM_tr_UV);
	PushVertex(d);

	AddTriIndices(idx + 0, idx + 1, idx + 2);
	AddTriIndices(idx + 2, idx + 1, idx + 3);

}

void MeshBuilder::AppendSphere(Vector3 center, float radius, int wedges, int slices, RGBA color, AABB2 uvs)
{
	if (wedges <=0 || slices <= 0){
		return;
	}
	//Vector2 uvSize = Vector2(uvs.GetWidth() / (float) wedges, uvs.GetHeight() / (float) slices);
	Vector2 uvWidth = Vector2(uvs.GetWidth() / (float) wedges, 0.f);
	Vector2 uvHeight = Vector2(0.f,  uvs.GetHeight() / (float) slices);
	SetColor(color);
	float xTheta = 360.f / (float) wedges;
	float yTheta = 180.f / (float) slices;
	unsigned int idx;
	for (int y = 0; y < slices; y++){
		float azimuth = RangeMapFloat((float) y, 0.f, (float) slices, -90.f, 90.f);
		for (int x = 0; x < wedges; x++){
			float rotation = RangeMapFloat((float) x, 0.f, (float) slices, 0.f, 360.f);

			float nextAzimuth = ClampFloat(azimuth + yTheta, -90.f, 90.f);
		
			Vector2 bl_uv = (uvWidth * (float) x) + (uvHeight * (float) y);
			Vector3 botLeft = center + SphericalToCartesian(radius, rotation, azimuth);
			//Vector3 bl_tan = Vector3(-cosA * sinR * radius, 0.f, cosA * cosR * radius);
			Vector3 bl_tan = GetSphereTangentForVertex(botLeft, radius, rotation, azimuth);

			Vector2 br_uv = bl_uv + uvWidth;
			Vector3 botRight = center + SphericalToCartesian(radius, rotation + xTheta,  azimuth);
			Vector3 br_tan = GetSphereTangentForVertex(botLeft, radius, rotation+ xTheta, azimuth);

			Vector2 tl_uv = bl_uv + uvHeight;
			Vector3 topLeft = center + SphericalToCartesian(radius, rotation,  nextAzimuth);
			Vector3 tl_tan = GetSphereTangentForVertex(botLeft, radius, rotation, nextAzimuth);

			Vector2 tr_uv = bl_uv + uvWidth + uvHeight;
			Vector3 topRight = center + SphericalToCartesian(radius, rotation + xTheta,  nextAzimuth);
			Vector3 tr_tan = GetSphereTangentForVertex(botLeft, radius, rotation+ xTheta, nextAzimuth);


			

			SetUV(tl_uv);
			SetNormal((topLeft - center).GetNormalized());
			SetTangent(tl_tan);
			idx = PushVertex(topLeft);

			SetUV(tr_uv);
			SetNormal((topRight - center).GetNormalized());
			SetTangent(tr_tan);
			PushVertex(topRight);

			SetUV(bl_uv);
			SetNormal((botLeft - center).GetNormalized());
			SetTangent(bl_tan);
			PushVertex(botLeft);

			SetUV(br_uv);
			SetNormal((botRight - center).GetNormalized());
			SetTangent(br_tan);
			PushVertex(botRight);

			

			AddTriIndices(idx + 0, idx + 1, idx + 2);
			AddTriIndices(idx + 2, idx + 1, idx + 3);
		}
	}
}


void MeshBuilder::AppendVertices(std::vector<Vertex3D_PCU> verts, Transform t)
{
	//vertsVector.clear();
	/*if (numInArray == -1){
		numInArray = numVertices;
	}*/
	int numInArray = verts.size();
	const int numToDraw = numInArray;
	Matrix44 model = t.GetLocalMatrix();
	if ((verts.size() >0)){
		std::vector<Vertex3D_PCU> vertsVector = std::vector<Vertex3D_PCU>();
		for (int i = 0; i < numInArray; i++){
			Vertex3D_PCU startPoint = verts[i];
			Vertex3D_PCU endPoint = verts[((i+1)%numInArray)];
			Vector3 startPos = Matrix44::TransformPosition(startPoint.m_position, model);
			Vector3 endPos = Matrix44::TransformPosition(endPoint.m_position, model);

			SetUV(startPoint.m_uvs);
			SetColor(startPoint.m_color);
			PushVertex(startPos);

			SetUV(endPoint.m_uvs);
			SetColor(endPoint.m_color);
			PushVertex(endPos);
			//DrawLine(vertices[i], vertices[((i+1)%numVertices)], color, color);
		}
	}
}

void MeshBuilder::AddTriIndices(unsigned int idx1, unsigned int idx2, unsigned int idx3)
{
	PushIndex(idx1);
	PushIndex(idx2);
	PushIndex(idx3);
}



SubMesh * MeshBuilder::CreateSubMesh3DPCU()
{
	const VertexLayout layout = GetLayoutForType(VERTEX_TYPE_3DPCU);
	int x = 0;
	SubMesh* mesh = new SubMesh(layout);
	//
	size_t vsize = layout.m_stride * m_vertices.size(); 
	Vertex3D_PCU *buffer = (Vertex3D_PCU*) malloc( vsize ); 
	for (unsigned int i = 0; i < m_vertices.size(); ++i) {
		// copy each vertex
		buffer[i] = Vertex3D_PCU( m_vertices[i] ); 
	}

	//m_vertices.CopyToGPU( buffer, vsize ); 
	mesh->SetVertices(sizeof(Vertex3D_PCU) * m_vertices.size(), buffer);		//sets vertices in vertex buffer
	free( buffer ); 
	//
	
	mesh->SetIndices(m_indices.size(), m_indices.data());		//sets indices in index buffer
	mesh->m_instruction = m_drawInstruction;

	return mesh;
}

SubMesh * MeshBuilder::CreateSubMeshLit()
{
	const VertexLayout layout = GetLayoutForType(VERTEX_TYPE_LIT);
	SubMesh* mesh = new SubMesh(layout);
	//
	size_t vsize = layout.m_stride * m_vertices.size(); 
	Vertex3D_LIT *buffer = (Vertex3D_LIT*) malloc( vsize ); 
	for (unsigned int i = 0; i < m_vertices.size(); ++i) {
		// copy each vertex
		buffer[i] = Vertex3D_LIT( m_vertices[i] ); 
	}

	//m_vertices.CopyToGPU( buffer, vsize ); 
	mesh->SetVertices(sizeof(Vertex3D_LIT) * m_vertices.size(), buffer);		//sets vertices in vertex buffer
	free( buffer ); 
	//

	mesh->SetIndices(m_indices.size(), m_indices.data());		//sets indices in index buffer
	mesh->m_instruction = m_drawInstruction;

	return mesh;
}

Vector3 MeshBuilder::GetSphereTangentForVertex(Vector3 pos, float radius, float theta, float azimuth)
{
	Vector3 rightPos = SphericalToCartesian(radius, theta + .01f, azimuth);
	Vector3 leftPos = SphericalToCartesian(radius, theta - .01f, azimuth);
	return (leftPos - rightPos).GetNormalized();
}
