#include "Vertex.hpp"

/*
Vertex3D_PCU
*/
 VertexAttribute const Vertex3D_PCU::ATTRIBUTES[] = {
	VertexAttribute( "POSITION", RDT_FLOAT,         3, false, offsetof(Vertex3D_PCU, m_position) ),
	VertexAttribute( "COLOR",    RDT_UNSIGNED_BYTE, 4, true,  offsetof(Vertex3D_PCU, m_color) ),
	VertexAttribute( "UV",       RDT_FLOAT,         2, false, offsetof(Vertex3D_PCU, m_uvs) ),
	VertexAttribute(), // like a null terminator, how do we know the list is done;
}; 

 VertexLayout const Vertex3D_PCU::LAYOUT = VertexLayout( sizeof(Vertex3D_PCU), Vertex3D_PCU::ATTRIBUTES, 3);

 /*
 Vertex3D_LIT
 */
 VertexAttribute const Vertex3D_LIT::ATTRIBUTES[] = {
	 VertexAttribute( "POSITION", RDT_FLOAT			, 3, false	, offsetof(Vertex3D_LIT, m_position) ),
	 VertexAttribute( "NORMAL"	, RDT_FLOAT			, 3, true	, offsetof(Vertex3D_LIT, m_normal)	 ),
	 VertexAttribute( "TANGENT"	, RDT_FLOAT			, 4, true	, offsetof(Vertex3D_LIT, m_tangent)	 ),
	 VertexAttribute( "COLOR"	, RDT_UNSIGNED_BYTE	, 4, true	, offsetof(Vertex3D_LIT, m_color)	 ),
	 VertexAttribute( "UV"		, RDT_FLOAT			, 2, false	, offsetof(Vertex3D_LIT, m_uvs)		 ),
	 VertexAttribute(), 
 }; 

 VertexLayout const Vertex3D_LIT::LAYOUT = VertexLayout( sizeof(Vertex3D_LIT), Vertex3D_LIT::ATTRIBUTES, 5);



 const VertexLayout& GetLayoutForType(eVertexType type)
 {
	 switch(type){
	 case VERTEX_TYPE_3DPCU:
		 return Vertex3D_PCU::LAYOUT;
		 break;
	 case VERTEX_TYPE_LIT:
		 return Vertex3D_LIT::LAYOUT;
		 break;
	 }

	 return VertexLayout();
 }



Vertex3D_PCU::Vertex3D_PCU(VertexMaster const & other)
{
	m_position = other.m_position;
	m_color = other.m_color;
	m_uvs = other.m_uvs;
}

Vertex3D_PCU::Vertex3D_PCU(Vector3 pos, RGBA color, Vector2 uvs)
{
	m_position = pos;
	m_color = color;
	m_uvs = uvs;
}

Vertex3D_PCU::Vertex3D_PCU(Vector2 pos, RGBA color, Vector2 uvs)
{
	m_position = Vector3(pos.x, pos.y);
	m_color = color;
	m_uvs = uvs;
}

Vertex3D_LIT::Vertex3D_LIT(VertexMaster const & other)
{
	m_position = other.m_position;
	m_normal = other.m_normal;
	m_tangent = other.m_tangent;
	m_color = other.m_color;
	m_uvs = other.m_uvs;
}
